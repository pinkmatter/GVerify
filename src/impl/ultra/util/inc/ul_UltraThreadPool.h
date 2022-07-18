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

#include "ul_UltraThread.h"

namespace ultra
{

class AUltraThreadPool
{
private:
    static unsigned long DEFAULT_THREAD_COUNT;
public:
    virtual void start(IRunnable *runnable, void *context = nullptr) = 0;
    virtual void start(std::shared_ptr<IRunnable> runnable, void *context = nullptr) = 0;
    virtual void start(std::function<void(void *) > fp, void *context = nullptr) = 0;
    virtual void start(std::function<void() > fp) = 0;
    virtual void start(std::function<void(void *, unsigned long) > fp, void *context = nullptr) = 0;
    virtual void start(std::function<void(unsigned long) > fp) = 0;
    virtual int JoinAllThreads() = 0;
    virtual unsigned long getPoolSize() const = 0;
    virtual unsigned long getActiveTaskSize() const = 0;
    virtual unsigned long getWaitingTaskSize() const = 0;

    void joinAllThreads();

    static unsigned long getCoreThreadCount();
    static unsigned long getDefaultPoolSize();
    /**
     * If the pool size is set to zero then the CPU thread count is used
     * @param defaultPoolSize
     */
    static void setDefaultPoolSize(unsigned long defaultPoolSize);
};

} //namespace ultra
