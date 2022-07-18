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

#include <iostream>
#include <map>

#include <ul_Image.h>
#include <ul_MatrixArray.h>
#include <ul_Pair.h>
#include <ul_Odl.h>
#include <ul_ImageMetadataObjects.h>
#include <ul_Int_ImageLoader.h>
#include <ul_KeyValue.h>

namespace ultra
{

class CImageLoader
{
private:

    __ultra_internal::IImageLoader* getLoaderInstance();

    // constructor
    CImageLoader();

    int CheckIfPathExists(const std::string &pathToImageFile) const;
public:
    virtual ~CImageLoader();
    static CImageLoader *getInstance();

    template<class T>
    int LoadImage(std::string pathToImageFile, CMatrixArray<T> &imageVec, const SSize &ul, const SSize &size)
    {
        if (CheckIfPathExists(pathToImageFile) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
            return 1;
        }
        return getLoaderInstance()->LoadImage(pathToImageFile, imageVec, ul, size);
    }

    template<class T>
    int LoadImage(std::string pathToImageFile, CMatrix<T> &image, const SSize &ul, const SSize &size, int bandNumber = 1)
    {
        if (CheckIfPathExists(pathToImageFile) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
            return 1;
        }
        return getLoaderInstance()->LoadImage(pathToImageFile, image, ul, size, bandNumber);
    }

    template<class T>
    int LoadImage(std::string pathToImageFile, CMatrixArray<T> &imageVec)
    {
        if (CheckIfPathExists(pathToImageFile) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
            return 1;
        }
        return getLoaderInstance()->LoadImage(pathToImageFile, imageVec);
    }

    template<class T>
    int LoadImage(std::string pathToImageFile, CMatrix<T> &image, int bandNumber = 1)
    {
        if (CheckIfPathExists(pathToImageFile) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
            return 1;
        }
        return getLoaderInstance()->LoadImage(pathToImageFile, image, bandNumber);
    }

    template<class T>
    int LoadImageWithMetadata(std::string pathToImageFile, CMatrixArray<T> &imageVec, const SSize &ul, const SSize &size, SImageMetadata &metadata)
    {
        if (LoadImage<T>(pathToImageFile, imageVec, ul, size) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
            return 1;
        }

        if (LoadImageMetadataPartial(pathToImageFile, ul, size, metadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadataPartial()");
            return 1;
        }
        return 0;
    }

    template<class T>
    int LoadImageWithMetadata(std::string pathToImageFile, CMatrix<T> &image, const SSize &ul, const SSize &size, SImageMetadata &metadata, int bandNumber = 1)
    {
        if (LoadImage<T>(pathToImageFile, image, ul, size, bandNumber) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
            return 1;
        }

        if (LoadImageMetadataPartial(pathToImageFile, ul, size, metadata, bandNumber) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadataPartial()");
            return 1;
        }
        return 0;
    }

    template<class T>
    int LoadImageWithMetadata(std::string pathToImageFile, CMatrixArray<T> &imageVec, SImageMetadata &metadata)
    {
        if (LoadImage<T>(pathToImageFile, imageVec) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
            return 1;
        }

        if (LoadImageMetadata(pathToImageFile, metadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
            return 1;
        }
        return 0;
    }

    template<class T>
    int LoadImageWithMetadata(std::string pathToImageFile, CMatrix<T> &image, SImageMetadata &metadata, int bandNumber = 1)
    {
        if (LoadImage<T>(pathToImageFile, image, bandNumber) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
            return 1;
        }

        if (LoadImageMetadata(pathToImageFile, metadata, bandNumber) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
            return 1;
        }
        return 0;
    }

    // metadata loaders
    int LoadImageMetadataPartial(std::string pathToImageFile, const SSize &ul, const SSize &size, SImageMetadata &metadata, int bandNumber = 1);
    int LoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber = 1);
    int LoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber = 1);
    int LoadImageDimensions(std::string pathToImageFile, SSize &dims, int bandNumber = 1);
    int LoadImageGsd(std::string pathToImageFile, SPair<double> &gsd, int bandNumber = 1);
    int LoadImageType(std::string pathToImageFile, EImage::EImageFormat &imageType);
    int LoadImage(std::string pathToImageFile, CImage &image);
    int LoadProj4Str(std::string pathToImageFile, std::string &proj4Str);
    int LoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt);
    bool canLoadImage(std::string pathToImageFile);

    int GetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber = 1);
};

} //namespace ultra
