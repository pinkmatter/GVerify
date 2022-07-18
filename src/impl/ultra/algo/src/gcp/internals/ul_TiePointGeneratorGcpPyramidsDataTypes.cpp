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
#include "ul_ChipsGen.h"
#include "ul_DigitalImageCorrelator.h"

namespace ultra
{

CTiePointGeneratorGcpPyramids::SContext::SContext()
{
    m_pathToRefImage = "";
    m_pathToInputImage = "";
    m_pathToWorkingFolder = "";
    m_chipGeneratorMethods.clear();
    m_correlationThreshold = 0;
    m_threadCount = 1;
    m_chipSizeMinimum = 65;
    m_useNullValue = false;
    m_pyramidCount = 1;
    m_nullValue = 0;
    m_pyramidCount = 1;
}

CTiePointGeneratorGcpPyramids::SContext::SContext(const CTiePointGeneratorGcpPyramids::SContext &r)
{
    m_pathToRefImage = r.m_pathToRefImage;
    m_pathToInputImage = r.m_pathToInputImage;
    m_pathToWorkingFolder = r.m_pathToWorkingFolder;
    m_chipGeneratorMethods = r.m_chipGeneratorMethods;
    m_correlationThreshold = r.m_correlationThreshold;
    m_threadCount = r.m_threadCount;
    m_chipSizeMinimum = r.m_chipSizeMinimum;
    m_useNullValue = r.m_useNullValue;
    m_nullValue = r.m_nullValue;
    m_typeOfChipCorrelationTechnique = r.m_typeOfChipCorrelationTechnique;
    m_typeOfSubPixelChipCorrelationTechnique = r.m_typeOfSubPixelChipCorrelationTechnique;
    m_pyramidCount = r.m_pyramidCount;
}

CTiePointGeneratorGcpPyramids::SContext::~SContext()
{

}

CTiePointGeneratorGcpPyramids::SContext &CTiePointGeneratorGcpPyramids::SContext::operator=(const CTiePointGeneratorGcpPyramids::SContext &r)
{
    m_pathToRefImage = r.m_pathToRefImage;
    m_pathToInputImage = r.m_pathToInputImage;
    m_pathToWorkingFolder = r.m_pathToWorkingFolder;
    m_chipGeneratorMethods = r.m_chipGeneratorMethods;
    m_correlationThreshold = r.m_correlationThreshold;
    m_threadCount = r.m_threadCount;
    m_chipSizeMinimum = r.m_chipSizeMinimum;
    m_useNullValue = r.m_useNullValue;
    m_nullValue = r.m_nullValue;
    m_typeOfChipCorrelationTechnique = r.m_typeOfChipCorrelationTechnique;
    m_typeOfSubPixelChipCorrelationTechnique = r.m_typeOfSubPixelChipCorrelationTechnique;
    m_pyramidCount = r.m_pyramidCount;
    return *this;
}

void CTiePointGeneratorGcpPyramids::SContext::setUseNullValue(bool useNullValue, float nullValue)
{
    m_useNullValue = useNullValue;
    m_nullValue = nullValue;
}

void CTiePointGeneratorGcpPyramids::SContext::setThreadCount(unsigned long threadCount)
{
    m_threadCount = threadCount;
}

void CTiePointGeneratorGcpPyramids::SContext::setCorrelationThreshold(double correlationThreshold)
{
    m_correlationThreshold = correlationThreshold;
}

void CTiePointGeneratorGcpPyramids::SContext::setChipSize(unsigned long chipSize, unsigned long chipGenerationGridSize)
{
    m_chipSizeMinimum = chipSize;
    m_chipGenerationGridSize = chipGenerationGridSize;
}

void CTiePointGeneratorGcpPyramids::SContext::setCorrelationTechnique(ECorrelationType chipCorrelationTechnique,
                                                                      ESubPixelCorrelationType subPixelChipCorrelationTechnique,
                                                                      unsigned long searchWindowSize)
{
    m_typeOfChipCorrelationTechnique = chipCorrelationTechnique;
    m_typeOfSubPixelChipCorrelationTechnique = subPixelChipCorrelationTechnique;
    m_searchWindowSize = searchWindowSize;
}

void CTiePointGeneratorGcpPyramids::SContext::setWorkingFolder(const std::string &workingFolderPath)
{
    m_pathToWorkingFolder = workingFolderPath.c_str();
}

void CTiePointGeneratorGcpPyramids::SContext::setInputImage(const std::string &inputImagePath)
{
    m_pathToInputImage = inputImagePath.c_str();
}

void CTiePointGeneratorGcpPyramids::SContext::setReferenceImage(const std::string &referenceImagePath)
{
    m_pathToRefImage = referenceImagePath.c_str();
}

void CTiePointGeneratorGcpPyramids::SContext::addChipGenerationMethod(EChips::EChipType chipGenMethod)
{
    if (!m_chipGeneratorMethods.contains(chipGenMethod))
        m_chipGeneratorMethods.pushBack(chipGenMethod);
}

void CTiePointGeneratorGcpPyramids::SContext::setChipGenerationMethod(const CVector<EChips::EChipType> &chipGenMethods)
{
    m_chipGeneratorMethods = chipGenMethods;
}

void CTiePointGeneratorGcpPyramids::SContext::clearChipGenerationMethod()
{
    m_chipGeneratorMethods.clear();
}

void CTiePointGeneratorGcpPyramids::SContext::setPyramidCount(unsigned long pyramidCount)
{
    m_pyramidCount = pyramidCount;
}

} // namespace ultra
