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

#include <map>
#include <iostream>
#include <memory>
#include <vector>

namespace ultra
{

class COdl
{
private:

    struct SOdlItem
    {
    private:
        bool m_isStr;
        bool m_isInt;
        bool m_isDouble;
        bool m_isDoubleExp;
        std::string m_data;

        void determineItem();
    public:

        SOdlItem();
        SOdlItem(const SOdlItem & r);
        SOdlItem &operator=(const SOdlItem & r);
        SOdlItem(const std::string & data);
        ~SOdlItem();

        void setItem(const std::string & data);

        bool _isInt();
        bool _isDouble();
        bool _isStr();

        int getInt() const;
        double getDouble() const;
        bool getBool() const;
        std::string getStr() const;
    };

    struct SOdlDisplayNode
    {
    private:
        std::string m_name;
        std::vector<SOdlDisplayNode*> m_children;
        std::vector<COdl::SOdlItem> m_values;
    public:
        SOdlDisplayNode(std::string name);
        ~SOdlDisplayNode();
        SOdlDisplayNode* getOrCreate(std::string childName);
        void setValues(const std::vector<COdl::SOdlItem> &values);
    };

private:

    enum EOdlSearchType
    {
        NORM = 0,
        EXACT_MATCH,
        ODL_SEARCH_TYPE_COUNT
    };
    mutable std::map<std::string, std::vector<COdl::SOdlItem> > m_map;
    void getItem(std::string key, EOdlSearchType method, std::pair<std::string, std::vector<COdl::SOdlItem> > &item) const;
public:
    COdl();
    COdl(const COdl &r);
    virtual ~COdl();

    int add(const std::string &key, const std::string &value);
    int add(const std::string &key, int value);
    int add(const std::string &key, double value);
    int add(const std::string &key, float value);
    int add(const std::string &key, const std::vector<std::string> &value);
    int add(const std::string &key, const std::vector<int> &value);
    int add(const std::string &key, const std::vector<double> &value);
    int add(const std::string &key, const std::vector<float> &value);
    COdl &operator=(const COdl &r);

    void clean();
    void remove(std::string key);

    std::string getItemStr(std::string key) const;

    bool isItemPresentExactMatch(std::string key) const;
    int getItemIntExactMatch(std::string key) const;
    bool getItemBoolExactMatch(std::string key) const;
    double getItemDoubleExactMatch(std::string key) const;
    std::string getItemStrExactMatch(std::string key) const;

    void getAll(std::vector<std::pair<std::string, std::vector<std::string> > > &data) const;
};

} //namespace ultra
