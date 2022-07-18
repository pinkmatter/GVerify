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

#include <sstream>
#include <ul_Exception.h>

namespace ultra
{

template<class T>
struct S3D
{

    union
    {
        T x;
        T sample;
        T lon;
    };

    union
    {
        T y;
        T line;
        T lat;
    };

    union
    {
        T z;
        T height;
    };

    S3D(T x_sample, T y_line, T z_height)
    {
        sample = x_sample;
        line = y_line;
        height = z_height;
    }

    S3D(const S3D<T> &r) :
    x(r.x),
    y(r.y),
    z(r.z)
    {
    }

    S3D(S3D<T> &&r) :
    x(std::move(r.x)),
    y(std::move(r.y)),
    z(std::move(r.z))
    {
    }

    S3D(const T &val)
    {
        sample = line = height = val;
    }

    S3D()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    ~S3D()
    {
    }

    S3D<T> operator+(const S3D<T> &r) const
    {
        S3D<T> ret(*this);

        ret.x += r.x;
        ret.y += r.y;
        ret.height += r.height;

        return ret;
    }

    S3D<T> operator-(const S3D<T> &r) const
    {
        S3D<T> ret(*this);

        ret.x -= r.x;
        ret.y -= r.y;
        ret.height -= r.height;

        return ret;
    }

    S3D<T> operator/(const T &r) const
    {
        S3D<T> ret(*this);

        ret.x /= r;
        ret.y /= r;
        ret.height /= r;

        return ret;
    }

    S3D<T> operator*(const T &r) const
    {
        S3D<T> ret(*this);

        ret.x *= r;
        ret.y *= r;
        ret.height *= r;

        return ret;
    }

    S3D<T> &operator-=(const T & r)
    {
        x -= r;
        y -= r;
        z -= r;

        return *this;
    }

    S3D<T> &operator+=(const T & r)
    {
        x += r;
        y += r;
        z += r;

        return *this;
    }

    S3D<T> &operator/=(const T & r)
    {
        x /= r;
        y /= r;
        z /= r;

        return *this;
    }

    S3D<T> &operator*=(const T & r)
    {
        x *= r;
        y *= r;
        z *= r;

        return *this;
    }

    S3D<T> &operator=(const S3D<T> &r)
    {
        if (this == &r)
            return *this;
        x = r.x;
        y = r.y;
        z = r.z;

        return *this;
    }

    S3D<T> &operator=(S3D<T> &&r)
    {
        if (this == &r)
            return *this;
        x = std::move(r.x);
        y = std::move(r.y);
        z = std::move(r.z);

        return *this;
    }

    S3D<T> &operator=(const T &r)
    {
        x = r;
        y = r;
        z = r;

        return *this;
    }

    S3D<T> normalize() const
    {
        T s = distance();
        return S3D<T>(x / s, y / s, z / s);
    }

    S3D<T> cross(const S3D<T> &s) const
    {
        return S3D<T>(y * s.z - z * s.y,
            z * s.x - x * s.z,
            x * s.y - y * s.x);
    }

    T dot(const S3D<T> &s) const
    {
        return x * s.x + y * s.y + z * s.z;
    }

    bool operator==(const S3D<T> &r) const
    {
        return x == r.x && y == r.y && z == r.z;
    }

    bool operator!=(const S3D<T> &r) const
    {
        return !(operator==(r));
    }

    T &get(unsigned long index)
    {
        switch (index)
        {
        case 0:return x;
        case 1:return y;
        case 2:return z;
        }
        throw CException(__FILE__, __LINE__, "Invalid index, expecting 0,1 or 2");
    }

    const T &get(unsigned long index) const
    {
        switch (index)
        {
        case 0:return x;
        case 1:return y;
        case 2:return z;
        }
        throw CException(__FILE__, __LINE__, "Invalid index, expecting 0,1 or 2");
    }

    T distance() const
    {
        return sqrt(distanceSquared());
    }

    T distanceSquared() const
    {
        return x * x + y * y + z * z;
    }

    friend std::ostream &operator<<(std::ostream &stream, const S3D<T> & o)
    {
        stream << "X " << toString(o.x) << " Y " << toString(o.y) << " Z " << toString(o.z);

        return stream;
    }
};

} // namespace ultra
