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

#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <ul_Utility.h>
#include <ul_Exception.h>
#include <functional>

namespace ultra
{

template<class T>
class CVector
{
private:

    void constructToSize(unsigned long newSize)
    {
        m_vec = nullptr;
        m_size = 0;
        if (newSize > 0)
        {
            m_vec = new T[newSize];
            m_size = newSize;
        }
    }

    void growToObject(unsigned long newSize)
    {
        unsigned long copySize = (newSize < m_size) ? (newSize) : (m_size);
        CVector<T> current(std::move(*this));
        resize(newSize);
        for (unsigned long t = 0; t < copySize; t++)
            m_vec[t] = std::move(current[t]);
    }

    CVector<T> &innerPushFront(const CVector<T> &in)
    {
        unsigned long inSize = in.size();
        if (inSize == 0)
            return *this;
        unsigned long oldSize = m_size;
        unsigned long newSize = oldSize + inSize;
        growTo(newSize);

        T *dp = getDataPointer();
        const T *inDp = in.getDataPointer();
        long t1 = m_size - 1;
        long t2 = t1 - inSize;
        for (; t1 > inSize - 1; t1--, t2--)
            dp[t1] = dp[t2];
        for (unsigned long t = 0; t < inSize; t++)
            dp[t] = inDp[t];
        return *this;
    }

    CVector<T> &innerPushBack(const CVector<T> &in)
    {
        if (in.size() == 0)
            return *this;
        unsigned long oldSize = m_size;
        unsigned long newSize = oldSize + in.size();
        growTo(newSize);
        unsigned long t1 = oldSize, t2;
        T *dp = getDataPointer();
        const T *inDp = in.getDataPointer();
        for (t2 = 0; t2 < in.size(); t2++, t1++)
            dp[t1] = inDp[t2];
        return *this;
    }

protected:
    T *m_vec;
    unsigned long m_size;

public:

    CVector() :
    m_vec(nullptr),
    m_size(0)
    {
    }

    CVector(unsigned long newSize)
    {
        constructToSize(newSize);
    }

    CVector(const CVector<T> &r)
    {
        constructToSize(r.size());
        for (unsigned long x = 0; x < m_size; x++)
            m_vec[x] = r[x];
    }

    template<class N>
    CVector(const CVector<N> &r)
    {
        constructToSize(r.size());
        for (unsigned long x = 0; x < m_size; x++)
            m_vec[x] = (T) (r[x]);

    }

    CVector(const T *r, unsigned long itemCount)
    {
        constructToSize(itemCount);
        for (unsigned long x = 0; x < itemCount; x++)
            m_vec[x] = r[x];
    }

    CVector(const std::vector<T> &r)
    {
        constructToSize(r.size());
        for (unsigned long x = 0; x < m_size; x++)
            m_vec[x] = r[x];
    }

    virtual ~CVector()
    {
        clear();
    }

    void clear()
    {
        if (m_vec != nullptr)
        {
            delete []m_vec;
            m_vec = nullptr;
        }
        m_size = 0;
    }

    unsigned long size() const
    {
        return m_size;
    }

    const T *begin() const
    {
        return &m_vec[0];
    }

    const T *end() const
    {
        return &m_vec[m_size];
    }

    T *begin()
    {
        return &m_vec[0];
    }

    T *end()
    {
        return &m_vec[m_size];
    }

    friend std::ostream &operator<<(std::ostream &stream, const CVector<T> &o)
    {
        stream << "Display items=" << o.m_size << std::endl;
        if (o.m_size != 0)
        {
            for (unsigned long x = 0; x < o.m_size; x++)
            {
                stream << o.m_vec[x] << " ";
            }
        }

        return stream;
    }

    T& operator[](unsigned long index)
    {
#ifdef DEBUG
        if (index >= m_size)
            throw CIndexOutOfBoundsException(__FILE__, __LINE__, "Trying to access memory outside of CVector");
#endif
        return m_vec[index];
    }

    const T& operator[](unsigned long index) const
    {
#ifdef DEBUG
        if (index >= m_size)
            throw CIndexOutOfBoundsException(__FILE__, __LINE__, "Trying to access memory outside of CVector");
#endif
        return m_vec[index];
    }

    T& operator()(unsigned long index)
    {
#ifdef DEBUG
        if (index >= m_size)
            throw CIndexOutOfBoundsException(__FILE__, __LINE__, "Trying to access memory outside of CVector");
#endif
        return m_vec[index];
    }

    const T& operator()(unsigned long index) const
    {
#ifdef DEBUG
        if (index >= m_size)
            throw CIndexOutOfBoundsException(__FILE__, __LINE__, "Trying to access memory outside of CVector");
#endif
        return m_vec[index];
    }

