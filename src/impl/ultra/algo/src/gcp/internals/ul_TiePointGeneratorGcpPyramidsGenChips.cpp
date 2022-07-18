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

#include "ul_TiePointGeneratorGcpPyramids.h"

#include "ul_EvenChips.h"

namespace ultra
{

CChipsGen<float>::CChipsGenContext *CTiePointGeneratorGcpPyramids::createEvenChipContext()
{
    CEvenChips::CEvenChipsContext *ret = new CEvenChips::CEvenChipsContext();
    if (ret == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CEvenChipsContext object");
        return nullptr;
    }

    ret->chipSize = m_context->m_chipSizeMinimum;
    ret->gridSize = m_context->m_chipGenerationGridSize;

    return ret;
}

CChipsGen<float>::CChipsGenContext *CTiePointGeneratorGcpPyramids::genDefaultChipGenContext(EChips::EChipType genType)
{
    switch (genType)
    {
    case EChips::CHIP_GEN_EVEN:
        return createEvenChipContext();
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call for '" + EChips::chipGenTypeToStr(genType) + "'");
        return nullptr;
    }
    return nullptr;
}

} // namespace ultra
