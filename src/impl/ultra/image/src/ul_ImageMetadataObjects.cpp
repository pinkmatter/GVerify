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

#include "ul_ImageMetadataObjects.h"

#include "ul_Logger.h"
#include <ul_File.h>

namespace ultra
{

SImageMetadata::STiePoint::STiePoint()
{

}

SImageMetadata::STiePoint::STiePoint(const S3D<double> &gcp, const S2D<double> &imagePoint, const std::string &info, const std::string &id)
{
    m_gcp = gcp;
    m_imagePoint = imagePoint;
    m_info = info.c_str();
    m_id = id.c_str();
}

SImageMetadata::STiePoint::STiePoint(const SImageMetadata::STiePoint &r)
{
    m_gcp = r.m_gcp;
    m_imagePoint = r.m_imagePoint;
    m_info = r.m_info.c_str();
    m_id = r.m_id.c_str();
}

SImageMetadata::STiePoint::~STiePoint()
{

}

SImageMetadata::STiePoint &SImageMetadata::STiePoint::operator=(const SImageMetadata::STiePoint &r)
{
    m_gcp = r.m_gcp;
    m_imagePoint = r.m_imagePoint;
    m_info = r.m_info.c_str();
    m_id = r.m_id.c_str();
    return *this;
}

S3D<double> SImageMetadata::STiePoint::getGcp() const
{
    return m_gcp;
}

std::string SImageMetadata::STiePoint::getId() const
{
    return m_id.c_str();
}

S2D<double> SImageMetadata::STiePoint::getImagePoint() const
{
    return m_imagePoint;
}

std::string SImageMetadata::STiePoint::getInfo() const
{
    return m_info.c_str();
}

SImageDate::SImageDate()
{
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_hour = 0;
    m_minute = 0;
    m_second = 0;
}

void SImageDate::set(int year, int month, int day, int hour, int minute, double second)
{
    m_year = year;
    m_month = month;
    m_day = day;
    m_hour = hour;
    m_minute = minute;
    m_second = second;
}

SImageDate::~SImageDate()
{

}

SImageDate::SImageDate(const SImageDate &r)
{
    m_year = r.m_year;
    m_month = r.m_month;
    m_day = r.m_day;
    m_hour = r.m_hour;
    m_minute = r.m_minute;
    m_second = r.m_second;
}

SImageDate &SImageDate::operator=(const SImageDate & r)
{
    m_year = r.m_year;
    m_month = r.m_month;
    m_day = r.m_day;
    m_hour = r.m_hour;
    m_minute = r.m_minute;
    m_second = r.m_second;

    return *this;
}

SImageMetadata::SStringRpc::SRpcCoeff::SRpcCoeff()
{
    m_pixelOffSet = m_pixelScale = 0;
    m_degreeOffSet = m_degreeScale = 0;
    m_degreeMin = m_degreeMax = 0;
    m_den = m_num = "";
}

SImageMetadata::SStringRpc::SRpcCoeff::SRpcCoeff(const std::string &den, const std::string &num,
                                                 double pixelOffSet, double pixelScale,
                                                 double degreeOffSet, double degreeScale,
                                                 double degreeMin, double degreeMax)
{
    m_pixelOffSet = pixelOffSet;
    m_pixelScale = pixelScale;
    m_degreeOffSet = degreeOffSet;
    m_degreeScale = degreeScale;
    m_degreeMin = degreeMin;
    m_degreeMax = degreeMax;
    m_den = den;
    m_num = num;
}

SImageMetadata::SStringRpc::SRpcCoeff::SRpcCoeff(const SImageMetadata::SStringRpc::SRpcCoeff &r)
{
    m_pixelOffSet = r.m_pixelOffSet;
    m_pixelScale = r.m_pixelScale;
    m_degreeOffSet = r.m_degreeOffSet;
    m_degreeScale = r.m_degreeScale;
    m_degreeMin = r.m_degreeMin;
    m_degreeMax = r.m_degreeMax;
    m_den = r.m_den;
    m_num = r.m_num;
}

SImageMetadata::SStringRpc::SRpcCoeff::~SRpcCoeff()
{
    m_den = m_num = "";
}

SImageMetadata::SStringRpc::SRpcCoeff &SImageMetadata::SStringRpc::SRpcCoeff::operator=(const SImageMetadata::SStringRpc::SRpcCoeff &r)
{
    m_pixelOffSet = r.m_pixelOffSet;
    m_pixelScale = r.m_pixelScale;
    m_degreeOffSet = r.m_degreeOffSet;
    m_degreeScale = r.m_degreeScale;
    m_degreeMin = r.m_degreeMin;
    m_degreeMax = r.m_degreeMax;
    m_den = r.m_den;
    m_num = r.m_num;
    return *this;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setDegreeMax(double degreeMax)
{
    m_degreeMax = degreeMax;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setDegreeMin(double degreeMin)
{
    m_degreeMin = degreeMin;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setDegreeOffSet(double degreeOffSet)
{
    m_degreeOffSet = degreeOffSet;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setDegreeScale(double degreeScale)
{
    m_degreeScale = degreeScale;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setPixelOffSet(double pixelOffSet)
{
    m_pixelOffSet = pixelOffSet;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setPixelScale(double pixelScale)
{
    m_pixelScale = pixelScale;
}

double SImageMetadata::SStringRpc::SRpcCoeff::getDegreeMax() const
{
    return m_degreeMax;
}

double SImageMetadata::SStringRpc::SRpcCoeff::getDegreeMin() const
{
    return m_degreeMin;
}

double SImageMetadata::SStringRpc::SRpcCoeff::getDegreeOffSet() const
{
    return m_degreeOffSet;
}

double SImageMetadata::SStringRpc::SRpcCoeff::getDegreeScale() const
{
    return m_degreeScale;
}

double SImageMetadata::SStringRpc::SRpcCoeff::getPixelOffSet() const
{
    return m_pixelOffSet;
}

double SImageMetadata::SStringRpc::SRpcCoeff::getPixelScale() const
{
    return m_pixelScale;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setDen(std::string den)
{
    this->m_den = den;
}

void SImageMetadata::SStringRpc::SRpcCoeff::setNum(std::string num)
{
    this->m_num = num;
}

std::string SImageMetadata::SStringRpc::SRpcCoeff::getDen() const
{
    return m_den;
}

std::string SImageMetadata::SStringRpc::SRpcCoeff::getNum() const
{
    return m_num;
}

SImageMetadata::SStringRpc::SRpcHeight::SRpcHeight()
{
    m_heightOffset = m_heightScale = 0;
}

SImageMetadata::SStringRpc::SRpcHeight::SRpcHeight(double heightOffset, double heightScale)
{
    m_heightOffset = heightOffset;
    m_heightScale = heightScale;
}

SImageMetadata::SStringRpc::SRpcHeight::SRpcHeight(const SImageMetadata::SStringRpc::SRpcHeight &r)
{
    m_heightOffset = r.m_heightOffset;
    m_heightScale = r.m_heightScale;
}

SImageMetadata::SStringRpc::SRpcHeight::~SRpcHeight()
{

}

SImageMetadata::SStringRpc::SRpcHeight &SImageMetadata::SStringRpc::SRpcHeight::operator=(const SImageMetadata::SStringRpc::SRpcHeight &r)
{
    m_heightOffset = r.m_heightOffset;
    m_heightScale = r.m_heightScale;
    return *this;
}

void SImageMetadata::SStringRpc::SRpcHeight::setHeightOffset(double heightOffset)
{
    m_heightOffset = heightOffset;
}

void SImageMetadata::SStringRpc::SRpcHeight::setHeightScale(double heightScale)
{
    m_heightScale = heightScale;
}

double SImageMetadata::SStringRpc::SRpcHeight::getHeightOffset() const
{
    return m_heightOffset;
}

double SImageMetadata::SStringRpc::SRpcHeight::getHeightScale() const
{
    return m_heightScale;
}

SImageMetadata::SStringRpc::SStringRpc()
{
    m_polynomial = "";
}

SImageMetadata::SStringRpc::SStringRpc(const SImageMetadata::SStringRpc &r)
{
    m_polynomial = r.m_polynomial;
    m_samples = r.m_samples;
    m_lines = r.m_lines;
    m_height = r.m_height;
}

SImageMetadata::SStringRpc::~SStringRpc()
{

}

SImageMetadata::SStringRpc &SImageMetadata::SStringRpc::operator=(const SImageMetadata::SStringRpc &r)
{
    m_height = r.m_height;
    m_polynomial = r.m_polynomial;
    m_samples = r.m_samples;
    m_lines = r.m_lines;
    return *this;
}

void SImageMetadata::SStringRpc::setPolynomial(std::string poly)
{
    m_polynomial = poly;
}

void SImageMetadata::SStringRpc::setLines(SImageMetadata::SStringRpc::SRpcCoeff lines)
{
    this->m_lines = lines;
}

void SImageMetadata::SStringRpc::setSamples(SImageMetadata::SStringRpc::SRpcCoeff samples)
{
    this->m_samples = samples;
}

void SImageMetadata::SStringRpc::setHeight(SImageMetadata::SStringRpc::SRpcHeight height)
{
    m_height = height;
}

std::string SImageMetadata::SStringRpc::getPolynomial() const
{
    return m_polynomial;
}

SImageMetadata::SStringRpc::SRpcCoeff SImageMetadata::SStringRpc::getLines() const
{
    return m_lines;
}

SImageMetadata::SStringRpc::SRpcCoeff SImageMetadata::SStringRpc::getSamples() const
{
    return m_samples;
}

SImageMetadata::SStringRpc::SRpcHeight SImageMetadata::SStringRpc::getHeight() const
{
    return m_height;
}

SImageMetadata::SImageMetadata()
{
    m_proj4StrSet = false;
    m_proj4Str = "";
    // Xgeo = GT(0) + Xpixel*GT(1) + Yline*GT(2)
    // Ygeo = GT(3) + Xpixel*GT(4) + Yline*GT(5)
    m_affineGeoTransform.resize(6);

    m_affineGeoTransform[0] = 0; //origin X
    m_affineGeoTransform[1] = 1; // GSD_X in X
    m_affineGeoTransform[2] = 0; // GSD_X in Y

    m_affineGeoTransform[3] = 0; //origin Y
    m_affineGeoTransform[4] = 0; // GSD_Y in X
    m_affineGeoTransform[5] = -1; // GSD_Y in Y

    m_hasGdalProjectionRefWkt = false;
    m_gdalProjectionRefWktStr = "";

    bpp = 0;
    bandCount = 0;

    m_tiePoints.clear();
    m_pixelIsAreaIsSet = false;
    m_pixelIsArea = true;
    projectionCode = static_cast<EProjectionEnum::EProjectionType> (0);

    for (int x = 0; x < static_cast<int> (EPROJECTION_PARM_INDEX_COUNT); x++)
    {
        m_projParms[x].projParm = 0;
        m_projParms[x].gotProjParm = false;
    }

    utmZone = 0;

    spheroid = EProjectionEnum::PROJECTION_SPHEROID_COUNT;
    datum = EProjectionEnum::PROJECTION_DATUM_COUNT;
    gcsDatum = EProjectionEnum::PROJECTION_GCS_DATUM_COUNT;
    projectionUnits = EProjectionEnum::PROJECTION_UNIT_COUNT;

    m_metadataTags.clean();
}

SImageMetadata::SImageMetadata(const SImageMetadata & r)
{
    m_proj4StrSet = r.m_proj4StrSet;
    m_proj4Str = r.m_proj4Str;
    m_affineGeoTransform = r.m_affineGeoTransform;
    m_rpc = r.m_rpc;
    bpp = r.bpp;
    bandCount = r.bandCount;
    originId = r.originId;
    originatorName = r.originatorName;
    imageId = r.imageId;
    title = r.title;
    imageComment = r.imageComment;

    m_dimension = r.m_dimension;

    m_hasGdalProjectionRefWkt = r.m_hasGdalProjectionRefWkt;
    m_gdalProjectionRefWktStr = r.m_gdalProjectionRefWktStr;

    fileDate = r.fileDate;
    imageDate = r.imageDate;

    geotifProjectionCode = r.geotifProjectionCode;
    m_tiePoints = r.m_tiePoints;
    m_pixelIsAreaIsSet = r.m_pixelIsAreaIsSet;
    m_pixelIsArea = r.m_pixelIsArea;
    projectionCode = r.projectionCode;
    projectionUnits = r.projectionUnits;

    for (int x = 0; x < static_cast<int> (EPROJECTION_PARM_INDEX_COUNT); x++)
    {
        m_projParms[x].projParm = r.m_projParms[x].projParm;
        m_projParms[x].gotProjParm = r.m_projParms[x].gotProjParm;
    }

    utmZone = r.utmZone;
    spheroid = r.spheroid;
    datum = r.datum;
    gcsDatum = r.gcsDatum;

    m_metadataTags = r.m_metadataTags;
}

SImageMetadata::~SImageMetadata()
{

}

SImageMetadata &SImageMetadata::operator=(const SImageMetadata & r)
{
    if (this == &r)
        return *this;
    m_proj4StrSet = r.m_proj4StrSet;
    m_proj4Str = r.m_proj4Str;
    m_affineGeoTransform = r.m_affineGeoTransform;
    m_rpc = r.m_rpc;
    bpp = r.bpp;
    bandCount = r.bandCount;
    originId = r.originId;
    originatorName = r.originatorName;
    imageId = r.imageId;
    title = r.title;
    imageComment = r.imageComment;

    m_hasGdalProjectionRefWkt = r.m_hasGdalProjectionRefWkt;
    m_gdalProjectionRefWktStr = r.m_gdalProjectionRefWktStr;

    m_dimension = r.m_dimension;

    fileDate = r.fileDate;
    imageDate = r.imageDate;

    geotifProjectionCode = r.geotifProjectionCode;
    projectionUnits = r.projectionUnits;
    m_tiePoints = r.m_tiePoints;
    m_pixelIsAreaIsSet = r.m_pixelIsAreaIsSet;
    m_pixelIsArea = r.m_pixelIsArea;
    projectionCode = r.projectionCode;
    for (int x = 0; x < static_cast<int> (EPROJECTION_PARM_INDEX_COUNT); x++)
    {
        m_projParms[x].projParm = r.m_projParms[x].projParm;
        m_projParms[x].gotProjParm = r.m_projParms[x].gotProjParm;
    }
    utmZone = r.utmZone;
    spheroid = r.spheroid;
    datum = r.datum;
    gcsDatum = r.gcsDatum;

    m_metadataTags = r.m_metadataTags;

    return *this;
}

void SImageMetadata::setDimensions(unsigned long height, unsigned long width)
{
    setDimensions(SSize(height, width));
}

void SImageMetadata::setDimensions(const SSize &size)
{
    m_dimension = size;
}

void SImageMetadata::setGsd(const SPair<double> &gsd)
{
    // Xgeo = GT(0) + Xpixel*GT(1) + Yline*GT(2)
    // Ygeo = GT(3) + Xpixel*GT(4) + Yline*GT(5)
    m_affineGeoTransform[1] = gsd.x; // GSD_X in X
    m_affineGeoTransform[5] = gsd.y; // GSD_Y in Y
}

void SImageMetadata::setGsd(double gsd_x, double gsd_y)
{
    setGsd(SPair<double>(gsd_y, gsd_x));
}

void SImageMetadata::setShear(const SPair<double> &shear)
{
    m_affineGeoTransform[2] = shear.x;
    m_affineGeoTransform[4] = shear.y;
}

void SImageMetadata::setShear(double shear_x, double shear_y)
{
    setShear(SPair<double>(shear_y, shear_x));
}

SSize SImageMetadata::getDimensions() const
{
    return m_dimension;
}

CVector<double> SImageMetadata::createAffineTransform(const SPair<double> &gsd, const SPair<double> &origin, const SPair<double> &shear)
{
    CVector<double> trans(6);

    trans[0] = origin.c;
    trans[1] = gsd.c;
    trans[2] = shear.c;

    trans[3] = origin.r;
    trans[4] = shear.r;
    trans[5] = gsd.r;

    return trans;
}

SPair<double> SImageMetadata::getGsd() const
{
    // Xgeo = GT(0) + Xpixel*GT(1) + Yline*GT(2)
    // Ygeo = GT(3) + Xpixel*GT(4) + Yline*GT(5)
    return SPair<double>(m_affineGeoTransform[5], m_affineGeoTransform[1]);
}

SPair<double> SImageMetadata::getOrigin() const
{
    // Xgeo = GT(0) + Xpixel*GT(1) + Yline*GT(2)
    // Ygeo = GT(3) + Xpixel*GT(4) + Yline*GT(5)
    return SPair<double>(m_affineGeoTransform[3], m_affineGeoTransform[0]);
}

void SImageMetadata::setOrigin(const SPair<double> &origin)
{
    // Xgeo = GT(0) + Xpixel*GT(1) + Yline*GT(2)
    // Ygeo = GT(3) + Xpixel*GT(4) + Yline*GT(5)
    m_affineGeoTransform[0] = origin.x; //origin X
    m_affineGeoTransform[3] = origin.y; //origin Y
}

SPair<double> SImageMetadata::getUL() const
{
    return getMapCoord(SPair<double>(0, 0));
}

SPair<double> SImageMetadata::getUR() const
{
    return getMapCoord(SPair<double>(0, m_dimension.col - 1));
}

SPair<double> SImageMetadata::getLL() const
{
    return getMapCoord(SPair<double>(m_dimension.row - 1, 0));
}

SPair<double> SImageMetadata::getLR() const
{
    return getMapCoord(SPair<double>(m_dimension.row - 1, m_dimension.col - 1));
}

void SImageMetadata::setOrigin(double origin_x, double origin_y)
{
    setOrigin(SPair<double>(origin_y, origin_x));
}

int SImageMetadata::Init(const COdl &metadata)
{
    cleanGdalProjectionRefWkt();
    SSize size;

    size.row = metadata.getItemIntExactMatch(EImage::NUM_ROWS);
    size.col = metadata.getItemIntExactMatch(EImage::NUM_COLS);
    setDimensions(size);

    if (metadata.isItemPresentExactMatch(EImage::PIXEL_IS_AREA))
    {
        setPixelIsArea(metadata.getItemBoolExactMatch(EImage::PIXEL_IS_AREA));
    }

    bool tiepointDone = false;
    int loopTiePoints = 0;
    while (!tiepointDone)
    {

        if (metadata.isItemPresentExactMatch(EImage::TIEPOINT_LINE + toString(loopTiePoints)) &&
            metadata.isItemPresentExactMatch(EImage::TIEPOINT_SAMPLE + toString(loopTiePoints)) &&
            metadata.isItemPresentExactMatch(EImage::TIEPOINT_X + toString(loopTiePoints)) &&
            metadata.isItemPresentExactMatch(EImage::TIEPOINT_Y + toString(loopTiePoints)) &&
            metadata.isItemPresentExactMatch(EImage::TIEPOINT_Z + toString(loopTiePoints)) &&
            metadata.isItemPresentExactMatch(EImage::TIEPOINT_ID + toString(loopTiePoints)) &&
            metadata.isItemPresentExactMatch(EImage::TIEPOINT_INFO + toString(loopTiePoints)))
        {
            S2D<double> imagePoint;
            S3D<double> gcp;
            imagePoint.y = metadata.getItemDoubleExactMatch(EImage::TIEPOINT_LINE + toString(loopTiePoints));
            imagePoint.x = metadata.getItemDoubleExactMatch(EImage::TIEPOINT_SAMPLE + toString(loopTiePoints));
            gcp.x = metadata.getItemDoubleExactMatch(EImage::TIEPOINT_X + toString(loopTiePoints));
            gcp.y = metadata.getItemDoubleExactMatch(EImage::TIEPOINT_Y + toString(loopTiePoints));
            gcp.z = metadata.getItemDoubleExactMatch(EImage::TIEPOINT_Z + toString(loopTiePoints));
            std::string id = metadata.getItemStrExactMatch(EImage::TIEPOINT_ID + toString(loopTiePoints));
            std::string info = metadata.getItemStrExactMatch(EImage::TIEPOINT_INFO + toString(loopTiePoints));

            m_tiePoints.pushBack(SImageMetadata::STiePoint(gcp, imagePoint, info, id));
        }
        else
        {
            tiepointDone = true;
        }
        loopTiePoints++;
    }

    bool canAddGeo = true;
    CVector<double> geoVec;
    for (int t = 0; t < 6; t++)
    {
        if (metadata.isItemPresentExactMatch("GEO_TRANSFORM_" + toString(t)))
            geoVec.pushBack(metadata.getItemDoubleExactMatch("GEO_TRANSFORM_" + toString(t)));
        else
        {
            canAddGeo = false;
            break;
        }
    }
    if (canAddGeo)
    {
        if (SetAffineGeoTransform(geoVec) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SetAffineGeoTransform()");
            return 1;
        }
    }
    else
    {
        if (metadata.isItemPresentExactMatch(EImage::UL_X) &&
            metadata.isItemPresentExactMatch(EImage::UL_Y))
        {
            SPair<double> origin;
            origin.x = metadata.getItemDoubleExactMatch(EImage::UL_X);
            origin.y = metadata.getItemDoubleExactMatch(EImage::UL_Y);
            setOrigin(origin);
        }
    }

    SImageMetadata::SStringRpc rpc;

    rpc.setPolynomial(metadata.getItemStrExactMatch(EImage::RPC_POLYNOMIAL));
    rpc.setLines(SImageMetadata::SStringRpc::SRpcCoeff(
                                                       metadata.getItemStrExactMatch(EImage::RPC_LINE_DEN_COEFF),
                                                       metadata.getItemStrExactMatch(EImage::RPC_LINE_NUM_COEFF),
                                                       metadata.getItemDoubleExactMatch(EImage::RPC_LINE_OFF),
                                                       metadata.getItemDoubleExactMatch(EImage::RPC_LINE_SCALE),
                                                       metadata.getItemDoubleExactMatch(EImage::RPC_LAT_OFF),
                                                       metadata.getItemDoubleExactMatch(EImage::RPC_LAT_SCALE),
                                                       metadata.getItemDoubleExactMatch(EImage::RPC_MIN_LAT),
                                                       metadata.getItemDoubleExactMatch(EImage::RPC_MAX_LAT)
                                                       ));
    rpc.setSamples(SImageMetadata::SStringRpc::SRpcCoeff(
                                                         metadata.getItemStrExactMatch(EImage::RPC_SAMP_DEN_COEFF),
                                                         metadata.getItemStrExactMatch(EImage::RPC_SAMP_NUM_COEFF),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_SAMP_OFF),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_SAMP_SCALE),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_LONG_OFF),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_LONG_SCALE),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_MIN_LONG),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_MAX_LONG)
                                                         ));
    rpc.setHeight(SImageMetadata::SStringRpc::SRpcHeight(
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_HEIGHT_OFF),
                                                         metadata.getItemDoubleExactMatch(EImage::RPC_HEIGHT_SCALE)
                                                         ));
    setRpc(rpc);
    geotifProjectionCode = metadata.getItemStrExactMatch(EImage::PROJ_CODE);
    if (metadata.isItemPresentExactMatch(EImage::ACTUAL_PROJECTION))
    {
        projectionCode = static_cast<EProjectionEnum::EProjectionType> (metadata.getItemIntExactMatch(EImage::ACTUAL_PROJECTION));
    }

    utmZone = metadata.getItemIntExactMatch(EImage::UTM_ZONE);
    bpp = metadata.getItemIntExactMatch(EImage::ACTUAL_BITS_PER_PIXEL);
    bandCount = metadata.getItemIntExactMatch(EImage::BAND_COUNT);

    if (metadata.isItemPresentExactMatch(EImage::SPHEROID))
    {
        spheroid = static_cast<EProjectionEnum::EProjectionSpheroid> (metadata.getItemIntExactMatch(EImage::SPHEROID));
    }

    if (metadata.isItemPresentExactMatch(EImage::DATUM))
    {
        datum = static_cast<EProjectionEnum::EProjectionDatum> (metadata.getItemIntExactMatch(EImage::DATUM));
    }

    if (metadata.isItemPresentExactMatch(EImage::GCS_DATUM))
    {
        gcsDatum = static_cast<EProjectionEnum::EProjectionGcsDatum> (metadata.getItemIntExactMatch(EImage::GCS_DATUM));
    }

    if (metadata.isItemPresentExactMatch(EImage::PROJECTION_UNITS))
    {
        projectionUnits = EProjectionEnum::strToProjectionUnits(metadata.getItemStrExactMatch(EImage::PROJECTION_UNITS));
    }

    for (int t = 0; t < static_cast<int> (SImageMetadata::EPROJECTION_PARM_INDEX_COUNT); t++)
    {
        std::string parmName = "PARM_" + toString(t);
        std::string gotParmName = "GOT_" + parmName;
        if (metadata.isItemPresentExactMatch(parmName) && metadata.isItemPresentExactMatch(gotParmName))
        {
            setProjParm(
                        static_cast<SImageMetadata::EProjectionParmIndex> (t),
                        metadata.getItemDoubleExactMatch(parmName),
                        metadata.getItemBoolExactMatch(gotParmName)
                        );
        }
    }

    m_metadataTags.clean();

    std::string metadataStartKey = EImage::METADATA_TAGS;
    metadataStartKey += ".'";
    std::vector<std::pair<std::string, std::vector<std::string> > > metaVals;
    metadata.getAll(metaVals);
    int l = metadataStartKey.length();
    for (unsigned long t = 0; t < metaVals.size(); t++)
    {
        if (startsWith(metaVals[t].first, metadataStartKey))
        {
            std::string key = "";
            for (long a = l; a < metaVals[t].first.length() - 1; a++)
            {
                key += metaVals[t].first[a];
            }
            replaceAllSubStr(key, "'.'", ".");
            m_metadataTags.add(key, metaVals[t].second);
        }
    }

    return 0;
}

bool SImageMetadata::getPixelIsArea() const
{
    return m_pixelIsArea;
}

void SImageMetadata::setPixelIsArea(bool pixelIsArea)
{
    m_pixelIsAreaIsSet = true;
    m_pixelIsArea = pixelIsArea;
}

bool SImageMetadata::getPixelIsAreaIsSet() const
{
    return m_pixelIsAreaIsSet;
}

void SImageMetadata::clearPixelIsArea()
{
    m_pixelIsAreaIsSet = false;
}

int SImageMetadata::getCornerCoordinates(EImage::EImageCorner corner, SPair<double> &cornerVal) const
{
    SSize pixelCoor = 0;
    switch (corner)
    {
    case EImage::IMAGE_CORNER_UL:
        break;
    case EImage::IMAGE_CORNER_UR:
        pixelCoor.col = m_dimension.col - 1;
        break;
    case EImage::IMAGE_CORNER_LR:
        pixelCoor.col = m_dimension.col - 1;
        pixelCoor.row = m_dimension.row - 1;
        break;
    case EImage::IMAGE_CORNER_LL:
        pixelCoor.row = m_dimension.row - 1;
        break;
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid corner ID requested");
        return 1;
    }
    // Xgeo = GT(0) + Xpixel * GT(1) + Yline * GT(2)
    // Ygeo = GT(3) + Xpixel * GT(4) + Yline * GT(5)

    cornerVal.c = m_affineGeoTransform[0] + pixelCoor.col * m_affineGeoTransform[1] + pixelCoor.row * m_affineGeoTransform[2];
    cornerVal.r = m_affineGeoTransform[3] + pixelCoor.col * m_affineGeoTransform[4] + pixelCoor.row * m_affineGeoTransform[5];

    return 0;
}

SImageMetadata::SProjParms::SProjParms()
{
    projParm = 0;
    gotProjParm = false;
}

SImageMetadata::SProjParms::~SProjParms()
{

}

SImageMetadata::SProjParms::SProjParms(const SImageMetadata::SProjParms &r)
{
    projParm = r.projParm;
    gotProjParm = false;
}

SImageMetadata::SProjParms &SImageMetadata::SProjParms::operator=(const SImageMetadata::SProjParms &r)
{
    projParm = r.projParm;
    gotProjParm = false;

    return *this;
}

int SImageMetadata::getProjParms(CVector<SKeyValue<int, double> > &parms) const
{
    parms.resize(static_cast<long> (SImageMetadata::EPROJECTION_PARM_INDEX_COUNT));
    parms.initVec(SKeyValue<int, double>(0, 0));

    for (long t = 0; t < parms.size(); t++)
    {
        parms[t].k = t;
        bool wasSet;
        double val;
        if (getProjParm(static_cast<SImageMetadata::EProjectionParmIndex> (t), val, wasSet) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getProjParm()");
            return 1;
        }
        if (wasSet)
        {
            parms[t].v = val;
        }
    }

    return 0;
}

int SImageMetadata::getProjParm(SImageMetadata::EProjectionParmIndex index, double &projVal, bool &wasSet) const
{
    int max = static_cast<int> (SImageMetadata::EPROJECTION_PARM_INDEX_COUNT);
    int indexVal = static_cast<int> (index);

    if (indexVal < 0 || indexVal >= max)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Index value is out of range");
        return 1;
    }

