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

#include "ul_UltraThreadFixedPool.h"
#include "ul_Exception.h"
#include <ul_Logger.h>
#include <ul_Utility.h>

namespace ultra
{

namespace
{

class CUltraThreadPool
{
private:

    class CThreadRunner : public IRunnable
    {
    private:
        bool m_done;
        std::shared_ptr<CThreadLock> m_lock;
        IRunnable *m_thread;
        std::shared_ptr<IRunnable> m_threadSp;
        void *m_context;
        unsigned long *m_currentlyRunning;
    public:
        CThreadRunner(IRunnable *thread, void *context, std::shared_ptr<CThreadLock> lock, unsigned long *currentlyRunning);
        CThreadRunner(std::shared_ptr<IRunnable> thread, void *context, std::shared_ptr<CThreadLock> lock, unsigned long *currentlyRunning);
        CThreadRunner(const CUltraThreadPool::CThreadRunner &r);
        CThreadRunner(CUltraThreadPool::CThreadRunner &&r);
        virtual ~CThreadRunner();

        CUltraThreadPool::CThreadRunner &operator=(const CUltraThreadPool::CThreadRunner &r);
        CUltraThreadPool::CThreadRunner &operator=(CUltraThreadPool::CThreadRunner &&r);
        virtual void run(void *context = nullptr) override;

        bool isDone() const;
    };

    class CBackGroundThreadSpawner : public IRunnable
    {
    private:
        unsigned long m_poolSize;
        unsigned long *m_currentlyRunning;
        std::shared_ptr<CThreadLock> m_lock;
        std::deque<std::shared_ptr<CUltraThreadPool::CThreadRunner> > *m_threadsToRun;
        std::vector<std::shared_ptr<CUltraThreadPool::CThreadRunner> > *m_threadsBusy;
        bool m_running;

        int StartANewThread();
    public:
        CBackGroundThreadSpawner(unsigned long poolSize, std::shared_ptr<CThreadLock> lock,
                                 std::deque<std::shared_ptr<CUltraThreadPool::CThreadRunner> > *threadsToRun,
                                 std::vector<std::shared_ptr<CUltraThreadPool::CThreadRunner> > *threadsBusy,
                                 unsigned long *currentlyRunning);
        virtual ~CBackGroundThreadSpawner();
        virtual void run(void *context = nullptr) override;
        void stop();
    };

    class CExecuteThreadFunction : public IRunnable
    {
    private:
        std::function<void(void *) > m_fp;
    public:
        CExecuteThreadFunction(std::function<void(void *) > fp);
        virtual ~CExecuteThreadFunction();
        virtual void run(void *context = nullptr) override;
    };

private:

    unsigned long m_poolSize;
    unsigned long m_currentlyRunning;
    std::shared_ptr<CThreadLock> m_lock;
    std::deque<std::shared_ptr<CUltraThreadPool::CThreadRunner> > m_threadsToRun;
    std::vector<std::shared_ptr<CUltraThreadPool::CThreadRunner> > m_threadsBusy;
    std::unique_ptr<CBackGroundThreadSpawner> m_backGroundThreadSpawner;

public:
    CUltraThreadPool(unsigned long poolSize);
    virtual ~CUltraThreadPool();

