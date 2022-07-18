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

#include "ul_SobelChips.h"
#include "ul_EvenChips.h"
#include "ul_HarrisChips.h"
#include "ul_FixedLocationChips.h"
#include <ul_ImageLoader.h>

namespace ultra
{

int CTiePointGenerator::GenerateSobelChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector)
{
    CSobelChips::CSobelChipsContext context;
    context.chipWidth = m_context.innerContext->chipSizeMinimum;

    std::unique_ptr<CSobelChips> sobelChips(new CSobelChips());
    if (sobelChips.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CSobelChips object");
        return 1;
    }

    if (GenerateChips(sobelChips.get(), &context, image, chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateChips()");
        return 1;
    }

    return 0;
}

int CTiePointGenerator::GenerateEvenChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector)
{
    CEvenChips::CEvenChipsContext context;
    context.chipSize = m_context.innerContext->chipSizeMinimum;
    context.gridSize = m_context.innerContext->chipGenerationGridSize;
    std::unique_ptr<CEvenChips> evenChips(new CEvenChips());
    if (evenChips.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CEvenChips object");
        return 1;
    }

    if (GenerateChips(evenChips.get(), &context, image, chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateChips()");
        return 1;
    }

    return 0;
}


int CTiePointGenerator::GenerateHarrisChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector)
{
    CHarrisChips::CHarrisChipsContext context;
    context.chipSize = m_context.innerContext->chipSizeMinimum;
    std::unique_ptr<CHarrisChips> harrisChips(new CHarrisChips());

    if (harrisChips.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CMinMaxChips object");
        return 1;
    }

    if (GenerateChips(harrisChips.get(), &context, image, chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateChips()");
        return 1;
    }

    return 0;
}

int CTiePointGenerator::GenerateFixedLocationChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector)
{
    CFixedLocationChips::CFixedLocationChipsContext context;
    context.gridSize = m_context.innerContext->chipGenerationGridSize;
    context.chipSize = m_context.innerContext->chipSizeMinimum;
    SImageMetadata meta;
    if (CImageLoader::getInstance()->LoadImageMetadata(m_context.innerContext->referenceScene.pathToImage, meta) != 0 ||
        CImageLoader::getInstance()->LoadProj4Str(m_context.innerContext->referenceScene.pathToImage, context.imageProj4Str) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }
    context.imageAffineTransform = meta.getAffineGeoTransform();
    context.locationsProj4Str = m_context.innerContext->fixedLocationChipProj4Str.c_str();
    context.locations = m_context.innerContext->fixedLocationChips;

    std::unique_ptr<CFixedLocationChips> locationFileChips(new CFixedLocationChips());

    if (locationFileChips.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CFixedLocationChips object");
        return 1;
    }

    if (GenerateChips(locationFileChips.get(), &context, image, chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateChips()");
        return 1;
    }
    return 0;
}

int CTiePointGenerator::GenerateChips(CChipsGen<float> *chipGenner, CChipsGen<float>::CChipsGenContext *context,
                                      CMatrix<float> &image, CVector<CChip<float> > &chipVector)
{
    if (chipGenner->LoadData(&image, &chipVector) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadData()");
        return 1;
    }

    if (chipGenner->Generate(context) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Generate()");
        return 1;
    }

    return 0;
}

} // namespace ultra
