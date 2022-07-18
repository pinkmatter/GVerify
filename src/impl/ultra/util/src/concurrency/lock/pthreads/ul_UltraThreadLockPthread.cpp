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

#include "ul_UltraThreadLockPthread.h"

#include <pthread.h>
#include <errno.h>

namespace ultra
{
namespace __ultra_internal
{

void CUltraThreadLockPthread::deleteMutex::operator()(void* i)
{
    if (i != nullptr)
    {
        pthread_mutex_t *mutex = (pthread_mutex_t*) i;
        pthread_mutex_destroy(mutex);
        delete mutex;
        mutex = nullptr;
    }
}

void CUltraThreadLockPthread::deleteCond::operator()(void* i)
{
    if (i != nullptr)
    {
        pthread_cond_t *cond = (pthread_cond_t*) i;
        pthread_cond_destroy(cond);
        delete cond;
        cond = nullptr;
    }
}

void CUltraThreadLockPthread::deleteMutexAttr::operator()(void* i)
{
    if (i != nullptr)
    {
        pthread_mutexattr_t *attr = (pthread_mutexattr_t*) i;
        pthread_mutexattr_destroy(attr);
        delete attr;
        attr = nullptr;
    }
}

CUltraThreadLockPthread::CUltraThreadLockPthread() : IUltraThreadLock()
{
    m_cond.reset((void*) new pthread_cond_t);
    m_mutex_attr.reset((void*) new pthread_mutexattr_t);
    m_mutex.reset((void*) new pthread_mutex_t);

    pthread_cond_init((pthread_cond_t*) m_cond.get(), nullptr);
    pthread_mutexattr_init((pthread_mutexattr_t*) m_mutex_attr.get());
    pthread_mutexattr_settype((pthread_mutexattr_t*) m_mutex_attr.get(), PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init((pthread_mutex_t*) m_mutex.get(), (pthread_mutexattr_t*) m_mutex_attr.get());
}

CUltraThreadLockPthread::~CUltraThreadLockPthread()
{
}

int CUltraThreadLockPthread::lock() const
{
    pthread_mutex_t* mutex = (pthread_mutex_t*) m_mutex.get();
    return pthread_mutex_lock(mutex);
}

int CUltraThreadLockPthread::unlock() const
{
    pthread_mutex_t* mutex = (pthread_mutex_t*) m_mutex.get();
    return pthread_mutex_unlock(mutex);
}

int CUltraThreadLockPthread::wait() const
{
    pthread_mutex_t* mutex = (pthread_mutex_t*) m_mutex.get();
    pthread_cond_t* cond = (pthread_cond_t*) m_cond.get();
    return pthread_cond_wait(cond, mutex);
}

int CUltraThreadLockPthread::timedWait(long int sec, long int nsec) const
{
    timespec waitTime;
    waitTime.tv_sec = sec;
    waitTime.tv_nsec = nsec;

    pthread_mutex_t* mutex = (pthread_mutex_t*) m_mutex.get();
    pthread_cond_t* cond = (pthread_cond_t*) m_cond.get();
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    now.tv_sec += waitTime.tv_sec;
    now.tv_nsec += waitTime.tv_nsec;
    int ret = pthread_cond_timedwait(cond, mutex, &now);
    if (ret == 0 || ret == ETIMEDOUT)
        return 0;
    return 1;
}

int CUltraThreadLockPthread::signal() const
{
    pthread_cond_t* cond = (pthread_cond_t*) m_cond.get();
    return pthread_cond_signal(cond);
}

int CUltraThreadLockPthread::broadcast() const
{
    pthread_cond_t* cond = (pthread_cond_t*) m_cond.get();
    return pthread_cond_broadcast(cond);
}

} // namespace __ultra_internal
} //namespace ultra
