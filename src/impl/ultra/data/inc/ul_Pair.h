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

#include <ostream>
#include <limits>
#include <math.h>
#include <ul_Exception.h>
#include "ul_Size.h"
#include "ul_3D.h"
#include "ul_2D.h"

namespace ultra
{

template<class T>
struct SPair
{

    union
    {
        T r;
        T i;
        T y;
        T max;
        T end;
        T lat;
        T in;
        T ul;
    };

    union
    {
        T c;
        T j;
        T x;
        T min;
        T start;
        T lon;
        T out;
        T lr;
        T size;
    };

    SPair() :
    i(0),
    j(0)
    {
    }

    SPair(const T &v) :
    i(v),
    j(v)
    {
    }

    SPair(const T &i, const T &j) :
    i(i), //r lat y max
    j(j) //c lon x min
    {
    }

    SPair(const T &i, T &&j) :
    i(i), //r lat y max
    j(std::move(j)) //c lon x min
    {
    }

    SPair(T &&i, const T &j) :
    i(std::move(i)), //r lat y max
    j(j) //c lon x min
    {
    }

    SPair(T &&i, T &&j) :
    i(std::move(i)), //r lat y max
    j(std::move(j)) //c lon x min
    {
    }

    SPair(const SPair<T> &r) :
    i(r.i),
    j(r.j)
    {
    }

    SPair(SPair<T> &&r) :
    i(std::move(r.i)),
    j(std::move(r.j))
    {
    }

    SPair(const S2D<T> &r) :
    x(r.x),
    y(r.y)
    {
    }

    SPair(const S3D<T> &r) :
    x(r.x),
    y(r.y)
    {
    }

    SPair(const SSize & r) :
    i((T) r.row),
    j((T) r.col)
    {
    }

    ~SPair()
    {
    }

    friend std::ostream &operator<<(std::ostream &stream, const SPair<T> & o)
    {
        stream << ("x = " + toString(o.x) + " y = " + toString(o.y));
        return stream;
    }

    SPair<T> &operator=(const SPair<T> &r)
    {
        if (this == &r)
            return *this;
        i = r.i;
        j = r.j;
        return *this;
    }

    SPair<T> &operator=(SPair<T> &&r)
    {
        if (this == &r)
            return *this;
        i = std::move(r.i);
        j = std::move(r.j);
        return *this;
    }

    SPair<T> &operator=(const SSize &r)
    {
        i = (T) r.row;
        j = (T) r.col;
        return *this;
    }

    SPair<T> &operator=(const S2D<T> &r)
    {
        x = r.x;
        y = r.y;
        return *this;
    }

    SPair<T> &operator=(const S3D<T> &r)
    {
        x = r.x;
        y = r.y;
        return *this;
    }

    SPair<T> &operator=(const T &r)
    {
        i = r;
        j = r;
        return *this;
    }

    SPair<T> operator+(const SPair<T> &r) const
    {
        SPair<T> ret(*this);
        ret.i += r.i;
        ret.j += r.j;
        return ret;
    }

    SPair<T> operator+(const T &r) const
    {
        SPair<T> ret(*this);
        ret.i += r;
        ret.j += r;
        return ret;
    }

    SPair<T> operator-(const SPair<T> &r) const
    {
        SPair<T> ret(*this);
        ret.i -= r.i;
        ret.j -= r.j;
        return ret;
    }

    SPair<T> operator-(const T &r) const
    {
        SPair<T> ret(*this);
        ret.i -= r;
        ret.j -= r;
        return ret;
    }

    SPair<T> operator-() const
    {
        return SPair<T> (-this->r, -this->c);
    }

    SPair<T> &operator+=(const SPair<T> &r)
    {
        i += r.i;
        j += r.j;
        return *this;
    }

    friend SPair<T> operator+(const T &val, const SPair<T> &r)
    {
        return SPair<T> (val + r.r, val + r.c);
    }

    friend SPair<T> operator-(const T &val, const SPair<T> &r)
    {
        return SPair<T> (val - r.r, val - r.c);
    }

    friend SPair<T> operator*(const T &val, const SPair<T> &r)
    {
        return SPair<T> (val * r.r, val * r.c);
    }

