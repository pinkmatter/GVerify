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

#include "ul_ChipsGenWrapper.h"
#include "ul_EvenChips.h"

namespace ultra
{

CChipsGenWrapper::CChipsGenWrapper()
{

}

CChipsGenWrapper::~CChipsGenWrapper()
{

}

int CChipsGenWrapper::Generate(EChips::EChipType genType, const CMatrix<float> *inputImage, CVector<CChip<float> > *chips, CChipsGen<float>::CChipsGenContext *context)
{
    std::unique_ptr<CChipsGen<float> > genner;
    switch (genType)
    {
    case EChips::CHIP_GEN_EVEN:genner.reset(new CEvenChips());
        break;
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call for '" + EChips::chipGenTypeToStr(genType) + "'");
        return 1;
    }

    if (genner.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CChipsGen object");
        return 1;
    }

    if (genner->LoadData(inputImage, chips) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CChipsGen::LoadData()");
        return 1;
    }

    if (genner->Generate(context) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CChipsGen::Generate()");
        return 1;
    }
    return 0;
}

} // namespace ultra
