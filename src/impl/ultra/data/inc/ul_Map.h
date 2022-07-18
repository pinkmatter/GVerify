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

#include "ul_SmartVector.h"
#include "ul_KeyValue.h"
#include "ul_Exception.h"

namespace ultra
{

template<class K, class V>
class CMap
{
private:
    CSmartVector<SKeyValue<K, V> > m_map;

public:

    CMap()
    {
        clear();
    }

    CMap(const CMap<K, V> &r) :
    m_map(r.m_map)
    {
    }

    CMap(CMap<K, V> &&r) :
    m_map(std::move(r.m_map))
    {
    }

    CMap(const CVector<SKeyValue<K, V> > &r) :
    m_map(r)
    {
    }

    CMap(const std::initializer_list<SKeyValue<K, V> > &l) :
    m_map(l)
    {
    }

    CMap(CSmartVector<SKeyValue<K, V> > &&r) :
    m_map(std::move(r))
    {
    }

    bool operator==(const CMap<K, V> &r) const
    {
        if (size() != r.size())
            return false;
        for (const auto &kv : * this)
        {
            if (!r.contains(kv.k))
                return false;
            if (kv.v != r.get(kv.k))
                return false;
        }
        return true;
    }

    bool operator!=(const CMap<K, V> &r) const
    {
        return !((*this) == r);
    }

    CMap<K, V> &operator=(const CMap<K, V> &r)
    {
        if (this == &r)
            return *this;
        m_map = r.m_map;
        return *this;
    }

    CMap<K, V> &operator=(CMap<K, V> &&r)
    {
        if (this == &r)
            return *this;
        m_map = std::move(r.m_map);
        return *this;
    }

    CMap<K, V> &operator=(const CVector<SKeyValue<K, V> > &r)
    {
        if (&m_map == &r)
            return *this;
        m_map = r;
        return *this;
    }

    CMap<K, V> &operator=(const CSmartVector<SKeyValue<K, V> > &r)
    {
        if (&m_map == &r)
            return *this;
        m_map = r;
        return *this;
    }

    CMap<K, V> &operator=(CSmartVector<SKeyValue<K, V> > &&r)
    {
        if (&m_map == &r)
            return *this;
        m_map = std::move(r);
        return *this;
    }

    virtual ~CMap()
    {
        clear();
    }

    void clear()
    {
        m_map.clear();
    }

    /**
     * This will not clear any memory, however the
     * <code>size</code> marker will be reset to zero
     */
    void reset()
    {
        m_map.reset();
    }

    void putAll(const CMap<K, V> &map)
    {
        for (const auto &kv : map)
        {
            put(kv.k, kv.v);
        }
    }

    void putAll(CMap<K, V> &&map)
    {
        for (const auto &kv : map)
        {
            put(std::move(kv.k), std::move(kv.v));
        }
        map.clear();
    }

    void put(const K &k, const V &v)
    {
        if (contains(k))
            get(k) = v;
        else
            m_map.pushBack(SKeyValue<K, V>(k, v));
    }

    void put(const K &k, V &&v)
    {
        if (contains(k))
            get(k) = std::move(v);
        else
            m_map.pushBack(SKeyValue<K, V>(k, std::move(v)));
    }

    void put(K &&k, const V &v)
    {
        if (contains(k))
            get(k) = v;
        else
            m_map.pushBack(SKeyValue<K, V>(std::move(k), v));
    }

    void put(K &&k, V &&v)
    {
        if (contains(k))
            get(k) = std::move(v);
        else
            m_map.pushBack(std::move(SKeyValue<K, V>(std::move(k), std::move(v))));
    }

    void remove(const K &k)
    {
        if (!contains(k))
            return;

        unsigned long s = size();
        for (unsigned long t = 0; t < s; t++)
        {
            if (m_map[t].k == k)
            {
                m_map.remove(t);
                break;
            }
        }
    }

    bool contains(const K &k) const
    {
        unsigned long s = size();
        for (unsigned long t = 0; t < s; t++)
        {
            if (m_map[t].k == k)
                return true;
        }
        return false;
    }

    unsigned long size() const
    {
        return m_map.size();
    }

    CVector<K> keys() const
    {
        unsigned long s = size();
        CVector<K> ks(s);
        for (unsigned long t = 0; t < s; t++)
        {
            ks[t] = m_map[t].k;
        }
        return ks;
    }

    CVector<V> values() const
    {
        unsigned long s = size();
        CVector<V> vs(s);
        for (unsigned long t = 0; t < s; t++)
        {
            vs[t] = m_map[t].v;
        }
        return vs;
    }

    const V& getOrDefault(const K &k, const V &defaultValue) const
    {
        const V *p = getPtr(k);
        return 0 == nullptr ? defaultValue : *p;
    }

    V &getOrDefault(const K &k, const V &defaultValue)
    {
        V *p = getPtr(k);
        return 0 == nullptr ? defaultValue : *p;
    }

    const V* getPtr(const K &k) const
    {
        unsigned long s = size();
        for (unsigned long t = 0; t < s; t++)
        {
            if (m_map[t].k == k)
                return &m_map[t].v;
        }
        return nullptr;
    }

    V* getPtr(const K &k)
    {
        unsigned long s = size();
        for (unsigned long t = 0; t < s; t++)
        {
            if (m_map[t].k == k)
                return &m_map[t].v;
        }
        return nullptr;
    }

    V& get(const K &k)
    {
        unsigned long s = size();
        for (unsigned long t = 0; t < s; t++)
        {
            if (m_map[t].k == k)
                return m_map[t].v;
        }
        throw CException(__FILE__, __LINE__, "Key '" + toString(k) + "' does not exist in the map");
    }

    const V& get(const K &k) const
    {
        unsigned long s = size();
        for (unsigned long t = 0; t < s; t++)
        {
            if (m_map[t].k == k)
                return m_map[t].v;
        }
        throw CException(__FILE__, __LINE__, "Key '" + toString(k) + "' does not exist in the map");
    }

    const SKeyValue<K, V> *begin() const
    {
        return m_map.begin();
    }

    const SKeyValue<K, V> *end() const
    {
        return m_map.end();
    }

    SKeyValue<K, V> *begin()
    {
        return m_map.begin();
    }

    SKeyValue<K, V> *end()
    {
        return m_map.end();
    }

    CMap<K, V> sort() const
    {
        return sort([](const SKeyValue<K, V> &o1, const SKeyValue<K, V> &o2)->bool
        {
            return o1.k < o2.k;
        });
    }

    CMap<K, V> sort(bool (*func)(const SKeyValue<K, V> &, const SKeyValue<K, V> &)) const
    {
        CMap<K, V> result(*this);
        result.m_map = result.m_map.sort(func);
        return result;
    }

    const CSmartVector<SKeyValue<K, V> > &getDataReference() const
    {
        return m_map;
    }

    friend std::ostream &operator<<(std::ostream &stream, const CMap<K, V> &o)
    {
        stream << o.m_map;
        return stream;
    }
};

} // namespace ultra
