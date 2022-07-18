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

#include "ul_UltraThreadPool.h"
#include <deque>

namespace ultra
{

class CUltraThreadFixedPool : public AUltraThreadPool
{
private:

    struct SJob
    {
        std::function<void() > jobPtr1;
        std::function<void(void *) > jobPtr2;
        IRunnable *jobPtr3;
        std::function<void(unsigned long) > jobPtr4;
        std::function<void(void *, unsigned long) > jobPtr5;
        std::shared_ptr<IRunnable> jobPtr6;
        void *context;
        SJob();
        SJob(std::function<void() > jobPtr1);
        SJob(std::function<void(void *) > jobPtr2, void *context);
        SJob(IRunnable *jobPtr3, void *context);
        SJob(std::function<void(unsigned long) > jobPtr4);
        SJob(std::function<void(void *, unsigned long) > jobPtr5, void *context);
        SJob(std::shared_ptr<IRunnable> jobPtr6, void *context);
        bool canRun() const;
        void execute(unsigned long threadIds);
    };
private:
    std::shared_ptr<void> m_pool;
    std::function<void(unsigned long) > m_startUp;
    std::function<void(unsigned long) > m_tearDown;
    std::shared_ptr<CThreadLock> m_lock;
    std::shared_ptr<CThreadLock> m_startUpLock;
    std::vector<bool> m_jobRunning;
    bool m_running;
    unsigned long m_startedCounter;
    std::deque<SJob> m_jobs;

    void addJob(const SJob &job);
public:
    /**
     * This calls CUltraThreadFixedPool(nullptr, nullptr, poolSize);
     * @param poolSize
     */
    CUltraThreadFixedPool(unsigned long poolSize = AUltraThreadPool::getDefaultPoolSize());
    /**
     * This will start a thread pool if a fixed number of threads, that are always 
     * running and will execute tasks in a FIFO manor.
     * 
     * The startUp and tearDown are used to control actions/tasks before and/or 
     * after a thread worker. Meaning startUp/tearDown if be called ONCE for each thread 
     * worker.
     * @param startUp a function that receives a threadID. (0 <= threadID < poolSize)
     * @param tearDown a function that receives a threadID. (0 <= threadID < poolSize)
     * @param poolSize
     */
    CUltraThreadFixedPool(std::function<void(unsigned long) > startUp,
                          std::function<void(unsigned long) > tearDown,
                          unsigned long poolSize = AUltraThreadPool::getDefaultPoolSize());
    virtual ~CUltraThreadFixedPool();

    void start(IRunnable *runnable, void *context = nullptr) override;
    void start(std::shared_ptr<IRunnable> runnable, void *context = nullptr) override;
    void start(std::function<void(void *) > fp, void *context = nullptr) override;
    void start(std::function<void() > fp) override;
    void start(std::function<void(void *, unsigned long) > fp, void *context = nullptr) override;
    void start(std::function<void(unsigned long) > fp) override;
    int JoinAllThreads() override;
    unsigned long getPoolSize() const override;
    unsigned long getActiveTaskSize() const override;
    unsigned long getWaitingTaskSize() const override;
};

} // namespace ultra
