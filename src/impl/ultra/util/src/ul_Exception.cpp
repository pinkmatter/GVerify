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

#include "ul_Exception.h"

#include <sstream>
#include <iomanip>

namespace ultra
{

namespace
{

template <class T> std::string CException__ToString(const T & t, unsigned int precision = 15)
{
    std::ostringstream oss;
    oss.clear();
    oss << std::fixed << std::setprecision(precision) << t;

    return oss.str();
}
}

CException::CException()
{
    m_file = "";
    m_line = 0;
    m_message = "";
}

CException::CException(std::string file, int line, std::string message)
{
    m_file = file;
    m_line = line;
    m_message = message;
}

CException::CException(const CException &ex)
{
    m_file = ex.m_file;
    m_line = ex.m_line;
    m_message = ex.m_message;
}

CException::~CException() throw ()
{

}

CException &CException::operator=(const CException &ex)
{
    m_file = ex.m_file;
    m_line = ex.m_line;
    m_message = ex.m_message;
    return *this;
}

std::ostream &operator<<(std::ostream &stream, const CException &ex)
{
    stream << ex.getExceptionName() << " " << ex.getFileName(ex.m_file) << " " << ex.m_line << " " << ex.m_message;
    return stream;
}

std::string CException::getExceptionName() const
{
    return "CException";
}

void CException::SetException(std::string file, int line, std::string message)
{
    m_file = file;
    m_line = line;
    m_message = message;
}

void CException::GetException(std::string &file, int &line, std::string &message) const
{
    file = m_file;
    line = m_line;
    message = m_message;
}

void CException::GetMessage(std::string &message) const
{
    message = m_message;
}

std::string CException::getFileName(const std::string name) const
{
    const char separatorChar = '/';
    std::string absPath = name;
    int l = absPath.length();
    std::string itemName = "";
    if (l == 0)
        return itemName;

    for (int x = l - 1; x >= 0; x--)
    {
        if (absPath[x] == separatorChar)
            break;
        itemName += absPath[x];
    }

    l = itemName.length();
    for (int x = 0; x < l / 2; x++)
    {
        char t = itemName[x];
        itemName[x] = itemName[l - 1 - x];
        itemName[l - 1 - x] = t;
    }

    return itemName;
}

const char* CException::what() const throw ()
{
    std::string str = getExceptionName() + " -> " + getFileName(m_file) + " " + CException__ToString(m_line) + ": " + m_message;
    return str.c_str();
}

};