    void initVec(T val = T(0))
    {
        for (unsigned long x = 0; x < m_size; x++)
        {
            m_vec[x] = val;
        }
    }

    CVector<T> &pushFront(const CVector<T> &in)
    {
        if (this == &in)
        {
            CVector<T> copy(in);
            return innerPushFront(copy);
        }
        return innerPushFront(in);
    }

    CVector<T> &pushBack(const CVector<T> &in)
    {
        if (this == &in)
        {
            CVector<T> copy(in);
            return innerPushBack(copy);
        }
        return innerPushBack(in);
    }

    void resize(unsigned long newSize)
    {
        if (newSize == m_size)
        {
            return;
        }
        clear();
        constructToSize(newSize);
    }

    CVector<T> &remove(unsigned long index)
    {
        return remove(index, index + 1);
    }

    void growTo(unsigned long newSize)
    {
        if (newSize == 0)
        {
            clear();
            return;
        }

        if (newSize == m_size && m_size > 0)
        {
            return;
        }

        if (m_size == 0)
            resize(newSize);
        else
            growToObject(newSize);
    }

    CVector<T> &pushBack(const T &val)
    {
        if (containsItemAddr(val))
        {
            T copy = val; // must copy before growTo call
            growTo(m_size + 1);
            m_vec[m_size - 1] = copy;
        }
        else
        {
            growTo(m_size + 1);
            m_vec[m_size - 1] = val;
        }
        return *this;
    }

    CVector<T> &pushFront(const T &val)
    {
        if (containsItemAddr(val))
        {
            T copy = val; // must copy before growTo call
            growTo(m_size + 1);
            for (long t = m_size - 1; t > 0; t--)
                m_vec[t] = std::move(m_vec[t - 1]);
            m_vec[0] = copy;
        }
        else
        {
            growTo(m_size + 1);
            for (long t = m_size - 1; t > 0; t--)
                m_vec[t] = std::move(m_vec[t - 1]);
            m_vec[0] = val;
        }
        return *this;
    }

    bool operator!=(const CVector<T> &r) const
    {
        unsigned long left = m_size;
        unsigned long right = r.size();
        if (left != right)
            return true;

        for (unsigned long it = 0; it < left; it++)
        {
            if (m_vec[it] != r.m_vec[it])
            {
                return true;
            }
        }

        return false;
    }

    bool operator==(const CVector<T> &r) const
    {
        unsigned long left = m_size;
        unsigned long right = r.size();
        if (left != right)
            return false;

        for (unsigned long it = 0; it < left; it++)
        {
            if (m_vec[it] != r.m_vec[it])
            {
                return false;
            }
        }

        return true;
    }

    friend CVector<T> operator+(const T &val, const CVector<T> &vec)
    {
        return (vec + val);
    }

    friend CVector<T> operator-(const T &val, const CVector<T> &vec)
    {
        return (vec * T(-1) + val);
    }

    friend CVector<T> operator*(const T &val, const CVector<T> &vec)
    {
        return (vec * val);
    }

    friend CVector<T> operator/(const T &val, const CVector<T> &vec)
    {
        CVector<T> ret(vec.m_size);
        for (unsigned long it = 0; it < ret.m_size; it++)
            ret[it] = val / vec.m_vec[it];
        return ret;
    }

    CVector<T> operator+(const CVector<T> &r) const
    {
        unsigned long left = m_size;
        unsigned long right = r.size();
        if (left != right)
            return CVector<T > (0);
        CVector<T> ret(left);

        for (unsigned long it = 0; it < left; it++)
        {
            ret(it) = m_vec[it] + r(it);
        }

        return ret;
    }

    CVector<T> operator+(const T &r) const
    {
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
        {
            ret(it) = m_vec[it] + r;
        }

        return ret;
    }

    CVector<T> operator-() const
    {
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
        {
            ret(it) = -m_vec[it];
        }
        return ret;
    }

    CVector<T> operator-(const CVector<T> &r) const
    {
        unsigned long left = m_size;
        unsigned long right = r.size();
        if (left != right)
            return CVector<T > (0);
        CVector<T> ret(left);

        for (unsigned long it = 0; it < left; it++)
        {
            ret(it) = m_vec[it] - r(it);
        }

        return ret;
    }

    CVector<T> operator-(const T &r) const
    {
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
        {
            ret(it) = m_vec[it] - r;
        }

        return ret;
    }

    CVector<T> operator*(const T &r) const
    {
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret[it] = m_vec[it] * r;
        return ret;
    }

