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

#include <ul_MatrixArray.h>
#include <ul_Image.h>
#include "ul_ImageMetadataObjects.h"
#include <ul_Int_ImageSaver.h>
#include <ul_File.h>

namespace ultra
{

class CImageSaver
{
private:
    __ultra_internal::IImageSaver* getSaverInstance();
    CImageSaver();
public:
    virtual ~CImageSaver();
    static CImageSaver *getInstance();

    template<class T>
    int SaveImage(std::string pathToImageFile, const CMatrix<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr)
    {
        CFile fPath = pathToImageFile;
        if (!fPath.getParentFolderFile().isDirectoryWritable())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot save file to '" + pathToImageFile + "' the path is not writable");
            return 1;
        }
        if (fPath.exists())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Going to overwrite a file at '" + pathToImageFile + "'");
        }
        return getSaverInstance()->SaveImage(pathToImageFile, image, imageType, metadata);
    }

    template<class T>
    int SaveImage(std::string pathToImageFile, const CMatrixArray<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr)
    {
        if (image.size() == 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector is of size 0");
            return 1;
        }
        CFile fPath = pathToImageFile;
        if (!fPath.getParentFolderFile().isDirectoryWritable())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot save file to '" + pathToImageFile + "' the path is not writable");
            return 1;
        }
        if (fPath.exists())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Going to overwrite a file at '" + pathToImageFile + "'");
        }
        return getSaverInstance()->SaveImage(pathToImageFile, image, imageType, metadata);
    }

    int SetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber = 1);
    int RemoveNoDataValue(std::string pathToImageFile, int bandNumber = 1);
    int SaveImage(std::string pathToImageFile, const CImage &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
};

} //namespace ultra
