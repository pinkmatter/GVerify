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

#include "ul_ImageEnums.h"
#include <ul_Utility.h>
#include <ul_Logger.h>

namespace ultra
{

const char *EImage::ACTUAL_BITS_PER_PIXEL = "ACTUAL_BITS_PER_PIXEL";
const char *EImage::BAND_COUNT = "BAND_COUNT";
const char *EImage::ACTUAL_PROJECTION = "ACTUAL_PROJECTION";
const char *EImage::DATUM = "DATUM";
const char *EImage::GCS_DATUM = "GCS_DATUM";
const char *EImage::GOT_PARM_0 = "GOT_PARM_0";
const char *EImage::GOT_PARM_1 = "GOT_PARM_1";
const char *EImage::GOT_PARM_10 = "GOT_PARM_10";
const char *EImage::GOT_PARM_11 = "GOT_PARM_11";
const char *EImage::GOT_PARM_12 = "GOT_PARM_12";
const char *EImage::GOT_PARM_13 = "GOT_PARM_13";
const char *EImage::GOT_PARM_14 = "GOT_PARM_14";
const char *EImage::GOT_PARM_2 = "GOT_PARM_2";
const char *EImage::GOT_PARM_3 = "GOT_PARM_3";
const char *EImage::GOT_PARM_4 = "GOT_PARM_4";
const char *EImage::GOT_PARM_5 = "GOT_PARM_5";
const char *EImage::GOT_PARM_6 = "GOT_PARM_6";
const char *EImage::GOT_PARM_7 = "GOT_PARM_7";
const char *EImage::GOT_PARM_8 = "GOT_PARM_8";
const char *EImage::GOT_PARM_9 = "GOT_PARM_9";
const char *EImage::NUM_COLS = "NUM_COLS";
const char *EImage::NUM_ROWS = "NUM_ROWS";
const char *EImage::PARM_0 = "PARM_0";
const char *EImage::PARM_1 = "PARM_1";
const char *EImage::PARM_10 = "PARM_10";
const char *EImage::PARM_11 = "PARM_11";
const char *EImage::PARM_12 = "PARM_12";
const char *EImage::PARM_13 = "PARM_13";
const char *EImage::PARM_14 = "PARM_14";
const char *EImage::PARM_2 = "PARM_2";
const char *EImage::PARM_3 = "PARM_3";
const char *EImage::PARM_4 = "PARM_4";
const char *EImage::PARM_5 = "PARM_5";
const char *EImage::PARM_6 = "PARM_6";
const char *EImage::PARM_7 = "PARM_7";
const char *EImage::PARM_8 = "PARM_8";
const char *EImage::PARM_9 = "PARM_9";
const char *EImage::PIXEL_IS_AREA = "PIXEL_IS_AREA";
const char *EImage::AREA_OR_POINT = "AREA_OR_POINT";
const char *EImage::PROJECTION_UNITS = "PROJECTION_UNITS";
const char *EImage::PROJ_CODE = "PROJ_CODE";
const char *EImage::PROJ_CODE_NAME = "PROJ_CODE_NAME";
const char *EImage::SPHEROID = "SPHEROID";
const char *EImage::UTM_ZONE = "UTM_ZONE";
const char *EImage::GEO_TRANSFORM_0 = "GEO_TRANSFORM_0";
const char *EImage::GEO_TRANSFORM_1 = "GEO_TRANSFORM_1";
const char *EImage::GEO_TRANSFORM_2 = "GEO_TRANSFORM_2";
const char *EImage::GEO_TRANSFORM_3 = "GEO_TRANSFORM_3";
const char *EImage::GEO_TRANSFORM_4 = "GEO_TRANSFORM_4";
const char *EImage::GEO_TRANSFORM_5 = "GEO_TRANSFORM_5";
const char *EImage::UL_X = "UL_X";
const char *EImage::UL_Y = "UL_Y";
const char *EImage::LL_X = "LL_X";
const char *EImage::LL_Y = "LL_Y";
const char *EImage::LR_X = "LR_X";
const char *EImage::LR_Y = "LR_Y";
const char *EImage::UR_X = "UR_X";
const char *EImage::UR_Y = "UR_Y";
const char *EImage::RPC_HEIGHT_OFF = "RPC_HEIGHT_OFF";
const char *EImage::RPC_HEIGHT_SCALE = "RPC_HEIGHT_SCALE";
const char *EImage::RPC_LAT_OFF = "RPC_LAT_OFF";
const char *EImage::RPC_LAT_SCALE = "RPC_LAT_SCALE";
const char *EImage::RPC_LINE_OFF = "RPC_LINE_OFF";
const char *EImage::RPC_LINE_SCALE = "RPC_LINE_SCALE";
const char *EImage::RPC_LONG_OFF = "RPC_LONG_OFF";
const char *EImage::RPC_LONG_SCALE = "RPC_LONG_SCALE";
const char *EImage::RPC_MAX_LAT = "RPC_MAX_LAT";
const char *EImage::RPC_MAX_LONG = "RPC_MAX_LONG";
const char *EImage::RPC_MIN_LAT = "RPC_MIN_LAT";
const char *EImage::RPC_MIN_LONG = "RPC_MIN_LONG";
const char *EImage::RPC_SAMP_OFF = "RPC_SAMP_OFF";
const char *EImage::RPC_SAMP_SCALE = "RPC_SAMP_SCALE";
const char *EImage::RPC_POLYNOMIAL = "RPC_POLYNOMIAL";
const char *EImage::RPC_SAMP_DEN_COEFF = "RPC_SAMP_DEN_COEFF";
const char *EImage::RPC_SAMP_NUM_COEFF = "RPC_SAMP_NUM_COEFF";
const char *EImage::RPC_LINE_DEN_COEFF = "RPC_LINE_DEN_COEFF";
const char *EImage::RPC_LINE_NUM_COEFF = "RPC_LINE_NUM_COEFF";
const char *EImage::TIEPOINT_LINE = "TIEPOINT_LINE_";
const char *EImage::TIEPOINT_SAMPLE = "TIEPOINT_SAMPLE_";
const char *EImage::TIEPOINT_X = "TIEPOINT_X_";
const char *EImage::TIEPOINT_Y = "TIEPOINT_Y_";
const char *EImage::TIEPOINT_Z = "TIEPOINT_Z_";
const char *EImage::TIEPOINT_ID = "TIEPOINT_ID_";
const char *EImage::TIEPOINT_INFO = "TIEPOINT_INFO_";
const char *EImage::METADATA_TAGS = "METADATA_TAGS";

std::string EImage::imageFormatToGdalStr(EImage::EImageFormat image)
{
    switch (image)
    {
    case EImage::IMAGE_TYPE_BMP:return "BMP";
    case EImage::IMAGE_TYPE_GEOTIFF:return "GTiff";
    case EImage::IMAGE_TYPE_COSAR:return "COSAR";
    case EImage::IMAGE_TYPE_DIMAP:return "DIMAP";
    case EImage::IMAGE_TYPE_BIG_GEOTIFF:return "GTiff";
    case EImage::IMAGE_TYPE_FAST:
    case EImage::IMAGE_TYPE_RAW:
    case EImage::IMAGE_TYPE_HDF:
    case EImage::IMAGE_TYPE_HDF5: return "";
    case EImage::IMAGE_TYPE_JPEG:return "JPEG";
    case EImage::IMAGE_TYPE_NITF:return "NITF";
    case EImage::IMAGE_TYPE_PCIDSK:return "PCIDSK";
    case EImage::IMAGE_TYPE_DIM:return "";
    case EImage::IMAGE_TYPE_PNG:return "PNG";
    case EImage::IMAGE_TYPE_ENVI:return "ENVI"; //envisat
    case EImage::IMAGE_TYPE_VRT:return "VRT";
    case EImage::IMAGE_TYPE_ECW:return "ECW";
    case EImage::IMAGE_TYPE_OPEN_J2000:return "JP2OpenJPEG";
    case EImage::IMAGE_TYPE_MEMORY:return "MEM";
    }
    return "";
}

std::string EImage::imageFormatToStr(EImage::EImageFormat image)
{
    switch (image)
    {
    case EImage::IMAGE_TYPE_BMP:return "BMP";
    case EImage::IMAGE_TYPE_DIMAP:return "DIMAP";
    case EImage::IMAGE_TYPE_GEOTIFF:return "GEOTIFF";
    case EImage::IMAGE_TYPE_BIG_GEOTIFF:return "BIG_GEOTIFF";
    case EImage::IMAGE_TYPE_FAST:return "FAST";
    case EImage::IMAGE_TYPE_RAW:return "RAW";
    case EImage::IMAGE_TYPE_HDF:return "HDF";
    case EImage::IMAGE_TYPE_HDF5:return "HDF5";
    case EImage::IMAGE_TYPE_JPEG:return "JPEG";
    case EImage::IMAGE_TYPE_COSAR:return "COSAR";
    case EImage::IMAGE_TYPE_NITF:return "NITF";
    case EImage::IMAGE_TYPE_PCIDSK:return "PCIDSK";
    case EImage::IMAGE_TYPE_DIM:return "DIM";
    case EImage::IMAGE_TYPE_PNG:return "PNG";
    case EImage::IMAGE_TYPE_ENVI:return "ENVI";
    case EImage::IMAGE_TYPE_VRT:return "VRT";
    case EImage::IMAGE_TYPE_ECW:return "ECW";
    case EImage::IMAGE_TYPE_OPEN_J2000:return "OPEN_J2000";
    case EImage::IMAGE_TYPE_MEMORY:return "MEMORY";
    }
    return "";
}

bool EImage::canWriteImage(EImage::EImageFormat image)
{
    if (image == EImage::IMAGE_TYPE_VRT ||
            image == EImage::IMAGE_TYPE_DIMAP ||
            image == EImage::IMAGE_TYPE_MEMORY)
        return false;
    return true;
}

bool EImage::canReadImage(EImage::EImageFormat image)
{
    return true;
}

EImage::EImageFormat EImage::gdalStrToImageFormat(const std::string &imageStr)
{
    std::string s = toUpper(imageStr);

    if (s == "BMP") return EImage::IMAGE_TYPE_BMP;
    if (s == "GTIFF") return EImage::IMAGE_TYPE_GEOTIFF;
    if (s == "PCIDSK") return EImage::IMAGE_TYPE_PCIDSK;
    if (s == "COSAR")return EImage::IMAGE_TYPE_COSAR;
    if (s == "DIMAP")return EImage::IMAGE_TYPE_DIMAP;
    //    if(s=="BMP") EImageEnum::IMAGE_TYPE_BIG_GEOTIFF:
    //    if(s=="BMP") EImageEnum::IMAGE_TYPE_FAST:
    //    if(s=="BMP") EImageEnum::IMAGE_TYPE_RAW:
    //    if(s=="BMP") EImageEnum::IMAGE_TYPE_HDF:return "";
    if (s == "JPEG") return EImage::IMAGE_TYPE_JPEG;
    if (s == "NITF") return EImage::IMAGE_TYPE_NITF;
    //    if(s=="BMP") EImageEnum::IMAGE_TYPE_PCIDSK:
    //    if(s=="BMP") EImageEnum::IMAGE_TYPE_DIM:return "";
    if (s == "PNG") return EImage::IMAGE_TYPE_PNG;
    if (s == "ENVI")return EImage::IMAGE_TYPE_ENVI; //envisat
    if (s == "VRT") return EImage::IMAGE_TYPE_VRT;
    if (s == "ECW")return EImage::IMAGE_TYPE_ECW;
    if (s == "JP2OPENJPEG")return EImage::IMAGE_TYPE_OPEN_J2000;
    if (s == "MEM")return EImage::IMAGE_TYPE_MEMORY;

    return EImage::IMAGE_TYPE_ENUM_COUNT;
}

EImage::EImageFormat EImage::strToImageFormat(const std::string &imageStr)
{
    std::string s = toUpper(imageStr);
    if (s == "BMP" || s == "IMAGE_TYPE_ BMP") return EImage::IMAGE_TYPE_BMP;
    if (s == "GEOTIFF" || s == "IMAGE_TYPE_GEOTIFF") return EImage::IMAGE_TYPE_GEOTIFF;
    if (s == "BIG_GEOTIFF" || s == "IMAGE_TYPE_BIG_GEOTIFF") return EImage::IMAGE_TYPE_BIG_GEOTIFF;
    if (s == "FAST" || startsWith(s, "FAST") || s == "IMAGE_TYPE_FAST") return EImage::IMAGE_TYPE_FAST;
    if (s == "RAW" || s == "IMAGE_TYPE_RAW") return EImage::IMAGE_TYPE_RAW;
    if (s == "HDF" || startsWith(s, "HDF") || s == "IMAGE_TYPE_HDF") return EImage::IMAGE_TYPE_HDF;
    if (s == "HDF5" || startsWith(s, "HDF5") || s == "IMAGE_TYPE_HDF5") return EImage::IMAGE_TYPE_HDF5;
    if (s == "JPEG" || s == "IMAGE_TYPE_JPEG") return EImage::IMAGE_TYPE_JPEG;
    if (s == "NITF" || s == "IMAGE_TYPE_NITF") return EImage::IMAGE_TYPE_NITF;
    if (s == "PCIDSK" || s == "IMAGE_TYPE_PCIDSK") return EImage::IMAGE_TYPE_PCIDSK;
    if (s == "DIM" || s == "IMAGE_TYPE_DIM") return EImage::IMAGE_TYPE_DIM;
    if (s == "PNG" || s == "IMAGE_TYPE_PNG") return EImage::IMAGE_TYPE_PNG;
    if (s == "ENVI" || s == "IMAGE_TYPE_ENVI") return EImage::IMAGE_TYPE_ENVI;
    if (s == "VRT" || s == "IMAGE_TYPE_VRT") return EImage::IMAGE_TYPE_VRT;
    if (s == "ECW" || s == "IMAGE_TYPE_ECW") return EImage::IMAGE_TYPE_ECW;
    if (s == "DIMAP" || s == "IMAGE_TYPE_DIMAP")return EImage::IMAGE_TYPE_DIMAP;
    if (s == "JP2OPENJPEG" || s == "IMAGE_TYPE_OPEN_J2000")return EImage::IMAGE_TYPE_OPEN_J2000;
    if (s == "MEM" || s == "IMAGE_TYPE_MEMORY")return EImage::IMAGE_TYPE_MEMORY;

    return EImage::IMAGE_TYPE_ENUM_COUNT;
}

} // namespace ultra
