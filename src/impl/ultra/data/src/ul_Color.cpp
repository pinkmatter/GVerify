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

#include "ul_Color.h"

#include <ul_Utility.h>

namespace ultra
{

SColor::SColor()
{
    this->argb = 0;
}

SColor::SColor(unsigned int argb)
{
    this->argb = argb;
}

SColor::~SColor()
{

}

SColor::SColor(unsigned char b, unsigned char g, unsigned char r, unsigned char a)
{
    this->c[0] = b;
    this->c[1] = g;
    this->c[2] = r;
    this->c[3] = a;
}

SColor::SColor(const SColor &r)
{
    this->argb = r.argb;
}

SColor &SColor::operator=(const SColor &r)
{
    if (this == &r)
        return *this;
    this->argb = r.argb;
    return *this;
}

SColor &SColor::operator=(unsigned int col)
{
    this->argb = col;
    return *this;
}

unsigned char &SColor::red()
{
    unsigned char *p = &c[2];
    return *p;
}

unsigned char &SColor::green()
{
    unsigned char *p = &c[1];
    return *p;
}

unsigned char &SColor::blue()
{
    unsigned char *p = &c[0];
    return *p;
}

unsigned char &SColor::alpha()
{
    unsigned char *p = &c[3];
    return *p;
}

unsigned char SColor::red() const
{
    return c[2];
}

unsigned char SColor::green() const
{
    return c[1];
}

unsigned char SColor::blue() const
{
    return c[0];
}

unsigned char SColor::alpha() const
{
    return c[3];
}

SColor &SColor::operator-=(const SColor &r)
{
    for (int x = 0; x < 4; x++)
        c[x] -= r.c[x];
    return *this;
}

SColor &SColor::operator+=(const SColor &r)
{
    for (int x = 0; x < 4; x++)
        c[x] += r.c[x];
    return *this;
}

SColor SColor::operator-(const SColor &r) const
{
    SColor ret(*this);
    for (int x = 0; x < 4; x++)
        ret.c[x] -= r.c[x];
    return ret;
}

SColor SColor::operator+(const SColor &r) const
{
    SColor ret(*this);
    for (int x = 0; x < 4; x++)
        ret.c[x] += r.c[x];
    return ret;
}

std::string SColor::getHexString() const
{
    return frontZeroPad(toHexString(argb), 8);
}

std::ostream &operator<<(std::ostream &stream, const SColor & o)
{
    stream << "0x" << frontZeroPad(toHexString(o.argb), 8);
    return stream;
}

SColor SColor::randomColorRgb()
{
    return SColor(rand() % 255, rand() % 255, rand() % 255, 0);
}

SColor SColor::randomColorRgba()
{
    return SColor(rand() % 255, rand() % 255, rand() % 255, rand() % 255);
}

SColor::operator unsigned int() const
{
    return argb;
}

SColor::operator unsigned char() const
{
    return (red() + blue() + green()) / 3;
}

} //namespace ultra
