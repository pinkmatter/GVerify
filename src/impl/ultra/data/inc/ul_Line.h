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

#include <ul_Pair.h>
#include <ul_Exception.h>

namespace ultra
{

template<class T>
class CLine
{
private:
    SPair<T> m_pt1, m_pt2;
public:

    CLine() :
    m_pt1(), m_pt2()
    {
    }

    CLine(const T &pt1Y, const T &pt1X, const T &pt2Y, const T &pt2X) :
    m_pt1(pt1Y, pt1X), m_pt2(pt2Y, pt2X)
    {
    }

    CLine(T &&pt1Y, T &&pt1X, T &&pt2Y, T &&pt2X) :
    m_pt1(std::move(pt1Y), std::move(pt1X)), m_pt2(std::move(pt2Y), std::move(pt2X))
    {
    }

    CLine(const SPair<T> &pt1, const SPair<T> &pt2) :
    m_pt1(pt1), m_pt2(pt2)
    {
    }

    CLine(const SPair<T> &pt1, SPair<T> &&pt2) :
    m_pt1(pt1), m_pt2(std::move(pt2))
    {
    }

    CLine(SPair<T> &&pt1, const SPair<T> &pt2) :
    m_pt1(std::move(pt1)), m_pt2(pt2)
    {
    }

    CLine(SPair<T> &&pt1, SPair<T> &&pt2) :
    m_pt1(std::move(pt1)), m_pt2(std::move(pt2))
    {
    }

    CLine(const CLine<T> &r) :
    m_pt1(r.m_pt1), m_pt2(r.m_pt2)
    {
    }

    CLine(CLine<T> &&r) :
    m_pt1(std::move(r.m_pt1)), m_pt2(std::move(r.m_pt2))
    {
    }

    virtual ~CLine()
    {
    }

    CLine<T> &operator=(const CLine<T> &r)
    {
        if (this == &r)
            return *this;
        m_pt1 = r.m_pt1;
        m_pt2 = r.m_pt2;
        return *this;
    }

    CLine<T> &operator=(CLine<T> &&r)
    {
        if (this == &r)
            return *this;
        m_pt1 = std::move(r.m_pt1);
        m_pt2 = std::move(r.m_pt2);
        return *this;
    }

    const SPair<T> &getPt1() const
    {
        return m_pt1;
    }

    SPair<T> &getPt1()
    {
        return m_pt1;
    }

    void setPt1(const SPair<T> &pt1)
    {
        m_pt1 = pt1;
    }

    void setPt1(SPair<T> &&pt1)
    {
        m_pt1 = std::move(pt1);
    }

    const SPair<T> &getPt2() const
    {
        return m_pt2;
    }

    SPair<T> &getPt2()
    {
        return m_pt2;
    }

    void setPt2(const SPair<T> &pt2)
    {
        m_pt2 = pt2;
    }

    void setPt2(SPair<T> &&pt2)
    {
        m_pt2 = std::move(pt2);
    }

    bool operator==(const CLine<T> &r) const
    {
        return m_pt1 == r.m_pt1 && m_pt2 = r.m_pt2;
    }

    bool operator!=(const CLine<T> &r) const
    {
        return !operator=(r);
    }

    CLine<T> operator+(const SPair<T> &r) const
    {
        CLine<T> ret(*this);
        ret.m_pt1 += r;
        ret.m_pt2 += r;
        return ret;
    }

    CLine<T> &operator+=(const SPair<T> &r)
    {
        m_pt1 += r;
        m_pt2 += r;
        return *this;
    }

    CLine<T> operator-(const SPair<T> &r) const
    {
        CLine<T> ret(*this);
        ret.m_pt1 -= r;
        ret.m_pt2 -= r;
        return ret;
    }

    CLine<T> &operator-=(const SPair<T> &r)
    {
        m_pt1 -= r;
        m_pt2 -= r;
        return *this;
    }

    std::unique_ptr<SPair<T> > getIntersectPoint(const CLine<T> &line) const
    {
        double x1 = m_pt1.x;
        double x2 = m_pt2.x;
        double x3 = line.m_pt1.x;
        double x4 = line.m_pt2.x;

        double y1 = m_pt1.y;
        double y2 = m_pt2.y;
        double y3 = line.m_pt1.y;
        double y4 = line.m_pt2.y;
        double bot = (x4 - x3)*(y1 - y2)-(x1 - x2)*(y4 - y3);
        if (getAbs(bot) < std::numeric_limits<double>::epsilon())
            return nullptr;
        double aTop = (y3 - y4)*(x1 - x3)+(x4 - x3)*(y1 - y3);
        double bTop = (y1 - y2)*(x1 - x3)+(x2 - x1)*(y1 - y3);
        double ta = aTop / bot;
        double tb = bTop / bot;
        if (ta < 0 || ta > 1 ||
                tb < 0 || tb > 1)
            return nullptr; // parallel line of on top of each other
        SPair<double> intPt(m_pt1.y + ta * (m_pt2.y - m_pt1.y),
                            m_pt1.x + ta * (m_pt2.x - m_pt1.x));

        if (std::numeric_limits<T>::is_integer)
        {
            return std::unique_ptr<SPair<T> >(new SPair<T>(intPt.roundValues().convertType<T>()));
        }
        return std::unique_ptr<SPair<T> >(new SPair<T>(intPt.convertType<T>()));
    }

    CLine<T> swapPoints() const
    {
        return CLine<T>(m_pt2, m_pt1);
    }

    /**
     * Please note that this->pt1 == otherLine.getPt1 || this->pt1 == otherLine.getPt2 
     * else an CAlgorithmicException is thrown. Also both line must have a length > 0
     * @param otherLine
     * @return 
     */
    double calcAngle(const CLine<T> &otherLine)
    {
        SPair<double> pt1 = m_pt2.template convertType<double>();
        SPair<double> pt2;
        if (m_pt1 == otherLine.m_pt1)
            pt2 = otherLine.m_pt2.template convertType<double>();
        else if (m_pt1 == otherLine.m_pt2)
            pt2 = otherLine.m_pt1.template convertType<double>();
        else
            throw CException(__FILE__, __LINE__, "No end-point is shared between the two lines");
        pt1 -= m_pt1.template convertType<double>();
        pt2 -= m_pt1.template convertType<double>();
        double top = pt1.x * pt2.x + pt1.y + pt2.y;
        double bot = std::hypot(pt1.x, pt1.y) * std::hypot(pt2.x, pt2.y);
        if (bot == 0)
            throw CException(__FILE__, __LINE__, "One or both line have a length or zero");
        double angle = acos(top / bot);
        return angle;
    }

    friend std::ostream &operator<<(std::ostream &stream, const CLine<T> &o)
    {
        stream << "pt1->(" << o.m_pt1 << ") pt2->(" << o.m_pt2 << ")";
        return stream;
    }

    friend CLine<T> round(const CLine<T> &r)
    {
        return CLine<T>(round(r.pt1), round(r.pt2));
    }

    template<class N>
    CLine<N> convertType() const
    {
        return CLine<N>(m_pt1.template convertType<N>(), m_pt2.template convertType<N>());
    }

};

} // namespace ultra
