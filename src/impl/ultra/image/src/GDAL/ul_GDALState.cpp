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

#include "ul_GDALState.h"

#include <ul_Logger.h>
#include <stdlib.h>
#include <assert.h>
#include <ul_UltraThread.h>

namespace ultra
{
namespace __ultra_internal
{

CGdalState::CGdalState() :
m_lock(new CThreadLock(), std::default_delete<CThreadLock>())
{
    m_readers = 0;
    m_writers = 0;
}

CGdalState::~CGdalState()
{
}

CGdalState *CGdalState::getInstance()
{
    static CGdalState instance;
    return &instance;
}

int CGdalState::WaitFor(int &emptyRequired, int &addTo)
{
    AUTO_LOCK(m_lock);
    while (emptyRequired != 0)
    {
        if (((CThreadLock*) m_lock.get())->wait(__FILE__, __LINE__) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from wait()");
            return 1;
        }
    }
    addTo++;
    return 0;
}

int CGdalState::Remove(int &toRemove)
{
    AUTO_LOCK(m_lock);
    assert(toRemove > 0);
    toRemove--;
    if (((CThreadLock*) m_lock.get())->broadcast(__FILE__, __LINE__) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from broadcast()");
        return 1;
    }
    return 0;
}

int CGdalState::WaitForReader()
{
    return WaitFor(m_writers, m_readers);
}

int CGdalState::WaitForWriter()
{
    return WaitFor(m_readers, m_writers);
}

int CGdalState::RemoveReader()
{
    return Remove(m_readers);
}

int CGdalState::RemoveWriter()
{
    return Remove(m_writers);
}

} // namespace __ultra_internal
} // namespace ultra
