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

#include "ul_ImageSaverGDAL.h"
#include "ul_GdalWrapper.h"
#include "ul_GDALState.h"

#include <ul_File.h>

#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_srs_api.h>
#include "ul_ImageTools.h"

#include "../ImageLoaderLocks/ul_ImageLoadSaveLocks.h"

namespace ultra
{
namespace __ultra_internal
{

namespace
{

CMap<std::string, GDALDataType> createDataTypeMap()
{
    CMap<std::string, GDALDataType> typeNames;
    typeNames.put(typeid (unsigned char).name(), GDT_Byte);
    typeNames.put(typeid (char).name(), GDT_Byte);
    typeNames.put(typeid (unsigned short).name(), GDT_UInt16);
    typeNames.put(typeid (short).name(), GDT_Int16);
    typeNames.put(typeid (unsigned int).name(), GDT_UInt32);
    typeNames.put(typeid (int).name(), GDT_Int32);
    typeNames.put(typeid (float).name(), GDT_Float32);
    typeNames.put(typeid (double).name(), GDT_Float64);
    typeNames.put(typeid (SComplex<short>).name(), GDT_CInt16);
    typeNames.put(typeid (SComplex<int>).name(), GDT_CInt32);
    typeNames.put(typeid (SComplex<float>).name(), GDT_CFloat32);
    typeNames.put(typeid (SComplex<double>).name(), GDT_CFloat64);
    return typeNames;
}

template<class T>
GDALDataType translateToGdalDataType()
{
    static const CMap<std::string, GDALDataType> typeNames = createDataTypeMap();
    const GDALDataType* dataTypePtr = typeNames.getPtr(typeid (T).name());
    if (dataTypePtr != nullptr)
    {
        return *dataTypePtr;
    }

    bool isSigned = std::numeric_limits<T>::is_signed;
    bool isInt = std::numeric_limits<T>::is_integer;
    int sizeofDataType = sizeof (T);
    GDALDataType dataType = GDT_TypeCount;
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

} // namespace

// this key MUST be the same for all the GDAL loader and saver classes
#ifndef GDAL_LOCK_KEY
#define GDAL_LOCK_KEY 1
#endif

const EImage::EImageFormat CImageSaverGDAL::DEFAULT_IMAGE_TYPE = EImage::IMAGE_TYPE_GEOTIFF;

CImageSaverGDAL::CImageSaverGDAL(bool isThreadSafe) :
IImageSaver(isThreadSafe, GDAL_LOCK_KEY)
{
    //need to call CGdalWrapper so that GDAL can register all drivers
    CGdalWrapper::getInstance()->getInstance();
}

CImageSaverGDAL::~CImageSaverGDAL()
{
}

IImageSaver *CImageSaverGDAL::getInstance()
{
#ifdef GDAL_THREADS
    static CImageSaverGDAL instance(true);
#else
    static CImageSaverGDAL instance(false);
#endif
    return &instance;
}

void CImageSaverGDAL::genGeoTransform(const SPair<double> &origin, const SPair<double> &gsd, double output[6], const SImageMetadata *metadata)
{
    output[0] = origin.x;
    output[1] = gsd.x;
    output[2] = 0;

    output[3] = origin.y;
    output[4] = 0;
    output[5] = gsd.y;

    if (metadata != nullptr)
    {
        CVector<double> vec = metadata->getAffineGeoTransform();
        if (vec.size() == 6)
        {
            for (int x = 0; x < 6; x++)
                output[x] = vec[x];
        }
    }
}

int CImageSaverGDAL::PopulateOSrs(const SImageMetadata *metadata, void *osrs, void *dataset)
{
    if (metadata == nullptr)
    {
        return 0;
    }

    GDALDataset *gdalDataSet = (GDALDataset*) (dataset);
    OGRSpatialReference *oSRS = (OGRSpatialReference*) (osrs);
    if (metadata->hasGdalProjectionRefWkt())
    {
        std::string line = metadata->getGdalProjectionRefWkt();
        const char *line_c = (const char *) line.c_str();
        if (oSRS->importFromWkt(&line_c) != OGRERR_NONE)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from importFromWkt()");
            return 1;
        }
    }
    else
    {
        std::string proj4Str;
        if (metadata->isProj4StringSet())
        {
            proj4Str = metadata->getProj4String();
        }
        else if (metadata->GenProj4String(proj4Str) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenProj4String()");
            return 1;
        }
        if (oSRS->importFromProj4(proj4Str.c_str()) != OGRERR_NONE)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from importFromProj4()");
            return 1;
        }
        if (metadata->geotifProjectionCode != "")
        {
            if (oSRS->SetAuthority("PROJCS", "EPSG", atoi(metadata->geotifProjectionCode.c_str())) != OGRERR_NONE)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetAuthority()");
                return 1;
            }
        }
    }

