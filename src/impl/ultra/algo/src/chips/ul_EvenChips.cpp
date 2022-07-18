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

#include "ul_EvenChips.h"

#include "ul_Logger.h"

namespace ultra
{

CEvenChips::CEvenChips() :
CChipsGen<float>("Even_ChipsGen")
{

}

CEvenChips::~CEvenChips()
{

}

int CEvenChips::Init(CChipsGen<float>::CChipsGenContext *context)
{
    m_context = dynamic_cast<CEvenChips::CEvenChipsContext *> (context);
    if (m_context == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to cast context, please pass in CEvenChips::CEvenChipsContext struct");
        return 1;
    }

    if (this->m_inputImage == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Please load the reference image before calling generate");
        return 1;
    }

    if (this->m_outputChips == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Output chip vector is null, please load a chip vector container");
        return 1;
    }

    if (m_context->chipSize <= 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid context values, 'chipSize' must be larger than zero");
        return 1;
    }

    if (m_context->chipSize % 2 != 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid context values, 'chipSize' must be an odd value");
        return 1;
    }

    if (m_context->gridSize <= 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid context values, 'gridSize' must be larger than zero");
        return 1;
    }

    this->m_outputChips->clear();

    m_imgSize = this->m_inputImage->getSize();

    if (m_imgSize.row < m_context->chipSize * 2 ||
        m_imgSize.col < m_context->chipSize * 2)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "'chipSize' is larger than the input image, will not be able to generate any chips");
        return 1;
    }

    return 0;
}

int CEvenChips::Process()
{
    std::vector<CChip<float> > tempChipVector;
    CChip<float> chip;

    unsigned long start;
    unsigned long step;
    unsigned long stopR, stopC;
    unsigned long r, c;
    unsigned long id = 0;
    int per = -1;
    SSize chipSize, chipSizeHalf;
    long cornerOffset = m_context->chipSize / 2;

    chip.chipType = EChips::chipGenTypeToStr(EChips::CHIP_GEN_EVEN);
    chip.worldMatrix.clear();
    SSize ul;

    step = m_context->gridSize;
    stopR = m_imgSize.row - step / 2 - 1;
    stopC = m_imgSize.col - step / 2 - 1;
    start = step / 2 + 1; //m_context->chipSize / 2;
    chipSize.row = chipSize.col = m_context->chipSize;
    chipSizeHalf = chipSize / 2;

    for (r = start; r < stopR; r += step)
    {
        for (c = start; c < stopC; c += step)
        {
            ul.row = r;
            ul.col = c;
            try
            {
                chip.chipData = this->m_inputImage->getSubMatrix(ul, chipSize);
            }
            catch (const CException &e)
            {
                CLogger::getInstance()->Log(CLogger::LOG_WARN, e);
                continue;
            }

            if (chip.chipData.getSize().containsZero())
                continue;

            chip.chipId = id++;
            chip.midCoordinate = SPair<double>((double) (r + chipSizeHalf.row), (double) (c + chipSizeHalf.col));

            /**
             * 0 = UL
             * 1 = LL
             * 2 = LR
             * 3 = UR
             */
            chip.boundingCoordinate[0] = chip.midCoordinate + SPair<double>(-cornerOffset, -cornerOffset);
            chip.boundingCoordinate[1] = chip.midCoordinate + SPair<double>(cornerOffset, -cornerOffset);
            chip.boundingCoordinate[2] = chip.midCoordinate + SPair<double>(cornerOffset, cornerOffset);
            chip.boundingCoordinate[3] = chip.midCoordinate + SPair<double>(-cornerOffset, cornerOffset);

            tempChipVector.push_back(chip);
        }

        if (per != (int) ((r * 100) / stopR))
        {
            per = (int) ((r * 100) / stopR);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing " + toString(per) + "%      \r");
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing 100%      ");

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Generated '" + toString(tempChipVector.size()) + "' chips");

    (*(this->m_outputChips)) = tempChipVector;

    return 0;
}

int CEvenChips::SplitImagesForThreads()
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CEvenChips::innerGenerate(CChipsGen<float>::CChipsGenContext *context)
{
    if (Init(context) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
        return 1;
    }

    if (Process() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Process()");
        return 1;
    }

    return 0;
}

} // namespace ultra
