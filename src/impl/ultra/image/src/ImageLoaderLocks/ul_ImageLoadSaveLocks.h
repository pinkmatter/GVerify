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

#include <ul_KeyValue.h>
#include <ul_Vector.h>
#include <ul_UltraThread.h>

namespace ultra
{

namespace __ultra_internal
{

class CImageLoadSaveLocks
{
private:
    std::shared_ptr<CThreadLock> m_lock;

    CVector<SKeyValue<int, std::shared_ptr<void> > > m_locks;

    CImageLoadSaveLocks();
    int AddLockIfNotExists(int key);
public:
    virtual ~CImageLoadSaveLocks();
    static CImageLoadSaveLocks *getInstance();
    std::shared_ptr<void> getLock(int key, std::shared_ptr<void> defaultLock);
};
} // namespace __ultra_internal

} // namespace ultra
