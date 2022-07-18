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

#include "ul_ImageLoaderGDAL.h"

#include "ul_Logger.h"
#include "ul_GdalWrapper.h"
#include "ul_GDALState.h"
#include <gdal_priv.h>
#include <ogr_spatialref.h>

#include "ul_ImageTools.h"
#include "../ImageLoaderLocks/ul_ImageLoadSaveLocks.h"

namespace ultra
{
namespace __ultra_internal
{

// this key MUST be the same for all the GDAL loader and saver classes
#ifndef GDAL_LOCK_KEY
#define GDAL_LOCK_KEY 1
#endif

CImageLoaderGDAL::CImageLoaderGDAL(bool isThreadSafe) :
IImageLoader(isThreadSafe, GDAL_LOCK_KEY)
{
    //need to call CGdalWrapper so that GDAL can register all drivers
    CGdalWrapper::getInstance()->getInstance();
}

CImageLoaderGDAL::~CImageLoaderGDAL()
{
}

IImageLoader *CImageLoaderGDAL::getInstance()
{
#ifdef GDAL_THREADS
    static CImageLoaderGDAL instance(true);
#else
    static CImageLoaderGDAL instance(false);
#endif
    return &instance;
}

int CImageLoaderGDAL::TranslateImageType(std::string imageDesc, EImage::EImageFormat &imageType)
{
    if (imageDesc == "GTiff")
    {
        imageType = EImage::IMAGE_TYPE_GEOTIFF;
        return 0;
    }
    else if (imageDesc == "PNG")
    {
        imageType = EImage::IMAGE_TYPE_PNG;
        return 0;
    }
    else if (imageDesc == "JPEG")
    {
        imageType = EImage::IMAGE_TYPE_JPEG;
        return 0;
    }
    else if (imageDesc == "FAST")
    {
        imageType = EImage::IMAGE_TYPE_FAST;
        return 0;
    }
    else if (imageDesc == "NITF")
    {
        imageType = EImage::IMAGE_TYPE_NITF;
        return 0;
    }
    else if (imageDesc == "PCIDSK")
    {
        imageType = EImage::IMAGE_TYPE_PCIDSK;
        return 0;
    }
    else if (imageDesc == "VRT")
    {
        imageType = EImage::IMAGE_TYPE_VRT;
        return 0;
    }
    else if (imageDesc == "BMP")
    {
        imageType = EImage::IMAGE_TYPE_BMP;
        return 0;
    }
    else if (imageDesc == "DIMAP")
    {
        imageType = EImage::IMAGE_TYPE_DIMAP;
        return 0;
    }
    else if (imageDesc == "HDF5Image")
    {
        imageType = EImage::IMAGE_TYPE_HDF;
        return 0;
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown format '" + imageDesc + "'");
    return 1;
}

int CImageLoaderGDAL::innerWrappedLoadProj4Str(std::string pathToImageFile, std::string &proj4Str)
{
    proj4Str = "";
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly);
    if (dataset == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to open image");
        return 1;
    }

    const char *projRef = (char *) dataset->GetProjectionRef();
    if (projRef == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to load projection reference");
        GDALClose(dataset);
        return 1;
    }
    proj4Str = trimStr(projRef);

    /* 
     * http://www.gdal.org/classGDALDataset.html#aa42537e1062ce254d124b29ff3ebe857
     * 
     * Fetch the projection definition string for this dataset.
     * Same as the C function GDALGetProjectionRef().
     * The returned string defines the projection coordinate system of the image in OpenGIS WKT format. It should be suitable for use with the OGRSpatialReference class.
     * When a projection definition is not available an empty (but not NULL) string is returned.
     */
    if (proj4Str == "")
        return 0;

    std::shared_ptr<OGRSpatialReference> hSRS(new OGRSpatialReference(), [](OGRSpatialReference * sr)->void
    {
        sr->Release();
    });
    if (hSRS.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create OGRSpatialReference object");
        GDALClose(dataset);
        return 1;
    }


    if (hSRS->importFromWkt(&projRef) == CE_None)
    {
        char *str = nullptr;
        if (hSRS->exportToProj4(&str) == CE_None)
        {
            proj4Str = str;
            free(str);
            str = nullptr;
        }
        else
        {
            if (str != nullptr)
            {
                free(str);
                str = nullptr;
            }
            GDALClose(dataset);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to export projection to PROJ4");
            return 1;
        }
    }
    else
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to import projection string");
        return 1;
    }
    GDALClose(dataset);
    return 0;
}

int CImageLoaderGDAL::innerWrappedLoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt)
{
    gdalProjectionRefWkt = "";
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly);
    if (dataset == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to open image");
        return 1;
    }

    char *projRef = (char *) dataset->GetProjectionRef();
    OGRSpatialReference *hSRS = new OGRSpatialReference();
    if (hSRS == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create OGRSpatialReference object");
        GDALClose(dataset);
        return 1;
    }

    gdalProjectionRefWkt = projRef;

    delete hSRS;
    hSRS = nullptr;
    GDALClose(dataset);

    return 0;
}

