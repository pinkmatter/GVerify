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

#include "ul_GdalWrapper.h"
#include "ul_GDALState.h"

#include <ul_Logger.h>
#include <ul_Utility.h>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_srs_api.h>
#include <ul_UltraThread.h>

namespace ultra
{
namespace __ultra_internal
{

CGdalWrapper::CGdalWrapper() :
m_defaultBlockSize(), m_defaultPapszOptions()
{
    GDALAllRegister();
    OGRRegisterAll();
    GDALDriver *driver = GetGDALDriverManager()->GetDriverByName("JP2ECW");
    if (driver != nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Removing JP2ECW driver from the GDALDriverManager");
        GetGDALDriverManager()->DeregisterDriver(driver);
        GDALDestroyDriver(driver);
    }
    m_defaultBlockSize.put(EImage::IMAGE_TYPE_BIG_GEOTIFF, SSize(256, 256));
    m_defaultBlockSize.put(EImage::IMAGE_TYPE_GEOTIFF, SSize(256, 256));

    for (const auto &kv : m_defaultBlockSize)
    {
        int size = 3;
        if (kv.k == EImage::IMAGE_TYPE_BIG_GEOTIFF)
            size++;
        char **ptr = new char*[size + 1];
        ptr[size] = nullptr;
        int maxL = 200;
        for (unsigned long t = 0; t < size; t++)
        {
            ptr[t] = new char[maxL + 1];
        }
        snprintf(ptr[0], maxL, "TILED=YES");
        snprintf(ptr[1], maxL, "BLOCKXSIZE=%d", kv.v.col);
        snprintf(ptr[2], maxL, "BLOCKYSIZE=%d", kv.v.row);

        if (kv.k == EImage::IMAGE_TYPE_BIG_GEOTIFF)
        {
            snprintf(ptr[3], maxL, "BIGTIFF=YES");
        }

        std::shared_ptr<char*> sptr(ptr, [size](char **p)
        {
            for (unsigned long t = 0; t < size; t++)
            {
                if (p[t] != nullptr)
                    delete []p[t];
            }
            delete []p;
        });
        m_defaultPapszOptions.put(kv.k, sptr);
    }
}

CGdalWrapper::~CGdalWrapper()
{
}

CGdalWrapper *CGdalWrapper::getInstance()
{
    static CGdalWrapper instance;
    return &instance;
}

int CGdalWrapper::SetBpp(void *raster, COdl &metadata, int bandNumber)
{
    int bpp = 0;
    switch (((GDALRasterBand*) raster)->GetRasterDataType())
    {
    case GDT_Unknown:bpp = 0;
        break;
    case GDT_Byte:bpp = 8;
        break; /*! Eight bit unsigned integer */
    case GDT_UInt16:bpp = 16;
        break; /*! Sixteen bit unsigned integer */
    case GDT_Int16:bpp = 16;
        break; /*! Sixteen bit signed integer */
    case GDT_UInt32:bpp = 32;
        break; /*! Thirty two bit unsigned integer */
    case GDT_Int32:bpp = 32;
        break; /*! Thirty two bit signed integer */
    case GDT_Float32:bpp = 32;
        break; /*! Thirty two bit floating point */
    case GDT_Float64:bpp = 64;
        break; /*! Sixty four bit floating point */
    case GDT_CInt16:bpp = 16;
        break; /*! Complex Int16 */
    case GDT_CInt32:bpp = 32;
        break; /*! Complex Int32 */
    case GDT_CFloat32:bpp = 32;
        break; /*! Complex Float32 */
    case GDT_CFloat64:bpp = 64;
        break; /*! Complex Float64 */
    }
    metadata.add(EImage::ACTUAL_BITS_PER_PIXEL, bpp);

    return 0;
}

int CGdalWrapper::SetExtraMetadata(void *dataset, COdl &metadata)
{
    char **charSet = ((GDALDataset*) dataset)->GetMetadata("RPC");
    if (CSLCount(charSet) > 0)
    {
        for (long i = 0; charSet[i] != nullptr; i++)
        {
            std::vector<std::string> vec = split(charSet[i], '=');
            if (vec.size() >= 2)
            {
                std::string key = vec[0];
                std::string val = vec[1];
                metadata.add("RPC_" + key, val);
            }
        }
        metadata.add(EImage::RPC_POLYNOMIAL, "1 x y z xy xz yz xx yy zz yxz xxx xyy xzz xxy yyy yzz xxz yyz zzz");
    }
    return 0;
}

int CGdalWrapper::AddTiePoints(COdl &metadata, void *dataset)
{
    int ret = 0;
    int count = ((GDALDataset*) dataset)->GetGCPCount();
    const GDAL_GCP *gcps = ((GDALDataset*) dataset)->GetGCPs();
    for (int t = 0; t < count; t++)
    {
        ret |= metadata.add(EImage::TIEPOINT_LINE + toString(t), gcps[t].dfGCPLine);
        ret |= metadata.add(EImage::TIEPOINT_SAMPLE + toString(t), gcps[t].dfGCPPixel);
        ret |= metadata.add(EImage::TIEPOINT_X + toString(t), gcps[t].dfGCPX);
        ret |= metadata.add(EImage::TIEPOINT_Y + toString(t), gcps[t].dfGCPY);
        ret |= metadata.add(EImage::TIEPOINT_Z + toString(t), gcps[t].dfGCPZ);
        ret |= metadata.add(EImage::TIEPOINT_ID + toString(t), gcps[t].pszId);
        ret |= metadata.add(EImage::TIEPOINT_INFO + toString(t), gcps[t].pszInfo);
    }

    return ret;
}

void CGdalWrapper::addMetadataDomain(void *dataset, COdl &metadata, char *domain)
{
    char **charSet = ((GDALDataset*) dataset)->GetMetadata(domain);
    if (CSLCount(charSet) > 0)
    {
        for (long i = 0; charSet[i] != nullptr; i++)
        {
            std::vector<std::string> vec = split(charSet[i], '=');
            if (vec.size() >= 2)
            {
                std::string key = vec[0];
                std::string val = vec[1];
                if (key == "AREA_OR_POINT")
                {
                    if (toUpper(val) == "POINT")
                        metadata.add(EImage::PIXEL_IS_AREA, 0);
                    else
                        metadata.add(EImage::PIXEL_IS_AREA, 1);
                }
                std::string keyStr = EImage::METADATA_TAGS;
                if (domain != nullptr && strlen(domain) != 0)
                {
                    keyStr += ".'" + toString(domain) + "'";
                }
                keyStr += ".'";
                keyStr += key + "'";
                metadata.add(keyStr, val);
            }
        }
    }
}

int CGdalWrapper::SetGeoLocation(void *dataset, COdl &metadata)
{
    int cols = ((GDALDataset*) dataset)->GetRasterXSize();
    int rows = ((GDALDataset*) dataset)->GetRasterYSize();
    metadata.add(EImage::NUM_COLS, cols);
    metadata.add(EImage::NUM_ROWS, rows);
    char** domainList = ((GDALDataset*) dataset)->GetMetadataDomainList();
    if (CSLCount(domainList) > 0)
    {
        for (long i = 0; domainList[i] != nullptr; i++)
        {
            addMetadataDomain(dataset, metadata, domainList[i]);
        }
    }
    CSLDestroy(domainList);
    addMetadataDomain(dataset, metadata, nullptr);

    double adfGeoTransform[6];
    if (((GDALDataset*) dataset)->GetGeoTransform(adfGeoTransform) == CE_None)
    {
        metadata.add(EImage::GEO_TRANSFORM_0, adfGeoTransform[0]);
        metadata.add(EImage::GEO_TRANSFORM_1, adfGeoTransform[1]);
        metadata.add(EImage::GEO_TRANSFORM_2, adfGeoTransform[2]);
        metadata.add(EImage::GEO_TRANSFORM_3, adfGeoTransform[3]);
        metadata.add(EImage::GEO_TRANSFORM_4, adfGeoTransform[4]);
        metadata.add(EImage::GEO_TRANSFORM_5, adfGeoTransform[5]);
    }
    else if (((GDALDataset*) dataset)->GetGCPCount() > 0)
    {
        int count = ((GDALDataset*) dataset)->GetGCPCount();
        if (count > 0)
        {
            const GDAL_GCP *gcps = ((GDALDataset*) dataset)->GetGCPs();
            if (gcps != nullptr)
            {
                for (int t = 0; t < count; t++)
                {
                    std::string id = gcps[t].pszId;
                    if (id == "UpperLeft")
                    {
                        metadata.add(EImage::UL_X, gcps[t].dfGCPX);
                        metadata.add(EImage::UL_Y, gcps[t].dfGCPY);
                    }
                    else if (id == "UpperRight")
                    {
                        metadata.add(EImage::UR_X, gcps[t].dfGCPX);
                        metadata.add(EImage::UR_Y, gcps[t].dfGCPY);
                    }
                    else if (id == "LowerRight")
                    {
                        metadata.add(EImage::LR_X, gcps[t].dfGCPX);
                        metadata.add(EImage::LR_Y, gcps[t].dfGCPY);
                    }
                    else if (id == "LowerLeft")
                    {
                        metadata.add(EImage::LL_X, gcps[t].dfGCPX);
                        metadata.add(EImage::LL_Y, gcps[t].dfGCPY);
                    }
                }
            }
        }
    }
    else
    {
        // no real metadata assume the following
        metadata.add(EImage::GEO_TRANSFORM_0, 0);
        metadata.add(EImage::GEO_TRANSFORM_1, 1);
        metadata.add(EImage::GEO_TRANSFORM_2, 0);
        metadata.add(EImage::GEO_TRANSFORM_3, 0);
        metadata.add(EImage::GEO_TRANSFORM_4, 0);
        metadata.add(EImage::GEO_TRANSFORM_5, -1);
    }

    if (AddTiePoints(metadata, dataset) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from AddTiePoints()");
        return 1;
    }

    return 0;
}

SImageMetadata::EProjectionParmIndex CGdalWrapper::setParmIndex(std::string key)
{
    SImageMetadata::EProjectionParmIndex ret = SImageMetadata::EPROJECTION_PARM_INDEX_COUNT;

    if (key == SRS_PP_CENTRAL_MERIDIAN)ret = SImageMetadata::LONGITUDE_OF_THE_CENTRAL_MERIDIAN;
    else if (key == SRS_PP_STANDARD_PARALLEL_1)ret = SImageMetadata::LATITUDE_OF_THE_FIRST_STANDARD_PARALLEL;
    else if (key == SRS_PP_STANDARD_PARALLEL_2)ret = SImageMetadata::LATITUDE_OF_THE_SECOND_STANDARD_PARALLEL;
    else if (key == SRS_PP_LONGITUDE_OF_CENTER)ret = SImageMetadata::LONGITUDE_OF_THE_CENTRAL_MERIDIAN;
    else if (key == SRS_PP_FALSE_EASTING)ret = SImageMetadata::FALSE_EASTING;
    else if (key == SRS_PP_FALSE_NORTHING)ret = SImageMetadata::FALSE_NORTHING;
    else if (key == SRS_PP_LATITUDE_OF_1ST_POINT)ret = SImageMetadata::LATITUDE_OF_FIRST_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LONGITUDE_OF_1ST_POINT)ret = SImageMetadata::LONGITUDE_OF_FIRST_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LATITUDE_OF_2ND_POINT)ret = SImageMetadata::LATITUDE_OF_SECOND_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LONGITUDE_OF_2ND_POINT)ret = SImageMetadata::LONGITUDE_OF_SECOND_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LONGITUDE_OF_POINT_1)ret = SImageMetadata::LONGITUDE_OF_FIRST_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LATITUDE_OF_POINT_1)ret = SImageMetadata::LATITUDE_OF_FIRST_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LONGITUDE_OF_POINT_2)ret = SImageMetadata::LONGITUDE_OF_SECOND_POINT_ON_CENTER_LINE;
    else if (key == SRS_PP_LATITUDE_OF_POINT_2)ret = SImageMetadata::LATITUDE_OF_SECOND_POINT_ON_CENTER_LINE;

    return ret;
}