    CVector<T> operator*(const CVector<T> &r) const
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret[it] = m_vec[it] * r[it];

        return ret;
    }

    CVector<T> operator/(const T &r) const
    {
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret[it] = m_vec[it] / r;
        return ret;
    }

    CVector<T> operator/(const CVector<T> &r) const
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        CVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret[it] = m_vec[it] / r[it];

        return ret;
    }

    CVector<T>& operator=(CVector<T> &&r)
    {
        if (this == &r)
            return *this;
        clear();
        m_size = std::move(r.m_size);
        m_vec = std::move(r.m_vec);
        r.m_size = 0;
        r.m_vec = nullptr;
        return *this;
    }

    CVector<T>& operator=(const CVector<T> &r)
    {
        if (this == &r)
            return *this;
        if (m_size != r.size())
            resize(r.size());
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] = r(it);

        return *this;
    }

    CVector<T>& operator=(const std::initializer_list<T> &l)
    {
        if (m_size != l.size())
            resize(l.size());
        unsigned long x = 0;
        for (const auto &elem : l)
        {
            m_vec[x++] = elem;
        }
        return *this;
    }

    template<class N>
    CVector<T>& operator=(const CVector<N> &r)
    {
        if (m_size != r.size())
            resize(r.size());
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] = (T) (r(it));

        return *this;
    }

    CVector<T>& operator/=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] /= r;

        return *this;
    }

    CVector<T>& operator*=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] *= r;

        return *this;
    }

    CVector<T>& operator*=(const CVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] *= r[it];

        return *this;
    }

    CVector<T>& operator/=(const CVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] /= r[it];

        return *this;
    }

    CVector<T>& operator+=(const CVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] += r[it];

        return *this;
    }

    CVector<T>& operator-=(const CVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] -= r[it];

        return *this;
    }

    CVector<T>& operator+=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] += r;

        return *this;
    }

    CVector<T>& operator-=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] -= r;

        return *this;
    }

    CVector<T>& operator=(const std::vector<T> &r)
    {
        if (m_size != r.size())
            resize(r.size());
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] = r[it];

        return *this;
    }

    /**
     * Will sort the CVector using:
     * 
     * CVector::sort([](const T&o1, const T & o2)->bool{return o1 < o2;});
     * @return 
     */
    CVector<T> sort() const
    {
        return sort([](const T&o1, const T & o2)->bool
        {
            return o1 < o2;
        });
    }

    /**
     * Sorts the vector based on a compare function.
     * 
     * The <code>func</code> is a binary function that accepts two elements in 
     * the range as arguments, and returns a value convertible to bool. The 
     * value returned indicates whether the element passed as first argument is 
     * considered to go before the second in the specific strict weak ordering 
     * it defines.
     * 
     * The function shall not modify any of its arguments. This can either be a 
     * function pointer or a function object.
     * @param func
     * @return 
     */
    CVector<T> sort(bool (*func)(const T &, const T &)) const
    {
        CVector<T> ret = *this;

        if (m_size == 0)
            return ret;

        unsigned long t1, t2;
        for (t1 = 0; t1 < m_size - 1; t1++)
        {
            for (t2 = t1 + 1; t2 < m_size; t2++)
            {
                if (func(ret[t2], ret[t1]))
                {
                    swapValues<T>(ret[t1], ret[t2]);
                }
            }
        }

        return ret;
    }

    const T* getDataPointer() const
    {
        return m_vec;
    }

    T* getDataPointer()
    {
        return m_vec;
    }

    T product() const
    {
        if (m_size == 0)
            throw CException(__FILE__, __LINE__, "Cannot determine the product of a vector of size zero");
        T pr = m_vec[0];
        for (unsigned long t = 1; t < m_size; t++)
            pr *= m_vec[t];
        return pr;
    }

    T max() const
    {
        T max;
        if (m_size == 0)
            throw CException(__FILE__, __LINE__, "Cannot calculate max as there are no elements in the vector");
        max = m_vec[0];
        for (unsigned long t = 1; t < m_size; t++)
        {
            if (max < m_vec[t])
                max = m_vec[t];
        }

        return max;
    }

    bool containsItemAddr(const T &val) const
    {
        return (m_vec != nullptr && m_size > 0
                && &val >= &m_vec[0]
                && &val <= &m_vec[m_size - 1]);
    }

    bool contains(const T &r) const
    {
        if (containsItemAddr(r))
            return true;
        for (const auto &e : * this)
        {
            if (e == r)
                return true;
        }
        return false;
    }

};

} //namespace ultra