    void start(IRunnable *runnable, void *context = nullptr);
    void start(std::function<void(void *) > fp, void *context = nullptr);
    void start(std::function<void() > fp);
    int JoinAllThreads();
    unsigned long getPoolSize() const;
};

CUltraThreadPool::CExecuteThreadFunction::CExecuteThreadFunction(std::function<void(void *) > fp) :
m_fp(fp)
{
}

CUltraThreadPool::CExecuteThreadFunction::~CExecuteThreadFunction()
{
}

void CUltraThreadPool::CExecuteThreadFunction::run(void *context)
{
    m_fp(context);
}

CUltraThreadPool::CThreadRunner::CThreadRunner(IRunnable *thread, void *context, std::shared_ptr<CThreadLock> lock, unsigned long *currentlyRunning) :
m_done(false),
m_thread(thread),
m_threadSp(nullptr),
m_context(context),
m_lock(lock),
m_currentlyRunning(currentlyRunning)
{
}

CUltraThreadPool::CThreadRunner::CThreadRunner(std::shared_ptr<IRunnable> thread, void *context, std::shared_ptr<CThreadLock> lock, unsigned long *currentlyRunning) :
m_done(false),
m_thread(nullptr),
m_threadSp(thread),
m_context(context),
m_lock(lock),
m_currentlyRunning(currentlyRunning)
{
}

CUltraThreadPool::CThreadRunner::CThreadRunner(const CUltraThreadPool::CThreadRunner &r) :
m_done(r.m_done),
m_thread(r.m_thread),
m_threadSp(r.m_threadSp),
m_context(r.m_context),
m_lock(r.m_lock),
m_currentlyRunning(r.m_currentlyRunning)
{
}

CUltraThreadPool::CThreadRunner::CThreadRunner(CUltraThreadPool::CThreadRunner &&r) :
m_done(std::move(r.m_done)),
m_thread(std::move(r.m_thread)),
m_threadSp(std::move(r.m_threadSp)),
m_context(std::move(r.m_context)),
m_lock(std::move(r.m_lock)),
m_currentlyRunning(std::move(r.m_currentlyRunning))
{
}

CUltraThreadPool::CThreadRunner::~CThreadRunner()
{
    m_thread = nullptr;
    m_threadSp = nullptr;
    m_context = nullptr;
    m_currentlyRunning = nullptr;
}

CUltraThreadPool::CThreadRunner &CUltraThreadPool::CThreadRunner::operator=(const CUltraThreadPool::CThreadRunner &r)
{
    if (this == &r)
        return *this;
    m_done = r.m_done;
    m_thread = r.m_thread;
    m_threadSp = r.m_threadSp;
    m_context = r.m_context;
    m_lock = r.m_lock;
    m_currentlyRunning = r.m_currentlyRunning;
    return *this;
}

CUltraThreadPool::CThreadRunner &CUltraThreadPool::CThreadRunner::operator=(CUltraThreadPool::CThreadRunner &&r)
{
    if (this == &r)
        return *this;
    m_done = std::move(r.m_done);
    m_thread = std::move(r.m_thread);
    m_threadSp = std::move(r.m_threadSp);
    m_context = std::move(r.m_context);
    m_lock = std::move(r.m_lock);
    m_currentlyRunning = std::move(r.m_currentlyRunning);
    return *this;
}

void CUltraThreadPool::CThreadRunner::run(void *context)
{
    IRunnable *th = m_thread != nullptr ? m_thread : m_threadSp.get();
    if (CUltraThread::getInstance()->start(th, m_context) == 0)
    {
        if (CUltraThread::getInstance()->join(th) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to join thread '" + toHexString(m_thread) + "'");
        }
    }
    else
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CUltraThread::addAndRun()");
    }

    {
        AUTO_LOCK(m_lock);
        m_done = true;
        (*m_currentlyRunning)--;
        m_lock->broadcast(__FILE__, __LINE__);
    }
}

bool CUltraThreadPool::CThreadRunner::isDone() const
{
    return m_done;
}

///////////////////////////
///////////////////////////
///////////////////////////

CUltraThreadPool::CBackGroundThreadSpawner::CBackGroundThreadSpawner(unsigned long poolSize, std::shared_ptr<CThreadLock> lock,
                                                                     std::deque<std::shared_ptr<CUltraThreadPool::CThreadRunner> > *threadsToRun,
                                                                     std::vector<std::shared_ptr<CUltraThreadPool::CThreadRunner> > *threadsBusy,
                                                                     unsigned long *currentlyRunning) :
m_lock(lock)
{
    m_poolSize = poolSize;
    m_threadsToRun = threadsToRun;
    m_threadsBusy = threadsBusy;
    m_currentlyRunning = currentlyRunning;
    m_running = true;
}

CUltraThreadPool::CBackGroundThreadSpawner::~CBackGroundThreadSpawner()
{
    stop();
}

int CUltraThreadPool::CBackGroundThreadSpawner::StartANewThread()
{
    long size = m_threadsBusy->size();
    size -= 1;
    for (long t = size; t >= 0; t--)
    {
        if ((*m_threadsBusy)[t]->isDone())
        {
            int counter = 0;
            while (!CUltraThread::getInstance()->hasFinished((*m_threadsBusy)[t].get()))
            {
                counter++;
                if (counter > 60 * 10000)
                {
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "POSSIBLE DEADLOCK DETECTED");
                    break;
                }
                usleep(1000);
            }
            m_threadsBusy->erase(m_threadsBusy->begin() + t);
            m_lock->broadcast(__FILE__, __LINE__);
        }
    }

