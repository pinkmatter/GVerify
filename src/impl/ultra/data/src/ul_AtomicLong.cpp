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

#include "ul_AtomicLong.h"
#include <ul_UltraThread.h>

namespace ultra
{

CAtomicLong::CAtomicLong(long val) : IAtomicType()
{
    m_val = val;
}

CAtomicLong::CAtomicLong() : IAtomicType()
{
    m_val = 0;
}

CAtomicLong::~CAtomicLong()
{
}

void CAtomicLong::set(long val)
{
    AUTO_LOCK(getLock());
    m_val = val;
}

void CAtomicLong::inc()
{
    AUTO_LOCK(getLock());
    m_val++;
}

void CAtomicLong::mul(long val)
{
    AUTO_LOCK(getLock());
    m_val *= val;
}

void CAtomicLong::div(long val)
{
    AUTO_LOCK(getLock());
    m_val /= val;
}

void CAtomicLong::add(long val)
{
    AUTO_LOCK(getLock());
    m_val += val;
}

void CAtomicLong::sub(long val)
{
    AUTO_LOCK(getLock());
    m_val -= val;
}

void CAtomicLong::dec()
{
    AUTO_LOCK(getLock());
    m_val--;
}

long CAtomicLong::get() const
{
    AUTO_LOCK(getLock());
    return m_val;
}

long CAtomicLong::getAndSet(long val)
{
    AUTO_LOCK(getLock());
    long ret = m_val;
    m_val = val;
    return ret;
}

long CAtomicLong::getAndInc()
{
    AUTO_LOCK(getLock());
    long ret = m_val;
    m_val++;
    return ret;
}

long CAtomicLong::getAndDec()
{
    AUTO_LOCK(getLock());
    long ret = m_val;
    m_val--;
    return ret;
}

long CAtomicLong::incAndGet()
{
    AUTO_LOCK(getLock());
    return (++m_val);
}

long CAtomicLong::decAndGet()
{
    AUTO_LOCK(getLock());
    return (--m_val);
}

long CAtomicLong::addAndGet(long val)
{
    AUTO_LOCK(getLock());
    m_val += val;
    return m_val;
}

long CAtomicLong::getAndAdd(long val)
{
    AUTO_LOCK(getLock());
    long ret = m_val;
    m_val += val;
    return ret;
}

} // namespace ultra
