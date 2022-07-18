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

#include "ul_Int_AtomicType.h"

#include "ul_Logger.h"
#include "ul_UltraThread.h"

namespace ultra
{

IAtomicType::IAtomicType() :
m_lock(new CThreadLock(), std::default_delete<CThreadLock>())
{
}

IAtomicType::~IAtomicType()
{
}

std::shared_ptr<void> IAtomicType::getLock() const
{
    return m_lock;
}

} // namespace ultra
