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

#include "ul_AtomicBool.h"
#include <ul_UltraThread.h>

namespace ultra
{

CAtomicBool::CAtomicBool(bool val) : IAtomicType()
{
    m_val = val;
}

CAtomicBool::CAtomicBool() : IAtomicType()
{
    m_val = true;
}

CAtomicBool::~CAtomicBool()
{
}

void CAtomicBool::set(bool val)
{
    AUTO_LOCK(getLock());
    m_val = val;
}

bool CAtomicBool::get() const
{
    AUTO_LOCK(getLock());
    return m_val;
}

bool CAtomicBool::getAndSet(bool val)
{
    AUTO_LOCK(getLock());
    bool ret = m_val;
    m_val = val;
    return ret;
}

} // namespace ultra
