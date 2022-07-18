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

#include "ul_ImageLoader.h"
#include "ul_ImageSaver.h"
#include "ul_File.h"
#include "ul_Resampler.h"

namespace ultra
{

CGaussianPyramidTiePointGenerator::CGaussianPyramidTiePointGenerator(const CGaussianPyramidTiePointGenerator::SContext *context) :
ITiePointGenerator()
{
    m_context.publicContex = context;
}

CGaussianPyramidTiePointGenerator::~CGaussianPyramidTiePointGenerator()
{

}

int CGaussianPyramidTiePointGenerator::NextPyramid(CMatrixArray<float> &imageVec, SImageMetadata &imageMetadata)
{
    double mod = 2;
    SSize size = imageVec[0].getSize() / (long) mod;
    CMatrix<SPair<double> > inputToOutput;
    inputToOutput.resize(size);
    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            inputToOutput[r][c].r = r*mod;
            inputToOutput[r][c].c = c*mod;
        }
    }


    CMatrix<float> output;
    for (unsigned long x = 0; x < imageVec.size(); x++)
    {
        if (CResampler<false>::Resample<float>(imageVec[x], output, inputToOutput, m_context.publicContex->resampleType) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Resample()");
            return 1;
        }
        imageVec[x] = output;
    }

    SPair<double> gsd = imageMetadata.getGsd();
    SSize dims = imageMetadata.getDimensions();

    gsd *= mod;
    dims /= (long) mod;

    SPair<double> gsdDiv = 0;
    SPair<double> newOrigin = imageMetadata.getOrigin();
    imageMetadata.setDimensions(dims);
    imageMetadata.setGsd(gsd);
    imageMetadata.setOrigin(newOrigin - gsdDiv);
    return 0;
}

int CGaussianPyramidTiePointGenerator::GeneratePyramidImages(const std::string &imagePath, CVector<SInnerContext::SPyramid> &pyramids)
{
    CMatrixArray<float> imageVec;
    CFile file = imagePath;

    pyramids.resize(m_context.pyramidDivLevels.size());
    pyramids[0].filePath = imagePath;
    std::string proj4Str;
    if (CImageLoader::getInstance()->LoadImageMetadata(pyramids[0].filePath, pyramids[0].imageMetadata) != 0 ||
        CImageLoader::getInstance()->LoadProj4Str(pyramids[0].filePath, proj4Str) != 0 ||
        CImageLoader::getInstance()->LoadImage(pyramids[0].filePath, imageVec) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to load image data");
        return 1;
    }
    pyramids[0].imageMetadata.setProj4String(proj4Str);

    std::string itemName = file.getFileNameWithoutExtension();
    for (long t = 1; t < m_context.pyramidDivLevels.size(); t++)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Generating pyramid level '" + toString(t + 1) + "' of '" + toString(m_context.pyramidDivLevels.size()) + "' for image '" + itemName + "'");

        pyramids[t].imageMetadata = pyramids[t - 1].imageMetadata;
        pyramids[t].filePath = CFile(m_context.publicContex->workingFolder).getPath() + CFile::separatorStr + "temp_" + toString(t + 1) + "_" + toString(m_context.pyramidDivLevels.size()) + "_" + itemName + ".TIF";
        if (NextPyramid(imageVec, pyramids[t].imageMetadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from NextPyramid()");
            return 1;
        }

        if (CImageSaver::getInstance()->SaveImage(pyramids[t].filePath, imageVec, EImage::IMAGE_TYPE_GEOTIFF, &pyramids[t].imageMetadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SaveImage()");
            return 1;
        }
    }

    return 0;
}

int CGaussianPyramidTiePointGenerator::Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists)
{
    if (overlapExists != nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented usage of 'overlapExists'");
        return 1;
    }

    if (!isContextOk())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isContextOk()");
        return 1;
    }

    if (GeneratePyramidImages(m_context.publicContex->tiePointGeneratorContext.inputScene.pathToImage, m_context.inputPyramids) != 0 ||
        GeneratePyramidImages(m_context.publicContex->tiePointGeneratorContext.referenceScene.pathToImage, m_context.referencePyramids) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GeneratePyramidImages()");
        return 1;
    }

    if (GenerateGcps(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateGcps()");
        return 1;
    }

    return 0;
}

} // namespace ultra
