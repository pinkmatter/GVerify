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

#include "ul_GaussianPyramidTiePointGenerator.h"
#include "ul_Int_TiePointGenerator.h"

namespace ultra
{

class CTiledGaussianPyramidTiePointGenerator : public ITiePointGenerator
{
public:

    struct SContext
    {
        std::string inputScene;
        std::string referenceScene;
        SSize tileSize;
        std::string workingFolder;
        unsigned int amountOfPyramids;
        EResamplerEnum::EResampleType resampleType;
        unsigned long threadCount;
        unsigned long bandNumberToUse;
        float nullValue;
        bool mayContainNullValues;
        unsigned long chipSize;
        unsigned long chipGenerationGridSize;
        ECorrelationType typeOfChipCorrelationTechnique;
        CVector<EChips::EChipType> chipGeneratorMethods;
        CVector<SPair<double> > fixedLocationChips;
        std::string fixedLocationChipProj4Str;
        double correlationThreshold;

        SContext();
        ~SContext();
        SContext(const CTiledGaussianPyramidTiePointGenerator::SContext &r);

        CTiledGaussianPyramidTiePointGenerator::SContext &operator=(const CTiledGaussianPyramidTiePointGenerator::SContext &r);
    };
private:
    static const unsigned long REF_INDEX;
    static const unsigned long INPUT_INDEX;
    const SContext *m_context;
    std::string m_paddedInputScene;
    std::string m_paddedRefScene;
    SSize m_paddedImageSize;
    CMatrix<SSize> m_subImageUl;
    CMatrix<SSize> m_subImageSize;
    CVector<CVector<std::string> > m_tilePaths;


    int ContextOk();
    int GenerateTilesIndices();
    int MinMaxBoxImages(bool &overlapExists);
    int PaddImagesToSameSize();
    int LoadAndSaveTile(const std::string &baseScenePath, const std::string &tileScenePath, const SSize &selectUl, const SSize &selectSize);
    int GenerateTiles();
    int GenerateTiePoints(CVector<SChipCorrelationResult> &result);
    int CleanSubItems(const std::string &path);
    int ProcessTile(const CVector<std::string> &scenePaths,
                    const std::string &workingFolder,
                    CVector<SChipCorrelationResult> &result);
    int MergeAndCleanOutput(const CVector<CVector<SChipCorrelationResult> > &gcps,
                            CVector<SChipCorrelationResult> &result);
    int RemoveDuplicateGcps(CVector<SChipCorrelationResult> &result);
public:
    CTiledGaussianPyramidTiePointGenerator(const SContext *context);
    virtual ~CTiledGaussianPyramidTiePointGenerator();

    virtual int Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists = nullptr) override;
};

} // namespace ultra
