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
struct S2D
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

    S2D(T x_sample, T y_line)
    {
        sample = x_sample;
        line = y_line;
    }

    S2D(const S2D<T> &r) :
    x(r.x),
    y(r.y)
    {
    }

    S2D(S2D<T> &&r) :
    x(std::move(r.x)),
    y(std::move(r.y))
    {
    }

    S2D()
    {
        x = 0;
        y = 0;
    }

    ~S2D()
    {
    }

    friend std::ostream &operator<<(std::ostream &stream, const S2D<T> & o)
    {
        stream << "X " << toString(o.x) << " Y " << toString(o.y);
        return stream;
    }

    S2D<T> operator+(const S2D<T> &r) const
    {
        S2D<T> ret(*this);
        ret.x += r.x;
        ret.y += r.y;
        return ret;
    }

    S2D<T> operator-(const S2D<T> &r) const
    {
        S2D<T> ret(*this);
        ret.x -= r.x;
        ret.y -= r.y;
        return ret;
    }

    S2D<T> &operator-=(const T & r)
    {
        x -= r;
        y -= r;

        return *this;
    }

    S2D<T> &operator+=(const T & r)
    {
        x += r;
        y += r;

        return *this;
    }

    S2D<T> &operator-=(const S2D<T> & r)
    {
        x -= r.x;
        y -= r.y;

        return *this;
    }

    S2D<T> &operator+=(const S2D<T> & r)
    {
        x += r.x;
        y += r.y;

        return *this;
    }

    S2D<T> &operator=(const S2D<T> &r)
    {
        if (this == &r)
            return *this;
        x = r.x;
        y = r.y;

        return *this;
    }

    S2D<T> &operator=(S2D<T> &&r)
    {
        if (this == &r)
            return *this;
        x = std::move(r.x);
        y = std::move(r.y);

        return *this;
    }

    T &get(unsigned long index)
    {
        switch (index)
        {
        case 0:return x;
        case 1:return y;
        }
        throw CException(__FILE__, __LINE__, "Invalid index, expecting 0 or 1");
    }

    const T &get(unsigned long index) const
    {
        switch (index)
        {
        case 0:return x;
        case 1:return y;
        }
        throw CException(__FILE__, __LINE__, "Invalid index, expecting 0 or 1");
    }

    bool operator==(const S2D<T> &r) const
    {
        return x == r.x && y == r.y;
    }

    bool operator!=(const S2D<T> &r) const
    {
        return !(operator==(r));
    }
};

} // namespace ultra