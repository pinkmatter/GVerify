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

#include "ul_Vector.h"
#include <algorithm>

namespace ultra
{

template<class T>
class CSmartVector
{
private:
    unsigned long m_size;
    CVector<T> m_vec;

    void growToDoubleCap()
    {
        growToCapacity(capacity() == 0 ? 1 : capacity() * 2);
    }

public:

    CSmartVector() :
    m_size(0),
    m_vec()
    {
    }

    CSmartVector(unsigned long size) :
    m_size(size),
    m_vec(size)
    {
    }

    CSmartVector(const CSmartVector<T> &r) :
    m_size(r.m_size),
    m_vec(r.m_vec)
    {
    }

    CSmartVector(CSmartVector<T> &&r) :
    m_size(std::move(r.m_size)),
    m_vec(std::move(r.m_vec))
    {
    }

    CSmartVector(const CVector<T> &r) :
    m_size(r.size()),
    m_vec(r)
    {
    }

    CSmartVector(CVector<T> &&r) :
    m_size(std::move(r.size())),
    m_vec(std::move(r))
    {
    }

    CSmartVector(const std::initializer_list<T> &l) :
    m_size(l.size()),
    m_vec(l.size())
    {
        unsigned long t = 0;
        for (const auto &elem : l)
        {
            m_vec[t++] = elem;
        }
    }

    virtual ~CSmartVector()
    {

    }

    bool operator!=(const CSmartVector<T> &r) const
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

    bool operator==(const CSmartVector<T> &r) const
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

    CSmartVector<T> sort() const
    {
        return toVector().sort();
    }

    CSmartVector<T> sort(bool (*func)(const T &, const T &)) const
    {
        return toVector().sort(func);
    }

    CSmartVector<T> &operator=(CSmartVector<T> &&r)
    {
        if (this == &r)
            return *this;
        m_size = std::move(r.m_size);
        m_vec = std::move(r.m_vec);
        return *this;
    }

    CSmartVector<T> &operator=(const CSmartVector<T> &r)
    {
        if (this == &r)
            return *this;
        m_size = r.m_size;
        capacity(r.capacity());
        T *dp = m_vec.getDataPointer();
        const T *rdp = r.m_vec.getDataPointer();
        for (unsigned long t = 0; t < m_size; t++)
            dp[t] = rdp[t];
        return *this;
    }

    CSmartVector<T> &operator=(CVector<T> &&r)
    {
        m_size = std::move(r.size());
        m_vec = std::move(r);
        return *this;
    }

    CSmartVector<T> &operator=(const CVector<T> &r)
    {
        m_size = r.size();
        capacity(m_size);
        T *dp = m_vec.getDataPointer();
        const T *rdp = r.getDataPointer();
        for (unsigned long t = 0; t < m_size; t++)
            dp[t] = rdp[t];
        return *this;
    }

    CSmartVector<T> operator+(const T &r)
    {
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] + r;
        return ret;
    }

