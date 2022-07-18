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

#include <ul_Matrix.h>
#include <ul_Pair.h>

namespace ultra
{

class CWorldModel : CMatrix<SPair<double> >
{
private:

protected:
    std::string m_projectionName;
    int m_utmZone;
    SSize m_size;
    SPair<double> m_pixelGroundSamplingDistance;
    SPair<double> m_origin;
    bool m_inited;
public:

    CWorldModel(const SSize &size, const SPair<double> &pixelGroundSamplingDistance, const SPair<double> &origin, const std::string &projectionName = "", int utmZone = 0);
    CWorldModel(const CWorldModel &r);
    CWorldModel &operator=(const CWorldModel &r);
    virtual ~CWorldModel();
    int Init();
    void CleanWorld();
    int GetWorldMatrix(CMatrix<SPair<double> >&outputWorldMatrix);
};

}//namespace ultra
