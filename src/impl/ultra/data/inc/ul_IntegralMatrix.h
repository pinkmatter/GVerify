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

#include "ul_Matrix.h"
namespace ultra
{

class CIntegralMatrix
{
private:
    bool m_useNoData;
    long double m_noData;
    CMatrix<long double> m_integralMatrix;
    CMatrix<long double> m_validPixelIntegralMatrix;
    void getSumAndCount(const SSize &ul, const SSize &size, long double &sum, long double *count) const;
    CIntegralMatrix(const SSize &size);
    CIntegralMatrix(const SSize &size, long double noData);
public:
    CIntegralMatrix(const CIntegralMatrix &r);
    CIntegralMatrix(CIntegralMatrix &&r);
    virtual ~CIntegralMatrix();

    CIntegralMatrix &operator=(const CIntegralMatrix &r);
    CIntegralMatrix &operator=(CIntegralMatrix &&r);

    /**
     * Will only use values where (Mask[r][c] != 0)
     * @param mask
     * @return 
     */
    template<class N>
    static CIntegralMatrix createIntegralMatrix(const CMatrix<N> &mat, const CMatrix<unsigned char> &mask, long double noDataValue)
    {
        SSize size = mat.getSize();
        if (mask.getSize() != size)
            throw CException("Input mask must be of size " + toString(size));
        CIntegralMatrix ret = CIntegralMatrix(size, noDataValue);


        if (size.containsZero())
            return ret;

        ret.m_integralMatrix[0][0] = mask[0][0] ? static_cast<long double> (mat[0][0]) : 0.0;
        ret.m_validPixelIntegralMatrix[0][0] = mask[0][0] ? 1 : 0;

        for (unsigned long r = 1; r < size.row; r++)
        {
            ret.m_integralMatrix[r][0] = ret.m_integralMatrix[r - 1][0] + (mask[r][0] ? static_cast<long double> (mat[r][0]) : 0.0);
            ret.m_validPixelIntegralMatrix[r][0] = ret.m_validPixelIntegralMatrix[r - 1][0] + (mask[r][0] ? 1.0 : 0.0);
        }

        for (unsigned long c = 1; c < size.col; c++)
        {
            ret.m_integralMatrix[0][c] = ret.m_integralMatrix[0][c - 1] + (mask[0][c] ? static_cast<long double> (mat[0][c]) : 0.0);
            ret.m_validPixelIntegralMatrix[0][c] = ret.m_validPixelIntegralMatrix[0][c - 1] + (mask[0][c] ? 1.0 : 0.0);
        }

        for (unsigned long r = 1; r < size.row; r++)
        {
            for (unsigned long c = 1; c < size.col; c++)
            {
                long double left = ret.m_integralMatrix[r][c - 1];
                long double top = ret.m_integralMatrix[r - 1][c];
                long double topLeft = ret.m_integralMatrix[r - 1][c - 1];
                ret.m_integralMatrix[r][c] = (mask[r][c] ? static_cast<long double> (mat[r][c]) : 0.0) + left + top - topLeft;

                left = ret.m_validPixelIntegralMatrix[r][c - 1];
                top = ret.m_validPixelIntegralMatrix[r - 1][c];
                topLeft = ret.m_validPixelIntegralMatrix[r - 1][c - 1];
                ret.m_validPixelIntegralMatrix[r][c] = (mask[r][c] ? 1.0 : 0.0) + left + top - topLeft;
            }
        }

        return ret;
    }

