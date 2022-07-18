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

#include "ul_GaussianPyramidTiePointGenerator.h"

namespace ultra
{

CGaussianPyramidTiePointGenerator::SContext::SContext()
{
    amountOfPyramids = 1;
    minimumRequiredGcp = 30;
    resampleType = EResamplerEnum::RESAMPLE_TYPE_BI;
}

CGaussianPyramidTiePointGenerator::SContext::SContext(const SContext &r)
{
    resampleType = r.resampleType;
    minimumRequiredGcp = r.minimumRequiredGcp;
    amountOfPyramids = r.amountOfPyramids;
    tiePointGeneratorContext = r.tiePointGeneratorContext;
    workingFolder = r.workingFolder;
}

CGaussianPyramidTiePointGenerator::SContext::~SContext()
{
}

CGaussianPyramidTiePointGenerator::SContext &CGaussianPyramidTiePointGenerator::SContext::operator=(const SContext &r)
{
    resampleType = r.resampleType;
    minimumRequiredGcp = r.minimumRequiredGcp;
    amountOfPyramids = r.amountOfPyramids;
    tiePointGeneratorContext = r.tiePointGeneratorContext;
    workingFolder = r.workingFolder;
    return *this;
}

CGaussianPyramidTiePointGenerator::SInnerContext::SInnerContext()
{
    publicContex = nullptr;
}

CGaussianPyramidTiePointGenerator::SInnerContext::SInnerContext(const SInnerContext &r)
{
    publicContex = r.publicContex;
    pyramidDivLevels = r.pyramidDivLevels;
    inputPyramids = r.inputPyramids;
    referencePyramids = r.referencePyramids;
}

CGaussianPyramidTiePointGenerator::SInnerContext::~SInnerContext()
{
    publicContex = nullptr;
}

CGaussianPyramidTiePointGenerator::SInnerContext &CGaussianPyramidTiePointGenerator::SInnerContext::operator=(const SInnerContext &r)
{
    publicContex = r.publicContex;
    pyramidDivLevels = r.pyramidDivLevels;
    inputPyramids = r.inputPyramids;
    referencePyramids = r.referencePyramids;
    return *this;
}

CGaussianPyramidTiePointGenerator::SInnerContext::SPyramid::SPyramid()
{

}

CGaussianPyramidTiePointGenerator::SInnerContext::SPyramid::SPyramid(const SPyramid &r)
{
    filePath = r.filePath;
    level = r.level;
    imageMetadata = r.imageMetadata;
}

CGaussianPyramidTiePointGenerator::SInnerContext::SPyramid::~SPyramid()
{

}

CGaussianPyramidTiePointGenerator::SInnerContext::SPyramid &CGaussianPyramidTiePointGenerator::SInnerContext::SPyramid::operator=(const SPyramid &r)
{
    filePath = r.filePath;
    level = r.level;
    imageMetadata = r.imageMetadata;

    return *this;
}

} // namespace ultra
