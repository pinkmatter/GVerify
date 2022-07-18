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

class CChipsGenWrapper
{
private:
    CChipsGenWrapper();
public:
    virtual ~CChipsGenWrapper();
    static int Generate(EChips::EChipType genType, const CMatrix<float> *inputImage, CVector<CChip<float> > *chips, CChipsGen<float>::CChipsGenContext *context);
};

} // namespace ultra
