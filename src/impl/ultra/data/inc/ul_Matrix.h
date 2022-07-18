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

#include <limits>
#include <memory.h>

#include "ul_Vector.h"
#include "ul_Logger.h"
#include "ul_Pair.h"
#include "cmath"
#include <ul_Exception.h>

namespace ultra
{

template<class T>
class CMatrix
{
protected:
    CVector< CVector<T> > m_mat;

public:

    CMatrix()
    {
        clear();
    }

    CMatrix(const SSize &size)
    {
        clear();
        resize(size);
    }

    CMatrix(const CVector<T> &r)
    {
        clear();
        unsigned long size = r.size();
        resize(size, 1);
        for (unsigned long t = 0; t < size; t++)
        {
            m_mat[t][0] = r[t];
        }
    }

    CMatrix(const CVector< CVector<T> > &r)
    {
        clear();
        m_mat.resize(r.size());

        for (unsigned long row = 0; row < r.size(); row++)
            m_mat[row].resize(r[row].size());

        for (unsigned long row = 0; row < getSize().row; row++)
        {
            T *dp = &m_mat[row][0];
            for (unsigned long col = 0; col < getSize().col; col++)
                dp[col] = r[row][col];
        }
    }

    CMatrix(const std::initializer_list<std::initializer_list<T> > &l)
    {
        clear();
        m_mat.resize(l.size());
        unsigned long row = 0;
        for (const auto &e1 : l)
        {
            m_mat[row].resize(e1.size());
            T *dp = m_mat[row].getDataPointer();
            unsigned long col = 0;
            for (const auto &e2 : e1)
            {
                dp[col++] = e2;
            }
            row++;
        }
    }

    CMatrix(CMatrix<T> &&r) :
    m_mat(std::move(r.m_mat))
    {
    }

    CMatrix(const CMatrix<T> &r)
    {
        SSize s = r.getSize();
        if (!s.containsZero())
        {
            resize(s);
            for (unsigned long x = 0; x < s.row; x++)
            {
                T *dp = &m_mat[x][0];
                const T *dpR = &r[x][0];
                for (unsigned long y = 0; y < s.col; y++)
                    dp[y] = dpR[y];
            }
        }
    }

    template<class N>
    CMatrix(const CMatrix<N> &r)
    {
        SSize s = r.getSize();
        if (!s.containsZero())
        {
            resize(s);
            for (unsigned long x = 0; x < s.row; x++)
            {
                T *dp = &m_mat[x][0];
                const N *dpR = &r[x][0];
                for (unsigned long y = 0; y < s.col; y++)
                    dp[y] = (T) (dpR[y]);
            }
        }
    }

    CMatrix(unsigned long beginSizeRow, unsigned long beginSizeCol)
    {
        clear();
        resize(SSize(beginSizeRow, beginSizeCol));
    }

    virtual ~CMatrix()
    {
        clear();
    }

    void resize(const SSize &size)
    {
        if (getSize() == size)
        {
            return;
        }

        m_mat.resize(size.row);
        for (unsigned long x = 0; x < size.row; x++)
            m_mat[x].resize(size.col);
    }

    void clear()
    {
        for (unsigned long x = 0; x < m_mat.size(); x++)
            m_mat[x].clear();
        m_mat.clear();
    }

    CMatrix<T> &initMat(const T &val = T(0))
    {
        SSize size = getSize();
        for (unsigned long row = 0; row < size.row; row++)
        {
            T *dp = &m_mat[row][0];
            for (unsigned long col = 0; col < size.col; col++)
            {
                dp[col] = val;
            }
        }
        return *this;
    }

    CMatrix<T> &initMatMemset(unsigned char val = 0)
    {
        SSize size = getSize();
        size_t num = size.col * sizeof (T);
        int innerVal = (int) val;
        for (unsigned long row = 0; row < size.row; row++)
        {
            if (memset(&m_mat[row][0], innerVal, num) != &m_mat[row][0])
                throw CException(__FILE__, __LINE__, "Failed to memset matrix");
        }
        return *this;
    }

    CMatrix<T> transpose() const
    {
        CMatrix<T> ret;
        SSize size = getSize();
        size.swap();
        ret.resize(size);

        for (unsigned long r = 0; r < size.row; r++)
        {
            T *dpR = &ret[r][0];
            for (unsigned long c = 0; c < size.col; c++)
            {
                dpR[c] = m_mat[c][r];
            }
        }

        return ret;
    }

