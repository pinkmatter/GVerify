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
#include <ul_Vector.h>
#include <ul_KeyValue.h>

namespace ultra
{

struct EImage
{
    static const char *ACTUAL_BITS_PER_PIXEL;
    static const char *BAND_COUNT;
    static const char *ACTUAL_PROJECTION;
    static const char *DATUM;
    static const char *GCS_DATUM;
    static const char *GOT_PARM_0;
    static const char *GOT_PARM_1;
    static const char *GOT_PARM_10;
    static const char *GOT_PARM_11;
    static const char *GOT_PARM_12;
    static const char *GOT_PARM_13;
    static const char *GOT_PARM_14;
    static const char *GOT_PARM_2;
    static const char *GOT_PARM_3;
    static const char *GOT_PARM_4;
    static const char *GOT_PARM_5;
    static const char *GOT_PARM_6;
    static const char *GOT_PARM_7;
    static const char *GOT_PARM_8;
    static const char *GOT_PARM_9;
    static const char *NUM_COLS;
    static const char *NUM_ROWS;
    static const char *PARM_0;
    static const char *PARM_1;
    static const char *PARM_10;
    static const char *PARM_11;
    static const char *PARM_12;
    static const char *PARM_13;
    static const char *PARM_14;
    static const char *PARM_2;
    static const char *PARM_3;
    static const char *PARM_4;
    static const char *PARM_5;
    static const char *PARM_6;
    static const char *PARM_7;
    static const char *PARM_8;
    static const char *PARM_9;
    static const char *PIXEL_IS_AREA;
    static const char *AREA_OR_POINT;
    static const char *PROJECTION_UNITS;
    static const char *PROJ_CODE;
    static const char *PROJ_CODE_NAME;
    static const char *SPHEROID;
    static const char *UTM_ZONE;
    static const char *GEO_TRANSFORM_0;
    static const char *GEO_TRANSFORM_1;
    static const char *GEO_TRANSFORM_2;
    static const char *GEO_TRANSFORM_3;
    static const char *GEO_TRANSFORM_4;
    static const char *GEO_TRANSFORM_5;
    static const char *UL_X;
    static const char *UL_Y;
    static const char *LL_X;
    static const char *LL_Y;
    static const char *LR_X;
    static const char *LR_Y;
    static const char *UR_X;
    static const char *UR_Y;
    static const char *RPC_HEIGHT_OFF;
    static const char *RPC_HEIGHT_SCALE;
    static const char *RPC_LAT_OFF;
    static const char *RPC_LAT_SCALE;
    static const char *RPC_LINE_OFF;
    static const char *RPC_LINE_SCALE;
    static const char *RPC_LONG_OFF;
    static const char *RPC_LONG_SCALE;
    static const char *RPC_MAX_LAT;
    static const char *RPC_MAX_LONG;
    static const char *RPC_MIN_LAT;
    static const char *RPC_MIN_LONG;
    static const char *RPC_SAMP_OFF;
    static const char *RPC_SAMP_SCALE;
    static const char *RPC_POLYNOMIAL;
    static const char *RPC_SAMP_DEN_COEFF;
    static const char *RPC_SAMP_NUM_COEFF;
    static const char *RPC_LINE_DEN_COEFF;
    static const char *RPC_LINE_NUM_COEFF;
    static const char *TIEPOINT_LINE;
    static const char *TIEPOINT_SAMPLE;
    static const char *TIEPOINT_X;
    static const char *TIEPOINT_Y;
    static const char *TIEPOINT_Z;
    static const char *TIEPOINT_ID;
    static const char *TIEPOINT_INFO;
    static const char *METADATA_TAGS;

    enum EImageFormat
    {
        IMAGE_TYPE_BMP = 0,
        IMAGE_TYPE_GEOTIFF,
        IMAGE_TYPE_BIG_GEOTIFF,
        IMAGE_TYPE_FAST,
        IMAGE_TYPE_RAW,
        IMAGE_TYPE_HDF,
        IMAGE_TYPE_HDF5,
        IMAGE_TYPE_JPEG,
        IMAGE_TYPE_NITF,
        IMAGE_TYPE_PCIDSK,
        IMAGE_TYPE_COSAR,
        IMAGE_TYPE_DIM,
        IMAGE_TYPE_PNG,
        IMAGE_TYPE_ENVI,
        IMAGE_TYPE_VRT,
        IMAGE_TYPE_ECW,
        IMAGE_TYPE_DIMAP,
        IMAGE_TYPE_OPEN_J2000,
        IMAGE_TYPE_MEMORY,
        IMAGE_TYPE_ENUM_COUNT
    };

    enum EImageCorner
    {
        IMAGE_CORNER_UL = 0,
        IMAGE_CORNER_LL,
        IMAGE_CORNER_LR,
        IMAGE_CORNER_UR,
        IMAGE_CORNER_ENUM_COUNT
    };

    static bool canWriteImage(EImage::EImageFormat image);
    static bool canReadImage(EImage::EImageFormat image);
    static std::string imageFormatToStr(EImage::EImageFormat image);
    static std::string imageFormatToGdalStr(EImage::EImageFormat image);
    static EImage::EImageFormat gdalStrToImageFormat(const std::string &imageStr);
    static EImage::EImageFormat strToImageFormat(const std::string &imageStr);

};

} // namespace ultra