    projVal = m_projParms[indexVal].projParm;
    wasSet = m_projParms[indexVal].gotProjParm;

    return 0;
}

int SImageMetadata::setProjParm(SImageMetadata::EProjectionParmIndex index, double projVal, bool setUse)
{
    int max = static_cast<int> (SImageMetadata::EPROJECTION_PARM_INDEX_COUNT);
    int indexVal = static_cast<int> (index);

    if (indexVal < 0 || indexVal >= max)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Index value is out of range");
        return 1;
    }

    m_projParms[indexVal].projParm = projVal;
    m_projParms[indexVal].gotProjParm = setUse;

    return 0;
}

int SImageMetadata::getProjParmStr(EProjectionParmIndex index, std::string &parmStr) const
{
    parmStr = "";
    char tempVal[2048] = {0};
    if (index < 0 || index >= EPROJECTION_PARM_INDEX_COUNT)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid projection parm index '0' to '" + toString(EXTRA_3) + "' expected");
        return 1;
    }
    sprintf(tempVal, "%.16g", m_projParms[index].projParm);
    parmStr = tempVal;
    return 0;
}

std::string SImageMetadata::getProj4String() const
{
    return m_proj4Str;
}

void SImageMetadata::setProj4String(const std::string &proj4Str)
{
    m_proj4Str = proj4Str;
    m_proj4StrSet = true;
}