    CSmartVector<T> operator-(const T &r)
    {
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] - r;
        return ret;
    }

    CSmartVector<T> operator/(const T &r)
    {
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] / r;
        return ret;
    }

    CSmartVector<T> operator*(const T &r)
    {
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] * r;
        return ret;
    }

    CSmartVector<T> &operator+=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] += r;
        return *this;
    }

    CSmartVector<T> &operator-=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] -= r;
        return *this;
    }

    CSmartVector<T> &operator/=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] /= r;
        return *this;
    }

    CSmartVector<T> &operator*=(const T &r)
    {
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] *= r;
        return *this;
    }

    CSmartVector<T> operator+(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] + r.m_vec[it];

        return ret;
    }

    CSmartVector<T> operator-(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] - r.m_vec[it];

        return ret;
    }

    CSmartVector<T> operator/(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] / r.m_vec[it];

        return ret;
    }

    CSmartVector<T> operator*(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        CSmartVector<T> ret(m_size);
        for (unsigned long it = 0; it < m_size; it++)
            ret.m_vec[it] = m_vec[it] * r.m_vec[it];
        return ret;
    }

    CSmartVector<T> &operator+=(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] += r.m_vec[it];
        return *this;
    }

    CSmartVector<T> &operator-=(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] -= r.m_vec[it];
        return *this;
    }

    CSmartVector<T> &operator/=(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] /= r.m_vec[it];
        return *this;
    }

    CSmartVector<T> &operator*=(const CSmartVector<T> &r)
    {
        if (m_size != r.size())
            throw CException(__FILE__, __LINE__, "Both vectors must be of the same length");
        for (unsigned long it = 0; it < m_size; it++)
            m_vec[it] *= r.m_vec[it];
        return *this;
    }

    void resize(unsigned long newSize)
    {
        if (newSize > m_vec.size())
        {
            m_vec.resize(newSize);
        }
        m_size = newSize;
    }

    /**
     * This will not clear any memory, however the
     * <code>size</code> marker will be reset to zero
     */
    void reset()
    {
        m_size = 0;
    }

    /**
     * This will clear all allocated memory
     */
    void clear()
    {
        reset();
        m_vec.clear();
    }

    unsigned long size() const
    {
        return m_size;
    }

    unsigned long capacity() const
    {
        return m_vec.size();
    }

    void capacity(unsigned long cap)
    {
        m_vec.resize(cap);
    }

    void growToCapacity(unsigned long cap)
    {
        m_vec.growTo(cap);
    }

    CSmartVector<T> &pop(unsigned long index, T &val)
    {
        if (index >= m_size)
        {
            throw CException(__FILE__, __LINE__, "Index is out of bounds");
        }
        val = m_vec[index];
        return remove(index);
    }

    CSmartVector<T> &popBack(T &val)
    {
        return pop(m_size - 1, val);
    }

    CSmartVector<T> &popFront(T &val)
    {
        return pop(0, val);
    }

    CSmartVector<T> &push(unsigned long index, const T &val)
    {
        if (index + 1 > m_size)
            return pushBack(val);
        if (index == 0)
            return pushFront(val);
        if (index >= m_size + 1)
            throw CException(__FILE__, __LINE__, "Index out of bound, the given index might have been -1");

        if (capacity() > m_size)
        {
            for (long t = m_size; t > index; t--)
                m_vec[t] = std::move(m_vec[t - 1]);
        }
        else
        {
            growToDoubleCap();
            for (long t = m_size; t > index; t--)
                m_vec[t] = std::move(m_vec[t - 1]);
        }
        m_vec[index] = val;
        m_size++;
        return *this;
    }

    CSmartVector<T> &push(unsigned long index, T &&val)
    {
        if (index + 1 > m_size)
            return pushBack(std::move(val));
        if (index == 0)
            return pushFront(std::move(val));
        if (index >= m_size + 1)
            throw CException(__FILE__, __LINE__, "Index out of bound, the given index might have been -1");

        if (capacity() > m_size)
        {
            for (long t = m_size; t > index; t--)
                m_vec[t] = std::move(m_vec[t - 1]);
        }
        else
        {
            growToDoubleCap();
            for (long t = m_size; t > index; t--)
                m_vec[t] = std::move(m_vec[t - 1]);
        }
        m_vec[index] = std::move(val);
        m_size++;
        return *this;
    }

    CSmartVector<T> &pushBack(const T &val)
    {
        if (m_size == capacity())
            growToDoubleCap();
        m_vec[m_size] = val;
        m_size++;
        return *this;
    }

    CSmartVector<T> &pushBack(T &&val)
    {
        if (m_size == capacity())
            growToDoubleCap();
        m_vec[m_size] = std::move(val);
        m_size++;
        return *this;
    }

    CSmartVector<T> &pushFront(const T &val)
    {
        if (m_size == capacity())
            growToDoubleCap();
        T *dp = m_vec.getDataPointer();
        for (long t = m_size; t > 0; t--)
            dp[t] = dp[t - 1];
        m_vec[0] = val;
        m_size++;
        return *this;
    }

    CSmartVector<T> &pushFront(T &&val)
    {
        if (m_size == capacity())
            growToDoubleCap();
        T *dp = m_vec.getDataPointer();
        for (long t = m_size; t > 0; t--)
            dp[t] = std::move(dp[t - 1]);
        m_vec[0] = std::move(val);
        m_size++;
        return *this;
    }

    CSmartVector<T> &pushBack(const CSmartVector<T> &val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        const T *dp = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[m_size] = dp[t];
            m_size++;
        }

        return *this;
    }

    CSmartVector<T> &pushBack(CSmartVector<T> &&val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        const T *dp = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[m_size] = std::move(dp[t]);
            m_size++;
        }
        val.clear();
        return *this;
    }

    CSmartVector<T> &pushFront(const CSmartVector<T> &val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        T *dp = m_vec.getDataPointer();
        for (long t = m_size + size; t > size - 1; t--)
            dp[t] = dp[t - size];
        const T *dpIn = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[t] = dpIn[t];
            m_size++;
        }

        return *this;
    }

    CSmartVector<T> &pushFront(CSmartVector<T> &&val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        T *dp = m_vec.getDataPointer();
        for (long t = m_size + size; t > size - 1; t--)
            dp[t] = dp[t - size];
        const T *dpIn = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[t] = std::move(dpIn[t]);
            m_size++;
        }
        val.clear();
        return *this;
    }

    CSmartVector<T> &pushBack(const CVector<T> &val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        const T *dp = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[m_size] = dp[t];
            m_size++;
        }

        return *this;
    }

    CSmartVector<T> &pushBack(CVector<T> &&val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        const T *dp = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[m_size] = std::move(dp[t]);
            m_size++;
        }
        val.clear();
        return *this;
    }

    CSmartVector<T> &pushFront(const CVector<T> &val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        T *dp = m_vec.getDataPointer();
        for (long t = m_size + size; t > size - 1; t--)
            dp[t] = dp[t - size];
        const T *dpIn = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[t] = dpIn[t];
            m_size++;
        }

        return *this;
    }

    CSmartVector<T> &pushFront(CVector<T> &&val)
    {
        unsigned long size = val.size();
        if (size == 0)
            return *this;
        if (m_size + size > capacity())
            growToCapacity(m_size + size);

        T *dp = m_vec.getDataPointer();
        for (long t = m_size + size; t > size - 1; t--)
            dp[t] = dp[t - size];
        const T *dpIn = val.getDataPointer();
        for (unsigned long t = 0; t < size; t++)
        {
            m_vec[t] = std::move(dpIn[t]);
            m_size++;
        }
        val.clear();
        return *this;
    }

    const T *begin() const
    {
        return m_vec.begin();
    }

    const T *end() const
    {
        return &m_vec.getDataPointer()[m_size];
    }

    T *begin()
    {
        return m_vec.begin();
    }

    T *end()
    {
        return &m_vec.getDataPointer()[m_size];
    }

    T& operator[](unsigned long index)
    {
#ifdef DEBUG
        if (index >= m_size)
            throw CException(__FILE__, __LINE__, "Trying to access memory outside of CSmartVector");
#endif
        return m_vec[index];
    }

    const T& operator[](unsigned long index) const
    {
#ifdef DEBUG
        if (index >= m_size)
            throw CException(__FILE__, __LINE__, "Trying to access memory outside of CSmartVector");
#endif
        return m_vec[index];
    }

    const T* getDataPointer() const
    {
        return m_vec.getDataPointer();
    }

    T* getDataPointer()
    {
        return m_vec.getDataPointer();
    }

    CVector<T> toVector() const
    {
        CVector<T> ret(m_size);
        T *dp = ret.getDataPointer();
        unsigned long t = 0;
        for (const auto &e : * this)
        {
            dp[t++] = e;
        }
        return ret;
    }
};

} // namespace ultra