bool CImageLoaderGDAL::innerWrappedCanLoadImage(std::string pathToImageFile)
{
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly);
    if (dataset == nullptr)
        return false;
    GDALClose(dataset);
    return true;
}

int CImageLoaderGDAL::innerWrappedGetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType)
{
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly);
    if (dataset == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to open image");
        return 1;
    }

    GDALDriver *driver = dataset->GetDriver();
    if (driver == nullptr)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetDriver()");
        return 1;
    }

    if (TranslateImageType(driver->GetDescription(), imageType) != 0)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from TranslateImageType()");
        return 1;
    }

    GDALClose(dataset);

    return 0;
}

int CImageLoaderGDAL::innerLoadProj4Str(std::string pathToImageFile, std::string &proj4Str)
{
    int ret = 0;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return 1;
    }
    try
    {
        ret = innerWrappedLoadProj4Str(pathToImageFile, proj4Str);
    }
    catch (const std::exception &e)
    {
        ret = 1;
    }
    if (CGdalState::getInstance()->RemoveReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::RemoveReader()");
        return 1;
    }
    return ret;
}

int CImageLoaderGDAL::innerLoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt)
{
    int ret = 0;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return 1;
    }
    try
    {
        ret = innerWrappedLoadGdalProjectionRefWkt(pathToImageFile, gdalProjectionRefWkt);
    }
    catch (const std::exception &e)
    {
        ret = 1;
    }
    if (CGdalState::getInstance()->RemoveReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::RemoveReader()");
        return 1;
    }
    return ret;
}

bool CImageLoaderGDAL::innerCanLoadImage(std::string pathToImageFile)
{
    bool ret = false;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return false;
    }
    try
    {
        ret = innerWrappedCanLoadImage(pathToImageFile);
    }
    catch (const std::exception &e)
    {
        ret = false;
    }
    if (CGdalState::getInstance()->RemoveReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::RemoveReader()");
        return false;
    }
    return ret;
}

