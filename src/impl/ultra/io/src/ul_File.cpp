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

#include "ul_File.h"

#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include "ul_Utility.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/statvfs.h>
#include <mntent.h>
#include "ul_Exception.h"

namespace ultra
{

const std::string CFile::separatorStr = "/";
const std::string CFile::pathSeparatorStr = ":";

const char *CFile::separator = "/\0";
const char *CFile::pathSeparator = ":\0";

const char CFile::separatorChar = '/';
const char CFile::pathSeparatorChar = ':';

const int CFile::BUF_SIZE = 4 * 1024;

CFile::CFile()
{
    setPath("");
}

CFile::CFile(const std::string &path)
{
    setPath(path);
}

CFile::CFile(const char path[])
{
    setPath(path);
}

CFile::CFile(const CFile &r, const CFile &subItem)
{
    if (r.m_path != "")
        setPath(r.m_path + CFile::separatorStr + subItem.getPath());
    else
        setPath(subItem.getPath());
}

CFile::CFile(const CFile &r)
{
    m_path = r.m_path;
}

CFile::CFile(CFile &&r) :
m_path(std::move(r.m_path))
{
}

bool CFile::operator==(const CFile &r) const
{
    return getAbsolutePath() == r.getAbsolutePath();
}

bool CFile::operator!=(const CFile &r) const
{
    return !((*this) == r);
}

CFile::~CFile()
{
}

CFile &CFile::operator=(const CFile &r)
{
    if (this == &r)
        return *this;
    m_path = r.m_path;
    return *this;
}

CFile &CFile::operator=(CFile &&r)
{
    if (this == &r)
        return *this;
    m_path = std::move(r.m_path);
    return *this;
}

CFile &CFile::operator=(const std::string &path)
{
    setPath(path);
    return *this;
}

CFile &CFile::operator=(const char path[])
{
    setPath(path);
    return *this;
}

CFile &CFile::operator+=(const CFile &file)
{
    m_path = CFile(m_path, file.m_path).getPath();
    return *this;
}

CFile &CFile::operator+=(const std::string &path)
{
    m_path = CFile(m_path, path).getPath();
    return *this;
}

CFile &CFile::operator+=(const char *path)
{
    std::string s = path;
    m_path = CFile(m_path, s).getPath();
    return *this;
}

CFile CFile::operator+(const CFile &file) const
{
    return CFile(*this, file);
}

CFile CFile::operator+(const std::string &path) const
{
    return CFile(*this, path);
}

CFile CFile::operator+(const char *path) const
{
    return CFile(*this, path);
}

std::ostream &operator<<(std::ostream &stream, const CFile &o)
{
    stream << o.m_path;
    return stream;
}

bool CFile::exists() const
{
    if (m_path == "")
        return false;

    struct stat st;

    if (stat(m_path.c_str(), &st) != 0)
        return false;

    return true;
}

bool CFile::isFile() const
{
    if (m_path == "")
        return false;

    struct stat st;

    if (stat(m_path.c_str(), &st) != 0)
        return false;

    return ((st.st_mode & S_IFMT) == S_IFREG);
}

bool CFile::isDirectory() const
{
    if (m_path == "")
        return false;

    struct stat st;

    if (stat(m_path.c_str(), &st) != 0)
        return false;

    return ((st.st_mode & S_IFMT) == S_IFDIR);
}

bool CFile::isDirectoryWritable() const
{
    if (!isDirectory())
        return false;

    struct stat statBuf;

    if (stat(m_path.c_str(), &statBuf) != 0)
        return false;

    if ((statBuf.st_mode & S_IWGRP) == S_IWGRP ||
            (statBuf.st_mode & S_IWOTH) == S_IWOTH ||
            (statBuf.st_mode & S_IWUSR) == S_IWUSR)
    {
        return true;
    }

    return false;
}

int CFile::list(CVector<CFile> &subitems) const
{
    std::vector<CFile> items;
    if (list(items) != 0)
        return 1;

    subitems = items;
    if (subitems.size() != items.size())
        return 1;
    return 0;
}

int CFile::list(std::vector<std::string> &subitems) const
{
    subitems.clear();
    if (!exists())
        return 1;

    if (isFile())
        return 1;

    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(m_path.c_str())) == nullptr)
    {
        return 1;
    }

    while ((dirp = readdir(dp)) != nullptr)
    {
        if (dirp->d_type == DT_UNKNOWN || dirp->d_type == 8 || dirp->d_type == 10 || dirp->d_type == 4)//8 is files 10 is links
        {
            if (dirp->d_type == 4 || dirp->d_type == DT_UNKNOWN)//folder
            {
                std::string folder = dirp->d_name;
                if (folder != "." && folder != "..")
                {
                    subitems.push_back(folder);
                }
            }
            else
            {
                subitems.push_back(std::string(dirp->d_name));
            }
        }
    }

    closedir(dp);
    return 0;
}

int CFile::list(std::vector<CFile> &subitems) const
{
    std::vector<std::string> subitems2;
    if (list(subitems2) != 0)
    {
        return 1;
    }

    subitems.clear();
    for (long x = 0; x < subitems2.size(); x++)
    {
        subitems.push_back(m_path + CFile::separatorStr + subitems2[x]);
    }

    return 0;
}

std::string CFile::getPath() const
{
    return m_path;
}

CFile::operator std::string() const
{
    return getPath();
}

CFile::operator const char*() const
{
    return getPath().c_str();
}