    friend SPair<T> operator/(const T &val, const SPair<T> &r)
    {
        return SPair<T> (val / r.r, val / r.c);
    }

    SPair<T> &operator-=(const SPair<T> &r)
    {
        i -= r.i;
        j -= r.j;
        return *this;
    }

    SPair<T> &operator++()
    {
        i++;
        j++;
        return *this;
    }

    SPair<T> &operator--()
    {
        i--;
        j--;
        return *this;
    }

    SPair<T> operator++(int)
    {
        SPair<T> tmp(*this);
        operator++();
        return tmp;
    }

    SPair<T> operator--(int)
    {
        SPair<T> tmp(*this);
        operator--();
        return tmp;
    }

    SPair<T> operator-(const SSize &r) const
    {
        SPair<T> ret(*this);
        ret.i -= (T) (r.row);
        ret.j -= (T) (r.col);
        return ret;
    }

    SPair<T> operator+(const SSize &r) const
    {
        SPair<T> ret(*this);
        ret.i += (T) (r.row);
        ret.j += (T) (r.col);
        return ret;
    }

    SPair<T> &operator-=(const SSize &r)
    {
        i -= (T) (r.row);
        j -= (T) (r.col);
        return *this;
    }

    SPair<T> &operator+=(const SSize &r)
    {
        i += (T) (r.row);
        j += (T) (r.col);
        return *this;
    }

    SPair<T> &operator+=(const T & r)
    {
        i += r;
        j += r;
        return *this;
    }

    SPair<T> &operator-=(const T & r)
    {
        i -= r;
        j -= r;
        return *this;
    }

    SPair<T> &operator/=(const T & r)
    {
        i /= r;
        j /= r;
        return *this;
    }

    SPair<T> &operator/=(const SPair<T> & r)
    {
        i /= r.i;
        j /= r.j;
        return *this;
    }

    SPair<T> operator/(const T & r) const
    {
        SPair<T> ret(*this);
        ret.i /= r;
        ret.j /= r;
        return ret;
    }

    SPair<T> operator/(const SPair<T> & r) const
    {
        SPair<T> ret(*this);
        ret.i /= r.i;
        ret.j /= r.j;
        return ret;
    }

    SPair<T> operator*(const T & r) const
    {
        SPair<T> ret(*this);
        ret.i *= r;
        ret.j *= r;
        return ret;
    }

    SPair<T> operator*(const SPair<T> & r) const
    {
        SPair<T> ret(*this);
        ret.i *= r.i;
        ret.j *= r.j;
        return ret;
    }

    SPair<T> operator*(const SSize & r) const
    {
        SPair<T> ret(*this);
        ret.i *= T(r.row);
        ret.j *= T(r.col);
        return ret;
    }

    SPair<T> &operator*=(const T & r)
    {
        i *= r;
        j *= r;
        return *this;
    }

    SPair<T> &operator*=(const SPair<T> & r)
    {
        i *= r.i;
        j *= r.j;
        return *this;
    }

    bool operator!=(const SPair<T> &r) const
    {
        if (i != r.i || j != r.j)
            return true;
        return false;
    }

    bool operator==(const SPair<T> &r) const
    {
        if (i == r.i && j == r.j)
            return true;
        return false;
    }

    T dot(const SPair<T> &r) const
    {
        return x * r.x + y * r.y;
    }

    template<class N >
    SPair<N> convertType() const
    {
        SPair<N> ret;

        ret.r = N(r);
        ret.c = N(c);

        return ret;
    }

    void swap()
    {
        T t = i;
        i = j;
        j = t;
    }

    SPair<T> swapReturn() const
    {
        SPair<T> ret(*this);
        T t = ret.i;
        ret.i = ret.j;
        ret.j = t;

        return ret;
    }

    SPair<T> roundValues() const
    {
        SPair<T> ret(*this);

        ret.x = round(ret.x);
        ret.y = round(ret.y);

        return ret;
    }

    SPair<T> ceilValues() const
    {
        SPair<T> ret(*this);

        ret.x = ceil(ret.x);
        ret.y = ceil(ret.y);

        return ret;
    }

    SPair<T> floorValues() const
    {
        SPair<T> ret(*this);

        ret.x = floor(ret.x);
        ret.y = floor(ret.y);

        return ret;
    }