    /**
     * Will only use values where (m_mats[r][c] != noDataValue)
     * @param noDataValue
     * @return 
     */
    template<class N>
    static CIntegralMatrix createIntegralMatrix(const CMatrix<N> &mat, const N &noDataValue)
    {
        SSize size = mat.getSize();
        CIntegralMatrix ret = CIntegralMatrix(size, noDataValue);

        if (size.containsZero())
            return ret;

        ret.m_integralMatrix[0][0] = mat[0][0] != noDataValue ? static_cast<long double> (mat[0][0]) : 0.0;
        ret.m_validPixelIntegralMatrix[0][0] = mat[0][0] != noDataValue ? 1.0 : 0.0;

        for (unsigned long r = 1; r < size.row; r++)
        {
            const N &v = mat[r][0];
            ret.m_integralMatrix[r][0] = ret.m_integralMatrix[r - 1][0] + (v != noDataValue ? static_cast<long double> (v) : 0.0);
            ret.m_validPixelIntegralMatrix[r][0] = ret.m_validPixelIntegralMatrix[r - 1][0] + (v != noDataValue ? 1.0 : 0.0);
        }

        for (unsigned long c = 1; c < size.col; c++)
        {
            const N &v = mat[0][c];
            ret.m_integralMatrix[0][c] = ret.m_integralMatrix[0][c - 1] + (v != noDataValue ? static_cast<long double> (v) : 0.0);
            ret.m_validPixelIntegralMatrix[0][c] = ret.m_validPixelIntegralMatrix[0][c - 1] + (v != noDataValue ? 1.0 : 0.0);
        }

        for (unsigned long r = 1; r < size.row; r++)
        {
            for (unsigned long c = 1; c < size.col; c++)
            {
                long double left = ret.m_integralMatrix[r][c - 1];
                long double top = ret.m_integralMatrix[r - 1][c];
                long double topLeft = ret.m_integralMatrix[r - 1][c - 1];
                const N &v = mat[r][c];
                ret.m_integralMatrix[r][c] = (v != noDataValue ? static_cast<long double> (v) : 0.0) + left + top - topLeft;

                left = ret.m_validPixelIntegralMatrix[r][c - 1];
                top = ret.m_validPixelIntegralMatrix[r - 1][c];
                topLeft = ret.m_validPixelIntegralMatrix[r - 1][c - 1];
                ret.m_validPixelIntegralMatrix[r][c] = (v != noDataValue ? 1.0 : 0.0) + left + top - topLeft;
            }
        }

        return ret;
    }

    template<class N>
    static CIntegralMatrix createIntegralMatrix(const CMatrix<N> &mat)
    {
        return createMeanIntegralMatrix(mat, 1);
    }

    /**
     *
     * @param windowSize if this is <=0 then the default Full window will be used as a mean weight
     * @return
     */
    template<class N>
    static CIntegralMatrix createMeanIntegralMatrix(const CMatrix<N> &mat, double windowSize = -1)
    {
        SSize size = mat.getSize();
        CIntegralMatrix ret = CIntegralMatrix(size);
        if (size.containsZero())
            return ret;

        if (windowSize <= 0)
            windowSize = static_cast<long double> (size.getProduct());
        ret.m_integralMatrix[0][0] = static_cast<long double> (mat[0][0]);
        ret.m_integralMatrix[0][0] /= windowSize;
        ret.m_validPixelIntegralMatrix[0][0] = 1.0;

        for (unsigned long r = 1; r < size.row; r++)
        {
            ret.m_integralMatrix[r][0] = ret.m_integralMatrix[r - 1][0]+(static_cast<long double> (mat[r][0])) / windowSize;
            ret.m_validPixelIntegralMatrix[r][0] = ret.m_validPixelIntegralMatrix[r - 1][0] + 1.0;
        }

        for (unsigned long c = 1; c < size.col; c++)
        {
            ret.m_integralMatrix[0][c] = ret.m_integralMatrix[0][c - 1]+(static_cast<long double> (mat[0][c])) / windowSize;
            ret.m_validPixelIntegralMatrix[0][c] = ret.m_validPixelIntegralMatrix[0][c - 1] + 1.0;
        }

        for (unsigned long r = 1; r < size.row; r++)
        {
            for (unsigned long c = 1; c < size.col; c++)
            {
                long double left = ret.m_integralMatrix[r][c - 1];
                long double top = ret.m_integralMatrix[r - 1][c];
                long double topLeft = ret.m_integralMatrix[r - 1][c - 1];
                ret.m_integralMatrix[r][c] = (static_cast<long double> (mat[r][c])) / windowSize + left + top - topLeft;
                
                left = ret.m_validPixelIntegralMatrix[r][c - 1];
                top = ret.m_validPixelIntegralMatrix[r - 1][c];
                topLeft = ret.m_validPixelIntegralMatrix[r - 1][c - 1];
                ret.m_validPixelIntegralMatrix[r][c] =  1.0 + left + top - topLeft;
            }
        }

        return ret;
    }

    long double getSum(const SSize &ul, const SSize &size) const;
    long double getMean(const SSize &ul, const SSize &size) const;
};

}// namespace ultra