    while (*m_currentlyRunning < m_poolSize && m_threadsToRun->size() > 0)
    {
        std::shared_ptr<CUltraThreadPool::CThreadRunner> th = m_threadsToRun->at(0);

        if (CUltraThread::getInstance()->start(th.get(), nullptr) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failure returned from CUltraThread::start('" + toHexString(th) + "')");
            return 1;
        }
        else
        {
            (*m_currentlyRunning)++;
            m_threadsBusy->push_back(th);
            m_threadsToRun->pop_front();
        }
    }

    return 0;
}

void CUltraThreadPool::CBackGroundThreadSpawner::run(void *context)
{
    AUTO_LOCK(m_lock);
    if (m_lock->broadcast(__FILE__, __LINE__) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failed to start thread pool");
    }
    while (true)
    {
        if (!m_running)
            break; //while loop

        if (StartANewThread() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failure returned from StartANewThread()");
        }

        if (m_lock->wait(__FILE__, __LINE__) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failed to wait on a mutex");
        }
    }
}

void CUltraThreadPool::CBackGroundThreadSpawner::stop()
{
    AUTO_LOCK(m_lock);
    m_running = false;
    m_lock->broadcast(__FILE__, __LINE__);
}

///////////////////////////
///////////////////////////
///////////////////////////

CUltraThreadPool::CUltraThreadPool(unsigned long poolSize) :
m_lock(std::make_shared<CThreadLock>())
{
    m_currentlyRunning = 0;
    m_poolSize = poolSize;
    if (m_poolSize <= 0)
        m_poolSize = 1;

    m_threadsToRun.clear();
    m_threadsBusy.clear();

    AUTO_LOCK(m_lock);
    m_backGroundThreadSpawner.reset(new CBackGroundThreadSpawner(m_poolSize, m_lock, &m_threadsToRun, &m_threadsBusy, &m_currentlyRunning));
    if (CUltraThread::getInstance()->start(m_backGroundThreadSpawner.get(), nullptr) != 0)
    {
        throw CException(__FILE__, __LINE__, "Failure returned from CUltraThread::start()");
    }
    // need to make sure that the thread pool is ready to start
    m_lock->wait(__FILE__, __LINE__);
}

CUltraThreadPool::~CUltraThreadPool()
{
    JoinAllThreads();
    m_backGroundThreadSpawner->stop();

    while (!CUltraThread::getInstance()->hasFinished(m_backGroundThreadSpawner.get()))
    {
        usleep(10000);
    }


    AUTO_LOCK(m_lock);
    m_threadsToRun.clear();
    m_threadsBusy.clear();
}

unsigned long CUltraThreadPool::getPoolSize() const
{
    return (unsigned long) m_poolSize;
}

int CUltraThreadPool::JoinAllThreads()
{
    AUTO_LOCK(m_lock);
    bool done;
    do
    {
        done = (m_threadsBusy.size() == 0) && (m_threadsToRun.size() == 0);
        if (!done)
            m_lock->wait(__FILE__, __LINE__);
    }
    while (!done);
    return 0;
}

void CUltraThreadPool::start(IRunnable *runnable, void *context)
{
    std::shared_ptr<CUltraThreadPool::CThreadRunner> aThread = std::make_shared<CUltraThreadPool::CThreadRunner>(runnable, context, m_lock, &m_currentlyRunning);
    AUTO_LOCK(m_lock);
    m_threadsToRun.push_back(aThread);
    m_lock->broadcast(__FILE__, __LINE__);
}

void CUltraThreadPool::start(std::function<void(void *) > fp, void *context)
{
    std::shared_ptr<IRunnable> runnable = std::make_shared<CExecuteThreadFunction>(fp);
    std::shared_ptr<CUltraThreadPool::CThreadRunner> aThread = std::make_shared<CUltraThreadPool::CThreadRunner>(runnable, context, m_lock, &m_currentlyRunning);
    AUTO_LOCK(m_lock);
    m_threadsToRun.push_back(aThread);
    m_lock->broadcast(__FILE__, __LINE__);
}

void CUltraThreadPool::start(std::function<void() > fp)
{
    start([fp](void *dummy)->void
    {
        fp();
    });
}

} // namespace 

