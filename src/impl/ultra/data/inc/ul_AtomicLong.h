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

#include "ul_Int_AtomicType.h"

namespace ultra
{

class CAtomicLong : public IAtomicType
{
private:
    long m_val;
public:

    CAtomicLong(long val);
    CAtomicLong();
    virtual ~CAtomicLong();

    void set(long val);
    void inc();
    void dec();
    void mul(long val);
    void div(long val);
    void add(long val);
    void sub(long val);
    long get() const;
    long getAndSet(long val);
    long getAndInc();
    long getAndDec();
    long incAndGet();
    long decAndGet();
    long addAndGet(long val);
    long getAndAdd(long val);
};

} // namespace ultra
