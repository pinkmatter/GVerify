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

#include "ul_Size.h"

#include "ul_Utility.h"

namespace ultra
{

SSize::SSize() :
row(0),
col(0)
{
}

SSize::~SSize()
{
}

SSize::SSize(unsigned long rowCol) :
row(rowCol),
col(rowCol)
{
}

SSize::SSize(unsigned long _row, unsigned long _col) :
row(_row),
col(_col)
{
}

SSize::SSize(const SSize &r) :
row(r.row),
col(r.col)
{
}

SSize::SSize(SSize && r) :
row(std::move(r.row)),
col(std::move(r.col))
{
}

std::ostream &operator<<(std::ostream &stream, const SSize &o)
{
    stream << ("col = " + toString(o.col) + " row = " + toString(o.row));
    return stream;
}

unsigned long SSize::min() const
{
    return (row < col) ? row : col;
}

unsigned long SSize::max() const
{
    return (row > col) ? row : col;
}

void SSize::swap()
{
    unsigned int t = row;
    row = col;
    col = t;
}

bool SSize::isSquare() const
{
    return row == col;
}

SSize SSize::getSwap() const
{
    return SSize(col, row);
}

bool SSize::containsZero() const
{
    return ((row == 0) || (col == 0));
}

unsigned long SSize::getProduct() const
{
    return row*col;
}

SSize &SSize::operator=(const SSize &r)
{
    if (this == &r)
        return *this;
    row = r.row;
    col = r.col;
    return *this;
}

SSize &SSize::operator=(SSize && r)
{
    if (this == &r)
        return *this;
    row = std::move(r.row);
    col = std::move(r.col);
    return *this;
}

SSize &SSize::operator/=(const SSize &r)
{
    row /= r.row;
    col /= r.col;
    return *this;
}

SSize &SSize::operator/=(const unsigned long &r)
{
    row /= r;
    col /= r;
    return *this;
}

SSize &SSize::operator*=(const SSize &r)
{
    row *= r.row;
    col *= r.col;
    return *this;
}

SSize &SSize::operator*=(const unsigned long &r)
{
    row *= r;
    col *= r;
    return *this;
}

SSize &SSize::operator+=(const SSize &r)
{
    row += r.row;
    col += r.col;
    return *this;
}

SSize &SSize::operator+=(const unsigned long &r)
{
    row += r;
    col += r;
    return *this;
}

SSize &SSize::operator-=(const SSize &r)
{
    row -= r.row;
    col -= r.col;
    return *this;
}

SSize &SSize::operator-=(const unsigned long &r)
{
    row -= r;
    col -= r;
    return *this;
}

SSize &SSize::operator++()
{
    row++;
    col++;
    return *this;
}

SSize SSize::operator++(int d)
{
    SSize ret(*this);
    operator++();
    return ret;
}

SSize &SSize::operator--()
{
    row--;
    col--;
    return *this;
}

SSize SSize::operator--(int d)
{
    SSize ret(*this);
    operator--();
    return ret;
}

SSize SSize::operator/(const SSize &r) const
{
    SSize ret = SSize(*this);
    ret.row /= r.row;
    ret.col /= r.col;
    return ret;
}

SSize SSize::operator/(const unsigned long &r) const
{
    SSize ret = SSize(*this);
    ret.row /= r;
    ret.col /= r;
    return ret;
}

SSize SSize::operator*(const SSize &r) const
{
    SSize ret = SSize(*this);
    ret.row *= r.row;
    ret.col *= r.col;
    return ret;
}

SSize SSize::operator*(const unsigned long &r) const
{
    SSize ret = SSize(*this);
    ret.row *= r;
    ret.col *= r;
    return ret;
}

SSize SSize::operator+(const SSize &r) const
{
    SSize ret = SSize(*this);
    ret.row += r.row;
    ret.col += r.col;
    return ret;
}

SSize SSize::operator+(const unsigned long &r) const
{
    SSize ret = SSize(*this);
    ret.row += r;
    ret.col += r;
    return ret;
}

SSize SSize::operator-(const SSize &r) const
{
    SSize ret = SSize(*this);
    ret.row -= r.row;
    ret.col -= r.col;
    return ret;
}

SSize SSize::operator-(const unsigned long &r) const
{
    SSize ret = SSize(*this);
    ret.row -= r;
    ret.col -= r;
    return ret;
}

bool SSize::operator!=(const SSize &r) const
{
    if (col != r.col || row != r.row)
        return true;
    return false;
}

bool SSize::operator==(const SSize &r) const
{
    if (col == r.col && row == r.row)
        return true;
    return false;
}

unsigned long long SSize::distanceSquared(const SSize &r) const
{
    unsigned long long rr = (row > r.row) ? (row - r.row) : (r.row - row);
    unsigned long long cc = (col > r.col) ? (col - r.col) : (r.col - col);
    return rr * rr + cc * cc;
}

SSize SSize::clipBetween(const SSize &minVal, const SSize &maxVal) const
{
    return SSize(clip<unsigned long>(row, minVal.row, maxVal.row),
                 clip<unsigned long>(col, minVal.col, maxVal.col));
}

bool SSize::isBoundedBetween(const SSize &minVal, const SSize &maxVal) const
{
    if (row < minVal.row)
        return false;
    else if (row > maxVal.row)
        return false;

    if (col < minVal.col)
        return false;
    else if (col > maxVal.col)
        return false;

    return true;
}

SSize operator+(unsigned long val, const SSize &r)
{
    return SSize(val + r.row, val + r.col);
}

SSize operator-(unsigned long val, const SSize &r)
{
    return SSize(val - r.row, val - r.col);
}

SSize operator*(unsigned long val, const SSize &r)
{
    return SSize(val * r.row, val * r.col);
}

SSize operator/(unsigned long val, const SSize &r)
{
    return SSize(val / r.row, val / r.col);
}

} // namespace ultra