CUltraThreadFixedPool::SJob::SJob() :
jobPtr1(nullptr),
jobPtr2(nullptr),
jobPtr3(nullptr),
jobPtr4(nullptr),
jobPtr5(nullptr),
jobPtr6(nullptr),
context(nullptr)
{
}

CUltraThreadFixedPool::SJob::SJob(std::function<void() > jobPtr1) :
jobPtr1(jobPtr1),
jobPtr2(nullptr),
jobPtr3(nullptr),
jobPtr4(nullptr),
jobPtr5(nullptr),
jobPtr6(nullptr),
context(nullptr)
{
}

CUltraThreadFixedPool::SJob::SJob(std::function<void(void *) > jobPtr2, void *context) :
jobPtr1(nullptr),
jobPtr2(jobPtr2),
jobPtr3(nullptr),
jobPtr4(nullptr),
jobPtr5(nullptr),
jobPtr6(nullptr),
context(context)
{
}

CUltraThreadFixedPool::SJob::SJob(IRunnable *jobPtr3, void *context) :
jobPtr1(nullptr),
jobPtr2(nullptr),
jobPtr3(jobPtr3),
jobPtr4(nullptr),
jobPtr5(nullptr),
jobPtr6(nullptr),
context(context)
{
}

CUltraThreadFixedPool::SJob::SJob(std::function<void(unsigned long) > jobPtr4) :
jobPtr1(nullptr),
jobPtr2(nullptr),
jobPtr3(nullptr),
jobPtr4(jobPtr4),
jobPtr5(nullptr),
jobPtr6(nullptr),
context(nullptr)
{
}

CUltraThreadFixedPool::SJob::SJob(std::function<void(void *, unsigned long) > jobPtr5, void *context) :
jobPtr1(nullptr),
jobPtr2(nullptr),
jobPtr3(nullptr),
jobPtr4(nullptr),
jobPtr5(jobPtr5),
jobPtr6(nullptr),
context(context)
{
}

CUltraThreadFixedPool::SJob::SJob(std::shared_ptr<IRunnable> jobPtr6, void *context) :
jobPtr1(nullptr),
jobPtr2(nullptr),
jobPtr3(nullptr),
jobPtr4(nullptr),
jobPtr5(nullptr),
jobPtr6(jobPtr6),
context(context)
{
}

bool CUltraThreadFixedPool::SJob::canRun() const
{
    return jobPtr1 || jobPtr2 || jobPtr3 != nullptr || jobPtr4 || jobPtr5 || jobPtr6;
}

void CUltraThreadFixedPool::SJob::execute(unsigned long threadId)
{
    if (jobPtr1)
        jobPtr1();
    else if (jobPtr2)
        jobPtr2(context);
    else if (jobPtr3 != nullptr)
        jobPtr3->run(context);
    else if (jobPtr4)
        jobPtr4(threadId);
    else if (jobPtr5)
        jobPtr5(context, threadId);
    else if (jobPtr6)
        jobPtr6->run(context);
    else
        throw CException(__FILE__, __LINE__, "Nothing to execute");
}

CUltraThreadFixedPool::CUltraThreadFixedPool(unsigned long poolSize) :
CUltraThreadFixedPool(nullptr, nullptr, poolSize)
{
}