void CGdalWrapper::SetProjectionParmsItem(void *hSRSptr, COdl &metadata, const char *key)
{
    OGRSpatialReference *hSRS = (OGRSpatialReference*) hSRSptr;
    OGRErr err = 0;

    double val = hSRS->GetProjParm(key, 0.0, &err);

    if (err == 0)
    {
        SImageMetadata::EProjectionParmIndex index = setParmIndex(key);
        if (index != SImageMetadata::EPROJECTION_PARM_INDEX_COUNT)
        {
            metadata.add("PARM_" + toString(static_cast<int> (index)), val);
            metadata.add("GOT_PARM_" + toString(static_cast<int> (index)), 1);
        }
    }
}

void CGdalWrapper::SetProjection(COdl &metadata, std::string projection, int zone)
{
    if (metadata.isItemPresentExactMatch(EImage::ACTUAL_PROJECTION))
        return;
    if (projection == "UTM" ||
        projection == SRS_PT_TRANSVERSE_MERCATOR ||
        projection == SRS_PT_TRANSVERSE_MERCATOR_MI_21 ||
        projection == SRS_PT_TRANSVERSE_MERCATOR_MI_22 ||
        projection == SRS_PT_TRANSVERSE_MERCATOR_MI_23 ||
        projection == SRS_PT_TRANSVERSE_MERCATOR_MI_24 ||
        projection == SRS_PT_TRANSVERSE_MERCATOR_MI_25)
    {
        if (zone != 0)
            metadata.add(EImage::ACTUAL_PROJECTION, static_cast<int> (EProjectionEnum::PROJECTION_UTM));
        else
            metadata.add(EImage::ACTUAL_PROJECTION, static_cast<int> (EProjectionEnum::PROJECTION_TM));
    }
    else if (projection == SRS_PT_EQUIRECTANGULAR)metadata.add(EImage::ACTUAL_PROJECTION, static_cast<int> (EProjectionEnum::PROJECTION_EQRECT));
}