    SSize getSizeType() const
    {
        if (r < 0 || c < 0)
            throw CException(__FILE__, __LINE__, "Cannot convert to SSize type as the values are negative");
        return SSize((unsigned long) r, (unsigned long) c);
    }

    S2D<T> getS2DType() const
    {
        return S2D<T>(x, y);
    }

    S3D<T> getS3DType() const
    {
        return S3D<T>(x, y, 0);
    }

    T distanceSquared(const SPair<T> &o) const
    {
        T rr = r - o.r;
        T cc = c - o.c;
        return rr * rr + cc*cc;
    }

    T distance(const SPair<T> &o) const
    {
        return std::hypot(c - o.c, r - o.r);
    }

    T modSizeSquared() const
    {
        return r * r + c * c;
    }

    T modSize() const
    {
        return std::hypot(c, r);
    }

    friend SPair<T> getAbs(const SPair<T> &r)
    {
        return SPair<T>(getAbs(r.r), getAbs(r.c));
    }

    friend SPair<T> round(const SPair<T> &r)
    {
        return SPair<T>(round(r.r), round(r.c));
    }

    friend SPair<T> sqrt(const SPair<T> &r)
    {
        return SPair<T>(sqrt(r.r), sqrt(r.c));
    }

    T getProduct() const
    {
        return r*c;
    }

    bool containsZero() const
    {
        T eps = std::numeric_limits<T>::epsilon();
        return getAbs(r) <= eps || getAbs(c) <= eps;
    }

    SPair<T> clipBetween(const SPair<T> &minVal, const SPair<T> &maxVal) const
    {
        SPair<T> ret(*this);
        if (ret.r < minVal.r)
            ret.r = minVal.r;
        else if (ret.r > maxVal.r)
            ret.r = maxVal.r;

        if (ret.c < minVal.c)
            ret.c = minVal.c;
        else if (ret.c > maxVal.c)
            ret.c = maxVal.c;

        return ret;
    }

    bool isBoundedBetween(const SPair<T> &minVal, const SPair<T> &maxVal) const
    {
        if (r < minVal.r)
            return false;
        else if (r > maxVal.r)
            return false;

        if (c < minVal.c)
            return false;
        else if (c > maxVal.c)
            return false;

        return true;
    }

    bool contains(const T &v) const
    {
        return v == r || v == c;
    }

    bool containsNaN() const
    {
        return isNumberNAN(r) || isNumberNAN(c);
    }

    const T &getMin() const
    {
        return x < y ? x : y;
    }

    const T &getMax() const
    {
        return x > y ? x : y;
    }

    T &getMin()
    {
        return x < y ? x : y;
    }

    T &getMax()
    {
        return x > y ? x : y;
    }

    /**
     * The dataset are assume to in a coordinate system used for image map coordinates
     * @param ul1
     * @param lr1
     * @param ul2
     * @param lr2
     * @param unionUl
     * @param unionLr
     * @return will return true if there is a union between to two data sets
     */
    template<class N>
    static bool getUnion(const SPair<N> &ul1, const SPair<N> &lr1,
                         const SPair<N> &ul2, const SPair<N> &lr2,
                         SPair<N> &unionUl, SPair<N> &unionLr)
    {
        unionUl.c = (ul1.c > ul2.c) ? (ul1.c) : (ul2.c);
        unionUl.r = (ul1.r < ul2.r) ? (ul1.r) : (ul2.r);
        unionLr.c = (lr1.c < lr2.c) ? (lr1.c) : (lr2.c);
        unionLr.r = (lr1.r > lr2.r) ? (lr1.r) : (lr2.r);

        SPair<T> temp = (unionLr - unionUl);
        return temp.r < 0 && temp.c > 0;
    }

    template<class N>
    static SPair<N> getMin(const SPair<N> &o1, const SPair<N> &o2)
    {
        return SPair<N>(getMIN(o1.r, o2.r), getMIN(o1.c, o2.c));
    }

    template<class N>
    static SPair<N> getMax(const SPair<N> &o1, const SPair<N> &o2)
    {
        return SPair<N>(getMAX(o1.r, o2.r), getMAX(o1.c, o2.c));
    }
};

} // namespace ultra