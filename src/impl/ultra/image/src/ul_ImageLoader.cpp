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

#include "ul_ImageLoader.h"

#include <stdio.h>

#include <ul_Logger.h>
#include <ul_Utility.h>
#include <ul_File.h>
#include "GDAL/ul_ImageLoaderGDAL.h"
#include <ul_DrawingTools.h>
#include <queue>

namespace ultra
{

CImageLoader::CImageLoader()
{
}

CImageLoader::~CImageLoader()
{
}

CImageLoader *CImageLoader::getInstance()
{
    static CImageLoader instance;
    return &instance;
}

__ultra_internal::IImageLoader* CImageLoader::getLoaderInstance()
{
    __ultra_internal::IImageLoader *defaultInstance = __ultra_internal::CImageLoaderGDAL::getInstance();
    return defaultInstance;
}

int CImageLoader::CheckIfPathExists(const std::string &pathToImageFile) const
{
    //some paths may contain subsets separated by ':'
    // example HDF5:/some/path/to/file.h5://name/of/subset
    CVector<std::string> splits = ultra::split(pathToImageFile, ':');
    splits.pushBack(pathToImageFile);
    for (auto str : splits)
    {
        if (CFile(str).exists())
            return 0;
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Nothing to load from '" + pathToImageFile + "' path does not exist");
    return 1;
}

////////////////////////////////////////////////////////
//////////   PUBLIC LOADERS  ///////////////////////////
////////////////////////////////////////////////////////

int CImageLoader::LoadImageDimensions(std::string pathToImageFile, SSize &dims, int bandNumber)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    SImageMetadata metadata;
    if (getLoaderInstance()->LoadImageMetadata(pathToImageFile, metadata, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }
    dims = metadata.getDimensions();
    return 0;
}

int CImageLoader::LoadImageGsd(std::string pathToImageFile, SPair<double> &gsd, int bandNumber)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    SImageMetadata metadata;
    if (getLoaderInstance()->LoadImageMetadata(pathToImageFile, metadata, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }
    gsd = metadata.getGsd();
    return 0;
}

int CImageLoader::LoadImageMetadataPartial(std::string pathToImageFile, const SSize &ul, const SSize &size, SImageMetadata &metadata, int bandNumber)
{
    if (size.containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Selection size contain zeroes");
        return 1;
    }

    if (LoadImageMetadata(pathToImageFile, metadata, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }

    SSize originalSize = metadata.getDimensions();

    SSize selectExtent = ul + size;

    if (selectExtent.col > originalSize.col ||
            selectExtent.row > originalSize.row)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Selection is out of bounds max size is '" + toString(originalSize) + "'");
        return 1;
    }

    SPair<double> origin = metadata.getMapCoord(ul);

    metadata.setDimensions(size);
    metadata.setOrigin(origin);

    return 0;
}

int CImageLoader::LoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->LoadImageMetadata(pathToImageFile, metadata, bandNumber);
}

int CImageLoader::LoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->LoadImageMetadata(pathToImageFile, metadata, bandNumber);
}

int CImageLoader::LoadImageType(std::string pathToImageFile, EImage::EImageFormat &imageType)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->GetImageType(pathToImageFile, imageType);
}

bool CImageLoader::canLoadImage(std::string pathToImageFile)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->canLoadImage(pathToImageFile);
}

int CImageLoader::GetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->GetNoDataValue(pathToImageFile, noDataValue, bandNumber);
}

int CImageLoader::LoadProj4Str(std::string pathToImageFile, std::string &proj4Str)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->LoadProj4Str(pathToImageFile, proj4Str);
}

int CImageLoader::LoadImage(std::string pathToImageFile, CImage &image)
{
    SImageMetadata meta;
    if (LoadImageMetadata(pathToImageFile, meta) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }
    image.resize(meta.getDimensions());
    image.initMat(SColor(0xff000000));
    for (unsigned long t = 0; t < getMIN<unsigned long>(4, meta.bandCount); t++)
    {
        CMatrix<unsigned char> temp;
        if (LoadImage(pathToImageFile, temp, t + 1) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
            return 1;
        }
        int ret = 0;
        if (t == 0)
            ret |= image.SetRed(temp);
        else if (t == 1)
            ret |= image.SetGreen(temp);
        else if (t == 2)
            ret |= image.SetBlue(temp);
        else
            ret |= image.SetAlpha(temp);
        if (ret != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to set image channel '" + toString(t + 1) + "' data");
            return 1;
        }
    }

    return 0;
}

int CImageLoader::LoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt)
{
    if (CheckIfPathExists(pathToImageFile) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckIfPathExists()");
        return 1;
    }
    return getLoaderInstance()->LoadGdalProjectionRefWkt(pathToImageFile, gdalProjectionRefWkt);
}

} //namespace ultra
