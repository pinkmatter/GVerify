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

namespace ultra
{

template<class K, class V = K>
struct SKeyValue
{
    K k;
    V v;

    SKeyValue()
    {
    }

    SKeyValue(const K &kk, const V &vv) :
    k(kk),
    v(vv)
    {
    }

    SKeyValue(K &&kk, V &&vv) :
    k(std::move(kk)),
    v(std::move(vv))
    {
    }

    SKeyValue(const K &kk, V &&vv) :
    k(kk),
    v(std::move(vv))
    {
    }

    SKeyValue(K &&kk, const V &vv) :
    k(std::move(kk)),
    v(vv)
    {
    }

    SKeyValue(const SKeyValue<K, V> &r) :
    k(r.k),
    v(r.v)
    {
    }

    SKeyValue(SKeyValue<K, V> &&r) :
    k(std::move(r.k)),
    v(std::move(r.v))
    {
    }

    ~SKeyValue()
    {

    }

    SKeyValue<K, V>& operator=(SKeyValue<K, V> &&r)
    {
        if (this == &r)
            return *this;
        k = std::move(r.k);
        v = std::move(r.v);
        return *this;
    }

    SKeyValue<K, V>& operator=(const SKeyValue<K, V> &r)
    {
        if (this == &r)
            return *this;
        k = r.k;
        v = r.v;
        return *this;
    }

    bool operator==(const SKeyValue<K, V> &r) const
    {
        return ((k == r.k) && (v == r.v));
    }

    bool operator!=(const SKeyValue<K, V> &r) const
    {
        return !((k == r.k) && (v == r.v));
    }

    friend std::ostream &operator<<(std::ostream &stream, const SKeyValue<K, V> & o)
    {
        stream << ("Key = " + toString(o.k) + " Value = " + toString(o.v));
        return stream;
    }

};

} //namespace ultra
