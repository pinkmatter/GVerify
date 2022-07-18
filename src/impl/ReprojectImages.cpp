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

#include "ReprojectImages.h"

#include <ul_File.h>
#include <ul_ImageLoader.h>
#include <ul_ImageSaver.h>
#include <ul_Reproject.h>

int ReprojectImages(SArgs &args)
{
    ultra::SImageMetadata refMeta, inMeta, inBackMeta;

    if (ultra::CImageLoader::getInstance()->LoadImageMetadata(args.referenceImage.pathToImage, refMeta) != 0 ||
        ultra::CImageLoader::getInstance()->LoadImageMetadata(args.inputImage.pathToImage, inMeta) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }

    if (args.referenceImage.proj4Str == "")
    {
        if (ultra::CImageLoader::getInstance()->LoadProj4Str(args.referenceImage.pathToImage, args.referenceImage.proj4Str) != 0)
        {
            ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadProj4Str()");
            return 1;
        }
    }

    if (args.inputImage.proj4Str == "")
    {
        if (ultra::CImageLoader::getInstance()->LoadProj4Str(args.inputImage.pathToImage, args.inputImage.proj4Str) != 0)
        {
            ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadProj4Str()");
            return 1;
        }
    }

    ultra::CMatrixArray<float> inputImages, outputImages;
    std::string newInputPath = ultra::CFile(args.workingDirectory, "warpedInput_" + ultra::CFile(args.inputImage.pathToImage).getFileName()).getPath();

    if (ultra::CImageLoader::getInstance()->LoadImage(args.inputImage.pathToImage, inputImages) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImage()");
        return 1;
    }

    ultra::SPair<double> outputOrigin;
    if (ultra::CReproject::Project(inputImages,
                                   args.inputImage.proj4Str, args.referenceImage.proj4Str,
                                   inMeta.getAffineGeoTransform(),
                                   refMeta.getGsd(), outputOrigin,
                                   outputImages, args.resampleType,
                                   args.nullValue) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CReproject::Project()");
        return 1;
    }

    inMeta.setGsd(refMeta.getGsd());
    inMeta.setOrigin(outputOrigin);
    inMeta.datum = refMeta.datum;
    inMeta.projectionCode = refMeta.projectionCode;
    inMeta.utmZone = refMeta.utmZone;
    inMeta.spheroid = refMeta.spheroid;
    inMeta.gcsDatum = refMeta.gcsDatum;
    inMeta.geotifProjectionCode = refMeta.geotifProjectionCode;
    inMeta.setProj4String(args.referenceImage.proj4Str);

    if (ultra::CImageSaver::getInstance()->SaveImage(newInputPath, outputImages, ultra::EImage::IMAGE_TYPE_GEOTIFF, &inMeta) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageSaver::getInstance()->SaveImage()");
        return 1;
    }

    args.inputImage.pathToImage = newInputPath;
    return 0;
}