CUltraThreadFixedPool::CUltraThreadFixedPool(std::function<void(unsigned long) > startUp, std::function<void(unsigned long) > tearDown, unsigned long poolSize) :
AUltraThreadPool(),
m_pool(new CUltraThreadPool(poolSize), std::default_delete<CUltraThreadPool>()),
m_startUp(startUp),
m_tearDown(tearDown),
m_lock(std::make_shared<CThreadLock>()),
m_startUpLock(std::make_shared<CThreadLock>()),
m_jobRunning(poolSize),
m_running(true),
m_startedCounter(0)
{
    CUltraThreadPool *pool = (CUltraThreadPool *) m_pool.get();
    for (unsigned long threadId = 0; threadId < poolSize; threadId++)
    {
        m_jobRunning[threadId] = false;
        pool->start([this, threadId]()->void
        {
            bool threadInError = false;
            std::string threadInErrorMessage1;
            std::string threadInErrorMessage2;
            try
            {
                    if (m_startUp)
                    m_startUp(threadId);
                }
            catch (const std::exception &e)
            {
                    threadInError = true;
                    threadInErrorMessage1 = e.what();
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, e);
            }

            {
                    AUTO_LOCK(m_startUpLock);
                    m_startedCounter++;
                    m_startUpLock->broadcast(__FILE__, __LINE__);
            }

            while (true)
            {
                    SJob job;
                {
                    AUTO_LOCK(m_lock);
                    if (!m_running)
                    break;
                    if (m_jobs.size())
                        {
                    job = m_jobs.at(0);
                    m_jobs.pop_front();
                    m_jobRunning[threadId] = true;
                        }
                        else
                        {
                    m_lock->wait(__FILE__, __LINE__);
                        }
                }
                    if (job.canRun())
                {
                    try
                    {
                    job.execute(threadId);
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
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Caught an unknown exception");
                    }
                }
                {
                    AUTO_LOCK(m_lock);
                    m_jobRunning[threadId] = false;
                    m_lock->broadcast(__FILE__, __LINE__);
                }
            }
            try
            {
                    if (m_tearDown)
                    m_tearDown(threadId);
                }
            catch (const std::exception &e)
            {
                    threadInError = true;
                    threadInErrorMessage2 = e.what();
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, e);
            }

            if (threadInError)
            {
                    std::string message = "";
                    if (threadInErrorMessage1 != "")
                    message += "(Startup error): " + threadInErrorMessage1;
                    if (threadInErrorMessage2 != "")
                    message += "(Teardown error): " + threadInErrorMessage2;

                    if (message == "")
                    message = "Something went wrong during worker thread startup and/or teardown";
                    throw CException(__FILE__, __LINE__, message);
                        }
        });
    }
    while (true)
    {
        AUTO_LOCK(m_startUpLock);
        if (m_startedCounter < poolSize)
            m_startUpLock->wait(__FILE__, __LINE__);
        else
            break;
    }
}

CUltraThreadFixedPool::~CUltraThreadFixedPool()
{
    JoinAllThreads();
    {
        AUTO_LOCK(m_lock);
        m_running = false;
        m_lock->broadcast(__FILE__, __LINE__);
    }
    CUltraThreadPool *pool = (CUltraThreadPool *) m_pool.get();
    pool->JoinAllThreads();
}

int CUltraThreadFixedPool::JoinAllThreads()
{
    do
    {
        AUTO_LOCK(m_lock);
        bool busy = false;
        for (unsigned long t = 0; t < m_jobRunning.size(); t++)
        {
            if (m_jobRunning[t])
                busy = true;
        }
        if (busy || m_jobs.size())
            m_lock->wait(__FILE__, __LINE__);
        else
            break;
    }
    while (true);
    return 0;
}

unsigned long CUltraThreadFixedPool::getPoolSize() const
{
    CUltraThreadPool *pool = (CUltraThreadPool *) m_pool.get();
    return pool->getPoolSize();
}

unsigned long CUltraThreadFixedPool::getActiveTaskSize() const
{
    AUTO_LOCK(m_lock);
    unsigned long runningTasks = 0;
    for (auto running : m_jobRunning)
    {
        if (running)
            runningTasks++;
    }
    return runningTasks;
}

unsigned long CUltraThreadFixedPool::getWaitingTaskSize() const
{
    AUTO_LOCK(m_lock);
    return m_jobs.size();
}

void CUltraThreadFixedPool::addJob(const SJob &job)
{
    AUTO_LOCK(m_lock);
    m_jobs.push_back(job);
    m_lock->broadcast(__FILE__, __LINE__);
}

void CUltraThreadFixedPool::start(std::function<void() > fp)
{
    SJob job(fp);
    addJob(job);
}

void CUltraThreadFixedPool::start(IRunnable *runnable, void *context)
{
    SJob job(runnable, context);
    addJob(job);
}

void CUltraThreadFixedPool::start(std::shared_ptr<IRunnable> runnable, void *context)
{
    SJob job(runnable, context);
    addJob(job);
}

void CUltraThreadFixedPool::start(std::function<void(void *) > fp, void *context)
{
    SJob job(fp, context);
    addJob(job);
}

void CUltraThreadFixedPool::start(std::function<void(void *, unsigned long) > fp, void *context)
{
    SJob job(fp, context);
    addJob(job);
}

void CUltraThreadFixedPool::start(std::function<void(unsigned long) > fp)
{
    SJob job(fp);
    addJob(job);
}

} // namespace ultra