void SImageMetadata::clearProj4String()
{
    m_proj4Str = "";
    m_proj4StrSet = false;
}

bool SImageMetadata::isProj4StringSet() const
{
    return m_proj4StrSet;
}

int SImageMetadata::GenProj4String(std::string &proj4Str) const
{
    //see GDAL "ogr_srs_proj4.cpp" file for other projections
    int ret = 0;
    std::string temp, ultraProjCodeStr;
    if (EProjectionEnum::GetUltraProjCode(projectionCode, ultraProjCodeStr) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetUltraProjCode()");
        return 1;
    }
    // at this point the data has a least a projection

    ret |= EProjectionEnum::ProjCodeToProj4String(projectionCode, temp);
    proj4Str = "+proj=" + temp;
    if (spheroid >= 0 && spheroid < EProjectionEnum::PROJECTION_SPHEROID_COUNT)
    {
        ret |= EProjectionEnum::ProjSpheriodToProj4String(spheroid, temp);
        proj4Str += " +ellps=" + temp;
    }
    if (datum >= 0 && datum < EProjectionEnum::PROJECTION_DATUM_COUNT)
    {
        ret |= EProjectionEnum::ProjDatumToProj4String(datum, temp);
        proj4Str += " +datum=" + temp;
    }
    if (projectionUnits >= 0 && projectionUnits < EProjectionEnum::PROJECTION_UNIT_COUNT)
    {
        ret |= EProjectionEnum::ProjUnitsToProj4String(projectionUnits, temp);
        if (temp != "")
            proj4Str += " +units=" + temp;
    }

    switch (projectionCode)
    {
    case EProjectionEnum::PROJECTION_GEO:
        break;
    case EProjectionEnum::PROJECTION_UTM:
        ret |= (getAbs(utmZone)<-60 || getAbs(utmZone) > 60) ? (1) : (0);
        proj4Str += " +zone=" + toString(getAbs(utmZone));
        if (utmZone < 0)
            proj4Str += " +south";
        break;
    case EProjectionEnum::PROJECTION_SPCS:
        //not proj4 codes
        break;
    case EProjectionEnum::PROJECTION_ALBERS:
        ret |= getProjParmStr(LATITUDE_OF_THE_FIRST_STANDARD_PARALLEL, temp);
        proj4Str += " +lat_1=" + temp;
        ret |= getProjParmStr(LATITUDE_OF_THE_SECOND_STANDARD_PARALLEL, temp);
        proj4Str += " +lat_2=" + temp;
        ret |= getProjParmStr(LATITUDE_OF_THE_PROJECTION_ORIGIN, temp);
        proj4Str += " +lat_0=" + temp;
        ret |= getProjParmStr(LONGITUDE_OF_THE_CENTRAL_MERIDIAN, temp);
        proj4Str += " +lon_0=" + temp;
        ret |= getProjParmStr(FALSE_EASTING, temp);
        proj4Str += " +x_0=" + temp;
        ret |= getProjParmStr(FALSE_NORTHING, temp);
        proj4Str += " +y_0=" + temp;
        break;
    case EProjectionEnum::PROJECTION_LAMCC:
        ret |= getProjParmStr(LATITUDE_OF_THE_FIRST_STANDARD_PARALLEL, temp);
        proj4Str += " +lat_1=" + temp;
        ret |= getProjParmStr(LATITUDE_OF_THE_SECOND_STANDARD_PARALLEL, temp);
        proj4Str += " +lat_2=" + temp;
        ret |= getProjParmStr(LATITUDE_OF_THE_PROJECTION_ORIGIN, temp);
        proj4Str += " +lat_0=" + temp;
        ret |= getProjParmStr(LONGITUDE_OF_THE_CENTRAL_MERIDIAN, temp);
        proj4Str += " +lon_0=" + temp;
        ret |= getProjParmStr(FALSE_EASTING, temp);
        proj4Str += " +x_0=" + temp;
        ret |= getProjParmStr(FALSE_NORTHING, temp);
        proj4Str += " +y_0=" + temp;
        break;
    case EProjectionEnum::PROJECTION_MERCAT:
        ret |= getProjParmStr(LONGITUDE_OF_THE_CENTRAL_MERIDIAN, temp);
        proj4Str += " +lon_0=" + temp;
        ret |= getProjParmStr(LATITUDE_OF_THE_PROJECTION_ORIGIN, temp);
        proj4Str += " +lat_ts=" + temp;
        ret |= getProjParmStr(FALSE_EASTING, temp);
        proj4Str += " +x_0=" + temp;
        ret |= getProjParmStr(FALSE_NORTHING, temp);
        proj4Str += " +y_0=" + temp;
        break;
    case EProjectionEnum::PROJECTION_PS:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_POLYC:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_EQUIDC:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_TM:
        if (m_projParms[0].gotProjParm)
        {
            ret |= getProjParmStr(SEMI_MAJOR_AXIS, temp);
            proj4Str += " +a=" + temp;
        }
        if (m_projParms[1].gotProjParm)
        {
            ret |= getProjParmStr(SEMI_MINOR_AXIS, temp);
            proj4Str += " +b=" + temp;
        }
        ret |= getProjParmStr(LATITUDE_OF_THE_PROJECTION_ORIGIN, temp);
        proj4Str += " +lat_0=" + temp;
        ret |= getProjParmStr(LONGITUDE_OF_THE_CENTRAL_MERIDIAN, temp);
        proj4Str += " +lon_0=" + temp;
        ret |= getProjParmStr(FALSE_EASTING, temp);
        proj4Str += " +x_0=" + temp;
        ret |= getProjParmStr(FALSE_NORTHING, temp);
        proj4Str += " +y_0=" + temp;
        break;
    case EProjectionEnum::PROJECTION_STEREO:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_LAMAZ:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_AZMEQD:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_GNOMON:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_ORTHO:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_GVNSP:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_SNSOID:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_EQRECT:

        if (!m_projParms[0].gotProjParm)
        {
            ret |= getProjParmStr(LATITUDE_OF_THE_FIRST_STANDARD_PARALLEL, temp);
            proj4Str += " +lat_ts=" + temp;
            ret |= getProjParmStr(LATITUDE_OF_THE_PROJECTION_ORIGIN, temp);
            proj4Str += " +lat_0=" + temp;
            ret |= getProjParmStr(LONGITUDE_OF_THE_CENTRAL_MERIDIAN, temp);
            proj4Str += " +lon_0=" + temp;
            ret |= getProjParmStr(FALSE_EASTING, temp);
            proj4Str += " +x_0=" + temp;
            ret |= getProjParmStr(FALSE_NORTHING, temp);
            proj4Str += " +y_0=" + temp;
        }
        else
        {
            proj4Str = "+proj=eqc +ellps=sphere";
            ret |= getProjParmStr(SEMI_MAJOR_AXIS, temp);
            proj4Str += " +R=" + temp;
            if (projectionUnits >= 0 || projectionUnits < EProjectionEnum::PROJECTION_UNIT_COUNT)
            {
                ret |= EProjectionEnum::ProjUnitsToProj4String(projectionUnits, temp);
                if (temp != "")
                    proj4Str += " +units=" + temp;
            }
            proj4Str += " +no_defs";
        }
        break;
    case EProjectionEnum::PROJECTION_MILLER:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_VGRINT:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_HOM:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_ROBIN:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_SOM:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_ALASKA:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_GOOD:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_MOLL:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_IMOLL:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_HAMMER:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_WAGIV:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_WAGVII:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    case EProjectionEnum::PROJECTION_OBEQA:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented translation for projection '" + ultraProjCodeStr + " to proj4");
        ret = 1;
        break;
    }

    proj4Str += " +no_defs";

    if (ret != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to translate metadata to proj4 projection string");
        return 1;
    }
    return 0;
}

