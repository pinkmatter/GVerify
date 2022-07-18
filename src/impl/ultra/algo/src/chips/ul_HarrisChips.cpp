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

#include "ul_HarrisChips.h"

#include "ul_HarrisCornerTransform.h"

namespace ultra
{

CHarrisChips::CHarrisChips() :
CChipsGen<float>("Harris_ChipsGen")
{

}

CHarrisChips::~CHarrisChips()
{

}

int CHarrisChips::Init(CChipsGen<float>::CChipsGenContext *context)
{
    m_context = dynamic_cast<CHarrisChips::CHarrisChipsContext *> (context);
    if (m_context == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to cast context, please pass in CEvenChips::CMinMaxChipsContext struct");
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

    this->m_outputChips->clear();

    return 0;
}

int CHarrisChips::ProcessHarrisTransformImageVector(CVector<CMatrix<unsigned char> > &imageVecOut)
{
    CVector<CMatrix<float> > imageVecIn;

    imageVecIn.resize(1);
    imageVecIn[0] = *(this->m_inputImage);

    if (imageVecIn[0].getSize() != this->m_inputImage->getSize())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to make a local copy of the input image");
        return 1;
    }

    CHarrisCornerTransform::CHarrisCornerContext harrisTransformContext;

    CTransform<float, unsigned char> *harrisCornerTransform = new CHarrisCornerTransform();
    if (harrisCornerTransform == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CHarrisCornerTransform object");
        return 1;
    }

    if (harrisCornerTransform->LoadData(&imageVecIn, &imageVecOut) != 0)
    {
        delete harrisCornerTransform;
        harrisCornerTransform = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadData()");
        return 1;
    }

    unsigned long gcpCount = imageVecIn[0].getSize().col * imageVecIn[0].getSize().row;
    gcpCount /= (m_context->chipSize * m_context->chipSize);
    gcpCount /= 4;
    unsigned long pointsFound;
    int loop = 0;
    harrisTransformContext.minHarrisValue = 1.0;
    while (true)
    {
        if (harrisCornerTransform->Transform(&harrisTransformContext) != 0)
        {
            delete harrisCornerTransform;
            harrisCornerTransform = nullptr;
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Transform()");
            return 1;
        }

        pointsFound = imageVecOut[0].countIfGreaterThan(0);
        if (pointsFound > gcpCount)
        {
            break;
        }
        loop++;
        harrisTransformContext.minHarrisValue -= 0.1;
        if (loop > 5)
        {
            break;
        }
    }

    delete harrisCornerTransform;
    harrisCornerTransform = nullptr;

    return 0;
}

int CHarrisChips::Process()
{
    CVector<CMatrix<unsigned char> > imageVecOut;
    if (ProcessHarrisTransformImageVector(imageVecOut) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ProcessHarrisTransformImageVector()");
        return 1;
    }

    SSize size = imageVecOut[0].getSize();
    long r, c;
    long chipHalf = m_context->chipSize / 2;
    SSize chipSize = SSize(m_context->chipSize, m_context->chipSize);
    long rMax = size.row - chipHalf, cMax = size.col - chipHalf;
    std::vector<CChip<float> > tempChipVector;
    CChip<float> chip;
    unsigned long id = 0;
    int per = 0;
    unsigned long ulR, ulC;
    chip.chipType = EChips::chipGenTypeToStr(EChips::CHIP_GEN_HARRIS);
    chip.worldMatrix.clear();

    for (r = chipHalf; r < rMax; r++)
    {
        for (c = chipHalf; c < cMax; c++)
        {
            if (imageVecOut[0][r][c] != 0)
            {
                ulR = r - chipHalf;
                ulC = c - chipHalf;
                chip.chipData = this->m_inputImage->getSubMatrix(SSize(ulR, ulC), chipSize);
                chip.chipId = id++;
                chip.midCoordinate = SPair<double>((double) (r), (double) (c));

                /**
                 * 0 = UL
                 * 1 = LL
                 * 2 = LR
                 * 3 = UR
                 */
                chip.boundingCoordinate[0] = chip.midCoordinate + SPair<double>(-chipHalf, -chipHalf);
                chip.boundingCoordinate[1] = chip.midCoordinate + SPair<double>(chipHalf, -chipHalf);
                chip.boundingCoordinate[2] = chip.midCoordinate + SPair<double>(chipHalf, chipHalf);
                chip.boundingCoordinate[3] = chip.midCoordinate + SPair<double>(-chipHalf, chipHalf);
                tempChipVector.push_back(chip);
            }
        }
        if (per != (int) ((r * 100) / rMax))
        {
            per = (int) ((r * 100) / rMax);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing " + toString(per) + "%      \r");
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing 100%      ");

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Generated '" + toString(tempChipVector.size()) + "' chips");

    (*(this->m_outputChips)) = tempChipVector;

    return 0;
}

int CHarrisChips::SplitImagesForThreads()
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CHarrisChips::innerGenerate(CChipsGen<float>::CChipsGenContext *context)
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

}