    SSize getSize() const
    {
        if (m_mat.size() != 0)
        {
            SSize size(m_mat.size(), m_mat[0].size());
            return size;
        }

        return SSize(0, 0);
    }

    virtual CVector<T>& operator[](unsigned long row)
    {
        return m_mat[row];
    }

    virtual const CVector<T>& operator[](unsigned long row) const
    {
        return m_mat[row];
    }

    virtual T& operator[](const SSize &coor)
    {
        return m_mat[coor.row][coor.col];
    }

    virtual const T& operator[](const SSize &coor) const
    {
        return m_mat[coor.row][coor.col];
    }

    bool operator==(const CMatrix<T> &in) const
    {
        SSize s1 = getSize();
        SSize s2 = in.getSize();

        if (s1 != s2)
            return false;

        for (unsigned long r = 0; r < s1.row; r++)
        {
            const T *dp1 = &m_mat[r][0];
            const T *dp2 = &in[r][0];
            for (unsigned long c = 0; c < s1.col; c++)
            {
                if (dp1[c] != dp2[c])
                    return false;
            }
        }

        return true;
    }

    virtual CMatrix<T> &operator=(const CMatrix<T> &r)
    {
        if (this == &r)
            return *this;
        SSize s = r.getSize();
        resize(s);

        for (unsigned long row = 0; row < s.row; row++)
        {
            T *dpR = &m_mat[row][0];
            const T *dp = &r[row][0];
            for (unsigned long col = 0; col < s.col; col++)
                dpR[col] = dp[col];
        }
        return *this;
    }

    virtual CMatrix<T> &operator=(const std::initializer_list<std::initializer_list<T> > &l)
    {
        if (m_mat.size() != l.size())
            m_mat.resize(l.size());
        unsigned long row = 0;
        for (const auto &e1 : l)
        {
            if (m_mat[row].size() != e1.size())
                m_mat[row].resize(e1.size());
            T *dp = m_mat[row].getDataPointer();
            unsigned long col = 0;
            for (const auto &e2 : e1)
            {
                dp[col++] = e2;
            }
            row++;
        }
        return *this;
    }

    template<class N>
    CMatrix<T> &operator=(const CMatrix<N> &r)
    {
        SSize s = r.getSize();
        resize(s);

        for (unsigned long row = 0; row < s.row; row++)
        {
            T *dpR = &m_mat[row][0];
            const N *dp = &r[row][0];
            for (unsigned long col = 0; col < s.col; col++)
                dpR[col] = (T) (dp[col]);
        }
        return *this;
    }

    CMatrix<T> pushBackRow(const CVector<T> &vec) const
    {
        if (vec.size() != getSize().col)
            throw CException(__FILE__, __LINE__, "Input vector needs to be of size " + toString(getSize().col));
        CMatrix<T> ret(*this);
        ret.m_mat.pushBack(vec);
        return ret;
    }

    CMatrix<T> pushFrontRow(const CVector<T> &vec) const
    {
        if (vec.size() != getSize().col)
            throw CException(__FILE__, __LINE__, "Input vector needs to be of size " + toString(getSize().col));
        CMatrix<T> ret(*this);
        ret.m_mat.pushFront(vec);
        return ret;
    }

    CMatrix<T> pushBackCol(const CVector<T> &vec)
    {
        SSize size = getSize();
        if (vec.size() != size.row)
            throw CException(__FILE__, __LINE__, "Input vector needs to be of size " + toString(getSize().row));
        size.col++;
        CMatrix<T> ret(size);
        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *dp = m_mat[r].getDataPointer();
            T *odp = ret[r].getDataPointer();
            for (unsigned long c = 0, s = size.col - 1; c < s; c++)
            {
                odp[c] = dp[c];
            }
            odp[size.col - 1] = vec[r];
        }
        return ret;
    }