void CGdalWrapper::SetSpheroid(COdl &metadata, std::string spheroid)
{
    if (metadata.isItemPresentExactMatch(EImage::SPHEROID))
        return;

    EProjectionEnum::EProjectionSpheroid sp = EProjectionEnum::PROJECTION_SPHEROID_COUNT;
    if (spheroid == SRS_DN_WGS84)
        sp = EProjectionEnum::PROJECTION_SPHEROID_WGS_84;
    else if (spheroid == "Geocentric_Datum_of_Australia_1994")
        sp = EProjectionEnum::PROJECTION_SPHEROID_GRS_1980;

    if (sp == EProjectionEnum::PROJECTION_SPHEROID_COUNT)
        sp = EProjectionEnum::strToProjectionSpheroidCode(spheroid);

    if (sp != EProjectionEnum::PROJECTION_SPHEROID_COUNT)
        metadata.add(EImage::SPHEROID, static_cast<int> (sp));
}

void CGdalWrapper::SetDatum(COdl &metadata, std::string datum)
{
    if (metadata.isItemPresentExactMatch(EImage::DATUM))
        return;
    if (datum == SRS_DN_WGS84)metadata.add(EImage::DATUM, static_cast<int> (EProjectionEnum::PROJECTION_DATUM_WGS84));
    else if (datum == "Geocentric_Datum_of_Australia_1994")metadata.add(EImage::DATUM, static_cast<int> (EProjectionEnum::PROJECTION_DATUM_GDA94));
}

