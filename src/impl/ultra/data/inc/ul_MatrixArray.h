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

template<class T>
class CMatrixArray : public CVector<CMatrix<T> >
{
public:

    CMatrixArray()
    : CVector<CMatrix<T> >()
    {
    }

    CMatrixArray(const CMatrixArray<T> &r)
    : CVector<CMatrix<T> >(r)
    {
    }

    CMatrixArray(CMatrixArray<T> &&r)
    : CVector<CMatrix<T> >(std::move(r))
    {
    }

    CMatrixArray(const CVector<CMatrix<T> > &r)
    : CVector<CMatrix<T> >(r)
    {
    }

    CMatrixArray(CVector<CMatrix<T> > &&r)
    : CVector<CMatrix<T> >(std::move(r))
    {
    }

    CMatrixArray(unsigned long arraySize)
    : CVector<CMatrix<T> >(arraySize)
    {
    }

    virtual ~CMatrixArray()
    {
    }

    SSize getDimensions(unsigned long band) const
    {
        return this->operator[](band).getSize();
    }

    CMatrixArray<T> getSubMatrixArray(const SSize &ULPoint, const SSize &size) const
    {
        unsigned long s = this->size();
        CMatrixArray<T> result(s);
        for (unsigned long t = 0; t < s; t++)
        {
            result[t] = (*this)[t].getSubMatrix(ULPoint, size);
        }
        return result;
    }

    CMatrixArray<T> getSubMatrixEqualsCopy(const SSize &ULPoint, const SSize &size) const
    {
        unsigned long s = this->size();
        CMatrixArray<T> result(s);
        for (unsigned long t = 0; t < s; t++)
        {
            result[t] = (*this)[t].getSubMatrixEqualsCopy(ULPoint, size);
        }
        return result;
    }

    /////////
    /////////

    CMatrixArray<T> &operator=(const CMatrixArray<T> &r)
    {
        if (this == &r)
            return *this;
        this->resize(r.size());
        for (unsigned long t = 0; t < this->size(); t++)
        {
            (*this)[t] = r[t];
        }
        return *this;
    }

    CMatrixArray<T> &operator=(CMatrixArray<T> &&r)
    {
        if (this == &r)
            return *this;
        this->resize(r.size());
        for (unsigned long t = 0; t < this->size(); t++)
        {
            (*this)[t] = std::move(r[t]);
        }
        return *this;
    }

    /////////
    /////////

    CMatrixArray<T>& operator+=(const T &r)
    {
        for (unsigned long it = 0; it < this->size(); it++)
            (*this)[it] += r;

        return *this;
    }

    CMatrixArray<T>& operator-=(const T &r)
    {
        for (unsigned long it = 0; it < this->size(); it++)
            (*this)[it] -= r;

        return *this;
    }

    CMatrixArray<T>& operator/=(const T &r)
    {
        for (unsigned long it = 0; it < this->size(); it++)
            (*this)[it] /= r;

        return *this;
    }

    CMatrixArray<T>& operator*=(const T &r)
    {
        for (unsigned long it = 0; it < this->size(); it++)
            (*this)[it] *= r;

        return *this;
    }
    /////////
    /////////

    CMatrixArray<T> operator+(const T &r) const
    {
        CMatrixArray<T> ret(*this);
        ret += r;
        return ret;
    }

    CMatrixArray<T> operator-(const T &r) const
    {
        CMatrixArray<T> ret(*this);
        ret -= r;
        return ret;
    }

    CMatrixArray<T> operator/(const T &r) const
    {
        CMatrixArray<T> ret(*this);
        ret /= r;
        return ret;
    }

    CMatrixArray<T> operator*(const T &r) const
    {
        CMatrixArray<T> ret(*this);
        ret *= r;
        return ret;
    }

    friend CMatrixArray<T> getAbs(const CMatrixArray<T> &in)
    {
        CMatrixArray<T> ret(in.size());
        for (unsigned long t = 0; t < ret.size(); t++)
        {
            ret[t] = getAbs(in[t]);
        }
        return ret;
    }

    friend CMatrixArray<T> round(const CMatrixArray<T> &in)
    {
        CMatrixArray<T> ret(in.size());
        for (unsigned long t = 0; t < ret.size(); t++)
        {
            ret[t] = round(in[t]);
        }
        return ret;
    }

    friend CMatrixArray<T> sqrt(const CMatrixArray<T> &in)
    {
        CMatrixArray<T> ret(in.size());
        for (unsigned long t = 0; t < ret.size(); t++)
        {
            ret[t] = sqrt(in[t]);
        }
        return ret;
    }
    /////////
    /////////
    using CVector<CMatrix<T> >::operator==;

    using CVector<CMatrix<T> >::operator+;
    using CVector<CMatrix<T> >::operator-;
    using CVector<CMatrix<T> >::operator/;
    using CVector<CMatrix<T> >::operator*;

    using CVector<CMatrix<T> >::operator+=;
    using CVector<CMatrix<T> >::operator-=;
    using CVector<CMatrix<T> >::operator/=;
    using CVector<CMatrix<T> >::operator*=;
};

}
