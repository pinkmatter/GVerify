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

#include "../ul_UltraThreadLock_int.h"

#include <time.h>
#include <memory>

namespace ultra
{
class CLogger;
namespace __ultra_internal
{

class CUltraThreadLockPthread : public IUltraThreadLock
{
public:

    struct deleteMutex
    {
        void operator()(void* i);
    };

    struct deleteMutexAttr
    {
        void operator()(void* i);
    };

    struct deleteCond
    {
        void operator()(void* i);
    };


private:

    // the order which the variables are declared here is important
    std::unique_ptr<void, CUltraThreadLockPthread::deleteCond> m_cond;
    std::unique_ptr<void, CUltraThreadLockPthread::deleteMutexAttr> m_mutex_attr;
    std::unique_ptr<void, CUltraThreadLockPthread::deleteMutex > m_mutex;

public:
    CUltraThreadLockPthread();
    virtual ~CUltraThreadLockPthread();

    virtual int lock() const;
    virtual int unlock() const;
    virtual int wait() const;
    virtual int timedWait(long int sec, long int nsec) const;
    virtual int signal() const;
    virtual int broadcast() const;
};

} // namespace __ultra_internal
} // ultra