int CImageLoaderGDAL::innerGetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType)
{
    int ret = 0;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return 1;
    }
    try
    {
        ret = innerWrappedGetImageType(pathToImageFile, imageType);
    }
    catch (const std::exception &e)
    {
        ret = 1;
    }
    if (CGdalState::getInstance()->RemoveReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::RemoveReader()");
        return 1;
    }
    return ret;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<unsigned char>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<char> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<char>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<unsigned short>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<short> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<short>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<unsigned int>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<int> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<int>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<float> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<float>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage(std::string pathToImageFile, CMatrix<double> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    if (inner_loadPartialImage<double>(pathToImageFile, bandNumber, image, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_loadPartialImage()");
        return 1;
    }
    return 0;
}

int CImageLoaderGDAL::innerLoadImage_file(FILE *fm, CMatrix<unsigned char> &image, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CImageLoaderGDAL::innerLoadImage_file(FILE *fm, CMatrix<char> &image, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CImageLoaderGDAL::innerLoadImage_file(FILE *fm, CMatrix<unsigned short> &image, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CImageLoaderGDAL::innerLoadImage_file(FILE *fm, CMatrix<short> &image, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CImageLoaderGDAL::innerLoadImage_file(FILE *fm, CMatrix<unsigned int> &image, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CImageLoaderGDAL::innerLoadImage_file(FILE *fm, CMatrix<int> &image, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CImageLoaderGDAL::innerLoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber)
{
    if (CGdalWrapper::getInstance()->LoadImageMetadata(pathToImageFile, metadata, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }

    return 0;
}

int CImageLoaderGDAL::innerLoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber)
{
    COdl metadataOdl;
    if (innerLoadImageMetadata(pathToImageFile, metadataOdl, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImageMetadata()");
        return 1;
    }

    if (metadata.Init(metadataOdl) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SImageMetadata::Init()");
        return 1;
    }

    return 0;
}

int CImageLoaderGDAL::innerLoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos)
{
    if (CGdalWrapper::getInstance()->LoadImageMetadata(fm, metadata, bandNumber, pos) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }

    return 0;
}

int CImageLoaderGDAL::innerLoadImageDimention(std::string pathToImageFile, SSize &size, int bandNumber)
{
    SImageMetadata metadata;
    if (innerLoadImageMetadata(pathToImageFile, metadata, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImageMetadata()");
        return 1;
    }
    size = metadata.getDimensions();
    return 0;
}

int CImageLoaderGDAL::innerLoadImageMetadata(FILE *fm, SImageMetadata &metadata, int bandNumber, const fpos64_t &pos)
{
    COdl metadataOdl;
    if (innerLoadImageMetadata(fm, metadataOdl, bandNumber, pos) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImageMetadata()");
        return 1;
    }

    if (metadata.Init(metadataOdl) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SImageMetadata::Init()");
        return 1;
    }
    return 0;
}

template<class T>
int CImageLoaderGDAL::inner_loadPartialImage(const std::string &pathToInputFile, int bandNumber, CMatrix<T> &img, const SSize &ul, const SSize &size)
{
    img.resize(size);
    if (GetImageRasterLines<T>(pathToInputFile, img, bandNumber, ul, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetImageRasterLines()");
        return 1;
    }

    return 0;
}

template<class T>
int CImageLoaderGDAL::GetImageRasterLines(std::string pathToInputFile, CMatrix<T> &img, int bandNumber,
                                          const SSize &ul, const SSize& size)
{
    int ret = 0;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return 1;
    }
    try
    {
        ret = WrappedGetImageRasterLines<T>(pathToInputFile, img, bandNumber, ul, size);
    }
    catch (const std::exception &e)
    {
        ret = 1;
    }
    if (CGdalState::getInstance()->RemoveReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::RemoveReader()");
        return 1;
    }
    return ret;
}

namespace
{

GDALDataType determineDataType(bool isSigned, bool isInt, int sizeofDataType)
{
    GDALDataType dataType = GDT_Unknown;
    switch (sizeofDataType * 8)
    {
    case 8:
        if (isInt)
        {
            if (isSigned)
                dataType = GDT_Byte;
            else
                dataType = GDT_Byte;
        }
        else
        {

        }
        break;
    case 16:
        if (isInt)
        {
            if (isSigned)
                dataType = GDT_Int16;
            else
                dataType = GDT_UInt16;
        }
        else
        {

        }
        break;
    case 32:
        if (isInt)
        {
            if (isSigned)
                dataType = GDT_Int32;
            else
                dataType = GDT_UInt32;
        }
        else
        {
            dataType = GDT_Float32;
        }
        break;
    case 64:
        if (isInt)
        {
        }
        else
        {
            dataType = GDT_Float64;
        }
        break;
    }
    return dataType;
}

template<class T>
int ReadLineForLine(GDALRasterBand *raster, long sizeCopy, CMatrix<T> &img, GDALDataType dataType, const SSize &ul, const SSize& size)
{
    T* line = (T *) CPLMalloc(sizeCopy);
    if (line == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to allocate memory for scan line");
        return 1;
    }
    for (unsigned long r = 0; r < size.row; r++)
    {
        if (raster->RasterIO(GF_Read, ul.col, ul.row + r, size.col, 1, line, size.col, 1, dataType, 0, 0) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from RasterIO");
            CPLFree(line);
            return 1;
        }
        if (memcpy(&img[r][0], line, sizeCopy) != &img[r][0])
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to copy scan line data");
            CPLFree(line);
            return 1;
        }
    }
    CPLFree(line);
    return 0;
}

} // namespace

template<class T>
int CImageLoaderGDAL::WrappedGetImageRasterLines(std::string pathToInputFile, CMatrix<T> &img, int bandNumber,
                                                 const SSize &ul, const SSize& size)
{
    bool isSigned = std::numeric_limits<T>::is_signed;
    bool isInt = std::numeric_limits<T>::is_integer;
    int sizeofDataType = sizeof (T);
    long sizeCopy = sizeofDataType * size.col;
    GDALDataType dataType = determineDataType(isSigned, isInt, sizeofDataType);

    if (dataType == GDT_Unknown)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unsupported BPP size of '" + toString(sizeofDataType * 8) + "'");
        return 1;
    }

    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpen(pathToInputFile.c_str(), GA_ReadOnly);
    if (dataset == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to open image");
        return 1;
    }

    EImage::EImageFormat imageType;
    if (TranslateImageType(dataset->GetDriver()->GetDescription(), imageType) != 0)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from TranslateImageType()");
        return 1;
    }

    if (!EImage::canReadImage(imageType))
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot read image '" + EImage::imageFormatToStr(imageType) + "'");
        return 1;
    }

    int bandCount = dataset->GetRasterCount();
    if (bandNumber <= 0 || bandCount < 1 || bandNumber > bandCount)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot load band '" + toString(bandNumber) + "' only '" + toString(bandCount) + "' to load from");
        return 1;
    }
    GDALRasterBand *raster = dataset->GetRasterBand(bandNumber);

    if (ReadLineForLine(raster, sizeCopy, img, dataType, ul, size) != 0)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ReadLineForLine()");
        return 1;
    }

    GDALClose(dataset);

    return 0;
}

int CImageLoaderGDAL::innerGetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber)
{
    return CImageTools::getInstance()->GetNoDataValue(pathToImageFile, noDataValue, bandNumber);
}

} // namespace __ultra_internal
} // namespace ultra