    CMatrix<T> pushFrontCol(const CVector<T> &vec)
    {
        SSize size = getSize();
        if (vec.size() != size.row)
            throw CException(__FILE__, __LINE__, "Input vector needs to be of size " + toString(getSize().row));
        size.col++;
        CMatrix<T> ret(size);
        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *dp = m_mat[r].getDataPointer();
            T *odp = ret[r].getDataPointer();
            for (unsigned long c = 0, s = size.col - 1; c < s; c++)
            {
                odp[c + 1] = dp[c];
            }
            odp[0] = vec[r];
        }
        return ret;
    }

    CMatrix<T> operator*(const CMatrix<T> &r) const
    {
        SSize left = getSize();
        SSize right = r.getSize();
        if (left.col != right.row)
        {
            throw CException(__FILE__, __LINE__, "Cannot multiply these matrices, left.col must equal right.row");
        }
        CMatrix<T> ret(left.row, right.col);

        for (unsigned long row = 0; row < left.row; row++)
        {
            const T *inDp = m_mat[row].getDataPointer();
            T *outDp = ret[row].getDataPointer();
            for (unsigned long col = 0; col < right.col; col++)
            {
                T t = 0;
                for (unsigned long itter = 0; itter < left.col; itter++)
                {
                    t += inDp[itter] * r[itter][col];
                }
                outDp[col] = t;
            }
        }

        return ret;
    }
  

    CMatrix<T> &operator/=(const T &r)
    {
        SSize size = getSize();
        for (unsigned long row = 0; row < size.row; row++)
        {

            for (unsigned long col = 0; col < size.col; col++)
            {
                m_mat[row][col] /= r;
            }
        }

        return *this;
    }
 

    CMatrix<T> operator-(const T &r) const
    {
        SSize size = getSize();
        CMatrix<T> ret;
        ret.resize(size);
        for (unsigned long row = 0; row < size.row; row++)
        {
            for (unsigned long col = 0; col < size.col; col++)
            {
                ret[row][col] = m_mat[row][col] - r;
            }
        }

        return ret;
    }
  

    CVector<unsigned long> getHistogram(unsigned long binCount, bool normalize = false, unsigned long maxNormalizeValue = 256, const CVector<T> *ignoreValues = nullptr) const
    {
        CVector<unsigned long> bins;
        bins.resize(binCount);

        unsigned long bc = binCount - 1;
        bins.initVec(0);
        T min, max;
        SSize size = getSize();
        if (size.containsZero())
        {
            bins.initVec(0);
            return bins;
        }

        bool foundMinMax = ignoreValues == nullptr;
        min = m_mat[0][0];
        max = m_mat[0][0];

        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *dp = &m_mat[r][0];
            for (unsigned long c = 0; c < size.col; c++)
            {
                if (ignoreValues != nullptr)
                {
                    if (ignoreValues->contains(dp[c]))
                        continue;
                }

                if (!foundMinMax)
                {
                    min = dp[c];
                    max = dp[c];
                    foundMinMax = true;
                }

                if (dp[c] < min)
                {
                    min = dp[c];
                }
                if (dp[c] > max)
                {
                    max = dp[c];
                }
            }
        }

        T div = (max - min);

        if (binCount < max || min < 0 || !foundMinMax)
        {
            bins.initVec(0);
            return bins;
        }

        T eps = std::numeric_limits<T>::epsilon();
        if (getAbs(div) < eps)
        {
            bins[binCount / 2] = maxNormalizeValue;
            return bins;
        }

        unsigned long index;
        for (unsigned long r = 0; r < size.row; r++)
        {
            for (unsigned long c = 0; c < size.col; c++)
            {
                T val = m_mat[r][c];
                if (ignoreValues != nullptr)
                {
                    if (ignoreValues->contains(val))
                        continue;
                }

                index = val + 0.5;
                if (index > bc)
                    index = bc;
                bins[index]++;
            }
        }

        if (!normalize)
            return bins;

        unsigned long maxBinVal = bins.max();

        for (unsigned long t = 0; t < binCount; t++)
        {
            bins[t] = (bins[t] * maxNormalizeValue) / maxBinVal;
        }

        return bins;
    }

    CMatrix<T> getSubMatrix(const SSize &ulPoint, const SSize &size) const
    {
        SSize s = getSize();
        SSize p = SSize(ulPoint.row + size.row, ulPoint.col + size.col);
        if (ulPoint.col > s.col ||
                ulPoint.row > s.row ||
                p.col > s.col ||
                p.row > s.row)
            throw CException(__FILE__, __LINE__, "Out of bounds");

        CMatrix<T> ret(size);
        int elementByteSize = sizeof (T);
        int counter = 0;
        for (unsigned long r = ulPoint.row; r < p.row; r++, counter++)
        {
            if (memcpy(&ret[counter][0], &m_mat[r][ulPoint.col], elementByteSize * size.col) != &ret[counter][0])
            {
                throw CException(__FILE__, __LINE__, "Failed to copy data");
            }
        }

        return ret;
    }

    CMatrix<T> getSubMatrixEqualsCopy(const SSize &ULPoint, const SSize &size) const
    {
        CMatrix<T> ret;
        SSize s = getSize();

        SSize p = SSize(ULPoint.row + size.row, ULPoint.col + size.col);

        if (ULPoint.col > s.col ||
                ULPoint.row > s.row ||
                p.col > s.col ||
                p.row > s.row)
            throw CException(__FILE__, __LINE__, "Out of bounds");

        ret = CMatrix<T > (size);

        int cCounter = 0;
        int rCounter = 0;
        for (unsigned long r = ULPoint.row; r < p.row; r++)
        {
            cCounter = 0;
            for (unsigned long c = ULPoint.col; c < p.col; c++)
            {
                ret[rCounter][cCounter++] = m_mat[r][c];
            }
            rCounter++;
        }

        return ret;
    }

    T mean() const
    {
        T ret = 0;
        SSize s = getSize();
        CVector<T> vec(s.row);

        for (unsigned long r = 0; r < s.row; r++)
        {
            ret = 0;
            for (unsigned long c = 0; c < s.col; c++)
            {
                ret += m_mat[r][c];
            }
            vec[r] = ret / ((T) s.col);
        }
        ret = 0;
        for (unsigned long x = 0; x < s.row; x++)
        {
            ret += vec[x];
        }
        ret = ret / ((T) s.row);

        return ret;
    }

    /**
     * Will only calc mean where values are not noData, if all points are noData then noData is returned
     */
    T mean(const T &noData) const
    {
        SSize s = getSize();
        T ret = 0;
        double count = 0;
        for (unsigned long r = 0; r < s.row; r++)
        {
            for (unsigned long c = 0; c < s.col; c++)
            {
                const T &v = m_mat[r][c];
                if (v != noData)
                {
                    ret += v;
                    count++;
                }
            }
        }

        if (count != 0)
            ret = ret / count;
        else
            ret = noData;
        return ret;
    }

    T variance() const
    {
        return variance(mean());
    }

    T variance(const T &mean) const
    {
        T temp, val;
        SSize s = getSize();
        double total = s.getProduct();
        T var = 0;
        for (unsigned long r = 0; r < s.row; r++)
        {
            val = 0;
            for (unsigned long c = 0; c < s.col; c++)
            {
                temp = m_mat[r][c] - mean;
                val += temp*temp;
            }
            var += val / ((T) total);
        }

        return var;
    }

    T stdDev() const
    {
        return sqrt(variance());
    }

    template<class N>
    CMatrix<N> map(const std::function<N(const T &)> &mapper) const
    {
        SSize size = getSize();
        CMatrix<N> result(getSize());
        for (SSize loop(0, 0); loop.row < size.row; loop.row++)
        {
            N *resDp = result[loop.row].getDataPointer();
            const T *inDp = m_mat[loop.row].getDataPointer();
            for (loop.col = 0; loop.col < size.col; loop.col++)
            {
                resDp[loop.col] = mapper(inDp[loop.col]);
            }
        }
        return result;
    }

    template<class N>
    CMatrix<N> convertType() const
    {
        CMatrix<N> ret;
        SSize s = getSize();
        if (s == SSize(0, 0))
        {
            return ret;
        }
        ret = CMatrix<N > (s);
        for (unsigned long r = 0; r < s.row; r++)
        {
            const T *dp = &m_mat[r][0];
            N *dpR = &ret[r][0];
            for (unsigned long c = 0; c < s.col; c++)
            {
                dpR[c] = N(dp[c]);
            }
        }

        return ret;
    }

    template<class N>
    CMatrix<N> fitConvertType(N outputMinValue = (std::numeric_limits<N>::is_integer ? std::numeric_limits<N>::min() : -1 * std::numeric_limits<N>::max()),
                              N outputMaxValue = std::numeric_limits<N>::max()) const
    {
        CMatrix<N> ret;
        SSize s = getSize();
        if (s == SSize(0, 0))
        {
            return ret;
        }
        ret.resize(s);

        SSize min, max;
        T minv, maxv;
        T outputMinValueT = T(outputMinValue);
        T outputMaxValueT = T(outputMaxValue);
        minMaxLoc(min, max);

        minv = m_mat[min.row][min.col];
        maxv = m_mat[max.row][max.col];

        for (unsigned long r = 0; r < getSize().row; r++)
        {
            const T *dp = &m_mat[r][0];
            N *dpR = &ret[r][0];
            for (unsigned long c = 0; c < getSize().col; c++)
            {
                if (minv == maxv)
                {
                    dpR[c] = N((outputMaxValueT - outputMinValueT) / 2);
                }
                else
                {
                    double temp = (double) (dp[c]);
                    temp -= ((double) (minv));
                    temp /= ((double) (maxv - minv));
                    temp *= (outputMaxValueT - outputMinValueT);
                    temp += outputMinValueT;

                    dpR[c] = ((N) (temp));
                }
            }
        }

        return ret;
    }

    void minMaxLoc(SSize &min, SSize &max) const
    {
        SSize size = getSize();
        if (size.containsZero())
            throw CException(__FILE__, __LINE__, "Matrix is of size contains zeroes");

        T minV = m_mat[0][0];
        T maxV = m_mat[0][0];

        min.row = min.col = 0;
        max.row = max.col = 0;

        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *dp = &m_mat[r][0];
            for (unsigned long c = 0; c < size.col; c++)
            {
                if (dp[c] < minV)
                {
                    minV = dp[c];
                    min.row = r;
                    min.col = c;
                }
                if (dp[c] > maxV)
                {
                    maxV = dp[c];
                    max.row = r;
                    max.col = c;
                }
            }
        }
    }

    SSize maxLoc() const
    {
        SSize max;
        SSize size = getSize();
        if (size.containsZero())
            throw CException(__FILE__, __LINE__, "Matrix is of size contains zeroes");

        T maxV = m_mat[0][0];

        max.row = max.col = 0;

        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *dp = &m_mat[r][0];
            for (unsigned long c = 0; c < size.col; c++)
            {
                if (dp[c] > maxV)
                {
                    maxV = dp[c];
                    max.row = r;
                    max.col = c;
                }
            }
        }
        return max;
    }

    int minMaxLocSubMatrix(SSize &min, SSize &max, const SSize &ul, const SSize &subMatrixSize) const
    {
        SSize tempSize = ul + subMatrixSize;
        SSize size = getSize();

        if (tempSize.row > size.row || tempSize.col > size.col)
            return 1;

        T minV = m_mat[ul.row][ul.col];
        T maxV = m_mat[ul.row][ul.col];

        min = max = ul;

        for (unsigned long r = ul.row; r < tempSize.row; r++)
        {
            const T *matDp = &m_mat[r][0];
            for (unsigned long c = ul.col; c < tempSize.col; c++)
            {
                if (matDp[c] < minV)
                {
                    minV = matDp[c];
                    min.row = r;
                    min.col = c;
                }
                if (matDp[c] > maxV)
                {
                    maxV = matDp[c];
                    max.row = r;
                    max.col = c;
                }
            }
        }

        return 0;
    }

    int peaks(CVector<SSize> &minPeaks, CVector<SSize> &maxPeaks, unsigned long maxPeaksToDetect = 1) const
    {
        SSize size = getSize();

        if (size == SSize(0, 0) || maxPeaksToDetect < 1 || size.col * size.row < maxPeaksToDetect)
            return 1;

        minPeaks.resize(maxPeaksToDetect);
        maxPeaks.resize(maxPeaksToDetect);

        for (unsigned long ittr = 0; ittr < maxPeaksToDetect; ittr++)
        {
            SSize maxStart = SSize(0, 0);
            SSize minStart = SSize(0, 0);
            bool canUseMax = true;
            bool canUseMin = true;
            do
            {
                canUseMax = true;
                for (unsigned long inner = 0; inner < ittr; inner++)
                {
                    if (maxPeaks[inner] == maxStart)
                    {
                        canUseMax = false;
                        maxStart.row++;
                        if (maxStart.row == size.row)
                        {
                            maxStart.col++;
                            maxStart.row = 0;
                        }

                        break;
                    }
                }//for inner
            }
            while (!canUseMax);


            do
            {
                canUseMin = true;
                for (unsigned long inner = 0; inner < ittr; inner++)
                {
                    if (minPeaks[inner] == minStart)
                    {
                        canUseMin = false;
                        minStart.row++;
                        if (minStart.row == size.row)
                        {
                            minStart.col++;
                            minStart.row = 0;
                        }

                        break;
                    }
                }//for inner
            }
            while (!canUseMin);

            T maxV = m_mat[maxStart.row][maxStart.col];
            T minV = m_mat[minStart.row][minStart.col];
            maxPeaks[ittr] = maxStart;
            minPeaks[ittr] = minStart;

            for (unsigned long r = 0; r < size.row; r++)
            {
                for (unsigned long c = 0; c < size.col; c++)
                {
                    if (m_mat[r][c] > maxV)
                    {
                        bool isInList = false;
                        for (unsigned long t = 0; t < ittr; t++)
                        {
                            if (r == maxPeaks[t].row && c == maxPeaks[t].col)
                            {
                                isInList = true;
                                break;
                            }
                        }
                        if (!isInList)
                        {
                            maxV = m_mat[r][c];
                            maxPeaks[ittr].row = r;
                            maxPeaks[ittr].col = c;
                        }
                    }

                    if (m_mat[r][c] < minV)
                    {
                        bool isInList = false;
                        for (unsigned long t = 0; t < ittr; t++)
                        {
                            if (r == minPeaks[t].row && c == minPeaks[t].col)
                            {
                                isInList = true;
                                break;
                            }
                        }
                        if (!isInList)
                        {
                            minV = m_mat[r][c];
                            minPeaks[ittr].row = r;
                            minPeaks[ittr].col = c;
                        }
                    }
                }
            }
        }

        return 0;
    }

    /**
     *
     * @param outputMinValue Default is 0
     * @param outputMaxValue Default is 255
     * @return
     */
    CMatrix<T> fit(T outputMinValue = 0, T outputMaxValue = 255) const
    {
        CMatrix<T> ret;
        if (getSize() == SSize(0, 0))
        {
            return ret;
        }
        ret.resize(getSize());

        SSize min, max;
        T minv, maxv;
        minMaxLoc(min, max);

        minv = m_mat[min.row][min.col];
        maxv = m_mat[max.row][max.col];

        for (unsigned long r = 0; r < getSize().row; r++)
        {
            const T *dp = &m_mat[r][0];
            T *dpRet = &ret[r][0];
            for (unsigned long c = 0; c < getSize().col; c++)
            {
                if (minv == maxv)
                {
                    dpRet[c] = (outputMaxValue - outputMinValue) / 2;
                }
                else
                {
                    double temp = (double) (dp[c]);
                    temp -= ((double) (minv));
                    temp /= ((double) (maxv - minv));
                    temp *= (outputMaxValue - outputMinValue);
                    temp += outputMinValue;

                    dpRet[c] = ((T) (temp));
                }
            }
        }

        return ret;
    }

    int CopyFrom(const SSize &ulPoint, const CMatrix<T> &copyMatrix)
    {
        SSize sizeCopy = ulPoint;
        SSize size = copyMatrix.getSize();
        sizeCopy.col += size.col;
        sizeCopy.row += size.row;
        SSize maxSize = getSize();
        if (sizeCopy.col > maxSize.col || sizeCopy.row > maxSize.row)
            return 1;

        unsigned long elementByteSize = sizeof (T);
        unsigned long counter = 0;
        bool useMove = this == &copyMatrix;

        if (useMove)
        {
            for (unsigned long r = ulPoint.row; r < sizeCopy.row; r++)
            {
                if (memmove(&m_mat[r][ulPoint.col], &copyMatrix[counter++][0], elementByteSize * size.col) != &m_mat[r][ulPoint.col])
                {
                    return 1;
                }
            }
        }
        else
        {
            for (unsigned long r = ulPoint.row; r < sizeCopy.row; r++)
            {
                if (memcpy(&m_mat[r][ulPoint.col], &copyMatrix[counter++][0], elementByteSize * size.col) != &m_mat[r][ulPoint.col])
                {
                    return 1;
                }
            }
        }

        return 0;
    }

    unsigned long countIfGreaterThan(const T &val) const
    {
        unsigned long ret = 0;
        SSize size = getSize();
        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *matDp = &m_mat[r][0];
            for (unsigned long c = 0; c < size.col; c++)
            {
                if (matDp[c] > val)
                    ret++;
            }
        }

        return ret;
    }

    CMatrix<T> binaryThreshold(const T &thrs,
                               const T &min = (std::numeric_limits<T>::is_integer ? std::numeric_limits<T>::min() : -1 * std::numeric_limits<T>::max()),
                               const T &max = std::numeric_limits<T>::max()) const
    {
        CMatrix<T> ret;
        SSize s = getSize();
        ret.resize(s);
        if (ret.getSize() != s)
            return ret;

        for (unsigned long r = 0; r < s.row; r++)
        {
            for (unsigned long c = 0; c < s.col; c++)
            {
                if (m_mat[r][c] > thrs)
                    ret[r][c] = max;
                else
                    ret[r][c] = min;
            }
        }

        return ret;
    }

    /**
     * Checks if a given point is on/(or beyond) the edge of the matrix
     * @param point
     * @return
     */
    bool isOnEdge(const SSize &point) const
    {
        SSize size = getSize() - 1;

        if (point.col <= 0 || point.row <= 0 ||
                point.col >= size.col || point.row >= size.row)
            return true;

        return false;
    }
  
    bool contains(const T &val) const
    {
        SSize size = getSize();
        if (size.containsZero())
            return false;
        const CVector<T> *dp = m_mat.getDataPointer();
        for (unsigned long r = 0; r < size.row; r++)
        {
            const T *dpInner = &dp[r][0];
            for (unsigned long c = 0; c < size.col; c++)
            {
                if (dpInner[c] == val)
                    return true;
            }
        }
        return false;
    }

    void setColumn(unsigned long col, const CVector<T> &columnData)
    {
        SSize size = getSize();
        if (col >= size.col)
            throw CException(__FILE__, __LINE__, "Cannot set column '" + toString(col) + "'");
        if (columnData.size() != size.row)
            throw CException(__FILE__, __LINE__, "Input column has '" + toString(columnData.size()) + "' but the matrix container only has '" + toString(size.row) + "' rows");
        for (unsigned long r = 0; r < columnData.size(); r++)
            m_mat[r][col] = columnData[r];
    }

    CVector<T> getColumn(unsigned long col) const
    {
        CVector<T> ret;
        SSize size = getSize();
        if (col >= size.col)
            return ret;

        ret.resize(size.row);

        for (unsigned long r = 0; r < size.row; r++)
            ret[r] = m_mat[r][col];

        return ret;
    }
  
    /**
     * if the matrix is singular then the returned matrix WILL not be UpperTriangular
     *
     * @param amountOfRowsSwapped This is an optional output parameter which will
     * contain the count of rows swapped, this will determine if the determinate
     * calculated from the diagonal needs to be negated or not
     * @return The UpperTriangularMatrix
     */
    CMatrix<T> getUpperTriangularMatrix(unsigned int *amountOfRowsSwapped = nullptr)
    {
        if (amountOfRowsSwapped != nullptr)
            *amountOfRowsSwapped = 0;
        CMatrix<T> C(*this);
        T eps = std::numeric_limits<T>::epsilon();
        T neg = T(-1);
        SSize size = C.getSize();
        unsigned long cols = size.min();
        for (unsigned long c = 0; c < cols; c++)
        {
            // using abs on both sized to make sure this will work if T = SComplex<N>
            if (getAbs(C[c][c]) <= getAbs(eps))
            {
                //swap rows
                bool swapped = false;
                for (unsigned long r = c + 1; r < size.row; r++)
                {
                    // using abs on both sized to make sure this will work if T = SComplex<N>
                    if (getAbs(C[r][c]) > getAbs(eps))// not zero
                    {
                        CVector<T> temp = C[r];
                        C[r] = C[c];
                        C[c] = temp;
                        swapped = true;
                        if (amountOfRowsSwapped != nullptr)
                            (*amountOfRowsSwapped)++;
                        break;
                    }
                }

                if (!swapped)
                {
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Upper triangular matrix has a Zero at row '" + toString(c) + "'");
                    continue;
                }
            }

            for (unsigned long r = c + 1; r < size.row; r++)
            {
                //            C[r] = C[r]+((C[c] * (neg * C[r][c])) / C[c][c]); //multiply first
                C[r] = C[r]+(C[c] * ((neg * C[r][c]) / C[c][c])); //div first

                for (unsigned long p = 0; p < C[r].size(); p++)
                {
                    // using abs on both sized to make sure this will work if T = SComplex<N>
                    if (getAbs(C[r][p]) <= getAbs(eps))
                        C[r][p] = 0;
                }
                C[r][c] = 0;
            }
        }

        return C;
    }

};

} //namespace ultra
