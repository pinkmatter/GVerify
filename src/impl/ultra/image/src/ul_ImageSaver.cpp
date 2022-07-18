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

#include "ul_ImageSaver.h"

#include "ul_Logger.h"
#include "ul_Utility.h"

#include "GDAL/ul_ImageSaverGDAL.h"

namespace ultra
{

CImageSaver::CImageSaver()
{
}

CImageSaver::~CImageSaver()
{
}

CImageSaver *CImageSaver::getInstance()
{
    static CImageSaver instance;
    return &instance;
}

__ultra_internal::IImageSaver* CImageSaver::getSaverInstance()
{
    __ultra_internal::IImageSaver *defaultInstance = __ultra_internal::CImageSaverGDAL::getInstance();
    return defaultInstance;
}

int CImageSaver::SaveImage(std::string pathToImageFile, const CImage &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    CMatrixArray<unsigned char> imgVec;
    imgVec.resize(3);
    imgVec[0] = image.getRed();
    imgVec[1] = image.getGreen();
    imgVec[2] = image.getBlue();

    if (SaveImage(pathToImageFile, imgVec, imageType, metadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SaveImage()");
        return 1;
    }
    return 0;
}

int CImageSaver::SetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber)
{
    CFile fPath = pathToImageFile;
    if (!fPath.exists())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Path to image '" + pathToImageFile + "' does not exist");
        return 1;
    }
    return getSaverInstance()->SetNoDataValue(pathToImageFile, noDataValue, bandNumber);
}

int CImageSaver::RemoveNoDataValue(std::string pathToImageFile, int bandNumber)
{
    CFile fPath = pathToImageFile;
    if (!fPath.exists())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Path to image '" + pathToImageFile + "' does not exist");
        return 1;
    }
    return getSaverInstance()->RemoveNoDataValue(pathToImageFile, bandNumber);
}

} //namespace ultra
