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


#define DEPRECATED __attribute__((deprecated))
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <cstdarg>
#include <cmath>
#include <memory>

namespace ultra
{
/**
        Utility methods
 */
std::string frontZeroPad(const std::string& str, int requiredLength);
std::string toUpper(const std::string& str);
std::string toLower(const std::string& str);
bool pathExists(std::string path);
int deleteChar(std::string &str, char searchChar);
void replaceAllSubStr(std::string &str, std::string subStr, std::string newSubStr);
bool startsWith(const std::string &src, const std::string &search);
int ReadAllLinesFromFile(std::string pathToFile, std::vector<std::string> &ret, bool includeBlankLines = false);
int ReadAllLinesFromFile(FILE *fm, std::vector<std::string> &ret, bool includeBlankLines = false);
bool isDoubleExpForm(const std::string &str);
bool isDoubleMayBeZero(const std::string &str);
bool isInt(std::string str);
std::vector<std::string> split(std::string str, char plitChar);
std::string trimStrLeft(const std::string &str);
std::string trimStrRight(const std::string &str);
std::string trimStr(const std::string &str);
int MkPath(const std::string &path, int mode);
bool strToBool(const std::string &str);
std::string boolToStr(bool val);

template <class T> std::string toString(const T & t, unsigned int precision = 15)
{
    std::ostringstream oss;
    oss.clear();
    oss << std::fixed << std::setprecision(precision) << t;

    return oss.str();
}

template <class T> std::string toHexString(const T & t)
{
    std::ostringstream oss;
    oss.clear();
    oss << std::hex << t << std::dec;

    return oss.str();
}

template<class N> N clip(N n, N min, N max)
{
    return n < min ? min :
        n > max ? max :
        n;
}

template<class N> N getAbs(const N &n)
{
    if (n < 0)
        return N(-1) * n;
    return n;
}

template<class N> N getMAX(const N &n1, const N &n2)
{
    if (n1 > n2)
        return n1;
    return n2;
}

template<class N> N getMIN(const N &n1, const N &n2)
{
    if (n1 < n2)
        return n1;
    return n2;
}

template<class N> void swapValues(N &n1, N &n2)
{
    if (&n1 == &n2)
        return;
    N t = std::move(n1);
    n1 = std::move(n2);
    n2 = std::move(t);
}

template<class T, size_t N>
size_t getArraySize(T(&arr)[N])
{
    return N;
}

template<class T>
T max_of(int n_args, ...)
{
    if (n_args < 1)
        return T();
    va_list ap;
    va_start(ap, n_args);
    T max = va_arg(ap, T);
    for (int i = 2; i <= n_args; i++)
    {
        T v = va_arg(ap, T);
        if (v > max)
            max = v;
    }
    va_end(ap);
    return max;
}

template<class T>
T min_of(int n_args, ...)
{
    if (n_args < 1)
        return T();
    va_list ap;
    va_start(ap, n_args);
    T min = va_arg(ap, T);
    for (int i = 2; i <= n_args; i++)
    {
        T v = va_arg(ap, T);
        if (v < min)
            min = v;
    }
    va_end(ap);
    return min;
}

#define isNumberNAN(x) ((x)!=(x))

} //namespace ultra