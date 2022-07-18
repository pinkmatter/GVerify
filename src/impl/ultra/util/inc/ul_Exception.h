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

#include <iostream>
#include <exception>

namespace ultra
{

class CException : public std::exception
{
private:
    std::string m_file;
    int m_line;
    std::string m_message;

    std::string getFileName(const std::string name) const;
public:
    CException();
    CException(std::string file, int line, std::string message);
    CException(const CException &ex);
    virtual ~CException() throw ();

    CException &operator=(const CException &ex);
    virtual std::string getExceptionName() const;
    friend std::ostream &operator<<(std::ostream &stream, const CException &ex);

    virtual void SetException(std::string file, int line, std::string message);
    virtual void GetException(std::string &file, int &line, std::string &message) const;
    virtual void GetMessage(std::string &message) const;

    virtual const char* what() const throw () override;
};

}
