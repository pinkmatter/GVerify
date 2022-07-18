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
#include <vector>
#include "ul_Vector.h"

namespace ultra
{

class CFile
{
public:
    static const std::string separatorStr;
    static const std::string pathSeparatorStr;

    static const char *separator;
    static const char *pathSeparator;

    static const char separatorChar;
    static const char pathSeparatorChar;

private:
    static const int BUF_SIZE;
    std::string m_path;
    void splitStr(const std::string inStr, std::vector<std::string> &items, const char splitChar) const;

public:
    CFile();
    CFile(const std::string &path);
    CFile(const char path[]);
    CFile(const CFile &r);
    CFile(CFile &&r);
    CFile(const CFile &r, const CFile &subItem);
    virtual ~CFile();

    CFile &operator=(const CFile &r);
    CFile &operator=(CFile &&r);
    CFile &operator=(const std::string &path);
    CFile &operator=(const char path[]);

    CFile &operator+=(const CFile &file);
    CFile &operator+=(const std::string &path);
    CFile &operator+=(const char *path);

    CFile operator+(const CFile &file) const;
    CFile operator+(const std::string &path) const;
    CFile operator+(const char *path) const;

    bool operator==(const CFile &r) const;
    bool operator!=(const CFile &r) const;
    friend std::ostream &operator<<(std::ostream &stream, const CFile &o);

    bool exists() const;
    bool isFile() const;
    bool isDirectory() const;
    bool isDirectoryWritable() const;
    int list(std::vector<std::string> &subitems) const;
    int list(std::vector<CFile> &subitems) const;
    int list(CVector<CFile> &subitems) const;
    std::string getPath() const;
    operator std::string() const;
    operator const char*() const;
    void setPath(const std::string &path);
    std::string getFileName() const;
    std::string getFileNameWithoutExtension() const;
    std::string getBaseFolder() const;
    std::string getParentFolder() const;
    CFile getParentFolderFile() const;
    static std::string getCurrentWorkingPath();
    std::string getAbsolutePath() const;
    std::string getExtension() const;
    int mkdir(int mode = 0777) const;
    int mkdirIfNotExists(int mode = 0777) const;
    int remove() const;
    static bool exists(const CFile &file);
    static bool isFile(const CFile &file);
    static bool isDirectory(const CFile &file);
    static bool isDirectoryWritable(const CFile &file);
    static int remove(const CFile &src);
    static int mkdir(const CFile &src, int mode = 0777);
};

} //namespace ultra
