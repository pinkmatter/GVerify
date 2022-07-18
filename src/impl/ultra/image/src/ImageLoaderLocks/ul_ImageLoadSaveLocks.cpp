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

#include "ul_ImageLoadSaveLocks.h"

#include  <ul_Logger.h>

namespace ultra
{

namespace __ultra_internal
{

CImageLoadSaveLocks::CImageLoadSaveLocks() :
m_lock(new CThreadLock())
{
}

CImageLoadSaveLocks::~CImageLoadSaveLocks()
{
}

CImageLoadSaveLocks *CImageLoadSaveLocks::getInstance()
{
    static CImageLoadSaveLocks instance;
    return &instance;
}

int CImageLoadSaveLocks::AddLockIfNotExists(int key)
{
    if (key < 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Cannot insert a key with a negative value");
        return 0;
    }
    AUTO_LOCK(m_lock);
    bool found = false;
    for (unsigned long t = 0; t < m_locks.size(); t++)
    {
        if (m_locks[t].k == key)
        {
            found = true;
            break;
        }
    }

    int ret = 0;
    if (!found)
    {
        try
        {
            std::shared_ptr<void> lock(new CThreadLock(), std::default_delete<CThreadLock>());
            m_locks.pushBack(SKeyValue<int, std::shared_ptr<void> >(key, lock));
        }
        catch (const std::exception &e)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, e);
            ret = 1;
        }
        catch (const std::exception *e)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, *e);
            delete e;
            ret = 1;
        }
    }

    return ret;
}

std::shared_ptr<void> CImageLoadSaveLocks::getLock(int key, std::shared_ptr<void> defaultLock)
{
    if (key < 0)
        return defaultLock;

    std::shared_ptr<void> returnLock = defaultLock;
    AUTO_LOCK(m_lock);
    if (AddLockIfNotExists(key) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from AddLockIfNotExists()");
        return returnLock;
    }

    for (unsigned long t = 0; t < m_locks.size(); t++)
    {
        if (m_locks[t].k == key)
        {
            returnLock = m_locks[t].v;
            break;
        }
    }

    return returnLock;
}

} // namespace __ultra_internal

} // namespace ultra
