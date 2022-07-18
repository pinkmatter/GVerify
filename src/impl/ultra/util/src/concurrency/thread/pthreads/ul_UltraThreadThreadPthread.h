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

#include "../ul_UltraThreadThread_Int.h"

namespace ultra
{
namespace __ultra_internal
{

class CUltraThreadThreadPthread : public IUltraThreadThread
{
private:

    struct CUltraThreadItem
    {
    private:
        bool m_hasEnded;
        bool m_hasStarted;
        CUltraThreadThreadPthread *m_base;

        std::shared_ptr<CThreadLock> m_lock;
        CUltraThreadItem(const CUltraThreadItem & r);

        struct deleteThread
        {
            void operator()(void* i);
        };
    public:
        IRunnable* m_runnable;
        std::unique_ptr<void, deleteThread> m_thread;
        void* m_return;
        void* m_context;

        CUltraThreadItem(IRunnable *runnable);
        virtual ~CUltraThreadItem();

        void setStarted(bool started);
        bool getStarted();
        void setEnded(bool ended);
        bool getEnded();
        void setBase(CUltraThreadThreadPthread *base);
        CUltraThreadThreadPthread *getBase();
    };

private:
    std::vector<std::shared_ptr<CUltraThreadItem> > m_threads;
    std::shared_ptr<CThreadLock> m_lock;

    bool alreadyExists(IRunnable *runnable);
    int inner_start(IRunnable *runnable, void *context);
    int inner_add(IRunnable *runnable);
    int inner_run(IRunnable *runnable, void *context);
    int inner_join(IRunnable *runnable);
    int findId(IRunnable *runnable, unsigned long &id);
    static void* innerThreadRunner(void *context);
    bool inner_isBusy(IRunnable *runnable);
    bool inner_hasFinished(IRunnable *runnable);
    int inner_clearCompleted();
    static int inner_remove(CUltraThreadThreadPthread *base, IRunnable *runnable);
    int inner_signal(IRunnable *runnable, int signal);
    void *getThread(IRunnable *runnable);
    static int remove(CUltraThreadThreadPthread *base, IRunnable *runnable);
public:
    CUltraThreadThreadPthread();
    virtual ~CUltraThreadThreadPthread();

    virtual int start(IRunnable *runnable, void *context) override;
    virtual int join(IRunnable *runnable) override;
    virtual bool isBusy(IRunnable *runnable) override;
    virtual bool hasFinished(IRunnable *runnable) override;
    virtual int clearCompleted() override;
    virtual bool someThreadsStillRunning() override;
    virtual int getMaxThreadCount(int &max) override;
    virtual int setMaxThreadCount(int max) override;
    virtual int signal(IRunnable *runnable, int signal) override;
    virtual THREAD_ID getCurrentThreadID() override;
};

} // namespace __ultra_internal
} // namespace ultra
