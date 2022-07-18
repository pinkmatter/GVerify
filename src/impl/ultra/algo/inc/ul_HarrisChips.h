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

class CHarrisChips : public CChipsGen<float>
{
public:

    struct CHarrisChipsContext : public CChipsGen<float>::CChipsGenContext
    {
    public:
        int chipSize;

        CHarrisChipsContext()
        {

        }

        virtual ~CHarrisChipsContext()
        {

        }
    };
private:
    CHarrisChipsContext *m_context;
    SSize m_imgSize;

    int Init(CChipsGen<float>::CChipsGenContext *context);
    int ProcessHarrisTransformImageVector(CVector<CMatrix<unsigned char> > &imageVecOut);
    int Process();

protected:
    virtual int innerGenerate(CChipsGen<float>::CChipsGenContext *context) override;
    virtual int SplitImagesForThreads() override;
public:
    CHarrisChips();
    virtual ~CHarrisChips();
};

}
