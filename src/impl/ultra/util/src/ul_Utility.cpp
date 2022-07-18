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

#include "ul_Utility.h"
#include <math.h>
#include <cstring>
#include <algorithm>

namespace ultra
{

namespace
{

std::string strFrontPad(const std::string& str, char ch, int requiredLength)
{
    std::string tmpStr = str;
    while (tmpStr.length() < requiredLength)
    {
        tmpStr = ch + tmpStr;
    }
    return tmpStr;
}

std::string readLine(FILE* file)
{
    std::string str = "";

    if (file == nullptr)
    {
        return str;
    }

    while (!feof(file))
    {
        char ch = 0;
        if (fread(&ch, 1, 1, file) == 1)
        {
            if (ch != '\n')
                str += ch;
            else
                break;
        }
        else
            break;
    }

    /*
            The data to read might be in DOS format, thus delete \r
     */

    deleteChar(str, '\r');

    return str;
}

/**
 *	if -1 is returned then the char is not found
 */
int getLastIndexOf(std::string inStr, char charToFind)
{
    int l = inStr.size();

    for (int x = l - 1; x >= 0; x--)
    {
        if (inStr[x] == charToFind)
        {
            return x;
        }
    }

    return -1;
}

int SplitStr(std::string str, std::string &left, std::string &right, char splitAtChar)
{
    int index = getLastIndexOf(str, splitAtChar);

    if (index == -1 || index == str.length())
        return 1;

    left = str.substr(0, index);
    right = str.substr(index + 1, str.length());

    return 0;
}

} //namespace 

std::string frontZeroPad(const std::string& str, int requiredLength)
{
    return strFrontPad(str, '0', requiredLength);
}

std::string toUpper(const std::string& str)
{
    std::string tmpStr = str;
    for (int i = 0; i < str.length(); i++)
    {
        tmpStr[i] = (char) toupper(str[i]);
    }
    return tmpStr;
}

std::string toLower(const std::string& str)
{
    std::string tmpStr = str;
    for (int i = 0; i < str.length(); i++)
    {
        tmpStr[i] = (char) tolower(str[i]);
    }
    return tmpStr;
}

bool startsWith(const std::string &src, const std::string &search)
{
    if (src.size() < search.size())
        return false;

    return (src.substr(0, search.length()) == search);
}

int ReadAllLinesFromFile(std::string pathToFile, std::vector<std::string> &ret, bool includeBlankLines)
{
    ret.clear();
    FILE *fm = fopen(pathToFile.c_str(), "re");
    if (fm == nullptr)
    {
        return 1;
    }

    int retCode = ReadAllLinesFromFile(fm, ret, includeBlankLines);
    fclose(fm);

    return retCode;
}

int ReadAllLinesFromFile(FILE *fm, std::vector<std::string> &ret, bool includeBlankLines)
{
    ret.clear();

    if (fm == nullptr)
        return 1;

    std::string line;
    while (!feof(fm))
    {
        line = readLine(fm);
        if (includeBlankLines || line != "")
        {
            ret.push_back(line);
        }
    }

    return 0;
}

bool pathExists(std::string path)
{
    struct stat st;

    if (stat(path.c_str(), &st) != 0)
        return false;

    return true;
}

int deleteChar(std::string &str, char searchChar)
{
    int ret = 0;

    for (int x = 0; x < str.size(); x++)
    {
        if (str[x] == searchChar)
        {
            str.erase(x, 1);
            x--;
            ret++;
        }
    }

    return ret;
}

void replaceAllSubStr(std::string &str, std::string subStr, std::string newSubStr)
{
    size_t pos = str.find(subStr, 0);
    while (pos != std::string::npos)
    {
        str.erase(pos, subStr.size());
        str.insert(pos, newSubStr);
        pos = str.find(subStr, pos + newSubStr.length());
    }
}

bool isInt(std::string str)
{
    int start = 0;
    if (str.length() == 0)
        return false;
    if (str[0] == '-')
    {
        start++;
        if (str.length() == 1)// thus only got '-'
            return false;
    }

    for (int x = start; x < str.length(); x++)
    {
        if (str[x] != '0' &&
                str[x] != '1' &&
                str[x] != '2' &&
                str[x] != '3' &&
                str[x] != '4' &&
                str[x] != '5' &&
                str[x] != '6' &&
                str[x] != '7' &&
                str[x] != '8' &&
                str[x] != '9')
        {
            return false;
        }
    }

    return true;
}

bool isDoubleExpForm(const std::string &str)
{
    double d;
    return (sscanf(str.c_str(), "%lf", &d) == 1);
}

bool isDoubleMayBeZero(const std::string &str)
{
    bool ret = true;
    bool gotNonZero = false;
    int points = 0;
    int begin = 0;

    if (str.length() == 0)
        return false;
    if (str[0] == '-')
    {
        /*
                Value is negative
         */
        begin++;
    }

    for (int x = begin; x < str.length(); x++)
    {
        bool alpha = false;
        bool gotPoint = false;

        if (str[x] != '0' &&
                str[x] != '1' &&
                str[x] != '2' &&
                str[x] != '3' &&
                str[x] != '4' &&
                str[x] != '5' &&
                str[x] != '6' &&
                str[x] != '7' &&
                str[x] != '8' &&
                str[x] != '9')
        {
            alpha = true;
            if (str[x] == '.')
            {
                gotPoint = true;
            }
        }
        if (str[x] == '0' ||
                str[x] == '1' ||
                str[x] == '2' ||
                str[x] == '3' ||
                str[x] == '4' ||
                str[x] == '5' ||
                str[x] == '6' ||
                str[x] == '7' ||
                str[x] == '8' ||
                str[x] == '9')
        {
            gotNonZero = true;
        }

        if (alpha)//  ! 0-9
        {
            if (gotPoint)//.
            {
                points++;
                if (points > 1)
                {
                    ret = false;
                }
            }
            else
            {
                ret = false;
            }
        }
    }

    return ret&gotNonZero;
}

std::vector<std::string> split(std::string str, char plitChar)
{
    std::vector<std::string> vec;
    char s[2] = {0};
    s[0] = plitChar;
    std::string temp;
    while (true)
    {
        size_t ret = str.find(s);
        if (ret == std::string::npos)
            break;
        temp = str.substr(0, ret);
        if (temp != "")
            vec.push_back(temp);
        str = str.substr(ret + 1);
    }
    if (str != "")
        vec.push_back(str);

    return vec;
}

std::string trimStrLeft(const std::string &str)
{
    std::string s = str;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string trimStrRight(const std::string &str)
{
    std::string s = str;
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string trimStr(const std::string &str)
{
    return trimStrLeft(trimStrRight(str));
}

int MkPath(const std::string &path, int mode)
{
    if (pathExists(path))
        return 1;

    std::string tempPath = "";
    int l = path.length();

    int index1 = -1;
    int index2 = -1;

    for (int x = 0; x < l; x++)
    {
        bool add = true;
        if (path[x] == '/')
        {
            index1 = x;
            if (index2 >= 0)
            {
                if (index1 - 1 == index2)//they are next to each other
                {
                    add = false;
                }
            }
            index2 = index1;
        }
        if (add)
        {
            tempPath += path[x];
        }

    }

    if (tempPath.length() > 0)
    {
        if (tempPath[tempPath.length() - 1] == '/')
        {
            tempPath = tempPath.substr(0, tempPath.length() - 1);
        }
    }

    if (pathExists(tempPath))
        return 1;

    std::string left;
    std::string right;
    std::vector<std::string> items;
    while (SplitStr(tempPath, left, right, '/') == 0)
    {
        items.push_back(right);
        tempPath = left;
    }

    tempPath = "";
    bool madeDir = false;
    for (int x = items.size() - 1; x >= 0; x--)
    {
        tempPath += "/";
        tempPath += items[x];
        if (pathExists(tempPath))
            continue;
        madeDir = (mkdir(tempPath.c_str(), mode) == 0);
    }

    if (!madeDir)
        return 1;

    return 0;
}

bool strToBool(const std::string &str)
{
    std::string s = toUpper(str);
    return s == "YES" || s == "TRUE";
}

std::string boolToStr(bool val)
{
    return (val ? "TRUE" : "FALSE");
}

} //namespace ultra
