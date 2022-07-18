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
#include <ostream>
#include <cmath>
#include <ul_Exception.h>

namespace ultra
{

template<class T>
struct SComplex
{
    T re;
    T im;

    SComplex(const SComplex<T> &r) :
    re(r.re),
    im(r.im)
    {
    }

    SComplex(SComplex<T> &&r) :
    re(std::move(r.re)),
    im(std::move(r.im))
    {
    }

    template<class N>
    SComplex(const SComplex<N> &r) :
    re((T) r.re),
    im((T) r.im)
    {
    }

    SComplex(T real = T(0), T imag = T(0)) :
    re(real),
    im(imag)
    {
    }

    ~SComplex()
    {
    }

    // OP ++

    SComplex<T> &operator++()//pre-inc
    {
        re++;
        return *this;
    }

    SComplex<T> operator++(int) //post-inc
    {
        return SComplex<T>(re++, im);
    }

    // OP --

    SComplex<T> &operator--()
    {
        re--;
        return *this;
    }

    SComplex<T> operator--(int)
    {
        return SComplex<T>(re--, im);
    }

    // OP =

    template<class N>
    SComplex<T> &operator=(const N &real)
    {
        re = (T) real;
        im = T(0);
        return *this;
    }

    SComplex<T> &operator=(const T &real)
    {
        re = real;
        im = T(0);
        return *this;
    }

    SComplex<T> &operator=(const SComplex<T> &r)
    {
        if (this == &r)
            return *this;
        re = r.re;
        im = r.im;
        return *this;
    }

    template<class N>
    SComplex<T> &operator=(const SComplex<N> &r)
    {
        re = (T) r.re;
        im = (T) r.im;
        return *this;
    }

    SComplex<T> &operator=(SComplex<T> &&r)
    {
        if (this == &r)
            return *this;
        re = std::move(r.re);
        im = std::move(r.im);
        return *this;
    }

    // OP ==

    bool operator==(const SComplex<T> &r) const
    {
        return getAbs(re - r.re) <= std::numeric_limits<T>::epsilon() && getAbs(im - r.im) <= std::numeric_limits<T>::epsilon();
    }

    bool operator!=(const SComplex<T> &r) const
    {
        return !((*this) == r);
    }

    // OP +

    SComplex<T> operator+(const SComplex<T> &r) const
    {
        SComplex<T> ret(*this);
        ret.re += r.re;
        ret.im += r.im;
        return ret;
    }

    SComplex<T> operator+(const T &r) const
    {
        SComplex<T> ret(*this);
        ret.re += r;
        return ret;
    }

    SComplex<T> &operator+=(const SComplex<T> &r)
    {
        re += r.re;
        im += r.im;
        return *this;
    }

    SComplex<T> &operator+=(const T &r)
    {
        re += r;
        return *this;
    }

    friend SComplex<T> operator+(const T &val, const SComplex<T> &r)
    {
        return SComplex<T> (val + r.re, r.im);
    }

    // OP -

    SComplex<T> operator-(const SComplex<T> &r) const
    {
        SComplex<T> ret(*this);
        ret.re -= r.re;
        ret.im -= r.im;
        return ret;
    }

    SComplex<T> operator-(const T &r) const
    {
        SComplex<T> ret(*this);
        ret.re -= r;
        return ret;
    }

    SComplex<T> &operator-=(const SComplex<T> &r)
    {
        re -= r.re;
        im -= r.im;
        return *this;
    }

    SComplex<T> &operator-=(const T &r)
    {
        re -= r;
        return *this;
    }

    friend SComplex<T> operator-(const T &val, const SComplex<T> &r)
    {
        return SComplex<T> (val - r.re, -r.im);
    }

    SComplex<T> operator-() const
    {
        return SComplex<T> (-this->re, -this->im);
    }

    // OP *

    SComplex<T> operator*(const SComplex<T> &r) const
    {
        SComplex<T> ret;
        ret.re = re * r.re - im * r.im;
        ret.im = re * r.im + im * r.re;
        return ret;
    }

    SComplex<T> operator*(const T &r) const
    {
        SComplex<T> ret(*this);
        ret.re *= r;
        ret.im *= r;
        return ret;
    }

    SComplex<T> &operator*=(const SComplex<T> &r)
    {
        SComplex<T> t = *this;
        re = t.re * r.re - t.im * r.im;
        im = t.re * r.im + t.im * r.re;
        return *this;
    }

    SComplex<T> &operator*=(const T &r)
    {
        re *= r;
        im *= r;
        return *this;
    }

    friend SComplex<T> operator*(const T &val, const SComplex<T> &r)
    {
        return SComplex<T> (val * r.re, val * r.im);
    }

    // OP /

    SComplex<T> operator/(const SComplex<T> &r) const
    {
        SComplex<T> rc = r.conjugate();
        SComplex<T> t = (*this) * rc;
        T div = (r.re * r.re) + (r.im * r.im);
        if (getAbs(div) <= std::numeric_limits<T>::epsilon())
        {
            if (div < T(0))
            {
                div = -std::numeric_limits<T>::epsilon();
            }
            else
            {
                div = std::numeric_limits<T>::epsilon();
            }
        }
        t.re /= div;
        t.im /= div;
        return t;
    }

