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

#include "ul_Image.h"

namespace ultra
{

CImage::CImage() :
CMatrix<SColor>()
{
}

CImage::CImage(const SSize &size) :
CMatrix<SColor>(size)
{
}

CImage::CImage(unsigned long row, unsigned long col) :
CMatrix<SColor>(row, col)
{
}

CImage::CImage(const CImage &r) :
CMatrix<SColor>(r)
{
}

CImage::CImage(const CMatrix<SColor> &r) :
CMatrix<SColor>(r)
{
}

CImage::CImage(const CMatrix<unsigned int> &r) :
CMatrix<SColor>(r.getSize())
{
    SSize size = this->getSize();
    for (unsigned long row = 0; row < size.row; row++)
    {
        for (unsigned long col = 0; col < size.col; col++)
        {
            this->m_mat[row][col] = SColor(r[row][col]);
        }
    }
}

CImage::CImage(const CVector<CMatrix<unsigned char> > &r) :
CMatrix<SColor>()
{
    this->clear();
    if (r.size() != 0)
    {
        unsigned long size = r.size();
        if (size > 4)
            size = 4;
        bool same = true;
        for (unsigned long t = 0; t < size - 1; t++)
        {
            if (r[t].getSize() != r[t + 1].getSize())
            {
                same = false;
                break;
            }
        }

        if (same)
        {
            SSize imgSize = r[0].getSize();
            this->resize(imgSize);
            for (unsigned long t = 0; t < size; t++)
            {
                for (unsigned long row = 0; row < imgSize.row; row++)
                {
                    for (unsigned long col = 0; col < imgSize.col; col++)
                    {
                        this->m_mat[row][col].c[t] = r[t][row][col];
                    }
                }
            }
        }
    }
}

CImage::~CImage()
{
    this->clear();
}

CMatrix<unsigned char> CImage::getChannel(CImage::EImageChannel channel) const
{
    CMatrix<unsigned char> ret;
    ret.resize(this->getSize());
    unsigned long index = static_cast<unsigned long> (channel);

    if (index < 0 || index>static_cast<unsigned long> (CImage::IMAGE_CHANNEL_COUNT))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid image channel");
        ret.initMat(0);
        return ret;
    }

    unsigned long r, c;
    SSize size = this->getSize();

    for (r = 0; r < size.row; r++)
    {
        for (c = 0; c < size.col; c++)
        {
            ret[r][c] = this->m_mat[r][c].c[index];
        }
    }

    return ret;
}

CMatrix<unsigned char> CImage::getRed() const
{
    return getChannel(CImage::RED);
}

CMatrix<unsigned char> CImage::getGreen() const
{
    return getChannel(CImage::GREEN);
}

CMatrix<unsigned char> CImage::getBlue() const
{
    return getChannel(CImage::BLUE);
}

CMatrix<unsigned char> CImage::getAlpha() const
{
    return getChannel(CImage::ALPHA);
}

int CImage::copyFromMat(const CMatrix<unsigned char> &mat, int index)
{
    unsigned long r, c;
    SSize size = this->getSize();
    for (r = 0; r < size.row; r++)
    {
        for (c = 0; c < size.col; c++)
        {
            m_mat[r][c].c[index] = mat[r][c];
        }
    }

    return 0;
}

int CImage::copyFromImage(const CImage &im, int index)
{
    unsigned long r, c;
    SSize size = this->getSize();
    for (r = 0; r < size.row; r++)
    {
        for (c = 0; c < size.col; c++)
        {
            m_mat[r][c].c[index] = im[r][c].c[index];
        }
    }
    return 0;
}

int CImage::SetRed(const CMatrix<unsigned char> &mat)
{
    resize(mat.getSize());

    if (copyFromMat(mat, static_cast<int> (CImage::RED)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromMat()");
        return 1;
    }

    return 0;
}

int CImage::SetBlue(const CMatrix<unsigned char> &mat)
{
    resize(mat.getSize());

    if (copyFromMat(mat, static_cast<int> (CImage::BLUE)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromMat()");
        return 1;
    }

    return 0;
}

int CImage::SetGreen(const CMatrix<unsigned char> &mat)
{
    resize(mat.getSize());

    if (copyFromMat(mat, static_cast<int> (CImage::GREEN)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromMat()");
        return 1;
    }

    return 0;
}

int CImage::SetAlpha(const CMatrix<unsigned char> &mat)
{
    resize(mat.getSize());

    if (copyFromMat(mat, static_cast<int> (CImage::ALPHA)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromMat()");
        return 1;
    }

    return 0;
}

int CImage::SetRed(const CImage &im)
{
    resize(im.getSize());

    if (copyFromImage(im, static_cast<int> (CImage::RED)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromImage()");
        return 1;
    }

    return 0;
}

int CImage::SetBlue(const CImage &im)
{
    resize(im.getSize());

    if (copyFromImage(im, static_cast<int> (CImage::BLUE)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromImage()");
        return 1;
    }

    return 0;
}

int CImage::SetGreen(const CImage &im)
{
    resize(im.getSize());

    if (copyFromImage(im, static_cast<int> (CImage::GREEN)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromImage()");
        return 1;
    }

    return 0;
}

int CImage::SetAlpha(const CImage &im)
{
    resize(im.getSize());

    if (copyFromImage(im, static_cast<int> (CImage::ALPHA)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from copyFromImage()");
        return 1;
    }

    return 0;
}

CImage CImage::invertColors() const
{
    CImage ret = *this;

    SSize size = ret.getSize();
    unsigned long r, c, i;

    for (r = 0; r < size.row; r++)
    {
        for (c = 0; c < size.col; c++)
        {
            for (i = 0; i < 4; i++)
            {
                ret[r][c].c[i] ^= 0xff;
            }
        }
    }

    return ret;
}

CImage &CImage::operator=(const CVector<CMatrix<unsigned char> > &r)
{
    this->clear();
    if (r.size() != 0)
    {
        unsigned long size = r.size();
        if (size > 4)
            size = 4;
        bool same = true;
        for (unsigned long t = 0; t < size - 1; t++)
        {
            if (r[t].getSize() != r[t + 1].getSize())
            {
                same = false;
                break;
            }
        }

        if (same)
        {
            SSize imgSize = r[0].getSize();
            this->resize(imgSize);
            for (unsigned long t = 0; t < size; t++)
            {
                for (unsigned long row = 0; row < imgSize.row; row++)
                {
                    for (unsigned long col = 0; col < imgSize.col; col++)
                    {
                        this->m_mat[row][col].c[t] = r[t][row][col];
                    }
                }
            }
        }
    }

    return *this;
}

} //namespace ultra
