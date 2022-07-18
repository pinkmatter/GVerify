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

#include "ul_UltraThread.h"

#include "ul_Utility.h"
#include "ul_Logger.h"

#include "lock/ul_UltraThreadLock_int.h"

#include "lock/pthreads/ul_UltraThreadLockPthread.h"
#include "ul_Exception.h"

namespace ultra
{

typedef __ultra_internal::IUltraThreadLock ULTRA_THREAD_LOCK_BASETYPE;
typedef ULTRA_THREAD_LOCK_BASETYPE* WRAPPER_LOCK_POINTER;
typedef __ultra_internal::CUltraThreadLockPthread WRAPPER_LOCK_CLASS_TO_CREATE;

CThreadLock::CThreadLock() :
m_lockSp(new WRAPPER_LOCK_CLASS_TO_CREATE(), std::default_delete<WRAPPER_LOCK_CLASS_TO_CREATE>())
{
    m_lock = m_lockSp.get();
}

CThreadLock::~CThreadLock()
{
    m_lock = nullptr;
    m_lockSp = nullptr;
}

int CThreadLock::lock(std::string file, int line) const
{
    WRAPPER_LOCK_POINTER theLock = (WRAPPER_LOCK_POINTER) m_lock;
    int ret = theLock->lock();
    if (ret != 0)
        CLogger::getInstance()->Log(file, line, CLogger::LOG_ERROR, "Failed to lock a mutex '" + toString((long long) this) + "'");
    return ret;
}

int CThreadLock::unlock(std::string file, int line) const
{
    WRAPPER_LOCK_POINTER theLock = (WRAPPER_LOCK_POINTER) m_lock;
    int ret = theLock->unlock();
    if (ret != 0)
        CLogger::getInstance()->Log(file, line, CLogger::LOG_ERROR, "Failed to unlock a mutex '" + toString((long long) this) + "'");
    return ret;
}

int CThreadLock::wait(std::string file, int line) const
{
    WRAPPER_LOCK_POINTER theLock = (WRAPPER_LOCK_POINTER) m_lock;
    int ret = theLock->wait();
    if (ret != 0)
        CLogger::getInstance()->Log(file, line, CLogger::LOG_ERROR, "Failed to wait on a mutex '" + toString((long long) this) + "'");
    return ret;
}

int CThreadLock::timedWait(long int sec, long int nsec, std::string file, int line) const
{
    WRAPPER_LOCK_POINTER theLock = (WRAPPER_LOCK_POINTER) m_lock;
    int ret = theLock->timedWait(sec, nsec);
    if (ret != 0)
        CLogger::getInstance()->Log(file, line, CLogger::LOG_ERROR, "Failed to wait on a mutex '" + toString((long long) this) + "'");
    return ret;
}

int CThreadLock::signal(std::string file, int line) const
{
    WRAPPER_LOCK_POINTER theLock = (WRAPPER_LOCK_POINTER) m_lock;
    int ret = theLock->signal();
    if (ret != 0)
        CLogger::getInstance()->Log(file, line, CLogger::LOG_ERROR, "Failed to signal a mutex '" + toString((long long) this) + "'");
    return ret;
}

int CThreadLock::broadcast(std::string file, int line) const
{
    WRAPPER_LOCK_POINTER theLock = (WRAPPER_LOCK_POINTER) m_lock;
    int ret = theLock->broadcast();
    if (ret != 0)
        CLogger::getInstance()->Log(file, line, CLogger::LOG_ERROR, "Failed to broadcast to a mutex '" + toString((long long) this) + "'");
    return ret;
}

namespace __ultra__innerThreadLocker__
{

CThreadLocker::CThreadLocker()
{
    m_file = __FILE__;
    m_line = __LINE__;
}

CThreadLocker::CThreadLocker(std::string file, int line, std::shared_ptr<CThreadLock> lock)
: m_lock(lock)
{
    m_file = file;
    m_line = line;
    if (m_lock)
    {
        if (m_lock->lock(m_file, m_line) != 0)
        {
            m_lockVp = nullptr;
            throw CException(m_file, m_line, "Failure returned from CThreadLock::lock()");
        }
    }
}

CThreadLocker::CThreadLocker(std::string file, int line, std::shared_ptr<void> lock)
: m_lockVp(lock)
{
    m_file = file;
    m_line = line;
    if (m_lockVp)
    {
        if (((CThreadLock*) m_lockVp.get())->lock(m_file, m_line) != 0)
        {
            m_lockVp = nullptr;
            throw CException(m_file, m_line, "Failure returned from CThreadLock::lock()");
        }
    }
}

CThreadLocker::~CThreadLocker()
{
    bool bad = false;
    if (m_lock)
    {
        if (m_lock->unlock(m_file, m_line) != 0)
            bad = true;

    }
    else if (m_lockVp)
    {
        if (((CThreadLock*) m_lockVp.get())->unlock(m_file, m_line) != 0)
            bad = true;
    }
    if (bad)
    {
        // destructors may not throw exceptions, rather log this
        CLogger::getInstance()->Log(m_file, m_line, CLogger::LOG_ERROR, "Failure returned from CThreadLock::unlock()");
        // must call terminate because:
        // if we cannot unlock then the program will go haywire, as the lock was definitely locked in the contructor
        std::terminate();
    }
}

} // namespace __ultra__innerThreadLocker__

} // namespace ultra
