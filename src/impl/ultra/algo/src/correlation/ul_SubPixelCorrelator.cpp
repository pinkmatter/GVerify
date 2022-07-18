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

#include "ul_SubPixelCorrelator.h"

namespace ultra
{

namespace __ultra_internal
{

ICorrelateInterface::ICorrelateInterface()
{
}

ICorrelateInterface::~ICorrelateInterface()
{
}

CCorrelatePhaseImpl::CCorrelatePhaseImpl(const SPair<double> &templateImageSizeHalf) :
ICorrelateInterface(),
m_templateImageSizeHalf(templateImageSizeHalf),
m_size3x3(3)
{
}

CCorrelatePhaseImpl::~CCorrelatePhaseImpl()
{
}

int CCorrelatePhaseImpl::correlate(const CMatrix<double> &scratchImage,
                                   const SSize &maxPeakLoc, double maxPeakVal,
                                   const double &corrThreshold, bool &success,
                                   SPair<double> &templateToInputOffset, EdgeType edgeType)
{
    templateToInputOffset = SPair<double>(maxPeakLoc) - m_templateImageSizeHalf;

    m_neighbors = scratchImage.getSubMatrix(maxPeakLoc - 1, m_size3x3);

    if (m_neighbors[0][1] > m_neighbors[2][1])
    {
        templateToInputOffset.r -= m_neighbors[0][1] / (m_neighbors[0][1] + maxPeakVal);
    }
    else
    {
        templateToInputOffset.r += m_neighbors[2][1] / (m_neighbors[2][1] + maxPeakVal);
    }

    if (m_neighbors[1][0] > m_neighbors[1][2])
    {
        templateToInputOffset.c -= m_neighbors[1][0] / (m_neighbors[1][0] + maxPeakVal);
    }
    else
    {
        templateToInputOffset.c += m_neighbors[1][2] / (m_neighbors[1][2] + maxPeakVal);
    }

    success = true;
    return 0;
}

CVector<double> CCorrelateSpacePolyImpl::genVec(double r, double c)
{
    CVector<double> vec;
    vec.resize(6);
    //poly = a0 + a1x + a2y + a3xy + a4xx + a5yy
    //poly = a0 + a1c + a2r + a3cr + a4cc + a5rr
    vec[0] = 1; //a0
    vec[1] = c; //a1
    vec[2] = r; //a2
    vec[3] = c*r; //a3
    vec[4] = c*c; //a4
    vec[5] = r*r; //a5
    return vec;
}

CCorrelateSpacePolyImpl::CCorrelateSpacePolyImpl() :
ICorrelateInterface(),
m_size3x3(3),
m_size3x1(3, 1),
m_size1x3(1, 3),
m_vec3(3)
{
    m_vec3.initVec(0);
    m_A.resize(SSize(9, 6));
    m_B.resize(SSize(9, 1));
    m_leastSquareAns.resize(6);
    int t = 0;
    for (int r = -1; r <= 1; r++)
    {
        for (int c = -1; c <= 1; c++, t++)
        {
            m_A[t] = genVec(r, c);
        }
    }
}

CCorrelateSpacePolyImpl::~CCorrelateSpacePolyImpl()
{
}

int CCorrelateSpacePolyImpl::correlate(const CMatrix<double> &scratchImage,
                                       const SSize &maxPeakLoc, double maxPeakVal,
                                       const double &corrThreshold, bool &success,
                                       SPair<double> &templateToInputOffset, EdgeType edgeType)
{
    templateToInputOffset = SPair<double>(maxPeakLoc);
    if (edgeType == EdgeType::CENTER)
    {
        success = true;
        return 0;
    }
    const SSize &loadSize = edgeType == EdgeType::FULL ? m_size3x3 :
        edgeType == EdgeType::X ? m_size1x3 : m_size3x1;
    const SSize delta = edgeType == EdgeType::FULL ? SSize(1, 1) :
        edgeType == EdgeType::X ? SSize(0, 1) : SSize(1, 0);
    CMatrix<double> scratchSubImage = scratchImage.getSubMatrixEqualsCopy(maxPeakLoc - delta, loadSize);
    if (edgeType == EdgeType::X)
    {
        scratchSubImage = scratchSubImage.pushFrontRow(m_vec3).pushBackRow(m_vec3);
    }
    else if (edgeType == EdgeType::Y)
    {
        scratchSubImage = scratchSubImage.pushFrontCol(m_vec3).pushBackCol(m_vec3);
    }
    m_B[0][0] = (double) scratchSubImage[0][0];
    m_B[1][0] = (double) scratchSubImage[0][1];
    m_B[2][0] = (double) scratchSubImage[0][2];

    m_B[3][0] = (double) scratchSubImage[1][0];
    m_B[4][0] = (double) scratchSubImage[1][1];
    m_B[5][0] = (double) scratchSubImage[1][2];

    m_B[6][0] = (double) scratchSubImage[2][0];
    m_B[7][0] = (double) scratchSubImage[2][1];
    m_B[8][0] = (double) scratchSubImage[2][2];

    try
    {
        m_leastSquareAns = CLeastSquares::calc<double>(&m_A, &m_B);
    }
    catch (const std::exception &e)
    {
        //skipping
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, e);
        return 0;
    }

    //   poly = a0 + a1x + a2y + a3xy + a4xx + a5yy
    //   poly = a0 + a1c + a2r + a3cr + a4cc + a5rr
    double bot = m_leastSquareAns[3] * m_leastSquareAns[3] - 4 * m_leastSquareAns[4] * m_leastSquareAns[5];
    double xOf = 2 * m_leastSquareAns[1] * m_leastSquareAns[5] - m_leastSquareAns[2] * m_leastSquareAns[3];
    double yOf = 2 * m_leastSquareAns[2] * m_leastSquareAns[4] - m_leastSquareAns[1] * m_leastSquareAns[3];
    if (getAbs(bot) < std::numeric_limits<double>::epsilon())
    {
        if (bot < 0)
            bot -= std::numeric_limits<double>::epsilon();
        else
            bot += std::numeric_limits<double>::epsilon();
    }
    xOf /= bot;
    yOf /= bot;

    templateToInputOffset.x += xOf;
    templateToInputOffset.y += yOf;
    success = true;
    return 0;
}

} // namespace __ultra_internal

} // namespace ultra