std::string CFile::getFileName() const
{
    std::string absPath = getAbsolutePath();
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

std::string CFile::getExtension() const
{
    std::string fileName = getFileName();

    long lastPost = fileName.find_last_of(".");
    if (lastPost == std::string::npos)
        return "";

    return fileName.substr(lastPost + 1);
}

std::string CFile::getFileNameWithoutExtension() const
{
    std::string filename = getFileName();
    std::string ext = getExtension();

    if (filename.length() != 0 && ext.length() != 0 &&
            filename.length() > ext.length())
        return filename.substr(0, filename.length() - ext.length() - 1);

    return filename;
}

std::string CFile::getBaseFolder() const
{
    if (isDirectory())
        return getAbsolutePath();

    std::string absPath = getAbsolutePath();
    std::string fileName = getFileName();
    std::string baseFolder = "";

    int l = fileName.length();
    int absL = absPath.length();
    if (l == 0)
        return absPath;

    for (int x = 0; x < absL - l; x++)
    {
        baseFolder += absPath[x];
    }
    CFile ret = baseFolder;
    return ret.getPath();
}

std::string CFile::getParentFolder() const
{
    std::string ret = getBaseFolder();
    if (!isDirectory())
        return ret;

    if (ret.length() < 2)
        return ret;

    std::size_t found = ret.find_last_of(CFile::separatorStr);

    if (found != std::string::npos)
        ret.erase(found + 1);

    return CFile(ret).getPath();
}

CFile CFile::getParentFolderFile() const
{
    return CFile(getParentFolder());
}

void CFile::setPath(const std::string &path)
{
    m_path = "";
    int l = path.length();
    for (int x = 0; x < l - 1; x++)
    {
        if (!(path[x] == path[x + 1] && path[x] == CFile::separatorChar))
        {
            m_path += path[x];
        }
    }

    if (l > 0)
    {
        if (path[l - 1] != CFile::separatorChar || m_path == "")
        {
            m_path += path[l - 1];
        }
    }

    deleteChar(m_path, '"');
}

std::string CFile::getCurrentWorkingPath()
{
    char *buf = new char[PATH_MAX];
    if (getcwd(buf, PATH_MAX - 1) == nullptr)
    {
        delete []buf;
        buf = nullptr;
        return "";
    }
    std::string wd = buf;
    delete []buf;
    buf = nullptr;
    return wd;
}

void CFile::splitStr(const std::string inStr, std::vector<std::string> &items, const char splitChar) const
{
    int l = inStr.length();
    std::string item = "";
    for (int x = 0; x < l; x++)
    {
        if (inStr[x] == splitChar)
        {
            if (item != "")
            {
                items.push_back(item);
                item = "";
            }
        }
        else
        {
            item += inStr[x];
        }
    }
    if (item != "")
    {
        items.push_back(item);
    }
}

std::string CFile::getAbsolutePath() const
{
    std::string wd = "";
    if (startsWith(m_path, "." + CFile::separatorStr) || !startsWith(m_path, CFile::separatorStr))
    {
        wd = getCurrentWorkingPath();
    }

    std::vector<std::string> wdFolders;
    this->splitStr(wd, wdFolders, CFile::separatorChar);

    std::vector<std::string> pathFolders;
    this->splitStr(m_path, pathFolders, CFile::separatorChar);

    std::vector<std::string> newPath;

    for (int x = 0; x < wdFolders.size(); x++)
        newPath.push_back(wdFolders[x]);
    for (int x = 0; x < pathFolders.size(); x++)
        newPath.push_back(pathFolders[x]);

    std::vector<std::string> retPath;
    for (int x = 0; x < newPath.size(); x++)
    {
        if (retPath.size() > 0 && newPath[x] == "..")
        {
            retPath.erase(retPath.end());
        }
        else if (newPath[x] == ".")
        {
            //skip this one
            continue;
        }
        else
        {
            retPath.push_back(newPath[x]);
        }
    }

    std::string retStr = CFile::separatorStr;
    for (int x = 0; x < retPath.size(); x++)
    {
        retStr += retPath[x].c_str();
        if (x < retPath.size() - 1)
        {
            retStr += CFile::separatorStr;
        }
    }
    return retStr;
}

int CFile::mkdir(int mode) const
{
    return MkPath(getAbsolutePath(), mode);
}

int CFile::mkdirIfNotExists(int mode) const
{
    if (isDirectory())
        return 0;
    return mkdir(mode);
}

bool CFile::exists(const CFile &file)
{
    return file.exists();
}

bool CFile::isFile(const CFile &file)
{
    return file.isFile();
}

bool CFile::isDirectory(const CFile &file)
{
    return file.isDirectory();
}

bool CFile::isDirectoryWritable(const CFile &file)
{
    return file.isDirectoryWritable();
}

int CFile::remove(const CFile &src)
{
    return src.remove();
}

int CFile::mkdir(const CFile &src, int mode)
{
    return src.mkdir(mode);
}

int CFile::remove() const
{
    if (isFile())
        return ((std::remove(m_path.c_str()) == 0) ? (0) : (1));
    else if (isDirectory())
    {
        std::vector<std::string> subItems;
        if (list(subItems) != 0)
            return 1;
        for (unsigned long t = 0; t < subItems.size(); t++)
        {
            CFile file = m_path + CFile::separatorStr + subItems[t];
            if (file.remove() != 0)
                return 1;
        }

        return ((rmdir(m_path.c_str()) == 0) ? (0) : (1));
    }

    return 1;
}

} // namespace ultra
