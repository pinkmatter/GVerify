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

#include "ul_Odl.h"

#include <algorithm>
#include <deque>
#include "ul_Logger.h"
#include "ul_Utility.h"
#include "ul_File.h"
#include <fcntl.h>

namespace ultra
{

COdl::SOdlItem::SOdlItem()
{
    m_isStr = false;
    m_isInt = false;
    m_isDouble = false;
    m_isDoubleExp = false;
}

COdl::SOdlItem::SOdlItem(const COdl::SOdlItem &r)
{
    m_isStr = r.m_isStr;
    m_isInt = r.m_isInt;
    m_isDouble = r.m_isDouble;
    m_isDoubleExp = r.m_isDoubleExp;
    m_data = r.m_data.c_str();
}

COdl::SOdlItem &COdl::SOdlItem::operator=(const COdl::SOdlItem &r)
{
    m_isStr = r.m_isStr;
    m_isInt = r.m_isInt;
    m_isDouble = r.m_isDouble;
    m_isDoubleExp = r.m_isDoubleExp;
    m_data = r.m_data.c_str();
    return *this;
}

COdl::SOdlItem::SOdlItem(const std::string &data)
{
    m_data = data.c_str();
    determineItem();
}

COdl::SOdlItem::~SOdlItem()
{
}

void COdl::SOdlItem::determineItem()
{
    m_isInt = false;
    m_isDouble = false;
    m_isDoubleExp = false;
    m_isStr = false;

    if (m_data == "")
        return;

    if (isInt(m_data))
        m_isInt = true;

    if (isDoubleMayBeZero(m_data))
        m_isDouble = true;

    if (isDoubleExpForm(m_data))
        m_isDoubleExp = true;

    m_isStr = true; //all data can be strings
}

void COdl::SOdlItem::setItem(const std::string &data)
{
    m_data = data.c_str();
    determineItem();
}

int COdl::SOdlItem::getInt() const
{
    if (m_isInt)
    {
        return atoi(m_data.c_str());
    }
    return 0;
}

bool COdl::SOdlItem::getBool() const
{
    return (getInt() != 0);
}

double COdl::SOdlItem::getDouble() const
{
    if (m_isDouble)
    {
        return atof(m_data.c_str());
    }
    else if (m_isDoubleExp)
    {
        double d = 0;
        sscanf(m_data.c_str(), "%lf", &d);
        return d;
    }
    return 0;
}

std::string COdl::SOdlItem::getStr() const
{
    if (m_isStr)
    {
        return m_data;
    }
    return "";
}

bool COdl::SOdlItem::_isInt()
{
    return m_isInt;
}

bool COdl::SOdlItem::_isDouble()
{
    return m_isDouble;
}

bool COdl::SOdlItem::_isStr()
{
    return m_isStr;
}

/**************************************************/
/**************************************************/
/**************************************************/
COdl::COdl()
{
    m_map.clear();
}

COdl::COdl(const COdl &r)
{
    m_map.clear();

    std::map<std::string, std::vector<SOdlItem> >::const_iterator it = r.m_map.begin();
    while (it != r.m_map.end())
    {
        m_map.insert(std::pair<std::string, std::vector<SOdlItem> > (it->first.c_str(), it->second));
        it++;
    }
}


COdl::~COdl()
{
    clean();
}

COdl &COdl::operator=(const COdl &r)
{
    if (this == &r)
        return *this;
    m_map.clear();
    std::map<std::string, std::vector<SOdlItem> >::const_iterator it = r.m_map.begin();
    while (it != r.m_map.end())
    {
        m_map.insert(std::pair<std::string, std::vector<SOdlItem> > (it->first.c_str(), it->second));
        it++;
    }

    return *this;
}

void COdl::clean()
{
    m_map.clear();
}

int COdl::add(const std::string &key, const std::string &value)
{
    if (isItemPresentExactMatch(key))
    {
        if (value != getItemStr(key))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Item '" + key + "' already exists, going to overwrite the value");
            remove(key);
            std::vector<SOdlItem> vec;
            vec.push_back(SOdlItem(value));
            m_map.insert(std::pair<std::string, std::vector<SOdlItem> > (key, vec));
        }
    }
    else
    {
        std::vector<SOdlItem> vec;
        vec.push_back(SOdlItem(value));
        m_map.insert(std::pair<std::string, std::vector<SOdlItem> > (key, vec));
    }

    return 0;
}

int COdl::add(const std::string &key, int value)
{
    return add(key, toString(value));
}

int COdl::add(const std::string &key, double value)
{
    return add(key, toString(value));
}

int COdl::add(const std::string &key, float value)
{
    return add(key, toString(value));
}

int COdl::add(const std::string &key, const std::vector<std::string> &value)
{
    size_t s = value.size();
    if (s == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to add an empty vector");
        return 1;
    }
    if (isItemPresentExactMatch(key))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Item '" + key + "' already exists, going to overwrite the value");
        remove(key);
    }

    std::vector<SOdlItem> vec;
    vec.resize(s);
    for (long x = 0; x < s; x++)
    {
        vec[x].setItem(value[x]);
    }
    m_map.insert(std::pair<std::string, std::vector<SOdlItem> > (key, vec));
    return 0;
}

int COdl::add(const std::string &key, const std::vector<int> &value)
{
    size_t s = value.size();
    if (s == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to add an empty vector");
        return 1;
    }
    std::vector<std::string> val;
    val.resize(s);
    for (long x = 0; x < value.size(); x++)
    {
        val[x] = toString(value[x]);
    }
    return add(key, val);
}

