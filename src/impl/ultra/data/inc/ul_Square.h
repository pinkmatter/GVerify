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

#include "ul_Pair.h"
#include "ul_Vector.h"
#include "ul_Matrix.h"

namespace ultra
{

template<class T>
class CSquare
{
private:
    SPair<T> m_ul, m_lr;

public:

    CSquare() :
    m_ul(), m_lr()
    {
    }

    CSquare(const CVector<SPair<T> > &items)
    {
        if (items.size() > 0)
            m_ul = m_lr = items[0];
        for (const auto &e : items)
        {
            m_ul.c = getMIN(m_ul.c, e.c);
            m_lr.c = getMAX(m_lr.c, e.c);

            m_ul.r = getMAX(m_ul.r, e.r);
            m_lr.r = getMIN(m_lr.r, e.r);
        }
    }

    CSquare(const CMatrix<SPair<T> > &items)
    {
        SSize s = items.getSize();
        if (!s.containsZero())
            m_ul = m_lr = items[0][0];
        for (const auto &ee : items)
        {
            for (const auto &e : ee)
            {
                m_ul.c = getMIN(m_ul.c, e.c);
                m_lr.c = getMAX(m_lr.c, e.c);

                m_ul.r = getMAX(m_ul.r, e.r);
                m_lr.r = getMIN(m_lr.r, e.r);
            }
        }
    }

    CSquare(const std::initializer_list<SPair<T> > &l)
    {
        if (l.size() > 0)
            m_ul = m_lr = *l.begin();
        for (const auto &e : l)
        {
            m_ul.c = getMIN(m_ul.c, e.c);
            m_lr.c = getMAX(m_lr.c, e.c);

            m_ul.r = getMAX(m_ul.r, e.r);
            m_lr.r = getMIN(m_lr.r, e.r);
        }
    }

    CSquare(const SPair<T> &pt1, const SPair<T> &pt2)
    {
        m_ul.c = getMIN(pt1.c, pt2.c);
        m_lr.c = getMAX(pt1.c, pt2.c);

        m_ul.r = getMAX(pt1.r, pt2.r);
        m_lr.r = getMIN(pt1.r, pt2.r);
    }

    CSquare(const CSquare<T> &r) :
    m_ul(r.m_ul), m_lr(r.m_lr)
    {
    }

    CSquare(CSquare<T> &&r) :
    m_ul(std::move(r.m_ul)), m_lr(std::move(r.m_lr))
    {
    }

    virtual ~CSquare()
    {
    }

    /**
     * UL in map coordinates
     * @return 
     */
    SPair<T> getUpperLeft() const
    {
        return m_ul;
    }

    /**
     * UR in map coordinates
     * @return 
     */
    SPair<T> getUpperRight() const
    {
        return SPair<T>(m_ul.r, m_lr.c);
    }

    /**
     * LL in map coordinates
     * @return 
     */
    SPair<T> getLowerLeft() const
    {
        return SPair<T>(m_lr.r, m_ul.c);
    }

    /**
     * LR in map coordinates
     * @return 
     */
    SPair<T> getLowerRight() const
    {
        return m_lr;
    }

    SPair<T> getMin() const
    {
        return SPair<T>(getMIN(m_lr.r, m_ul.r),
                getMIN(m_lr.c, m_ul.c));
    }

    SPair<T> getMax() const
    {
        return SPair<T>(getMAX(m_lr.r, m_ul.r),
                getMAX(m_lr.c, m_ul.c));
    }

    CSquare<T> &operator-=(const SPair<T> &v)
    {
        m_lr -= v;
        m_ul -= v;
        return *this;
    }

    CSquare<T> operator-(const SPair<T> &v) const
    {
        CSquare<T> ret(*this);
        ret -= v;
        return ret;
    }

    CSquare<T> &operator+=(const SPair<T> &v)
    {
        m_lr += v;
        m_ul += v;
        return *this;
    }

    CSquare<T> operator+(const SPair<T> &v) const
    {
        CSquare<T> ret(*this);
        ret += v;
        return ret;
    }

    CSquare<T> &operator*=(const SPair<T> &v)
    {
        m_lr *= v;
        m_ul *= v;
        return *this;
    }

    CSquare<T> operator*(const SPair<T> &v) const
    {
        CSquare<T> ret(*this);
        ret *= v;
        return ret;
    }