SImageMetadata::SStringRpc SImageMetadata::getRpc() const
{
    return m_rpc;
}

void SImageMetadata::setRpc(SImageMetadata::SStringRpc rpc)
{
    this->m_rpc = rpc;
}

int SImageMetadata::SetAffineGeoTransform(const CVector<double> &vec)
{
    if (vec.size() == 6)
    {
        SPair<double> gsd, origin, shear;
        m_affineGeoTransform = vec;
        gsd.x = m_affineGeoTransform[1];
        gsd.y = m_affineGeoTransform[5];

        shear.x = m_affineGeoTransform[2];
        shear.y = m_affineGeoTransform[4];

        origin.x = m_affineGeoTransform[0];
        origin.y = m_affineGeoTransform[3];

        setGsd(gsd);
        setShear(shear);
        setOrigin(origin);
    }
    else
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot set the affine geo-transformation, vector requires 6 doubles");
        return 1;
    }
    return 0;
}

CVector<double> SImageMetadata::getAffineGeoTransform() const
{
    return m_affineGeoTransform;
}

CVector<SImageMetadata::STiePoint> SImageMetadata::getTiePoints() const
{
    return m_tiePoints;
}

bool SImageMetadata::hasGdalProjectionRefWkt() const
{
    return m_hasGdalProjectionRefWkt;
}

