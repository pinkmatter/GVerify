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
#include <ul_Vector.h>
#include <ul_KeyValue.h>

namespace ultra
{

struct SColor
{
private:

    template<class T>
    static bool sortFunc(const SKeyValue<T, SColor>&t1, const SKeyValue<T, SColor> &t2)
    {
        return t1.k < t2.k;
    }

public:

    union
    {
        unsigned char c[4];
        unsigned int argb;
    };
    SColor();
    SColor(unsigned int argb);
    SColor(unsigned char b, unsigned char g, unsigned char r, unsigned char a = 0);
    SColor(const SColor & r);
    ~SColor();

    SColor &operator=(const SColor &r);
    SColor &operator=(unsigned int col);
    SColor &operator-=(const SColor &r);
    SColor &operator+=(const SColor &r);
    SColor operator-(const SColor &r) const;
    SColor operator+(const SColor &r) const;

    unsigned char red() const;
    unsigned char blue() const;
    unsigned char green() const;
    unsigned char alpha() const;
    unsigned char &red();
    unsigned char &blue();
    unsigned char &green();
    unsigned char &alpha();
    operator unsigned int() const;
    operator unsigned char() const;
    std::string getHexString() const;
    friend std::ostream &operator<<(std::ostream &stream, const SColor & o);

    static SColor randomColorRgb();
    static SColor randomColorRgba();
};

} // namespace utlra
