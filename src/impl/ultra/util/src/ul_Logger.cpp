/*
* Copyright 2018 Pinkmatter Solutions
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "ul_Logger.h"

#include <stdlib.h>
#include <fcntl.h>
#include "ul_Utility.h"
#include <sys/time.h>

#include "concurrency/lock/ul_UltraThreadLock_int.h"
#include "concurrency/lock/pthreads/ul_UltraThreadLockPthread.h"

namespace ultra
{

typedef __ultra_internal::IUltraThreadLock ULTRA_THREAD_BASETYPE;
typedef ULTRA_THREAD_BASETYPE* WRAPPER_LOCK_POINTER;
typedef __ultra_internal::CUltraThreadLockPthread WRAPPER_LOCK_CLASS_TO_CREATE;

namespace ultra_InternalLoggingStructs
{

bool SLoggerMaxLog::operator==(const SLoggerMaxLog &r)
{
    return (line == r.line && file == r.file);
}

SLoggerMaxLog::~SLoggerMaxLog()
{
    file.clear();
}

SLoggerMaxLog::SLoggerMaxLog()
{
    counter = 0;
}

SLoggerMaxLog::SLoggerMaxLog(std::string &file, int line)
{
    this->file = file;
    this->line = line;
    counter = 0;
}

} // namespace ultra_InternalLoggingStructs

CLogger::CLogger() :
m_lock(new WRAPPER_LOCK_CLASS_TO_CREATE(), std::default_delete<WRAPPER_LOCK_CLASS_TO_CREATE>())
{
    m_loggerIsuseTee = true;
    m_details = true;
    m_getLocalTime = false;
    m_logPath = "";
    m_limitedLogItems.clear();
    m_logLevel = CLogger::LOG_INFO;
}

CLogger::~CLogger()
{
    m_logPath = "";
    m_limitedLogItems.clear();
}

CLogger *CLogger::getInstance()
{
    static CLogger instance;
    return &instance;
}

int CLogger::lock() const
{
    return ((WRAPPER_LOCK_CLASS_TO_CREATE*) m_lock.get())->lock();
}

int CLogger::unlock() const
{
    return ((WRAPPER_LOCK_CLASS_TO_CREATE*) m_lock.get())->unlock();
}

std::string CLogger::getTimeStr() const
{
    std::string ret;
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    time_t now = curTime.tv_sec;
    tm *ltm = m_getLocalTime ? localtime(&now) : gmtime(&now);
    char temp[2048];
    snprintf(temp, 2047, "%04d-%02d-%02dT%02d:%02d:%02d:%03d", ltm->tm_year + 1900,
             ltm->tm_mon + 1,
             ltm->tm_mday,
             ltm->tm_hour,
             ltm->tm_min,
             ltm->tm_sec,
             milli);
    ret = temp;

    return ret;
}

void CLogger::setUseTEE(bool useTeeCommand)
{
    lock();
    m_loggerIsuseTee = useTeeCommand;
    unlock();
}

void CLogger::setUseLocalTime(bool useLocalTime)
{
    lock();
    m_getLocalTime = useLocalTime;
    unlock();
}

int CLogger::setLogFilePath(std::string path)
{
    lock();
    if (path == "")
    {
        unlock();
        return 0;
    }
    int fd = open(path.c_str(), O_TRUNC | O_CLOEXEC | O_RDWR | O_CREAT, 0666);
    if (fd < 0)
    {
        unlock();
        return 1;
    }
    close(fd);
    m_logPath = path;
    unlock();
    return 0;
}

int CLogger::innerLogLimited(std::string file, int line, std::string msg, std::string msgColor, int maxAmountOfLogsAllowed) const
{
    int ret = 0;
    bool found = false;
    int index = 0;
    for (unsigned int p = 0; p < m_limitedLogItems.size(); p++)
    {
        if (line == m_limitedLogItems[p]->line && file == m_limitedLogItems[p]->file)
        {
            found = true;
            index = p;
            break;
        }
    }

    if (!found)
    {
        std::shared_ptr<ultra_InternalLoggingStructs::SLoggerMaxLog> item = std::make_shared<ultra_InternalLoggingStructs::SLoggerMaxLog>(file, line);
        item->counter++;
        std::string head = "Limited log (" + toString(item->counter) + "/" + toString(maxAmountOfLogsAllowed) + "): ";
        msg = head + msg;
        msgColor = head + msgColor;
        innerLog(getFileName(item->file), item->line, msg, msgColor, false);
        m_limitedLogItems.push_back(item);
    }
    else
    {
        if (m_limitedLogItems[index]->counter < maxAmountOfLogsAllowed)
        {
            m_limitedLogItems[index]->counter++;
            std::string head = "Limited log (" + toString(m_limitedLogItems[index]->counter) + "/" + toString(maxAmountOfLogsAllowed) + "): ";
            msg = head + msg;
            msgColor = head + msgColor;
            innerLog(getFileName(m_limitedLogItems[index]->file), m_limitedLogItems[index]->line, msg, msgColor, false);
        }
    }

    return ret;
}

int CLogger::innerLog(std::string file, int line, std::string msg, std::string msgColor, bool isTitle) const
{
    if (!m_logPath.empty())
    {
        int fd = open(m_logPath.c_str(), O_CLOEXEC | O_RDWR | O_APPEND, 0666);
        if (fd < 0)
        {
            std::cout << "Failed to write to logFile at " << m_logPath.c_str() << std::endl;
            return 1;
        }
        std::string message;
        if (isTitle)
            message = msg;
        else
            message = getTimeStr() + " " + file + " " + toString(line) + ": " + msg;
        write(fd, message.c_str(), message.length());
        if (msg.length() > 0)
        {
            if (msg[msg.length() - 1] != '\n')
            {
                message = "\n";
                write(fd, message.c_str(), message.length());
            }
        }
        else
        {
            message = "\n";
            write(fd, message.c_str(), message.length());
        }
        close(fd);
    }

    if (m_loggerIsuseTee)
    {
        if (m_details)
        {
            if (isTitle)
                std::cout << msgColor;
            else
                std::cout << getTimeStr() << " " << file << " " << line << ": " << msgColor;
        }
        else
        {
            std::cout << msgColor;
        }

        if (msg.length() > 0)
        {
            if (msg[msg.length() - 1] != '\n')
            {
                std::cout << std::endl;
            }
        }
        else
        {
            std::cout << std::endl;
        }
    }

    return 0;
}

std::string CLogger::buildMessage(ELogLevel level, std::string msg, std::string &colorMsg) const
{
    std::string ret;
    switch (level)
    {
    case LOG_DEBUG:
        if (m_details)
        {
            ret = "DEBUG: " + msg;
            colorMsg = ret;
        }
        else
            colorMsg = ret = msg;

        break;
    case LOG_INFO:
        if (m_details)
        {
            ret = "INFO: " + msg;
            colorMsg = ret;
        }
        else
            colorMsg = ret = msg;
        break;
    case LOG_WARN:
        if (m_details)
        {
            ret = "WARN: " + msg;
            colorMsg = ret;
        }
        else
            colorMsg = ret = msg;
        break;
    case LOG_ERROR:
        if (m_details)
        {
            ret = "ERROR: " + msg;
            colorMsg = ret;
        }
        else
            colorMsg = ret = msg;

        break;
    }
    return ret;
}

int CLogger::LogLimited(std::string file, int line, ELogLevel level, std::string msg, int maxAmountOfLogsAllowed) const
{
    if (!canLog(level))
        return 0;

    lock();
    bool found = false;
    int index = 0;
    for (unsigned int p = 0; p < m_limitedLogItems.size(); p++)
    {
        if (line == m_limitedLogItems[p]->line && file == m_limitedLogItems[p]->file)
        {
            found = true;
            index = p;
            break;
        }
    }
    if (found)
    {
        if (m_limitedLogItems[index]->counter >= maxAmountOfLogsAllowed)
        {
            unlock();
            return 0;
        }
    }

    std::string colorMsg;
    msg = buildMessage(level, msg, colorMsg);

    if (innerLogLimited(file, line, msg, colorMsg, maxAmountOfLogsAllowed) != 0)
    {
        unlock();
        return 1;
    }

    unlock();
    return 0;
}

int CLogger::Log(ELogLevel level, const CException &ex) const
{
    std::string file;
    int line;
    std::string msg;
    ex.GetException(file, line, msg);
    std::string ecpMsg = ex.getExceptionName() + ": With message = '" + msg + "'";
    return Log(file, line, level, ecpMsg);
}

int CLogger::Log(std::string file, int line, ELogLevel level, const std::exception &ex) const
{
    const std::exception *ptr = &ex;
    const CException *uEx = dynamic_cast<const CException *> (ptr);
    if (uEx != nullptr)
        return Log(level, *uEx);
    else
        return Log(file, line, level, ex.what());
}

int CLogger::LogToFile(ELogLevel level, std::string msg) const
{
    if (!canLog(level))
        return 0;

    lock();
    if (!m_logPath.empty())
    {
        int fd = open(m_logPath.c_str(), O_CLOEXEC | O_RDWR | O_APPEND, 0666);
        if (fd < 0)
        {
            std::cout << "Failed to write to logFile at " << m_logPath.c_str() << std::endl;
            unlock();
            return 1;
        }

        if (msg.length() > 0)
            write(fd, msg.c_str(), msg.length());
        if (msg.length() > 0)
        {
            if (msg[msg.length() - 1] != '\n')
            {
                msg = "\n";
                write(fd, msg.c_str(), msg.length());
            }
        }
        else
        {
            msg = "\n";
            write(fd, msg.c_str(), msg.length());
        }
        close(fd);
    }
    unlock();
    return 0;
}

bool CLogger::canLog(ELogLevel level) const
{
    if (level == LOG_NONE)
        return false;
    lock();
    int LoggerLogLevel = static_cast<int> (m_logLevel);
    bool result = LoggerLogLevel <= static_cast<int> (level);
    unlock();
    return result;
}

int CLogger::Log(std::string file, int line, ELogLevel level, std::string msg) const
{
    if (!canLog(level))
        return 0;

    file = getFileName(file);
    lock();
    std::string colorMsg;
    msg = buildMessage(level, msg, colorMsg);
    if (innerLog(file, line, msg, colorMsg, false) != 0)
    {
        unlock();
        return 1;
    }
    unlock();
    return 0;
}

int CLogger::LogNoNewLine(std::string file, int line, ELogLevel level, std::string msg) const
{
    if (!canLog(level))
        return 0;

    file = getFileName(file);

    lock();
    std::string colorMsg;
    msg = buildMessage(level, msg, colorMsg);
    if (innerLogNoNewLine(file, line, colorMsg) != 0)
    {
        unlock();
        return 1;
    }
    unlock();
    return 0;
}

int CLogger::innerLogNoNewLine(std::string file, int line, std::string msgColor) const
{
    if (m_loggerIsuseTee)
    {
        if (m_details)
            std::cout << getTimeStr() << " " << file << " " << line << ": " << msgColor << std::flush;
        else
            std::cout << msgColor << std::flush;
    }

    return 0;
}

void CLogger::setLogLevel(ELogLevel logLevel)
{
    if (static_cast<int> (logLevel) < 0 || static_cast<int> (logLevel) >= static_cast<int> (LOG_ENUM_COUNT))
    {
        return;
    }
    lock();
    m_logLevel = logLevel;
    unlock();
}

CLogger::ELogLevel CLogger::getLogLevel() const
{
    ELogLevel ret;

    lock();
    ret = m_logLevel;
    unlock();

    return ret;
}

void CLogger::LogLogLevels() const
{
    lock();
    std::string msg;
    switch (m_logLevel)
    {
    case CLogger::LOG_DEBUG:
        msg = "LOGGER: Log level is: DEBUG";
        innerLog(getFileName(__FILE__), __LINE__, msg, msg, false);
        break;
    case CLogger::LOG_ERROR:
        msg = "LOGGER: Log level is: ERROR";
        innerLog(getFileName(__FILE__), __LINE__, msg, msg, false);
        break;
    case CLogger::LOG_INFO:
        msg = "LOGGER: Log level is: INFO";
        innerLog(getFileName(__FILE__), __LINE__, msg, msg, false);
        break;
    case CLogger::LOG_WARN:
        msg = "LOGGER: Log level is: WARN";
        innerLog(getFileName(__FILE__), __LINE__, msg, msg, false);
        break;
    }
    unlock();
}

std::string CLogger::getFileName(const std::string name) const
{
    const char separatorChar = '/';
    std::string absPath = name;
    int l = absPath.length();
    std::string itemName = "";
    if (l == 0)
        return itemName;

    for (int x = l - 1; x >= 0; x--)
    {
        if (absPath[x] == separatorChar)
            break;
        itemName += absPath[x];
    }

    l = itemName.length();
    for (int x = 0; x < l / 2; x++)
    {
        char t = itemName[x];
        itemName[x] = itemName[l - 1 - x];
        itemName[l - 1 - x] = t;
    }

    return itemName;
}

void CLogger::disableLogDetails()
{
    lock();
    m_details = false;
    unlock();
}

void CLogger::enableLogDetails()
{
    lock();
    m_details = true;
    unlock();
}

} //namespace ultra