int COdl::add(const std::string &key, const std::vector<double> &value)
{
    size_t s = value.size();
    if (s == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to add an empty vector");
        return 1;
    }
    std::vector<std::string> val;
    val.resize(s);
    for (long x = 0; x < value.size(); x++)
    {
        val[x] = toString(value[x]);
    }
    return add(key, val);
}

int COdl::add(const std::string &key, const std::vector<float> &value)
{
    size_t s = value.size();
    if (s == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to add an empty vector");
        return 1;
    }
    std::vector<std::string> val;
    val.resize(s);
    for (long x = 0; x < value.size(); x++)
    {
        val[x] = toString(value[x]);
    }
    return add(key, val);
}

void COdl::remove(std::string key)
{
    std::map<std::string, std::vector<SOdlItem> >::iterator b = m_map.begin();
    std::map<std::string, std::vector<SOdlItem> >::iterator e = m_map.end();

    for (; b != e; ++b)
    {
        if (b->first == key)
        {
            m_map.erase(b);
            return;
        }
    }
}

void COdl::getItem(std::string key, EOdlSearchType method, std::pair<std::string, std::vector<COdl::SOdlItem> > &item) const
{
    item.second.clear();

    std::map<std::string, std::vector<SOdlItem> > *map = const_cast<std::map<std::string, std::vector<SOdlItem> > *> (&m_map);
    if (map == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to cast ODL map");
        return;
    }
    std::map<std::string, std::vector<SOdlItem> >::iterator b = map->begin();
    std::map<std::string, std::vector<SOdlItem> >::iterator e = map->end();

    if (method == EXACT_MATCH)
    {
        std::map<std::string, std::vector<SOdlItem> >::iterator i = map->find(key);
        if (i != e)
        {
            item.first = i->first;
            for (int t = 0; t < i->second.size(); t++)
                item.second.push_back(i->second[t]);
        }
    }
    else
    {
        for (; b != e; ++b)
        {
            switch (method)
            {
            case NORM:
                if ((*b).first.find(key) != std::string::npos)
                {
                    item.first = (*b).first;
                    for (int t = 0; t < (*b).second.size(); t++)
                        item.second.push_back((*b).second[t]);
                }
                break;
            case EXACT_MATCH:
                if ((*b).first == key)
                {
                    item.first = (*b).first;
                    for (int t = 0; t < (*b).second.size(); t++)
                        item.second.push_back((*b).second[t]);
                }
                break;
            default:
                return;
            }
        }
    }
}

std::string COdl::getItemStr(std::string key) const
{
    std::pair<std::string, std::vector<COdl::SOdlItem> >item;
    getItem(key, NORM, item);
    if (item.second.size() > 0)
        return item.second[0].getStr();
    return "";
}

int COdl::getItemIntExactMatch(std::string key) const
{
    std::pair<std::string, std::vector<COdl::SOdlItem> >item;
    getItem(key, EXACT_MATCH, item);
    if (item.second.size() > 0)
        return item.second[0].getInt();
    return 0;
}

double COdl::getItemDoubleExactMatch(std::string key) const
{
    std::pair<std::string, std::vector<COdl::SOdlItem> >item;
    getItem(key, EXACT_MATCH, item);
    if (item.second.size() > 0)
        return item.second[0].getDouble();
    return 0;
}

bool COdl::getItemBoolExactMatch(std::string key) const
{
    std::pair<std::string, std::vector<COdl::SOdlItem> >item;
    getItem(key, EXACT_MATCH, item);
    if (item.second.size() > 0)
        return item.second[0].getBool();
    return false;
}

std::string COdl::getItemStrExactMatch(std::string key) const
{
    std::pair<std::string, std::vector<COdl::SOdlItem> >item;
    getItem(key, EXACT_MATCH, item);
    if (item.second.size() > 0)
        return item.second[0].getStr();
    return "";
}

bool COdl::isItemPresentExactMatch(std::string key) const
{
    std::pair<std::string, std::vector<COdl::SOdlItem> >item;
    getItem(key, EXACT_MATCH, item);
    return item.second.size() > 0;
}

COdl::SOdlDisplayNode::SOdlDisplayNode(std::string name)
{
    m_name = name;
}

COdl::SOdlDisplayNode::~SOdlDisplayNode()
{
    for (int x = 0; x < m_children.size(); x++)
    {
        delete m_children[x];
        m_children[x] = nullptr;
    }
    m_children.clear();
}

COdl::SOdlDisplayNode* COdl::SOdlDisplayNode::getOrCreate(std::string childName)
{
    for (int x = 0; x < m_children.size(); x++)
    {
        if (childName == m_children[x]->m_name)
        {
            return m_children[x];
        }
    }
    SOdlDisplayNode *child = new SOdlDisplayNode(childName);
    m_children.push_back(child);
    return child;
}

void COdl::SOdlDisplayNode::setValues(const std::vector<COdl::SOdlItem> &values)
{
    m_values.clear();
    for (int x = 0; x < values.size(); x++)
    {
        m_values.push_back(values[x]);
    }
}

void COdl::getAll(std::vector<std::pair<std::string, std::vector<std::string> > > &data) const
{
    data.clear();

    std::map<std::string, std::vector<SOdlItem> >::const_iterator it = m_map.begin();
    while (it != m_map.end())
    {
        std::pair<std::string, std::vector<std::string> > d;
        data.push_back(d);
        int l = data.size() - 1;
        data[l].first = it->first;
        for (unsigned long t = 0; t < it->second.size(); t++)
        {
            data[l].second.push_back(it->second[t].getStr());
        }
        it++;
    }
}

} //namespace ultra