std::string SImageMetadata::getGdalProjectionRefWkt() const
{
    return m_gdalProjectionRefWktStr.c_str();
}

void SImageMetadata::cleanGdalProjectionRefWkt()
{
    m_hasGdalProjectionRefWkt = false;
    m_gdalProjectionRefWktStr = "";
}

const COdl &SImageMetadata::getExtraMetadata() const
{
    return m_metadataTags;
}

void SImageMetadata::setGdalProjectionRefWkt(const std::string &gdalProjectionRefWkt)
{
    m_hasGdalProjectionRefWkt = true;
    m_gdalProjectionRefWktStr = gdalProjectionRefWkt.c_str();
}

int SImageMetadata::GenerateWorldFile(std::string pathToWorldFile) const
{
    return GenerateWorldFile(pathToWorldFile, getAffineGeoTransform());
}

int SImageMetadata::GenerateWorldFile(std::string pathToWorldFile, const CVector<double> &geoAffineTransform)
{
    CFile fPath = pathToWorldFile;
    if (!fPath.getParentFolderFile().isDirectoryWritable())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot save file to '" + pathToWorldFile + "' the path is not writable");
        return 1;
    }
    if (fPath.exists())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Going to overwrite a file at '" + pathToWorldFile + "'");
    }

    FILE *fm = fopen(pathToWorldFile.c_str(), "we");
    if (fm == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create a world file at '" + pathToWorldFile + "'");
        return 1;
    }

    SPair<double> gsd = getGsd(geoAffineTransform); // 1 5
    SPair<double> origin = getOrigin(geoAffineTransform); // 0 3
    SPair<double> shear = getShear(geoAffineTransform); // 2 4

    // Xgeo = GT(0) + Xpixel*GT(1) + Yline*GT(2)
    // Ygeo = GT(3) + Xpixel*GT(4) + Yline*GT(5)

    fprintf(fm, "%lf\n", gsd.c);
    fprintf(fm, "%lf\n", shear.c);
    fprintf(fm, "%lf\n", shear.r);
    fprintf(fm, "%lf\n", gsd.r);
    fprintf(fm, "%lf\n", origin.c);
    fprintf(fm, "%lf\n", origin.r);

    fclose(fm);
    return 0;
}

} //namespace ultra
