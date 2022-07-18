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
#include "ul_Logger.h"

namespace ultra
{

template <class T>
class CConvKernel : public CMatrix<T>
{
private:
    SSize m_anchor;
public:

    /**
     * defualts to a 3x3 kernel with 1,1 as anchor
     */
    CConvKernel() :
    CMatrix<T>(3, 3)
    {
        m_anchor = SSize(1, 1);
        this->initMat(1);
    }

    CConvKernel(const CConvKernel<T>&r) :
    CMatrix<T>(r),
    m_anchor(r.getAnchor())
    {
    }

    CConvKernel(const std::initializer_list<std::initializer_list<T> > &l) :
    CMatrix<T>(l),
    m_anchor(this->getSize() / 2)
    {
    }

    CConvKernel(SSize anchorPoint, SSize kernelSize) :
    CMatrix<T>(kernelSize),
    m_anchor(anchorPoint)
    {
        this->initMat(1);
    }

    CConvKernel(SSize anchorPoint, SSize kernelSize, T initialValue) :
    CMatrix<T>(kernelSize),
    m_anchor(anchorPoint)
    {
        this->initMat(initialValue);
    }

    CConvKernel(unsigned int row_anchorPoint, unsigned int col_anchorPoint, unsigned int row_kernel, unsigned int col_kernel) :
    CMatrix<T>(row_anchorPoint, col_anchorPoint),
    m_anchor(row_anchorPoint, col_anchorPoint)
    {
        this->initMat(1);
    }

    virtual ~CConvKernel()
    {
    }

    friend std::ostream &operator<<(std::ostream &stream, const CConvKernel<T> & o)
    {
        if (o.m_mat.size() != 0)
        {
            stream << "Anchor rows=" << o.m_anchor.row << " cols=" << o.m_anchor.col << std::endl;
            stream << "Display rows=" << o.m_mat.size() << " cols=" << o.m_mat[0].size() << std::endl;
            for (unsigned int x = 0; x < o.m_mat.size(); x++)
            {
                for (unsigned int y = 0; y < o[x].size(); y++)
                    stream << o.m_mat[x][y] << " ";
                stream << "\n";
            }
        }
        return stream;
    }

    SSize getAnchor()
    {
        return m_anchor;
    }

    SSize getAnchor() const
    {
        return m_anchor;
    }

    int setAnchor(unsigned long row, unsigned long col)
    {
        return setAnchor(SSize(row, col));
    }

    int setAnchor(SSize anchor)
    {
        SSize kerSize = this->getSize();
        if (anchor.row < 0 ||
            anchor.row >= kerSize.row ||
            anchor.col < 0 ||
            anchor.col >= kerSize.col)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot set anchor beyond kernel size extents");
            return 1;
        }
        m_anchor = anchor;
        return 0;
    }

    CConvKernel<T> &operator=(const CConvKernel<T> &r)
    {
        if (this == &r)
            return *this;
        this->clear();
        this->m_mat.resize(r.getSize().row);
        for (unsigned int row = 0; row < r.getSize().row; row++)
            this->m_mat[row].resize(r.getSize().col);
        for (unsigned int row = 0; row < r.getSize().row; row++)
            for (unsigned int col = 0; col < r.getSize().col; col++)
                this->m_mat[row][col] = r[row][col];

        m_anchor = r.getAnchor();

        return *this;
    }

    virtual CConvKernel<T> &operator=(const CMatrix<T> &r) override
    {
        this->clear();
        this->m_mat.resize(r.getSize().row);
        for (unsigned int row = 0; row < r.getSize().row; row++)
            this->m_mat[row].resize(r.getSize().col);
        for (unsigned int row = 0; row < r.getSize().row; row++)
            for (unsigned int col = 0; col < r.getSize().col; col++)
                this->m_mat[row][col] = r[row][col];

        m_anchor = SSize(0, 0);

        return *this;
    }

    virtual CConvKernel<T> &operator=(const std::initializer_list<std::initializer_list<T> > &l) override
    {
        if (this->m_mat.size() != l.size())
            this->m_mat.resize(l.size());
        unsigned long row = 0;
        for (const auto &e1 : l)
        {
            if (this->m_mat[row].size() != e1.size())
                this->m_mat[row].resize(e1.size());
            T *dp = this->m_mat[row].getDataPointer();
            unsigned long col = 0;
            for (const auto &e2 : e1)
            {
                dp[col++] = e2;
            }
            row++;
        }
        m_anchor = this->getSize() / 2;
        return *this;
    }

    bool operator==(const CConvKernel<T> &r) const
    {
        if (m_anchor != r.m_anchor)
            return false;

        SSize size1 = this->getSize();
        SSize size2 = r.getSize();

        if (size1 != size2)
            return false;

        for (unsigned long rr = 0; rr < size1.row; rr++)
        {
            for (unsigned long cc = 0; cc < size1.col; cc++)
            {
                if (this->m_mat[rr][cc] != r.m_mat[rr][cc])
                    return false;
            }
        }

        return true;
    }

    bool operator!=(const CConvKernel<T> &r) const
    {
        return (!((*this) == r));
    }

    CMatrix<T> getMatrix() const
    {
        CMatrix<T> result(*this);
        return result;
    }
};

} //namespace ultra
