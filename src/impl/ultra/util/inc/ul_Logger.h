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

#pragma once

#include <iostream>
#include <vector>

#include <ul_Exception.h>
#include <memory>

namespace ultra
{

namespace ultra_InternalLoggingStructs
{

struct SLoggerMaxLog
{
    std::string file;
    int line;
    int counter;

    bool operator==(const SLoggerMaxLog & r);
    ~SLoggerMaxLog();
    SLoggerMaxLog();
    SLoggerMaxLog(std::string &file, int line);
};

} // namespace ultra_InternalLoggingStructs

class CLogger
{
public:

    enum ELogLevel
    {
        LOG_DEBUG = 0,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_NONE,
        LOG_ENUM_COUNT
    };
private:
    std::shared_ptr<void> m_lock;
    std::string m_logPath;
    bool m_loggerIsuseTee;
    bool m_getLocalTime;
    bool m_details;
    mutable std::vector<std::shared_ptr<ultra_InternalLoggingStructs::SLoggerMaxLog> > m_limitedLogItems;
    std::string getTimeStr() const;
    ELogLevel m_logLevel;
    int innerLog(std::string file, int line, std::string msg, std::string msgColor, bool isTitle) const;
    int innerLogLimited(std::string file, int line, std::string msg, std::string msgColor, int maxAmountOfLogsAllowed = 5) const;
    int innerLogNoNewLine(std::string file, int line, std::string msgColor) const;
    std::string getFileName(const std::string name) const;
    std::string buildMessage(ELogLevel level, std::string msg, std::string &colorMsg) const;
    int lock() const;
    int unlock() const;
    CLogger();
public:
    virtual ~CLogger();
    static CLogger *getInstance();
    void setLogLevel(ELogLevel logLevel);
    ELogLevel getLogLevel() const;
    int Log(std::string file, int line, ELogLevel level, std::string msg) const;
    int Log(ELogLevel level, const CException &ex) const;
    int Log(std::string file, int line, ELogLevel level, const std::exception &ex) const;
    int LogLimited(std::string file, int line, ELogLevel level, std::string msg, int maxAmountOfLogsAllowed = 5) const;
    int LogNoNewLine(std::string file, int line, ELogLevel level, std::string msg) const;
    int LogToFile(ELogLevel level, std::string msg) const;
    int setLogFilePath(std::string path);
    void LogLogLevels() const;
    void setUseTEE(bool useTeeCommand);
    void setUseLocalTime(bool useLocalTime);
    void disableLogDetails();
    void enableLogDetails();
    bool canLog(ELogLevel level) const;
};

} //namespace ultra