    if (metadata->getPixelIsAreaIsSet())
    {
        char *metaTags[2] = {0};
        metaTags[0] = new char[50];
        if (metadata->getPixelIsArea())
        {
            sprintf(metaTags[0], "AREA_OR_POINT=AREA");
        }
        else
        {
            sprintf(metaTags[0], "AREA_OR_POINT=POINT");
        }
        if (gdalDataSet->SetMetadata(metaTags, nullptr) != CE_None)
        {
            delete []metaTags[0];
            metaTags[0] = nullptr;
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetMetadata()");
            return 1;
        }
        delete []metaTags[0];
        metaTags[0] = nullptr;
    }

    return 0;
}

template<class T>
int CImageSaverGDAL::DetermineType(int &gdalImageType)
{
    gdalImageType = translateToGdalDataType<T>();
    if (gdalImageType == GDT_Unknown ||
        gdalImageType == GDT_TypeCount)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unsupported datatype");
        return 1;
    }
    return 0;
}

template<class T>
int CImageSaverGDAL::inner_saveImageVecLineForLine(void *PoDriver, const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    GDALDriver *poDriver = (GDALDriver*) (PoDriver);
    SSize imgSize = imgVec[0]->getSize();
    GDALDataset *poDstDS = nullptr;
    char **papszOptions = nullptr;
    int noBands = imgVec.size();
    int gdalImageType;
    if (DetermineType<T>(gdalImageType) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from DetermineType()");
        return 1;
    }
    poDstDS = poDriver->Create(pathToImageFile.c_str(), imgSize.col, imgSize.row, noBands, (GDALDataType) gdalImageType,
                               papszOptions);

    if (poDstDS == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create dataset");
        return 1;
    }

    double adfGeoTransform[6];
    SPair<double> origin = 0;
    SPair<double> gsd = SPair<double>(-1, 1);
    if (metadata != nullptr)
    {
        origin = metadata->getOrigin();
        gsd = metadata->getGsd();
    }
    genGeoTransform(origin, gsd, adfGeoTransform, metadata);

    OGRSpatialReference oSRS;
    char *pszSRS_WKT = nullptr;
    GDALRasterBand *poBand;

    poDstDS->SetGeoTransform(adfGeoTransform);

    if (metadata != nullptr)
    {
        if (PopulateOSrs(metadata, &oSRS, poDstDS) != 0)
        {
            GDALClose(poDstDS);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PopulateOSrs()");
            return 1;
        }
    }
    oSRS.exportToWkt(&pszSRS_WKT);
    poDstDS->SetProjection(pszSRS_WKT);
    CPLFree(pszSRS_WKT);


    for (unsigned long t = 1; t <= noBands; t++)
    {
        poBand = poDstDS->GetRasterBand(t);
        if (poBand == nullptr)
        {
            GDALClose(poDstDS);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid raster returned for band '" + toString(t) + "'");
            return 1;
        }

        for (unsigned long r = 0; r < imgSize.row; r++)
        {
            if (poBand->RasterIO(GF_Write, 0, r, imgSize.col, 1, &(*imgVec[t - 1])[r][0], imgSize.col, 1, (GDALDataType) gdalImageType, 0, 0) != 0)
            {
                GDALClose(poDstDS);
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from RasterIO()");
                return 1;
            }
        }
    }

    if (poDstDS != nullptr)
        GDALClose(poDstDS);
    return 0;
}

