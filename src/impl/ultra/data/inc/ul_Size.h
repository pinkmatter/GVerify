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
#include <ul_Utility.h>

namespace ultra
{

struct SSize
{
    unsigned long row;
    unsigned long col;
    SSize();
    ~SSize();
    SSize(unsigned long rowCol);
    SSize(unsigned long _row, unsigned long _col);
    SSize(const SSize & r);
    SSize(SSize && r);
    friend std::ostream &operator<<(std::ostream &stream, const SSize & o);
    SSize &operator=(const SSize &r);
    SSize &operator=(SSize &&r);
    bool operator!=(const SSize &r) const;
    bool operator==(const SSize &r) const;

    SSize &operator/=(const SSize &r);
    SSize &operator/=(const unsigned long &r);
    SSize &operator*=(const SSize &r);
    SSize &operator*=(const unsigned long &r);
    SSize &operator+=(const SSize &r);
    SSize &operator+=(const unsigned long &r);
    SSize &operator-=(const SSize &r);
    SSize &operator-=(const unsigned long &r);

    SSize operator/(const SSize &r) const;
    SSize operator/(const unsigned long &r) const;
    SSize operator*(const SSize &r) const;
    SSize operator*(const unsigned long &r)const;
    SSize operator+(const SSize &r) const;
    SSize operator+(const unsigned long &r)const;
    SSize operator-(const SSize &r) const;
    SSize operator-(const unsigned long &r)const;

    friend SSize operator+(unsigned long val, const SSize &r);
    friend SSize operator-(unsigned long val, const SSize &r);
    friend SSize operator*(unsigned long val, const SSize &r);
    friend SSize operator/(unsigned long val, const SSize &r);
    
    SSize &operator++();
    SSize operator++(int d);
    SSize &operator--();
    SSize operator--(int d);

    unsigned long min() const;
    unsigned long max() const;
    void swap();
    bool isSquare() const;
    SSize getSwap() const;
    bool containsZero() const;
    unsigned long getProduct() const;
    unsigned long long distanceSquared(const SSize &r) const;
    SSize clipBetween(const SSize &minVal, const SSize &maxVal) const;
    bool isBoundedBetween(const SSize &minVal, const SSize &maxVal) const;
};

} // namespace ultra
