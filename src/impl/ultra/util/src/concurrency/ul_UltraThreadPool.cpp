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

#include "ul_UltraThreadPool.h"
#include "ul_Exception.h"

#include <thread>

namespace ultra
{

#ifdef DEBUG
unsigned long AUltraThreadPool::DEFAULT_THREAD_COUNT = 1;
#else
unsigned long AUltraThreadPool::DEFAULT_THREAD_COUNT = 0;
#endif

unsigned long AUltraThreadPool::getCoreThreadCount()
{
    unsigned long amount = std::thread::hardware_concurrency();
    return amount == 0 ? 1 : amount;
}

unsigned long AUltraThreadPool::getDefaultPoolSize()
{
    if (DEFAULT_THREAD_COUNT != 0)
        return DEFAULT_THREAD_COUNT;
    return getCoreThreadCount();
}

/**
 * If the pool size is set to zero then the CPU thread count is used
 * @param defaultPoolSize
 */
void AUltraThreadPool::setDefaultPoolSize(unsigned long defaultPoolSize)
{
    DEFAULT_THREAD_COUNT = defaultPoolSize;
}

void AUltraThreadPool::joinAllThreads()
{
    if (JoinAllThreads() != 0)
        throw CException(__FILE__, __LINE__, "Failure returned from JoinAllThreads()");
}

} //namespace ultra

