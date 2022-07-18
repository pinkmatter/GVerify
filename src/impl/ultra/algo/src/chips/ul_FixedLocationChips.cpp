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

#include "ul_FixedLocationChips.h"

#include <ul_Proj4Projection.h>

namespace ultra
{

CFixedLocationChips::CFixedLocationChips() :
CChipsGen<float>("FixedLocation_ChipsGen")
{

}

CFixedLocationChips::~CFixedLocationChips()
{

}

int CFixedLocationChips::SplitImagesForThreads()
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CFixedLocationChips::Init(CChipsGen<float>::CChipsGenContext *context)
{
    m_context = dynamic_cast<CFixedLocationChips::CFixedLocationChipsContext *> (context);
    if (m_context == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to cast context, please pass in CFixedLocationChips::CFixedLocationChipsContext struct");
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

    if (m_context->locations.size() == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "No input locations found");
        return 1;
    }

    if (m_context->imageAffineTransform.size() != 6)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Affine transform requires 6 values");
        return 1;
    }

    return 0;
}

bool CFixedLocationChips::outOfBounds(const SPair<double> &sl, const SPair<double> &ulLimit, const SPair<double> &lrLimit) const
{
    return (sl.r < ulLimit.r || sl.c < ulLimit.c ||
        sl.r > lrLimit.r || sl.c > lrLimit.c);
}

int CFixedLocationChips::Process()
{
    std::unique_ptr<CProj4Projection> proj(new CProj4Projection(m_context->locationsProj4Str, m_context->imageProj4Str));

    SPair<double> zero = 0;
    SPair<double> imageSize = this->m_inputImage->getSize();

    SPair<double> ulLimit = zero + (m_context->chipSize * 2);
    SPair<double> lrLimit = imageSize - (m_context->chipSize * 2) - 1;

    CChip<float> chip;
    chip.chipType = EChips::chipGenTypeToStr(EChips::CHIP_GEN_FIXED_LOCATION);
    chip.worldMatrix.clear();
    unsigned long chipId = 0;
    long cornerOffset = m_context->chipSize / 2;
    SSize ul;
    SSize chipSize = m_context->chipSize;
    CSmartVector<CChip<float> > q;
    int per = 0;
    unsigned long size = m_context->locations.size();
    for (unsigned long t = 0; t < size; t++)
    {
        SPair<double> pt = m_context->locations[t];
        if (proj->Project(pt, pt) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failure returned from CProj4Projection::Project()");
            continue;
        }
        SPair<double> sl = round(SImageMetadata::getSampleLine(m_context->imageAffineTransform, pt));
        if (!outOfBounds(sl, ulLimit, lrLimit))
        {
            ul.row = sl.r - cornerOffset;
            ul.col = sl.c - cornerOffset;
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


            chip.chipId = chipId++;
            chip.midCoordinate = sl;

            chip.boundingCoordinate[0] = sl + SPair<double>(-cornerOffset, -cornerOffset);
            chip.boundingCoordinate[1] = sl + SPair<double>(cornerOffset, -cornerOffset);
            chip.boundingCoordinate[2] = sl + SPair<double>(cornerOffset, cornerOffset);
            chip.boundingCoordinate[3] = sl + SPair<double>(-cornerOffset, cornerOffset);

            q.pushBack(chip);

            if (per != (int) ((t * 100) / size))
            {
                per = (int) ((t * 100) / size);
                CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing " + toString(per) + "%      \r");
            }
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing 100%      ");
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Generated '" + toString(q.size()) + "' chips");
    (*this->m_outputChips) = q.toVector();
    return 0;
}

int CFixedLocationChips::innerGenerate(CChipsGen<float>::CChipsGenContext *context)
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