int CGdalWrapper::UpdateMetadataUsingProj4Str(const std::string &proj4Str, COdl &metadata)
{
    EImage::DATUM;
    EImage::SPHEROID;
    EImage::ACTUAL_PROJECTION;
    std::vector<std::string> splits = split(proj4Str, ' ');
    bool isSouth = false;
    int zone = 0;
    std::string proj = "";
    for (unsigned long t = 0; t < splits.size(); t++)
    {
        std::string item = trimStr(splits[t]);
        if (toUpper(item) == toUpper("+south"))
        {
            isSouth = true;
        }
        else
        {
            std::vector<std::string> subSplits = split(item, '=');
            if (subSplits.size() == 2)
            {
                std::string left = toUpper(trimStr(subSplits[0]));
                std::string right = toUpper(trimStr(subSplits[1]));

                if (left == toUpper("+proj"))
                {
                    proj = right;
                }
                else if (left == toUpper("+zone"))
                {
                    zone = atoi(right.c_str());
                }
                else if (left == toUpper("+datum"))
                {
                    SetDatum(metadata, right);
                }
                else if (left == toUpper("+ellps"))
                {
                    SetSpheroid(metadata, right);
                }
            }
        }
    }

    if (isSouth)
        zone *= -1;

    if (proj != "")
    {
        SetProjection(metadata, proj, zone);
    }

    return 0;
}

