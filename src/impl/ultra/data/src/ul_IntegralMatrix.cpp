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

#include "ul_IntegralMatrix.h"

namespace ultra
{

CIntegralMatrix::CIntegralMatrix(const SSize &size) :
m_useNoData(false),
m_noData(0),
m_integralMatrix(size),
m_validPixelIntegralMatrix(size)
{
}

CIntegralMatrix::CIntegralMatrix(const SSize &size, long double noData) :
m_useNoData(true),
m_noData(noData),
m_integralMatrix(size),
m_validPixelIntegralMatrix(size)
{
}

CIntegralMatrix::~CIntegralMatrix()
{
}

CIntegralMatrix::CIntegralMatrix(const CIntegralMatrix &r) :
m_useNoData(r.m_useNoData),
m_noData(r.m_noData),
m_integralMatrix(r.m_integralMatrix),
m_validPixelIntegralMatrix(r.m_validPixelIntegralMatrix)
{
}

CIntegralMatrix::CIntegralMatrix(CIntegralMatrix &&r) :
m_useNoData(std::move(r.m_useNoData)),
m_noData(std::move(r.m_noData)),
m_integralMatrix(std::move(r.m_integralMatrix)),
m_validPixelIntegralMatrix(std::move(r.m_validPixelIntegralMatrix))
{
}

CIntegralMatrix &CIntegralMatrix::operator=(const CIntegralMatrix &r)
{
    if (this == &r)
        return *this;
    m_useNoData = r.m_useNoData;
    m_noData = r.m_noData;
    m_integralMatrix = r.m_integralMatrix;
    m_validPixelIntegralMatrix = r.m_validPixelIntegralMatrix;
    return *this;
}

CIntegralMatrix &CIntegralMatrix::operator=(CIntegralMatrix &&r)
{
    if (this == &r)
        return *this;
    m_useNoData = std::move(r.m_useNoData);
    m_noData = std::move(r.m_noData);
    m_integralMatrix = std::move(r.m_integralMatrix);
    m_validPixelIntegralMatrix = std::move(r.m_validPixelIntegralMatrix);
    return *this;
}

void CIntegralMatrix::getSumAndCount(const SSize &ul, const SSize &size, long double &sum, long double *count) const
{
    SSize s = m_integralMatrix.getSize();
    SSize lr = ul + size;

    if (ul.row >= s.row || ul.col >= s.col ||
        lr.row > s.row || lr.col > s.col)
        throw CException(__FILE__, __LINE__, "Range is out of bounds");
    sum = 0;
    if (count)*count = 0;

    if (size.containsZero())
        return;

    lr -= 1;
    if (ul.col == 0 && ul.row == 0)
    {
        sum = m_integralMatrix[lr.row][lr.col];
        if (count)*count = m_validPixelIntegralMatrix[lr.row][lr.col];
    }
    else if (ul.col == 0)
    {
        sum = m_integralMatrix[lr.row][lr.col] - m_integralMatrix[ul.row - 1][lr.col];
        if (count)*count = m_validPixelIntegralMatrix[lr.row][lr.col] - m_validPixelIntegralMatrix[ul.row - 1][lr.col];
    }
    else if (ul.row == 0)
    {
        sum = m_integralMatrix[lr.row][lr.col] - m_integralMatrix[lr.row][ul.col - 1];
        if (count)*count = m_validPixelIntegralMatrix[lr.row][lr.col] - m_validPixelIntegralMatrix[lr.row][ul.col - 1];
    }
    else
    {
        sum = m_integralMatrix[lr.row][lr.col] -
            m_integralMatrix[lr.row][ul.col - 1] -
            m_integralMatrix[ul.row - 1][lr.col] +
            m_integralMatrix[ul.row - 1][ul.col - 1];

        if (count)*count = m_validPixelIntegralMatrix[lr.row][lr.col] -
            m_validPixelIntegralMatrix[lr.row][ul.col - 1] -
            m_validPixelIntegralMatrix[ul.row - 1][lr.col] +
            m_validPixelIntegralMatrix[ul.row - 1][ul.col - 1];
    }
}

long double CIntegralMatrix::getSum(const SSize &ul, const SSize &size) const
{
    long double sum;
    getSumAndCount(ul, size, sum, nullptr);
    return sum;
}

long double CIntegralMatrix::getMean(const SSize &ul, const SSize &size) const
{
    long double sum;
    long double count;
    getSumAndCount(ul, size, sum, &count);
    if (m_useNoData)
    {
        if (count > 0)
            sum /= count;
        else
            sum = m_noData;
    }
    else
    {
        sum /= ((long double) size.getProduct());
    }
    return sum;
}

}
