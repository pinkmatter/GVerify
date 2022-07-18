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

#include "ul_UltraThreadThreadPthread.h"

#include "ul_Logger.h"
#include "ul_Utility.h"
#include "ul_Exception.h"
#include <signal.h>
#include <errno.h>
#include <sys/resource.h>
#include <ios>
#include <pthread.h>

namespace ultra
{
namespace __ultra_internal
{

CUltraThreadThreadPthread::CUltraThreadThreadPthread() :
IUltraThreadThread(),
m_lock(std::make_shared<CThreadLock>())
{
    m_threads.clear();
}

CUltraThreadThreadPthread::~CUltraThreadThreadPthread()
{
    m_threads.clear();
}

bool CUltraThreadThreadPthread::alreadyExists(IRunnable *runnable)
{
    if (runnable == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot add nullptr UltraThread pointer");
        return true;
    }

    for (unsigned long x = 0; x < m_threads.size(); x++)
    {
        if (m_threads[x]->m_runnable == runnable)
            return true;
    }
    return false;
}

int CUltraThreadThreadPthread::inner_start(IRunnable *runnable, void *context)
{
    if (inner_add(runnable) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_add()");
        return 1;
    }

    if (inner_run(runnable, context) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_run()");
        return 1;
    }

    return 0;
}

int CUltraThreadThreadPthread::inner_add(IRunnable *runnable)
{
    if (alreadyExists(runnable))
    {
        std::stringstream add;
        add.clear();
        add << std::hex << runnable << std::dec;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot add this '" + add.str() + "' UltraThread as it is already added, or it is nullptr");
        return 1;
    }

    m_threads.push_back(std::make_shared<CUltraThreadItem>(runnable));

    return 0;
}

int CUltraThreadThreadPthread::findId(IRunnable *runnable, unsigned long &id)
{
    for (unsigned long x = 0; x < m_threads.size(); x++)
    {
        if (m_threads[x]->m_runnable == runnable)
        {
            id = x;
            return 0;
        }
    }
    return 1;
}

int CUltraThreadThreadPthread::inner_run(IRunnable *runnable, void *context)
{
    unsigned long id;
    if (findId(runnable, id) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to find UltraThread id to start");
        return 1;
    }

    if (m_threads[id]->getStarted())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot start the same UltraThread twice");
        return 1;
    }

    m_threads[id]->setStarted(true);
    m_threads[id]->m_context = context;
    m_threads[id]->setBase(this);
    int startCode = pthread_create
        ((pthread_t*) m_threads[id]->m_thread.get(),
         nullptr,
         CUltraThreadThreadPthread::innerThreadRunner,
         m_threads[id].get()
         );

    if (startCode == EAGAIN)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Maximum amount of threads running, cannot start more threads, see setrlimit(RLIMIT_NPROC)");
    }
    if (startCode != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to start UltraThread");
        return 1;
    }

    return 0;
}

void* CUltraThreadThreadPthread::innerThreadRunner(void *context)
{
    CUltraThreadItem *threadAndcontext = (CUltraThreadItem *) context;
    CUltraThreadThreadPthread* base = threadAndcontext->getBase();
    if (pthread_detach(pthread_self()) != 0) // now this thread does not need to be joined
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to detach a thread, resources will not be cleaned up");
    }
    try
    {
        threadAndcontext->m_runnable->run(threadAndcontext->m_context);
    }
    catch (const std::exception *e)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, *e);
        delete e;
    }
    catch (const std::exception &e)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, e);
    }
    catch (...)
    {
        std::string add = toHexString((size_t) threadAndcontext->m_runnable);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Some unknown exception was thrown in an UltraThread (" + add + ")");
    }
    threadAndcontext->setEnded(true);
    if (remove(base, threadAndcontext->m_runnable) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from remove()");
    }
    pthread_exit(nullptr); // this contains the return, you cannot call 'return pthread_exit(nullptr);'
}

int CUltraThreadThreadPthread::inner_join(IRunnable *runnable)
{
    unsigned long id;
    while (findId(runnable, id) == 0)
    {
        m_lock->wait(__FILE__, __LINE__);
    }

    return 0;
}

bool CUltraThreadThreadPthread::inner_hasFinished(IRunnable *runnable)
{
    if (runnable == nullptr)
    {
        return true;
    }

    if (alreadyExists(runnable))
        return false;

    return true;
}

bool CUltraThreadThreadPthread::inner_isBusy(IRunnable *runnable)
{
    if (runnable == nullptr)
    {
        return false;
    }

    for (unsigned long x = 0; x < m_threads.size(); x++)
    {
        if (m_threads[x]->m_runnable == runnable)
        {
            if (m_threads[x]->getStarted() && !m_threads[x]->getEnded())
                return true;
        }
    }
    return false;
}

int CUltraThreadThreadPthread::inner_clearCompleted()
{
    for (long x = m_threads.size() - 1; x >= 0; x--)
    {
        if (m_threads[x]->getEnded())
        {
            m_threads[x].reset();
            m_threads.erase(m_threads.begin() + x);
        }
    }
    return 0;
}