int CGdalWrapper::SetProjectionParms(void *hSRSptr, COdl &metadata)
{
    OGRSpatialReference *hSRS = (OGRSpatialReference*) hSRSptr;
    const char *units = nullptr;
    for (int x = 0; x<static_cast<int> (SImageMetadata::EPROJECTION_PARM_INDEX_COUNT); x++)
    {
        metadata.add("PARM_" + toString(static_cast<int> (x)), 0);
        metadata.add("GOT_PARM_" + toString(static_cast<int> (x)), 0);
    }

    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_CENTRAL_MERIDIAN);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_SCALE_FACTOR);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_STANDARD_PARALLEL_1);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_STANDARD_PARALLEL_2);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_PSEUDO_STD_PARALLEL_1);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_CENTER);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_CENTER);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_ORIGIN);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_ORIGIN);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_FALSE_EASTING);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_FALSE_NORTHING);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_AZIMUTH);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_POINT_1);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_POINT_1);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_POINT_2);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_POINT_2);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_POINT_3);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_POINT_3);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_RECTIFIED_GRID_ANGLE);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LANDSAT_NUMBER);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_PATH_NUMBER);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_PERSPECTIVE_POINT_HEIGHT);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_SATELLITE_HEIGHT);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_FIPSZONE);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_1ST_POINT);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_1ST_POINT);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LATITUDE_OF_2ND_POINT);
    SetProjectionParmsItem(hSRSptr, metadata, SRS_PP_LONGITUDE_OF_2ND_POINT);

    OGRErr err;
    double val;

    int pbNorth;
    int zone = hSRS->GetUTMZone(&pbNorth);
    if (zone != 0)
    {
        if (pbNorth == 1)
            metadata.add(EImage::UTM_ZONE, zone);
        else
            metadata.add(EImage::UTM_ZONE, -1 * zone);
    }


    const char *geotifEPSGCode = hSRS->GetAuthorityCode("PROJCS");
    if (geotifEPSGCode != nullptr)
    {
        metadata.add(EImage::PROJ_CODE, geotifEPSGCode);
    }

    val = hSRS->GetSemiMajor(&err);
    if (err == 0)
    {
        metadata.add("PARM_" + toString(static_cast<int> (SImageMetadata::SEMI_MAJOR_AXIS)), val);
        metadata.add("GOT_PARM_" + toString(static_cast<int> (SImageMetadata::SEMI_MAJOR_AXIS)), 1);
    }
    val = hSRS->GetSemiMinor(&err);
    if (err == 0)
    {
        metadata.add("PARM_" + toString(static_cast<int> (SImageMetadata::SEMI_MINOR_AXIS)), val);
        metadata.add("GOT_PARM_" + toString(static_cast<int> (SImageMetadata::SEMI_MINOR_AXIS)), 1);
    }


    if (hSRS->IsGeographic())
    {
        hSRS->GetAngularUnits(&units);
        metadata.add(EImage::ACTUAL_PROJECTION, static_cast<int> (EProjectionEnum::PROJECTION_GEO));
    }
    else
    {
        hSRS->GetLinearUnits(&units);
        const char *projection = hSRS->GetAttrValue("PROJECTION", 0);
        if (projection != nullptr)
        {
            SetProjection(metadata, projection, zone);
        }
    }

    if (units != nullptr)
    {
        metadata.add(EImage::PROJECTION_UNITS, units);
    }

    const char *datum = hSRS->GetAttrValue("DATUM", 0);
    if (datum != nullptr)
    {
        SetDatum(metadata, datum);
    }

    const char *ellps = hSRS->GetAttrValue("SPHEROID", 0);
    if (ellps != nullptr)
    {
        SetSpheroid(metadata, ellps);
    }

    return 0;
}

