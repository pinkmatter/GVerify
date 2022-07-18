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

#include "ul_TiledGaussianPyramidTiePointGenerator.h"

namespace ultra
{

CTiledGaussianPyramidTiePointGenerator::SContext::SContext()
{
    inputScene = "";
    referenceScene = "";
    tileSize = 3000;
    workingFolder = "";
    amountOfPyramids = 5;
    resampleType = EResamplerEnum::RESAMPLE_TYPE_BI;
    threadCount = 1;
    bandNumberToUse = 1;
    nullValue = 0;
    mayContainNullValues = true;
    chipSize = 65;
    chipGenerationGridSize = chipSize * 1.2;
    typeOfChipCorrelationTechnique = ECorrelationType::CCOEFF_NORM;
    chipGeneratorMethods.pushBack(EChips::CHIP_GEN_EVEN);
    correlationThreshold = 0.75;
}

CTiledGaussianPyramidTiePointGenerator::SContext::~SContext()
{

}

CTiledGaussianPyramidTiePointGenerator::SContext::SContext(const CTiledGaussianPyramidTiePointGenerator::SContext &r)
{
    inputScene = r.inputScene;
    referenceScene = r.referenceScene;
    tileSize = r.tileSize;
    workingFolder = r.workingFolder;
    amountOfPyramids = r.amountOfPyramids;
    resampleType = r.resampleType;
    threadCount = r.threadCount;
    bandNumberToUse = r.bandNumberToUse;
    nullValue = r.nullValue;
    mayContainNullValues = r.mayContainNullValues;
    chipSize = r.chipSize;
    chipGenerationGridSize = r.chipGenerationGridSize;
    typeOfChipCorrelationTechnique = r.typeOfChipCorrelationTechnique;
    chipGeneratorMethods = r.chipGeneratorMethods;
    correlationThreshold = r.correlationThreshold;
    fixedLocationChips = r.fixedLocationChips;
    fixedLocationChipProj4Str = r.fixedLocationChipProj4Str;
}

CTiledGaussianPyramidTiePointGenerator::SContext &CTiledGaussianPyramidTiePointGenerator::SContext::operator=(const CTiledGaussianPyramidTiePointGenerator::SContext &r)
{
    if (this == &r)
        return *this;
    inputScene = r.inputScene;
    referenceScene = r.referenceScene;
    tileSize = r.tileSize;
    workingFolder = r.workingFolder;
    amountOfPyramids = r.amountOfPyramids;
    resampleType = r.resampleType;
    threadCount = r.threadCount;
    bandNumberToUse = r.bandNumberToUse;
    nullValue = r.nullValue;
    mayContainNullValues = r.mayContainNullValues;
    chipSize = r.chipSize;
    chipGenerationGridSize = r.chipGenerationGridSize;
    typeOfChipCorrelationTechnique = r.typeOfChipCorrelationTechnique;
    chipGeneratorMethods = r.chipGeneratorMethods;
    correlationThreshold = r.correlationThreshold;
    fixedLocationChips = r.fixedLocationChips;
    fixedLocationChipProj4Str = r.fixedLocationChipProj4Str;
    return *this;
}

} // namespace ultra
