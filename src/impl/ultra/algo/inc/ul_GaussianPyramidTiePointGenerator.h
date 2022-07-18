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

#include "ul_TiePointGenerator.h"
#include "ul_Resampler.h"
#include "ul_Int_TiePointGenerator.h"

namespace ultra
{

class CGaussianPyramidTiePointGenerator : public ITiePointGenerator
{
public:

    struct SContext
    {
        EResamplerEnum::EResampleType resampleType;
        std::string workingFolder;
        unsigned long amountOfPyramids;
        CTiePointGenerator::SContext tiePointGeneratorContext;
        unsigned long minimumRequiredGcp;

        SContext();
        SContext(const SContext &r);
        ~SContext();

        SContext &operator=(const SContext &r);
    };

private:

    struct SInnerContext
    {

        struct SPyramid
        {
            std::string filePath;
            long level;
            SImageMetadata imageMetadata;

            SPyramid();
            SPyramid(const SPyramid &r);
            ~SPyramid();

            SPyramid& operator=(const SPyramid &r);
        };

        CVector<SPyramid> inputPyramids;
        CVector<SPyramid> referencePyramids;
        const SContext *publicContex;
        CVector<unsigned long> pyramidDivLevels;

        SInnerContext();
        SInnerContext(const SInnerContext &r);
        ~SInnerContext();

        SInnerContext &operator=(const SInnerContext &r);
    };

    SInnerContext m_context;

    bool isContextOk();
    int checkImageSize(const std::string &pathToImage, unsigned long modVal);
    int GeneratePyramidImages(const std::string &imagePath, CVector<SInnerContext::SPyramid> &pyramids);
    int NextPyramid(CMatrixArray<float> &imageVec, SImageMetadata &imageMetadata);
    int GenerateGcps(CVector<SChipCorrelationResult> &result);
    int CalcAvgShift(const CVector<SChipCorrelationResult> &result,
                     const SPair<double> &gsd, SPair<double> &avgShift,
                     bool &skip);
    int UpdateResult(
                     CVector<SChipCorrelationResult> &result,
                     const SPair<double> &avgShift1
                     );

public:
    CGaussianPyramidTiePointGenerator(const CGaussianPyramidTiePointGenerator::SContext *context);
    virtual ~CGaussianPyramidTiePointGenerator();

    virtual int Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists = nullptr) override;
};

} // namespace ultra