    CSquare<T> &operator/=(const SPair<T> &v)
    {
        m_lr /= v;
        m_ul /= v;
        return *this;
    }

    CSquare<T> operator/(const SPair<T> &v)const
    {
        CSquare<T> ret(*this);
        ret /= v;
        return ret;
    }

    void set(const SPair<T> &pt1, const SPair<T> &pt2)
    {
        m_ul = m_lr = pt1;

        m_ul.c = getMIN(m_ul.c, pt2.c);
        m_lr.c = getMAX(m_lr.c, pt2.c);

        m_ul.r = getMAX(m_ul.r, pt2.r);
        m_lr.r = getMIN(m_lr.r, pt2.r);
    }

    void set(const CVector<SPair<T> > &items)
    {
        if (items.size() > 0)
            m_ul = m_lr = items[0];
        for (const auto &e : items)
        {
            m_ul.c = getMIN(m_ul.c, e.c);
            m_lr.c = getMAX(m_lr.c, e.c);

            m_ul.r = getMAX(m_ul.r, e.r);
            m_lr.r = getMIN(m_lr.r, e.r);
        }
    }

    void set(const std::initializer_list<SPair<T> > &l)
    {
        if (l.size() > 0)
            m_ul = m_lr = *l.begin();
        for (const auto &e : l)
        {
            m_ul.c = getMIN(m_ul.c, e.c);
            m_lr.c = getMAX(m_lr.c, e.c);

            m_ul.r = getMAX(m_ul.r, e.r);
            m_lr.r = getMIN(m_lr.r, e.r);
        }
    }

    SPair<T> getSize() const
    {
        return SPair<T>(getAbs(m_ul - m_lr));
    }

    bool contains(const SPair<T> &v) const
    {
        return !(v.x < m_ul.x || v.x > m_lr.x ||
                v.y > m_ul.y || v.y < m_lr.y);
    }

    bool operator==(const CSquare<T> &r) const
    {
        return m_ul == r.m_ul && m_lr = r.m_lr;
    }

    bool operator!=(const CSquare<T> &r) const
    {
        return !operator=(r);
    }

    CSquare<T> &operator=(const CSquare<T> &r)
    {
        if (this == &r)
            return *this;
        m_ul = r.m_ul;
        m_lr = r.m_lr;
        return *this;
    }

    CSquare<T> &operator=(CSquare<T> &&r)
    {
        if (this == &r)
            return *this;
        m_ul = std::move(r.m_ul);
        m_lr = std::move(r.m_lr);
        return *this;
    }

    CSquare<T> getIntersection(const CSquare<T> &s) const
    {
        return getIntersection(*this, s);
    }

    static CSquare<T> getIntersection(const CSquare<T> &s1, const CSquare<T> &s2)
    {
        CSquare<T> result;
        result.m_ul.c = getMAX(s1.m_ul.c, s2.m_ul.c);
        result.m_lr.c = getMIN(s1.m_lr.c, s2.m_lr.c);

        result.m_ul.r = getMIN(s1.m_ul.r, s2.m_ul.r);
        result.m_lr.r = getMAX(s1.m_lr.r, s2.m_lr.r);
        return result;
    }

    CSquare<T> getUnion(const CSquare<T> &s) const
    {
        return getUnion(*this, s);
    }

    static CSquare<T> getUnion(const CSquare<T> &s1, const CSquare<T> &s2)
    {
        CSquare<T> result;
        result.m_ul.c = getMIN(s1.m_ul.c, s2.m_ul.c);
        result.m_lr.c = getMAX(s1.m_lr.c, s2.m_lr.c);

        result.m_ul.r = getMAX(s1.m_ul.r, s2.m_ul.r);
        result.m_lr.r = getMIN(s1.m_lr.r, s2.m_lr.r);
        return result;
    }

    friend std::ostream &operator<<(std::ostream &stream, const CSquare<T> &o)
    {
        stream << "ul->(" << o.m_ul << ") lr->(" << o.m_lr << ")";
        return stream;
    }

    friend CSquare<T> round(const CSquare<T> &r)
    {
        return CSquare<T>(round(r.m_lr), round(r.m_ul));
    }

    template<class N>
    CSquare<N> convertType() const
    {
        return CSquare<N>(m_lr.template convertType<N>(), m_ul.template convertType<N>());
    }
};

} // namespace ultra
