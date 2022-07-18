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

#include <vector>
#include <iostream>
#include <memory>
#include <ul_Logger.h>
#include <ul_Runnable.h>
#include <functional>

namespace ultra
{
using THREAD_ID = unsigned long int;

namespace __ultra__innerThreadLocker__
{
class CThreadLocker;
} // namespace __ultra__innerThreadLocker__

class CThreadLock
{
private:
    std::shared_ptr<void> m_lockSp;
    void *m_lock;
    friend class __ultra__innerThreadLocker__::CThreadLocker;
public:
    CThreadLock();
    virtual ~CThreadLock();
private:
    int lock(std::string file, int line) const;
    int unlock(std::string file, int line) const;
public:
    int wait(std::string file, int line) const;
    int timedWait(long int sec, long int nsec, std::string file, int line) const;
    int signal(std::string file, int line) const;
    int broadcast(std::string file, int line) const;
};

class CUltraThread
{
private:
    std::shared_ptr<void> m_controller;
    void* m_controllerPtr;
    CUltraThread();

public:
    static CUltraThread *getInstance();
    virtual ~CUltraThread();

    int start(IRunnable *runnable, void *context);
    int join(IRunnable *runnable);
    bool isBusy(IRunnable *runnable);
    bool hasFinished(IRunnable *runnable);
    int clearCompleted();
    bool someThreadsStillRunning();
    int getMaxThreadCount(int &max);
    int setMaxThreadCount(int max);
    int signal(IRunnable *runnable, int signal);
    THREAD_ID getCurrentThreadID();
};

namespace __ultra__innerThreadLocker__
{

/**
 * The class CThreadLocker will lock a CThreadLock object in its constructor 
 * and unlock in its destructor making locking and unlocking of CThreadLock objects
 * exceptions safe
 * <br><br>
 * <code>
 * CThreadLock lock;<br>
 * <br>
 * {<br>
 *    volatile CThreadLocker l(__FILE__,__LINE__,lock); // here the lock will be locked<br>
 * <br>
 *    // do stuff here<br>
 *    // and here<br>
 *    // and here<br>
 * <br>
 * }  // the moment the closing bracket is called the CThreadLocker::~CThreadLocker() is called which will unlock the lock<br>
 * <br>
 * <br>
 * // it is better to call the macro AUTO_LOCK as it does the same
 * {<br>
 *    AUTO_LOCK(lock); // here the lock will be locked<br>
 * <br>
 *    // do stuff here<br>
 *    // and here<br>
 *    // and here<br>
 * <br>
 * }  // the moment the closing bracket is called the CThreadLocker::~CThreadLocker() is called which will unlock the lock<br>
 * </code>
 */
class CThreadLocker
{
private:
    std::shared_ptr<CThreadLock> m_lock;
    std::shared_ptr<void> m_lockVp;
    std::string m_file;
    int m_line;
    CThreadLocker();
public:
    explicit CThreadLocker(std::string file, int line, std::shared_ptr<CThreadLock> lock);
    explicit CThreadLocker(std::string file, int line, std::shared_ptr<void> lock);
    virtual ~CThreadLocker();
};

} // namespace namespace __ultra__innerThreadLocker__

#define __AUTO_LOCK_CAT(a, b) __AUTO_LOCK_CAT_I(a, b)
#define __AUTO_LOCK_CAT_I(a, b) a ## b
#define __AUTO_LOCK_UNIQUE_NAME(base) __AUTO_LOCK_CAT(base, __LINE__)
#define AUTO_LOCK(lock) volatile ultra::__ultra__innerThreadLocker__::CThreadLocker __AUTO_LOCK_UNIQUE_NAME(ULTRA_AUTO_LOCK_)(__FILE__, __LINE__, lock)

} //namespace ultra

