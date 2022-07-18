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

#include "ul_MatrixArray.h"
#include "ul_Color.h"
#include "ul_KeyValue.h"

namespace ultra
{

class CImage : public CMatrix<SColor>
{
public:

    enum EImageChannel
    {
        BLUE = 0,
        GREEN,
        RED,
        ALPHA,
        IMAGE_CHANNEL_COUNT
    };

private:
    int copyFromMat(const CMatrix<unsigned char> &mat, int index);
    int copyFromImage(const CImage &im, int index);

    template<class T>
    static bool sortFunc(const SKeyValue<T, SColor>&t1, const SKeyValue<T, SColor> &t2)
    {
        return t1.k < t2.k;
    }

public:
    CImage();
    CImage(const SSize &size);
    CImage(unsigned long row, unsigned long col);
    CImage(const CImage &r);
    CImage(const CMatrix<unsigned int> &r);
    CImage(const CMatrix<SColor> &r);
    CImage(const CVector<CMatrix<unsigned char> > &r);
    virtual ~CImage();

    CMatrix<unsigned char> getChannel(CImage::EImageChannel channel) const;
    CMatrix<unsigned char> getRed() const;
    CMatrix<unsigned char> getGreen() const;
    CMatrix<unsigned char> getBlue() const;
    CMatrix<unsigned char> getAlpha() const;
    int SetRed(const CMatrix<unsigned char> &mat);
    int SetBlue(const CMatrix<unsigned char> &mat);
    int SetGreen(const CMatrix<unsigned char> &mat);
    int SetAlpha(const CMatrix<unsigned char> &mat);
    int SetRed(const CImage &im);
    int SetBlue(const CImage &im);
    int SetGreen(const CImage &im);
    int SetAlpha(const CImage &im);
    CImage invertColors() const;

    virtual CImage &operator=(const CVector<CMatrix<unsigned char> > &r);

};

} // namespace ultra
