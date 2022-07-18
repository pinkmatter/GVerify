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

#include <ul_Utility.h>

namespace ultra
{
namespace __ultra_internal
{

/**
 * This is class keeps track of GDAL reader/writer.
 * 
 * Gdal CANNOT read and write to datasets at the same time (being the same or
 * different datasets does not matter.
 * 
 * However:
 * multiple writers can be used at any time, 
 * multiple readers can be used at any time.
 * 
 * NOTE: 
 * This is found not to be true, gdal just is not thread safe, the GDAL image
 * loader/saver classes now shares a global mutex, however this is still used 
 * as the gdalwrapper is used for shapefiles. This will still ensure that there
 * are only writers or reader active on shapefiles.
 */
class CGdalState
{
private:
    std::shared_ptr<void> m_lock;
    int m_readers;
    int m_writers;

    int WaitFor(int &emptyRequired, int &addTo);
    int Remove(int &toRemove);
    CGdalState();
public:
    virtual ~CGdalState();

    static CGdalState *getInstance();

    int WaitForReader();
    int WaitForWriter();

    int RemoveReader();
    int RemoveWriter();
};

} // namespace __ultra_internal
} // namespace ultra
