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

#include "ul_ChipsGen.h"

namespace ultra
{

class CFixedLocationChips : public CChipsGen<float>
{
public:

    struct CFixedLocationChipsContext : public CChipsGen<float>::CChipsGenContext
    {
    public:
        int chipSize;
        std::string locationsProj4Str;
        CVector<SPair<double> > locations;
        std::string imageProj4Str;
        CVector<double> imageAffineTransform;

        CFixedLocationChipsContext()
        {
            chipSize = -1;
        }

        virtual ~CFixedLocationChipsContext()
        {
        }
    };
private:
    CFixedLocationChipsContext *m_context;
    SSize m_imgSize;

    int Init(CChipsGen<float>::CChipsGenContext *context);
    int Process();
    bool outOfBounds(const SPair<double> &sl, const SPair<double> &ulLimit, const SPair<double> &lrLimit) const;
protected:
    virtual int innerGenerate(CChipsGen<float>::CChipsGenContext *context);
    virtual int SplitImagesForThreads();

public:
    CFixedLocationChips();
    virtual ~CFixedLocationChips();
};


} // namespace ultra
