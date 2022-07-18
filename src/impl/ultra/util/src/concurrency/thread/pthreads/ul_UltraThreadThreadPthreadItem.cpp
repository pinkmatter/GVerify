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

namespace ultra
{
namespace __ultra_internal
{

void CUltraThreadThreadPthread::CUltraThreadItem::deleteThread::operator()(void* i)
{
    if (i != nullptr)
    {
        pthread_t* t = (pthread_t*) i;
        delete t;
        t = nullptr;
        i = nullptr;
    }
}

CUltraThreadThreadPthread::CUltraThreadItem::CUltraThreadItem(IRunnable *runnable) :
m_lock(std::make_shared<CThreadLock>()),
m_runnable(runnable),
m_thread(new pthread_t)
{
    m_hasEnded = false;
    m_hasStarted = false;
    m_return = nullptr;
    m_context = nullptr;
}

CUltraThreadThreadPthread::CUltraThreadItem::CUltraThreadItem(const CUltraThreadItem &r)
{
    CException ex(__FILE__, __LINE__, "OOPS unreachable code");
    throw ex;
}

CUltraThreadThreadPthread::CUltraThreadItem::~CUltraThreadItem()
{
    m_runnable = nullptr;
    m_return = nullptr;
    m_context = nullptr;
}

void CUltraThreadThreadPthread::CUltraThreadItem::setStarted(bool started)
{
    AUTO_LOCK(m_lock); // this will lock the method
    m_hasStarted = started;
}

bool CUltraThreadThreadPthread::CUltraThreadItem::getStarted()
{
    AUTO_LOCK(m_lock); // this will lock the method
    return m_hasStarted;
}

void CUltraThreadThreadPthread::CUltraThreadItem::setEnded(bool ended)
{
    AUTO_LOCK(m_lock); // this will lock the method
    m_hasEnded = ended;
}

bool CUltraThreadThreadPthread::CUltraThreadItem::getEnded()
{
    AUTO_LOCK(m_lock); // this will lock the method
    return m_hasEnded;
}

void CUltraThreadThreadPthread::CUltraThreadItem::setBase(CUltraThreadThreadPthread *base)
{
    AUTO_LOCK(m_lock); // this will lock the method
    m_base = base;
}

CUltraThreadThreadPthread *CUltraThreadThreadPthread::CUltraThreadItem::getBase()
{
    AUTO_LOCK(m_lock); // this will lock the method
    return m_base;
}

} // namespace __ultra_internal
} // namespace ultra