    SComplex<T> operator/(const T &r) const
    {
        T rr = (T) r;
        SComplex<T> ret(*this);
        if (getAbs(r) <= std::numeric_limits<T>::epsilon())
        {
            if (r < 0)
                rr = (T) (-std::numeric_limits<T>::epsilon());
            else
                rr = (T) std::numeric_limits<T>::epsilon();
        }
        ret.re /= rr;
        ret.im /= rr;
        return ret;
    }

    SComplex<T> &operator/=(const SComplex<T> &r)
    {
        SComplex<T> rc = r.conjugate();
        (*this) *= rc;
        T div = (r.re * r.re) + (r.im * r.im);
        if (getAbs(div) <= std::numeric_limits<T>::epsilon())
        {
            if (div < T(0))
                div = -std::numeric_limits<T>::epsilon();
            else
                div = std::numeric_limits<T>::epsilon();
        }
        re /= div;
        im /= div;
        return *this;
    }

    SComplex<T> &operator/=(const T &r)
    {
        T rr = (T) r;
        if (getAbs(r) <= std::numeric_limits<T>::epsilon())
        {
            if (r < 0)
                rr = (T) (-std::numeric_limits<T>::epsilon());
            else
                rr = (T) std::numeric_limits<T>::epsilon();
        }
        re /= rr;
        im /= rr;
        return *this;
    }

    friend SComplex<T> operator/(const T &val, const SComplex<T> &r)
    {
        return SComplex<T>((T) val) / r;
    }

    template<class N>
    SComplex<N> convertType() const
    {
        return SComplex<T>((N) re, (N) im);
    }

    bool hasImaginary() const
    {
        return getAbs(im) > std::numeric_limits<T>::epsilon();
    }

    bool hasReal() const
    {
        return getAbs(re) > std::numeric_limits<T>::epsilon();
    }

    bool isRealOnly() const
    {
        return getAbs(im) <= std::numeric_limits<T>::epsilon() && getAbs(re) > std::numeric_limits<T>::epsilon();
    }

    bool isImaginaryOnly() const
    {
        return getAbs(re) <= std::numeric_limits<T>::epsilon() && getAbs(im) > std::numeric_limits<T>::epsilon();
    }

    operator T() const
    {
        if (hasImaginary())
            throw CException(__FILE__, __LINE__, "Cannot cast " + toString(*this) + " to T as it has imaginary data");
        return re;
    }

    friend std::ostream &operator<<(std::ostream &stream, const SComplex<T> & o)
    {
        bool r = o.hasReal();
        bool i = o.hasImaginary();
        if (r && i)
            stream << "(" << o.re << "," << o.im << "i)";
        else if (r)
            stream << o.re;
        else if (i)
            stream << "(" << o.im << "i)";
        else
            stream << "0";
        return stream;
    }

    SComplex<T> conjugate() const
    {
        return SComplex<T > (this->re, -1 * this->im);
    }

    T phase() const
    {
        return (T) atan2(im, re);
    }

    T modSquared() const
    {
        return re * re + im * im;
    }

    T mod() const
    {
        return hypot(re, im);
    }

    friend SComplex<T> sqrt(const SComplex<T> &z)
    {
        if (getAbs(z.im) <= std::numeric_limits<T>::epsilon())
        {
            T re = std::sqrt(getAbs(z.re));
            if (z.re < T(0))
                return SComplex<T>(0, re);
            else
                return SComplex<T>(re, 0);
        }
        else
        {
            T r = z.mod();
            SComplex<T> zr = z + r;
            SComplex<T> ret = (zr / zr.mod()) * std::sqrt(r);
            return ret;
        }
    }

    friend SComplex<T> pow(const SComplex<T> &o, const SComplex<T> &p)
    {
        T a = o.re;
        T b = o.im;
        T c = p.re;
        T d = p.im;

        // (a*a+b*b)^(c/2) * e^(-d)*arg(a+ib) * { cos[c*arg(a+ib)+0.5d*ln(a*a+b*b)] + isin[c*arg(a+ib)+0.5d*ln(a*a+b*b)] }
        T v1 = a * a + b * b;
        T v2 = pow(v1, c / T(2.0));
        T v3 = o.phase();
        T v4 = v2 * exp(-d * v3);
        T v5 = 0;
        if (v1 > std::numeric_limits<T>::epsilon())
            v5 = c * v3 + T(0.5) * d * log(v1);
        return SComplex<T>(v4 * cos(v5), v4 * sin(v5));
    }

    friend T getAbs(const SComplex<T> &o)
    {
        return o.mod();
    }

    friend SComplex<T> getMAX(const SComplex<T> &v1, const SComplex<T> &v2)
    {
        return v1.modSquared() > v2.modSquared() ? v1 : v2;
    }

    friend SComplex<T> getMIN(const SComplex<T> &v1, const SComplex<T> &v2)
    {
        return v1.modSquared() < v2.modSquared() ? v1 : v2;
    }

    SComplex<T> round() const
    {
        return SComplex<T>(std::round(re), std::round(im));
    }

    static SComplex<T> fromMagPhase(const T &mag, const T &phase)
    {
        return SComplex<T>(mag * cos(phase), mag * sin(phase));
    }
};

} // namespace ultra