int CGdalWrapper::SetProjectionParmsToGeo(COdl &metadata)
{
    metadata.add(EImage::ACTUAL_PROJECTION, static_cast<int> (EProjectionEnum::PROJECTION_GEO));
    return 0;
}

int CGdalWrapper::innerLoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber)
{
    GDALDataset *dataset;
    dataset = (GDALDataset *) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly);
    if (dataset == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to open image");
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
    metadata.add(EImage::BAND_COUNT, bandCount);

    if (SetBpp(raster, metadata, bandNumber) != 0)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetBpp()");
        return 1;
    }

    if (SetGeoLocation(dataset, metadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetGeoLocation()");
        GDALClose(dataset);
        return 1;
    }

    if (SetExtraMetadata(dataset, metadata) != 0)
    {
        GDALClose(dataset);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetExtraMetadata()");
        return 1;
    }

    if (dataset->GetProjectionRef() != nullptr)
    {
        std::shared_ptr<OGRSpatialReference> hSRSPtr(new OGRSpatialReference(), [](OGRSpatialReference * sr)->void
        {
            sr->Release();
        });
        OGRSpatialReference *hSRS = hSRSPtr.get();
        if (hSRS == nullptr)
        {
            GDALClose(dataset);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create OGRSpatialReference object");
            return 1;
        }

        const char *projRef = (char *) dataset->GetProjectionRef();

        if (hSRS->importFromWkt(&projRef) == CE_None)
        {
            char *str = nullptr;
            if (hSRS->exportToProj4(&str) == CE_None)
            {
                std::string proj4Str = str;

                free(str);
                str = nullptr;
                if (UpdateMetadataUsingProj4Str(proj4Str, metadata) != 0)
                {
                    GDALClose(dataset);
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from UpdateMetadataUsingProj4Str()");
                    return 1;
                }
            }
            if (SetProjectionParms(hSRS, metadata) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetProjectionParms()");
                GDALClose(dataset);
                return 1;
            }
        }
        else
        {
            if (SetProjectionParmsToGeo(metadata) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetProjectionParmsToGeo()");
                GDALClose(dataset);
                return 1;
            }
        }
    }

    GDALClose(dataset);
    return 0;
}

int CGdalWrapper::innerLoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CGdalWrapper::LoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber)
{
    int ret = 0;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return 1;
    }
    try
    {
        ret = innerLoadImageMetadata(pathToImageFile, metadata, bandNumber);
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

int CGdalWrapper::LoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos)
{
    int ret = 0;
    if (CGdalState::getInstance()->WaitForReader() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGdalState::WaitForReader()");
        return 1;
    }
    try
    {
        ret = innerLoadImageMetadata(fm, metadata, bandNumber, pos);
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


const SSize *CGdalWrapper::getDefaultBlockSize(EImage::EImageFormat imageType) const
{
    if (m_defaultBlockSize.contains(imageType))
        return &m_defaultBlockSize.get(imageType);
    return nullptr;
}

char** CGdalWrapper::getGdalDefaultPapszOptions(EImage::EImageFormat imageType) const
{
    if (m_defaultPapszOptions.contains(imageType))
        return m_defaultPapszOptions.get(imageType).get();
    return nullptr;
}

} // namespace __ultra_internal
} // namespace ultra