template<class T>
int CImageSaverGDAL::inner_saveImageVecFromMemory(void *PoDriver, const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    GDALDriver *poDriver = (GDALDriver*) (PoDriver);
    SSize imgSize = imgVec[0]->getSize();
    char **papszOptions = nullptr;
    int noBands = imgVec.size();
    int gdalImageType;
    if (DetermineType<T>(gdalImageType) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from DetermineType()");
        return 1;
    }

    GDALDataset *memDS = (GDALDataset*) GDALCreate(GDALGetDriverByName("MEM"), "tmp", imgSize.col, imgSize.row, noBands, (GDALDataType) gdalImageType, papszOptions);

    if (memDS == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create memory dataset");
        return 1;
    }
    double adfGeoTransform[6];
    SPair<double> origin = 0;
    SPair<double> gsd = SPair<double>(-1, 1);
    if (metadata != nullptr)
    {
        origin = metadata->getOrigin();
        gsd = metadata->getGsd();
    }
    genGeoTransform(origin, gsd, adfGeoTransform, metadata);

    OGRSpatialReference oSRS;
    char *pszSRS_WKT = nullptr;
    GDALRasterBand *poBand;

    memDS->SetGeoTransform(adfGeoTransform);

    if (metadata != nullptr)
    {
        if (PopulateOSrs(metadata, &oSRS, memDS) != 0)
        {
            GDALClose(memDS);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PopulateOSrs()");
            return 1;
        }
    }
    oSRS.exportToWkt(&pszSRS_WKT);
    memDS->SetProjection(pszSRS_WKT);
    CPLFree(pszSRS_WKT);

    for (unsigned long t = 1; t <= noBands; t++)
    {
        poBand = memDS->GetRasterBand(t);
        if (poBand == nullptr)
        {
            GDALClose(memDS);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid raster returned for band '" + toString(t) + "'");
            return 1;
        }

        for (unsigned long r = 0; r < imgSize.row; r++)
        {
            if (poBand->RasterIO(GF_Write, 0, r, imgSize.col, 1, &(*imgVec[t - 1])[r][0], imgSize.col, 1, (GDALDataType) gdalImageType, 0, 0) != 0)
            {
                GDALClose(memDS);
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from RasterIO()");
                return 1;
            }
        }
    }

    GDALDataset* dstDS = (GDALDataset*) GDALCreateCopy(poDriver, pathToImageFile.c_str(), memDS, FALSE, papszOptions, nullptr, nullptr);
    if (dstDS == nullptr)
    {
        GDALClose(memDS);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create final dataset");
        return 1;
    }

    GDALClose(memDS);
    GDALClose(dstDS);
    return 0;
}

template<class T>
int CImageSaverGDAL::inner_saveImageVec(const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    if (!EImage::canWriteImage(imageType))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Cannot write '" + EImage::imageFormatToStr(imageType) + "' defaulting to " + EImage::imageFormatToStr(DEFAULT_IMAGE_TYPE));
        imageType = DEFAULT_IMAGE_TYPE;
    }
    int ret = 0;
    if (CGdalState::getInstance()->WaitForWriter() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForWriter()");
        return 1;
    }
    ret = inner_wrappedSaveImageVec(pathToImageFile, imgVec, imageType, metadata);
    if (CGdalState::getInstance()->RemoveWriter() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::RemoveWriter()");
        return 1;
    }
    return ret;
}

template<class T>
int CImageSaverGDAL::inner_wrappedSaveImageVec(const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata * metadata)
{
    std::string format = EImage::imageFormatToGdalStr(imageType);
    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
    if (format == "" || poDriver == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unsupported format '" + EImage::imageFormatToStr(imageType) + "'");
        return 1;
    }

    int noBands = imgVec.size();
    SSize imgSize = imgVec[0]->getSize();
    for (long t = 1; t < noBands; t++)
    {
        if (imgSize != imgVec[t]->getSize())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Image vector needs to contain matrices with the same size");
            return 1;
        }
    }

    if (GDALGetMetadataItem(poDriver, GDAL_DCAP_CREATE, nullptr) != nullptr)
    {
        if (inner_saveImageVecLineForLine<T>((void*) poDriver, pathToImageFile, imgVec, imageType, metadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_saveImageVecLineForLine()");
            return 1;
        }
    }
    else
    {
        if (inner_saveImageVecFromMemory<T>((void*) poDriver, pathToImageFile, imgVec, imageType, metadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner_saveImageVecFromMemory()");
            return 1;
        }
    }

    CFile delFile = CFile(pathToImageFile + ".aux.xml");
    if (delFile.isFile())
    {
        delFile.remove();
    }

    return 0;
}

template<class T>
int CImageSaverGDAL::inner_saveImage(const std::string &pathToImageFile, const CMatrix<T> &img, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    CVector<CMatrix<T>*> imgVec;
    imgVec.resize(1);
    imgVec[0] = const_cast<CMatrix<T> *> (&img);
    return inner_saveImageVec<T>(pathToImageFile, imgVec, imageType, metadata);
}

template<class T>
int CImageSaverGDAL::inner_saveImage(const std::string &pathToImageFile, const CMatrixArray<T> &img, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    CVector<CMatrix<T>*> imgVec;
    imgVec.resize(img.size());
    for (long t = 0; t < imgVec.size(); t++)
    {
        imgVec[t] = const_cast<CMatrix<T> *> (&img[t]);
    }
    return inner_saveImageVec<T>(pathToImageFile, imgVec, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return inner_saveImage(pathToImageFile, image, imageType, metadata);
}

int CImageSaverGDAL::innerSetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber)
{
    return CImageTools::getInstance()->SetNoDataValue(pathToImageFile, noDataValue, bandNumber);
}

int CImageSaverGDAL::innerRemoveNoDataValue(std::string pathToImageFile, int bandNumber)
{
    return CImageTools::getInstance()->RemoveNoDataValue(pathToImageFile, bandNumber);
}

} // namespace __ultra_internal
} // namespace ultra