void *CUltraThreadThreadPthread::getThread(IRunnable *runnable)
{
    if (runnable == nullptr)
    {
        return nullptr;
    }

    for (unsigned long x = 0; x < m_threads.size(); x++)
    {
        if (m_threads[x]->m_runnable == runnable)
        {
            return m_threads[x]->m_thread.get();
        }
    }

    return nullptr;
}

int CUltraThreadThreadPthread::inner_signal(IRunnable *runnable, int signal)
{
    void *th_ptr = getThread(runnable);
    if (th_ptr == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "No thread found that matches argument IRunnable");
        return 0;
    }
    pthread_t *th = (pthread_t*) th_ptr;
    int ret;
    if ((ret = pthread_kill(*th, signal)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from pthread_kill(), return code = '" + toString(ret) + "'");
        return 1;
    }

    return 0;
}

int CUltraThreadThreadPthread::inner_remove(CUltraThreadThreadPthread *base, IRunnable *runnable)
{
    std::stringstream add;
    add.clear();
    add << std::hex << runnable << std::dec;

    for (long x = base->m_threads.size() - 1; x >= 0; x--)
    {
        if (base->m_threads[x]->m_runnable == runnable)
        {
            if (base->m_threads[x]->getEnded() // it has ended
                || !base->m_threads[x]->getStarted())// it has NOT yet started
            {
                base->m_threads[x].reset();
                base->m_threads.erase(base->m_threads.begin() + x);
                base->m_lock->broadcast(__FILE__, __LINE__);
                return 0;
            }
            // if we are here then the thread has started but not ended, so the user must first kill/stop it
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot remove this UltraThread '" + add.str() + "', it is busy running, please stop/kill if first");
            return 1;
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Cannot find the UltraThread '" + add.str() + "' to remove");
    return 0;
}

/**
 * From here on we have the entry points to the thread handler 
 */

int CUltraThreadThreadPthread::start(IRunnable *runnable, void *context)
{
    AUTO_LOCK(m_lock); // this will lock the method
    int r = inner_start(runnable, context);
    if (r != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_start()");
    }
    return r;
}

THREAD_ID CUltraThreadThreadPthread::getCurrentThreadID()
{
    return (THREAD_ID) pthread_self();
}

int CUltraThreadThreadPthread::signal(IRunnable *runnable, int signal)
{
    AUTO_LOCK(m_lock); // this will lock the method
    int r = inner_signal(runnable, signal);
    if (r != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_signal()");
    }
    return r;
}

int CUltraThreadThreadPthread::join(IRunnable *runnable)
{
    AUTO_LOCK(m_lock); // this will lock the method
    int r = inner_join(runnable);
    if (r != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_join()");
    }
    return r;
}

bool CUltraThreadThreadPthread::isBusy(IRunnable *runnable)
{
    AUTO_LOCK(m_lock); // this will lock the method
    bool ret = inner_isBusy(runnable);
    return ret;
}

bool CUltraThreadThreadPthread::hasFinished(IRunnable *runnable)
{
    AUTO_LOCK(m_lock); // this will lock the method
    bool ret = inner_hasFinished(runnable);
    return ret;
}

int CUltraThreadThreadPthread::clearCompleted()
{
    AUTO_LOCK(m_lock); // this will lock the method
    int r = inner_clearCompleted();
    if (r != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_clearCompleted()");
    }
    return r;
}

int CUltraThreadThreadPthread::remove(CUltraThreadThreadPthread *base, IRunnable *runnable)
{
    if (runnable == nullptr)
        return 0;
    AUTO_LOCK(base->m_lock); // this will lock the method
    int r = inner_remove(base, runnable);
    if (r != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_remove()");
    }
    return r;
}

bool CUltraThreadThreadPthread::someThreadsStillRunning()
{
    bool ret = false;
    AUTO_LOCK(m_lock); // this will lock the method
    for (unsigned long x = 0; x < m_threads.size(); x++)
    {
        ret = inner_isBusy(m_threads[x]->m_runnable);
        if (ret)
            break;
    }
    return ret;
}

int CUltraThreadThreadPthread::getMaxThreadCount(int &max)
{
    int ret = 0;
    AUTO_LOCK(m_lock); // this will lock the method
    rlimit s;
    if (getrlimit(RLIMIT_NPROC, &s) != 0)
        ret = 1;
    else
        max = s.rlim_cur;
    return ret;
}

int CUltraThreadThreadPthread::setMaxThreadCount(int max)
{
    int ret = 0;
    AUTO_LOCK(m_lock); // this will lock the method
    rlimit s;
    if (getrlimit(RLIMIT_NPROC, &s) != 0)
        ret = 1;
    s.rlim_cur = max;
    if (ret == 0 && setrlimit(RLIMIT_NPROC, &s) != 0)
        ret = 1;
    return ret;
}

} // namespace __ultra_internal
} // namespace ultra
