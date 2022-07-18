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
namespace __ultra_internal
{

class IUltraThreadThread
{
public:
    IUltraThreadThread();
    virtual ~IUltraThreadThread();

    virtual int start(IRunnable *runnable, void *context) = 0;
    virtual int join(IRunnable *runnable) = 0;
    virtual bool isBusy(IRunnable *runnable) = 0;
    virtual bool hasFinished(IRunnable *runnable) = 0;
    virtual int clearCompleted() = 0;
    virtual bool someThreadsStillRunning() = 0;
    virtual int getMaxThreadCount(int &max) = 0;
    virtual int setMaxThreadCount(int max) = 0;
    virtual int signal(IRunnable *runnable, int signal) = 0;
    virtual THREAD_ID getCurrentThreadID() = 0;
};

} // namespace __ultra_internal
} // namespace ultra
