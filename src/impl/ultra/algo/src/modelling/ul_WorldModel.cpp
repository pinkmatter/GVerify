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

#include "ul_WorldModel.h"
#include "ul_Logger.h"

namespace ultra
{

CWorldModel::CWorldModel(const SSize &size, const SPair<double> &pixelGroundSamplingDistance, const SPair<double> &origin, const std::string &projectionName, int utmZone) :
CMatrix<SPair<double> >()
{
    m_size = size;
    m_projectionName = projectionName;
    m_utmZone = utmZone;
    m_origin = origin;
    m_pixelGroundSamplingDistance = pixelGroundSamplingDistance;
    m_inited = false;
}

CWorldModel::CWorldModel(const CWorldModel &r) :
CMatrix<SPair<double> >(r)
{
    m_projectionName = r.m_projectionName;
    m_utmZone = r.m_utmZone;
    m_size = r.m_size;
    m_pixelGroundSamplingDistance = r.m_pixelGroundSamplingDistance;
    m_origin = r.m_origin;
    m_inited = r.m_inited;
}

CWorldModel &CWorldModel::operator=(const CWorldModel &r)
{
    if (this == &r)
        return *this;
    m_projectionName = r.m_projectionName;
    m_utmZone = r.m_utmZone;
    m_size = r.m_size;
    m_pixelGroundSamplingDistance = r.m_pixelGroundSamplingDistance;
    m_origin = r.m_origin;
    m_inited = r.m_inited;

    return *this;
}

CWorldModel::~CWorldModel()
{
    m_inited = false;
    this->clear();
}

void CWorldModel::CleanWorld()
{
    m_inited = false;
    this->clear();
}

int CWorldModel::Init()
{
    if (!m_inited)
    {
        if (m_size == SSize(0, 0))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "World matrix cannot be of size 0x0");
            return 1;
        }

        this->resize(m_size);
        if (this->getSize() != m_size)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to resize world model matrix");
            return 1;
        }

        for (unsigned long r = 0; r < m_size.row; r++)
        {
            for (unsigned long c = 0; c < m_size.col; c++)
            {
                this->m_mat[r][c] = m_origin;
                this->m_mat[r][c].r += m_pixelGroundSamplingDistance.r*r;
                this->m_mat[r][c].c += m_pixelGroundSamplingDistance.c*c;
            }
        }

        m_inited = true;
    }

    return 0;
}

int CWorldModel::GetWorldMatrix(CMatrix<SPair<double> >&outputWorldMatrix)
{
    if (Init() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
        return 1;
    }

    if (outputWorldMatrix.getSize() != m_size)
    {
        outputWorldMatrix.resize(m_size);
        if (outputWorldMatrix.getSize() != m_size)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to resize output matrix");
            return 1;
        }
    }

    for (unsigned long r = 0; r < m_size.row; r++)
    {
        if (memcpy(&outputWorldMatrix[r][0], &(this->m_mat[r][0]), sizeof (SPair<double>) * m_size.col) != &outputWorldMatrix[r][0])
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to copy world matrix into output matrix");
            return 1;
        }
    }

    return 0;
}

}//namespace ultra
