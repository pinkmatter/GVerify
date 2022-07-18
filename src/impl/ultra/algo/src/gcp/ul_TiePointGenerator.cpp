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

#include "ul_TiePointGenerator.h"

#include "ul_Utility.h"
#include "ul_File.h"
#include "ul_ImageLoader.h"

namespace ultra
{

const int CTiePointGenerator::REF_IMG_INDEX = 0;
const int CTiePointGenerator::INPUT_IMG_INDEX = 1;

CTiePointGenerator::CTiePointGenerator(const CTiePointGenerator::SContext *context) :
ITiePointGenerator()
{
    m_context.innerContext = context;
}

CTiePointGenerator::~CTiePointGenerator()
{

}

int CTiePointGenerator::TranslateChipCoordinatesToMap(CVector<CChip<float> > &chipVector)
{
    unsigned long size = chipVector.size();
    for (unsigned long x = 0; x < size; x++)
    {
        chipVector[x].midCoordinate *= m_context.referenceSceneMetadata.gsd;
        chipVector[x].midCoordinate += m_context.referenceSceneMetadata.origin;
        for (unsigned long p = 0; p < 4; p++)
        {
            chipVector[x].boundingCoordinate[p] *= m_context.referenceSceneMetadata.gsd;
            chipVector[x].boundingCoordinate[p] += m_context.referenceSceneMetadata.origin;
        }
    }

    return 0;
}

int CTiePointGenerator::GenerateChips(const std::string pathToImageWhereChipsAreLoadedFrom,
                                      CMatrix<float> &image, CVector<CChip<float> > &chipVector,
                                      EChips::EChipType chipType)
{
    m_fp_generateChips = nullptr;
    switch (chipType)
    {
    case EChips::CHIP_GEN_SOBEL:
        m_fp_generateChips = &CTiePointGenerator::GenerateSobelChips;
        break;
    case EChips::CHIP_GEN_EVEN:
        m_fp_generateChips = &CTiePointGenerator::GenerateEvenChips;
        break;
    case EChips::CHIP_GEN_HARRIS:
        m_fp_generateChips = &CTiePointGenerator::GenerateHarrisChips;
        break;
    case EChips::CHIP_GEN_FIXED_LOCATION:
        m_fp_generateChips = &CTiePointGenerator::GenerateFixedLocationChips;
        break;
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid chip generations system system");
        return 1;
    }

    if (m_fp_generateChips == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid chip generation function pointer");
        return 1;
    }

    unsigned long chipSizeBuffered = m_context.innerContext->chipSizeMinimum * 3;
    SSize imageSize = image.getSize();
    if (chipSizeBuffered > imageSize.col ||
        chipSizeBuffered > imageSize.row)
    {
        chipVector.clear();
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Image size is too small to extract chips");
        return 0;
    }

    if ((this->*m_fp_generateChips)(image, chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from m_fp_generateChips()");
        return 1;
    }

    if (TranslateChipCoordinatesToMap(chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from TranslateChipCoordinatesToMap()");
        return 1;
    }

    return 0;
}

int CTiePointGenerator::StartCorrelation(
                                         CVector<CChip<float> > &chipVector,
                                         CVector<SChipCorrelationResult> &result
                                         )
{
    SSize chipSize = m_context.innerContext->chipSizeMinimum;
    SSize searchWindowSize = chipSize;
    if (m_context.innerContext->typeOfChipCorrelationTechnique != ECorrelationType::PHASE)
        searchWindowSize += m_context.innerContext->searchWindowSize;

    std::string infoMessage = "Correlating '" + toString(chipVector.size()) + "' chips "
        "of size '" + toString(chipSize.row) + "x" + toString(chipSize.col) + "' "
        "on a search window of size '" + toString(searchWindowSize.row) + "x" + toString(searchWindowSize.col) + "' "
        "using a '" + CCorrelationHelper::typeToStr(m_context.innerContext->typeOfChipCorrelationTechnique) + "' correlator";
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, infoMessage);

    CMatrix<SPair<double> > worldMatrix;

    SPair<double> originDiv = m_context.referenceSceneMetadata.origin - m_context.inputSceneMetadata.origin;

    if (GenerateWorldMatrix(worldMatrix,
                            m_context.inputSceneMetadata.imageSize,
                            m_context.inputSceneMetadata.gsd,
                            m_context.inputSceneMetadata.origin + originDiv,
                            m_context.innerContext->inputOffsetShiftInPixels) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateWorldMatrix()");
        return 1;
    }

    if (Correlate(worldMatrix, chipVector, result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Correlate()");
        return 1;
    }

    return 0;
}

void CTiePointGenerator::RemoveDuplicateGcps(CVector<SChipCorrelationResult> &finalResult)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Removing duplicate GCPs");
    unsigned long resultSize = finalResult.size();
    long counter = 0;
    if (resultSize > 0)
    {
        SChipCorrelationResult *dp = finalResult.getDataPointer();
        for (unsigned long t = 0; t < resultSize - 1; t++)
        {
            if (dp[t].correlationResultIsGood)
            {
                unsigned long start = t + 1;
                for (unsigned long i = start; i < resultSize; i++)
                {
                    if (dp[i].correlationResultIsGood &&
                        dp[t].chip.midCoordinate == dp[i].chip.midCoordinate)
                    {
                        dp[i].correlationResultIsGood = false;
                        counter++;
                    }
                }
            }
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Removed '" + toString(counter) + "' duplicate GCPs");
}

int CTiePointGenerator::MainLoop(CVector<SChipCorrelationResult> &finalResult)
{
    CVector<CVector<CChip<float> > > chipVectorTemp;
    CVector<CChip<float> > chipVector;

    unsigned long chipLoop;
    unsigned long copyLoop;
    unsigned long totalChipsSize;

    chipVectorTemp.resize(m_context.innerContext->chipGeneratorMethods.size());

    if (LoadTwoImages() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadTwoImages()");
        return 1;
    }

    totalChipsSize = 0;
    for (chipLoop = 0; chipLoop < chipVectorTemp.size(); chipLoop++)
    {
        if (GenerateChips(m_context.innerContext->referenceScene.pathToImage,
                          m_context.images[REF_IMG_INDEX], chipVectorTemp[chipLoop],
                          m_context.innerContext->chipGeneratorMethods[chipLoop]) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateChips()");
            return 1;
        }
        totalChipsSize += chipVectorTemp[chipLoop].size();
    }

    // no chips found so we can just skip the rest
    if (totalChipsSize == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "No chips found, skipping correlation");
    }

    chipVector.resize(totalChipsSize);

    totalChipsSize = 0;
    for (chipLoop = 0; chipLoop < chipVectorTemp.size(); chipLoop++)
    {
        for (copyLoop = 0; copyLoop < chipVectorTemp[chipLoop].size(); copyLoop++)
        {
            chipVector[totalChipsSize] = chipVectorTemp[chipLoop][copyLoop];
            chipVector[totalChipsSize].chipId = totalChipsSize;
            totalChipsSize++;
        }
    }

    if (StartCorrelation(chipVector, finalResult) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from StartCorrelation()");
        return 1;
    }

    if (finalResult.size() == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "No GCPs correlated");
        return 0;
    }

    RemoveDuplicateGcps(finalResult);

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Obtained '" + toString(finalResult.size()) + "' chips");

    return 0;
}

int CTiePointGenerator::Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists)
{
    if (overlapExists != nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented usage of 'overlapExists'");
        return 1;
    }

    if (!isContextOK(m_context.innerContext))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isContextOK()");
        return 1;
    }

    if (LoadImgMetadata() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImgMetadata()");
        return 1;
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "All input data and parameters seem OK");

    if (MainLoop(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from MainLoop()");
        return 1;
    }

    if (HullRejectGcps(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from HullRejectGcps()");
        return 1;
    }

    return 0;
}

} //namespace ultra
