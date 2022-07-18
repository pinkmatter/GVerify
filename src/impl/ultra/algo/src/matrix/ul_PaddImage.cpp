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

#include "ul_PaddImage.h"

#include "ul_ImageLoader.h"
#include "ul_ImageSaver.h"

namespace ultra
{

CPaddImage::CPaddImage()
{

}

CPaddImage::~CPaddImage()
{

}

int CPaddImage::UpdateMetadata(const SImageMetadata &oldMetadata, SImageMetadata &newMetadata, const SSize &padSizeUl, const SSize &padSizeLr)
{
    SSize paddSizeBoth = padSizeUl + padSizeLr;
    newMetadata = oldMetadata;

    SPair<double> gsd, origin;
    SSize dims;

    gsd.x = 1;
    gsd.y = -1;

    gsd = newMetadata.getGsd();
    origin = newMetadata.getOrigin();
    origin -= gsd * padSizeUl;
    newMetadata.setOrigin(origin);

    dims = newMetadata.getDimensions();
    dims += paddSizeBoth;
    newMetadata.setDimensions(dims);

    return 0;
}

int CPaddImage::Padd(
                     const std::string &pathToImage,
                     const SSize &padSize,
                     float paddVal,
                     CMatrixArray<float>& outputImage,
                     SImageMetadata &newMetadata,
                     EImage::EImageFormat &imageType,
                     int &bpp
                     )
{
    CMatrixArray<float> inputImage;
    SImageMetadata imageMetadata;

    if (CImageLoader::getInstance()->LoadImageMetadata(pathToImage, imageMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetImageType");
        return 1;
    }

    bpp = imageMetadata.bpp;

    if (CImageLoader::getInstance()->LoadImage(pathToImage, inputImage) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }

    if (CImageLoader::getInstance()->LoadImageType(pathToImage, imageType) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageType");
        return 1;
    }

    if (Padd(inputImage, imageMetadata, padSize, paddVal, outputImage, newMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Padd");
        return 1;
    }

    return 0;
}

int CPaddImage::Padd(
                     const CMatrixArray<float>& inputImage,
                     const SImageMetadata &inputMetadata,
                     const SSize &padSize,
                     float paddVal,
                     CMatrixArray<float>& outputImage,
                     SImageMetadata &newMetadata
                     )
{
    if (Padd(inputImage, inputMetadata, padSize, padSize, paddVal, outputImage, newMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Padd");
        return 1;
    }

    return 0;
}

int CPaddImage::InnerPadd(
                          const CMatrixArray<float>& inputImage,
                          const SSize &padSizeUl,
                          const SSize &padSizeLr,
                          float paddVal,
                          CMatrixArray<float>& outputImage
                          )
{
    SSize paddSizeBoth = padSizeUl + padSizeLr;
    SSize newImageSize;
    outputImage.resize(inputImage.size());

    int per = 0;
    CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing 0%     \r");
    for (unsigned long t = 0; t < inputImage.size(); t++)
    {
        newImageSize = inputImage[t].getSize() + paddSizeBoth;
        outputImage[t].resize(newImageSize);

        // cannot use the memcpy call here
        outputImage[t].initMat(paddVal);

        if (outputImage[t].CopyFrom(padSizeUl, inputImage[t]) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CopyFrom()");
            return 1;
        }

        if (per != (100 * (t + 1)) / inputImage.size())
        {
            per = (100 * (t + 1)) / inputImage.size();
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing " + toString(per) + "%     \r");
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing 100%     ");
    return 0;
}

int CPaddImage::Padd(
                     const CMatrixArray<float>& inputImage,
                     const SImageMetadata &inputMetadata,
                     const SSize &padSizeUl,
                     const SSize &padSizeLr,
                     float paddVal,
                     CMatrixArray<float>& outputImage,
                     SImageMetadata &newMetadata
                     )
{
    if (InnerPadd(inputImage, padSizeUl, padSizeLr, paddVal, outputImage) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from InnerPadd()");
        return 1;
    }

    if (UpdateMetadata(inputMetadata, newMetadata, padSizeUl, padSizeLr) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from UpdateMetadata()");
        return 1;
    }

    return 0;
}

int CPaddImage::Padd(CMatrix<float> &image, const SSize &paddSizeUl, const SSize &paddSizeLr, float paddVal)
{
    if (paddSizeUl == SSize(0, 0) &&
        paddSizeLr == SSize(0, 0))
    {
        //nothing to pad
        return 0;
    }

    SSize imageSize = image.getSize();
    CMatrix<float> tempInput = image;
    if (tempInput.getSize() != imageSize)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to copy matrix");
        return 1;
    }

    imageSize += paddSizeUl + paddSizeLr;
    image.resize(imageSize);
    image.initMatMemset(paddVal);
    if (image.CopyFrom(paddSizeUl, tempInput) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CopyFrom()");
        return 1;
    }

    return 0;
}

int CPaddImage::Padd(CMatrix<float> &image, const SSize &paddSize)
{
    if (paddSize == SSize(0, 0))
    {
        //nothing to pad
        return 0;
    }

    SSize imageSize = image.getSize();
    CMatrix<float> tempInput = image;
    if (tempInput.getSize() != imageSize)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to copy matrix");
        return 1;
    }

    imageSize += paddSize;
    image.resize(imageSize);
    image.initMatMemset(0);

    if (image.CopyFrom(SSize(0, 0), tempInput) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CopyFrom()");
        return 1;
    }

    return 0;
}

int CPaddImage::RemovePadd(CMatrix<float> &image, const SSize &removeSizeUl, const SSize &removeSizeLr)
{
    if (removeSizeUl == SSize(0, 0) &&
        removeSizeLr == SSize(0, 0))
    {
        //nothing to remove
        return 0;
    }

    SSize imageSize = image.getSize();

    SPair<long> newImageSize = imageSize;
    newImageSize -= removeSizeUl;
    newImageSize -= removeSizeLr;

    if (newImageSize.r < 0 || newImageSize.c < 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to remove more rows and/or columns than the input matrix has, rather call CMatrix.clear()");
        return 1;
    }

    image = image.getSubMatrix(removeSizeUl, newImageSize.getSizeType());

    if (image.getSize() != newImageSize.getSizeType())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to get sub-matrix");
        return 1;
    }

    return 0;
}

int CPaddImage::RemovePadd(CMatrix<float> &image, const SSize &removeSize)
{
    if (removeSize == SSize(0, 0))
    {
        //nothing to remove
        return 0;
    }

    SSize imageSize = image.getSize();

    if (removeSize.row > imageSize.row ||
        removeSize.col > imageSize.col)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to remove more rows and/or columns than the input matrix has, rather call CMatrix.clear()");
        return 1;
    }

    imageSize -= removeSize;
    image = image.getSubMatrix(SSize(0, 0), imageSize);

    if (image.getSize() != imageSize)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to get sub-matrix");
        return 1;
    }

    return 0;
}

int CPaddImage::Padd(const std::string &pathToImage, const SSize &padSizeUl, const SSize &padSizeLr, float paddVal, const std::string pathToImageSaveLocation)
{
    CMatrixArray<float> inputImage, outputImage;
    SImageMetadata inputMetadata, newMetadata;
    EImage::EImageFormat imageType;

    if (CImageLoader::getInstance()->LoadImage(pathToImage, inputImage) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }

    if (CImageLoader::getInstance()->LoadImageType(pathToImage, imageType) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageType()");
        return 1;
    }
    std::string proj4str;
    if (CImageLoader::getInstance()->LoadImageMetadata(pathToImage, inputMetadata) != 0 ||
        CImageLoader::getInstance()->LoadProj4Str(pathToImage, proj4str) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetImageType()");
        return 1;
    }
    inputMetadata.setProj4String(proj4str);

    if (Padd(inputImage, inputMetadata, padSizeUl, padSizeLr, paddVal, outputImage, newMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Padd()");
        return 1;
    }

    if (CImageSaver::getInstance()->SaveImage(pathToImageSaveLocation,
                                              outputImage,
                                              imageType,
                                              &newMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SaveImage()");
        return 1;
    }
    return 0;
}

} // namespace ultra

