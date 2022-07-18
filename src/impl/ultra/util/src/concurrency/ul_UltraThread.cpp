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

#include <memory>

#include "ul_UltraThread.h"
#include "ul_Logger.h"

#include "thread/ul_UltraThreadThread_Int.h"

#include "thread/pthreads/ul_UltraThreadThreadPthread.h"

namespace ultra
{

typedef __ultra_internal::IUltraThreadThread ULTRA_THREAD_BASETYPE;
typedef ULTRA_THREAD_BASETYPE* WRAPPER_THREAD_POINTER;

typedef __ultra_internal::CUltraThreadThreadPthread WRAPPER_THREAD_CLASS_TO_CREATE;

CUltraThread::CUltraThread()
{
    m_controller.reset(new WRAPPER_THREAD_CLASS_TO_CREATE(), std::default_delete<WRAPPER_THREAD_CLASS_TO_CREATE>());
    m_controllerPtr = m_controller.get();
}

CUltraThread::~CUltraThread()
{
}

CUltraThread* CUltraThread::getInstance()
{
    static CUltraThread instance;
    return &instance;
}

bool CUltraThread::isBusy(IRunnable *runnable)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    return controller->isBusy(runnable);
}

bool CUltraThread::hasFinished(IRunnable *runnable)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    return controller->hasFinished(runnable);
}

int CUltraThread::start(IRunnable *runnable, void *context)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->start(runnable, context) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CThreadController::start()");
        return 1;
    }
    return 0;
}

int CUltraThread::join(IRunnable *runnable)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->join(runnable) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CThreadController::join()");
        return 1;
    }
    return 0;
}

int CUltraThread::clearCompleted()
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->clearCompleted() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CThreadController::clearCompleted()");
        return 1;
    }
    return 0;
}

bool CUltraThread::someThreadsStillRunning()
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->someThreadsStillRunning() != 0)
    {
        return true;
    }
    return 0;
}

int CUltraThread::getMaxThreadCount(int &max)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->getMaxThreadCount(max) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CThreadController::getMaxThreadCount()");
        return 1;
    }
    return 0;
}

int CUltraThread::setMaxThreadCount(int max)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->setMaxThreadCount(max) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CThreadController::setMaxThreadCount()");
        return 1;
    }
    return 0;
}

int CUltraThread::signal(IRunnable *runnable, int signal)
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    if (controller->signal(runnable, signal) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CThreadController::signal()");
        return 1;
    }
    return 0;
}

THREAD_ID CUltraThread::getCurrentThreadID()
{
    WRAPPER_THREAD_POINTER controller = (WRAPPER_THREAD_POINTER) m_controllerPtr;
    return controller->getCurrentThreadID();
}

} //namespace ultra

