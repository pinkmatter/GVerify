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

namespace ultra
{

CTiePointGenerator::SContext::SScene::SScene()
{
    pathToImage = "";
    bandNumber = 1;
}

CTiePointGenerator::SContext::SScene::SScene(const CTiePointGenerator::SContext::SScene & r)
{
    pathToImage = r.pathToImage;
    bandNumber = r.bandNumber;
}

CTiePointGenerator::SContext::SScene::~SScene()
{

}

CTiePointGenerator::SContext::SScene &CTiePointGenerator::SContext::SScene::operator=(const CTiePointGenerator::SContext::SScene &r)
{
    if (this == &r)
        return *this;
    pathToImage = r.pathToImage;
    bandNumber = r.bandNumber;
    return *this;
}

CTiePointGenerator::SContext::SContext()
{
    inputOffsetShiftInPixels = 0;
    threadCount = 0;
    chipSizeMinimum = 0;
    searchWindowSize = 0;
    correlationThreshold = 0.75;
    m_useNullValue = false;
    m_mayContainNullValues = true;
    outerHullRejectGcps = true;
    typeOfChipCorrelationTechnique = ECorrelationType::CORRELATION_TYPE_COUNT;
}

CTiePointGenerator::SContext::SContext(const CTiePointGenerator::SContext &r)
{
    chipGenerationGridSize = r.chipGenerationGridSize;
    inputOffsetShiftInPixels = r.inputOffsetShiftInPixels;
    threadCount = r.threadCount;
    chipSizeMinimum = r.chipSizeMinimum;
    typeOfChipCorrelationTechnique = r.typeOfChipCorrelationTechnique;
    correlationThreshold = r.correlationThreshold;
    searchWindowSize = r.searchWindowSize;
    outerHullRejectGcps = r.outerHullRejectGcps;
    chipGeneratorMethods = r.chipGeneratorMethods;
    fixedLocationChips = r.fixedLocationChips;
    fixedLocationChipProj4Str = r.fixedLocationChipProj4Str;

    inputScene = r.inputScene;
    referenceScene = r.referenceScene;

    m_useNullValue = r.m_useNullValue;
    m_nullValue = r.m_nullValue;
    m_mayContainNullValues = r.m_mayContainNullValues;
}

CTiePointGenerator::SContext::~SContext()
{

}

CTiePointGenerator::SContext &CTiePointGenerator::SContext::operator=(const CTiePointGenerator::SContext &r)
{
    if (this == &r)
        return *this;
    chipGenerationGridSize = r.chipGenerationGridSize;
    inputOffsetShiftInPixels = r.inputOffsetShiftInPixels;
    threadCount = r.threadCount;
    chipSizeMinimum = r.chipSizeMinimum;
    typeOfChipCorrelationTechnique = r.typeOfChipCorrelationTechnique;
    correlationThreshold = r.correlationThreshold;
    searchWindowSize = r.searchWindowSize;
    outerHullRejectGcps = r.outerHullRejectGcps;
    chipGeneratorMethods = r.chipGeneratorMethods;
    fixedLocationChips = r.fixedLocationChips;
    fixedLocationChipProj4Str = r.fixedLocationChipProj4Str;

    inputScene = r.inputScene;
    referenceScene = r.referenceScene;

    m_useNullValue = r.m_useNullValue;
    m_nullValue = r.m_nullValue;
    m_mayContainNullValues = r.m_mayContainNullValues;

    return *this;
}

void CTiePointGenerator::SContext::setNullValue(float val)
{
    m_nullValue = val;
    m_useNullValue = true;
}

float CTiePointGenerator::SContext::getNullValue() const
{
    return m_nullValue;
}

void CTiePointGenerator::SContext::disableNullValue()
{
    m_useNullValue = false;
}

bool CTiePointGenerator::SContext::useNullValue() const
{
    return m_useNullValue;
}

bool CTiePointGenerator::SContext::getMayContainNullValues() const
{
    return m_mayContainNullValues;
}

void CTiePointGenerator::SContext::setMayContainNullValues(bool mayContainNullValues)
{
    m_mayContainNullValues = mayContainNullValues;
}

CTiePointGenerator::SInnerContext::SSceneMetadata::SSceneMetadata()
{

}

CTiePointGenerator::SInnerContext::SSceneMetadata::SSceneMetadata(const CTiePointGenerator::SInnerContext::SSceneMetadata &r)
{
    origin = r.origin;
    gsd = r.gsd;
    imageEnumType = r.imageEnumType;
    imageSize = r.imageSize;
    bpp = r.bpp;
}

CTiePointGenerator::SInnerContext::SSceneMetadata &CTiePointGenerator::SInnerContext::SSceneMetadata::operator=(const CTiePointGenerator::SInnerContext::SSceneMetadata &r)
{
    if (this == &r)
        return *this;
    origin = r.origin;
    gsd = r.gsd;
    imageEnumType = r.imageEnumType;
    imageSize = r.imageSize;
    bpp = r.bpp;
    return *this;
}

CTiePointGenerator::SInnerContext::SSceneMetadata::~SSceneMetadata()
{

}

CTiePointGenerator::SInnerContext::SInnerContext()
{
}

CTiePointGenerator::SInnerContext::~SInnerContext()
{

}

CTiePointGenerator::SInnerContext::SInnerContext(const SInnerContext & r)
{
    referenceSceneMetadata = r.referenceSceneMetadata;
    inputSceneMetadata = r.inputSceneMetadata;
    innerContext = r.innerContext;
    images[0] = r.images[0];
    images[1] = r.images[1];
}

} // namespace ultra
