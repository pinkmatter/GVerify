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

#include "ul_ProjectionEnum.h"

#include "ul_Logger.h"
#include "ul_Utility.h"

namespace ultra
{

const char* EProjectionEnum::EProjectionTypeUltraStr [EProjectionEnum::PROJECTION_TYPE_COUNT] = {
    "PROJECTION_GEO",
    "PROJECTION_UTM",
    "PROJECTION_SPCS",
    "PROJECTION_ALBERS",
    "PROJECTION_LAMCC",
    "PROJECTION_MERCAT",
    "PROJECTION_PS",
    "PROJECTION_POLYC",
    "PROJECTION_EQUIDC",
    "PROJECTION_TM",
    "PROJECTION_STEREO",
    "PROJECTION_LAMAZ",
    "PROJECTION_AZMEQD",
    "PROJECTION_GNOMON",
    "PROJECTION_ORTHO",
    "PROJECTION_GVNSP",
    "PROJECTION_SNSOID",
    "PROJECTION_EQRECT",
    "PROJECTION_MILLER",
    "PROJECTION_VGRINT",
    "PROJECTION_HOM",
    "PROJECTION_ROBIN",
    "PROJECTION_SOM",
    "PROJECTION_ALASKA",
    "PROJECTION_GOOD",
    "PROJECTION_MOLL",
    "PROJECTION_IMOLL",
    "PROJECTION_HAMMER",
    "PROJECTION_WAGIV",
    "PROJECTION_WAGVII",
    "PROJECTION_OBEQA"
};

const char* EProjectionEnum::EProjectionTypeProj4Str[EProjectionEnum::PROJECTION_TYPE_COUNT] = {
    "longlat",
    "utm",
    "",
    "aea",
    "lcc",
    "merc",
    "stere",
    "poly",
    "eqdc",
    "tmerc",
    "sterea",
    "laea",
    "aeqd",
    "gnom",
    "ortho",
    "",
    "sinu",
    "eqc",
    "mill",
    "vandg",
    "omerc",
    "robin",
    "",
    "",
    "goode",
    "moll",
    "",
    "",
    "wag4",
    "wag7",
    ""
};

const double EProjectionEnum::EProjectionSpheroidAxis[EProjectionEnum::PROJECTION_SPHEROID_COUNT][2] = {
    {6378206.4, 6356583.8}, /* 0: Clarke 1866 (default) */
    {6378249.145, 6356514.86955}, /* 1: Clarke 1880 */
    {6377397.155, 6356078.96284}, /* 2: Bessel */
    {6378157.5, 6356772.2}, /* 3: International 1967 */
    {6378388.0, 6356911.94613}, /* 4: International 1909 */
    {6378135.0, 6356750.519915}, /* 5: WGS 72 */
    {6377276.3452, 6356075.4133}, /* 6: Everest */
    {6378145.0, 6356759.769356}, /* 7: WGS 66 */
    {6378137.0, 6356752.31414}, /* 8: GRS 1980 */
    {6377563.396, 6356256.91}, /* 9: Airy */
    {6377304.063, 6356103.039}, /* 10: Modified Everest */
    {6377340.189, 6356034.448}, /* 11: Modified Airy */
    {6378137.0, 6356752.314245}, /* 12: WGS 84 */
    {6378155.0, 6356773.3205}, /* 13: Southeast Asia */
    {6378160.0, 6356774.719}, /* 14: Australian National */
    {6378245.0, 6356863.0188}, /* 15: Krassovsky */
    {6378270.0, 6356794.343479}, /* 16: Hough */
    {6378166.0, 6356784.283666}, /* 17: Mercury 1960 */
    {6378150.0, 6356768.337303}, /* 18: Modified Mercury 1968 */
    {6370997.0, 6370997.0}, /* 19: Sphere of Radius 6370997 meters*/
    {6377483.865, 6356165.382966}, /* 20: Bessel 1841(Namibia) */
    {6377298.556, 6356097.571445}, /* 21: Everest (Sabah & Sarawak) */
    {6377301.243, 6356100.228368}, /* 22: Everest (India 1956) */
    {6377295.664, 6356094.667915}, /* 23: Everest (Malaysia 1969) */
    {6377304.063, 6356103.038993}, /* 24: Everest (Malay & Singapr 1948)*/
    {6377309.613, 6356108.570542}, /* 25: Everest (Pakistan) */
    {6378388.0, 6356911.946128}, /* 26: Hayford */
    {6378200.0, 6356818.169}, /* 27: Helmert 1906 */
    {6378160.000, 6356774.504086}, /* 28: Indonesian 1974 */
    {6378160.0, 6356774.719}, /* 29: South American 1969 */
    {6378165.0, 6356783.287}, /* 30: WGS 60 */
    {6371007.181, 6371007.181} /* 31: MODIS Sphere */
};

int EProjectionEnum::GetUltraProjCode(EProjectionEnum::EProjectionType proj, std::string &strName)
{
    if (static_cast<int> (proj) < 0 || static_cast<int> (proj) >= EProjectionEnum::PROJECTION_TYPE_COUNT)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown projection code '" + toString(static_cast<int> (proj)) + "', expected '0' to '" + toString(static_cast<int> (EProjectionEnum::PROJECTION_TYPE_COUNT) - 1) + "'");
        return 1;
    }

    strName = EProjectionTypeUltraStr[static_cast<int> (proj)];
    return 0;
}

int EProjectionEnum::ProjCodeToProj4String(EProjectionEnum::EProjectionType proj, std::string &proj4Str)
{
    int index = static_cast<int> (proj);
    if (index < 0 || index >= static_cast<int> (EProjectionEnum::PROJECTION_TYPE_COUNT))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid projection code");
        return 1;
    }

    proj4Str = EProjectionEnum::EProjectionTypeProj4Str[index];
    if (proj4Str == "")
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Empty proj4 projection code returned");
        return 1;
    }

    return 0;
}

int EProjectionEnum::ProjUnitsToProj4String(EProjectionEnum::EProjectionUnits unit, std::string &proj4Str)
{
    switch (unit)
    {
    case EProjectionEnum::PROJECTION_UNIT_METER:
        proj4Str = "m";
        break;
    case EProjectionEnum::PROJECTION_UNIT_FEET:
        proj4Str = "us-ft";
        break;
    case EProjectionEnum::PROJECTION_UNIT_INT_FEET:
        proj4Str = "ft";
        break;
    case EProjectionEnum::PROJECTION_UNIT_DEGREE:
        proj4Str = "";
        break;
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown projection unit code '" + toString(unit) + "'");
        return 1;
    }
    return 0;
}

int EProjectionEnum::ProjSpheriodToProj4String(EProjectionEnum::EProjectionSpheroid spheriod, std::string &proj4Str)
{
    switch (spheriod)
    {
    case EProjectionEnum::PROJECTION_SPHEROID_WGS_84:
        proj4Str = "WGS84";
        break;
    case EProjectionEnum::PROJECTION_SPHEROID_GRS_1980:
        proj4Str = "GRS80";
        break;
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown projection spheriod code '" + toString(spheriod) + "'");
        return 1;
    }
    return 0;
}

int EProjectionEnum::ProjDatumToProj4String(EProjectionEnum::EProjectionDatum datum, std::string &proj4Str)
{
    switch (datum)
    {
    case EProjectionEnum::PROJECTION_DATUM_GDA94:
        proj4Str = "GDA94";
        break;
    case EProjectionEnum::PROJECTION_DATUM_WGS84:
        proj4Str = "WGS84";
        break;
    default:
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown projection datum code '" + toString(datum) + "'");
        return 1;
    }
    return 0;
}

int EProjectionEnum::projectionCodeToInt(EProjectionEnum::EProjectionType proj)
{
    return static_cast<int> (proj);
}

std::string EProjectionEnum::projectionCodeToStr(EProjectionEnum::EProjectionType proj)
{
    switch (proj)
    {
    case EProjectionEnum::PROJECTION_GEO:return "GEO";
        break;
    case EProjectionEnum::PROJECTION_UTM:return "UTM";
        break;
    case EProjectionEnum::PROJECTION_SPCS:return "";
        break;
    case EProjectionEnum::PROJECTION_ALBERS:return "AEA";
        break;
    case EProjectionEnum::PROJECTION_LAMCC:return "";
        break;
    case EProjectionEnum::PROJECTION_MERCAT:return "";
        break;
    case EProjectionEnum::PROJECTION_PS:return "";
        break;
    case EProjectionEnum::PROJECTION_POLYC:return "";
        break;
    case EProjectionEnum::PROJECTION_EQUIDC:return "";
        break;
    case EProjectionEnum::PROJECTION_TM:return "";
        break;
    case EProjectionEnum::PROJECTION_STEREO:return "";
        break;
    case EProjectionEnum::PROJECTION_LAMAZ:return "";
        break;
    case EProjectionEnum::PROJECTION_AZMEQD:return "";
        break;
    case EProjectionEnum::PROJECTION_GNOMON:return "";
        break;
    case EProjectionEnum::PROJECTION_ORTHO:return "";
        break;
    case EProjectionEnum::PROJECTION_GVNSP:return "";
        break;
    case EProjectionEnum::PROJECTION_SNSOID:return "";
        break;
    case EProjectionEnum::PROJECTION_EQRECT:return "EQR";
        break;
    case EProjectionEnum::PROJECTION_MILLER:return "";
        break;
    case EProjectionEnum::PROJECTION_VGRINT:return "";
        break;
    case EProjectionEnum::PROJECTION_HOM:return "";
        break;
    case EProjectionEnum::PROJECTION_ROBIN:return "";
        break;
    case EProjectionEnum::PROJECTION_SOM:return "";
        break;
    case EProjectionEnum::PROJECTION_ALASKA:return "";
        break;
    case EProjectionEnum::PROJECTION_GOOD:return "";
        break;
    case EProjectionEnum::PROJECTION_MOLL:return "";
        break;
    case EProjectionEnum::PROJECTION_IMOLL:return "";
        break;
    case EProjectionEnum::PROJECTION_HAMMER:return "";
        break;
    case EProjectionEnum::PROJECTION_WAGIV:return "";
        break;
    case EProjectionEnum::PROJECTION_WAGVII:return "";
        break;
    case EProjectionEnum::PROJECTION_OBEQA:return "";
        break;
    }
    return "";
}

EProjectionEnum::EProjectionType EProjectionEnum::intToProjectionCode(int projInt)
{
    switch (projInt)
    {
    case static_cast<int> (EProjectionEnum::PROJECTION_GEO):return EProjectionEnum::PROJECTION_GEO;
    case static_cast<int> (EProjectionEnum::PROJECTION_UTM):return EProjectionEnum::PROJECTION_UTM;
    case static_cast<int> (EProjectionEnum::PROJECTION_SPCS):return EProjectionEnum::PROJECTION_SPCS;
    case static_cast<int> (EProjectionEnum::PROJECTION_ALBERS):return EProjectionEnum::PROJECTION_ALBERS;
    case static_cast<int> (EProjectionEnum::PROJECTION_LAMCC):return EProjectionEnum::PROJECTION_LAMCC;
    case static_cast<int> (EProjectionEnum::PROJECTION_MERCAT):return EProjectionEnum::PROJECTION_MERCAT;
    case static_cast<int> (EProjectionEnum::PROJECTION_PS):return EProjectionEnum::PROJECTION_PS;
    case static_cast<int> (EProjectionEnum::PROJECTION_POLYC):return EProjectionEnum::PROJECTION_POLYC;
    case static_cast<int> (EProjectionEnum::PROJECTION_EQUIDC):return EProjectionEnum::PROJECTION_EQUIDC;
    case static_cast<int> (EProjectionEnum::PROJECTION_TM):return EProjectionEnum::PROJECTION_TM;
    case static_cast<int> (EProjectionEnum::PROJECTION_STEREO):return EProjectionEnum::PROJECTION_STEREO;
    case static_cast<int> (EProjectionEnum::PROJECTION_LAMAZ):return EProjectionEnum::PROJECTION_LAMAZ;
    case static_cast<int> (EProjectionEnum::PROJECTION_AZMEQD):return EProjectionEnum::PROJECTION_AZMEQD;
    case static_cast<int> (EProjectionEnum::PROJECTION_GNOMON):return EProjectionEnum::PROJECTION_GNOMON;
    case static_cast<int> (EProjectionEnum::PROJECTION_ORTHO):return EProjectionEnum::PROJECTION_ORTHO;
    case static_cast<int> (EProjectionEnum::PROJECTION_GVNSP):return EProjectionEnum::PROJECTION_GVNSP;
    case static_cast<int> (EProjectionEnum::PROJECTION_SNSOID):return EProjectionEnum::PROJECTION_SNSOID;
    case static_cast<int> (EProjectionEnum::PROJECTION_EQRECT):return EProjectionEnum::PROJECTION_EQRECT;
    case static_cast<int> (EProjectionEnum::PROJECTION_MILLER):return EProjectionEnum::PROJECTION_MILLER;
    case static_cast<int> (EProjectionEnum::PROJECTION_VGRINT):return EProjectionEnum::PROJECTION_VGRINT;
    case static_cast<int> (EProjectionEnum::PROJECTION_HOM):return EProjectionEnum::PROJECTION_HOM;
    case static_cast<int> (EProjectionEnum::PROJECTION_ROBIN):return EProjectionEnum::PROJECTION_ROBIN;
    case static_cast<int> (EProjectionEnum::PROJECTION_SOM):return EProjectionEnum::PROJECTION_SOM;
    case static_cast<int> (EProjectionEnum::PROJECTION_ALASKA):return EProjectionEnum::PROJECTION_ALASKA;
    case static_cast<int> (EProjectionEnum::PROJECTION_GOOD):return EProjectionEnum::PROJECTION_GOOD;
    case static_cast<int> (EProjectionEnum::PROJECTION_MOLL):return EProjectionEnum::PROJECTION_MOLL;
    case static_cast<int> (EProjectionEnum::PROJECTION_IMOLL):return EProjectionEnum::PROJECTION_IMOLL;
    case static_cast<int> (EProjectionEnum::PROJECTION_HAMMER):return EProjectionEnum::PROJECTION_HAMMER;
    case static_cast<int> (EProjectionEnum::PROJECTION_WAGIV):return EProjectionEnum::PROJECTION_WAGIV;
    case static_cast<int> (EProjectionEnum::PROJECTION_WAGVII):return EProjectionEnum::PROJECTION_WAGVII;
    case static_cast<int> (EProjectionEnum::PROJECTION_OBEQA):return EProjectionEnum::PROJECTION_OBEQA;
    }
    return EProjectionEnum::PROJECTION_TYPE_COUNT;

}

EProjectionEnum::EProjectionType EProjectionEnum::strToProjectionCode(std::string projStr)
{
    std::string s = toUpper(projStr);
    if (s == "GEO")return EProjectionEnum::PROJECTION_GEO;
    if (s == "UTM")return EProjectionEnum::PROJECTION_UTM;
    if (s == "EQR")return EProjectionEnum::PROJECTION_EQRECT;
    if (s == "AEA")return EProjectionEnum::PROJECTION_ALBERS;
    return EProjectionEnum::PROJECTION_TYPE_COUNT;
}

std::string EProjectionEnum::projectionDatumCodeToStr(EProjectionEnum::EProjectionDatum datum)
{
    switch (datum)
    {
    case EProjectionEnum::PROJECTION_DATUM_WGS84:return "WGS_84";
        break;
    case EProjectionEnum::PROJECTION_DATUM_GDA94:return "GDA_94";
        break;
    }
    return "";
}

EProjectionEnum::EProjectionDatum EProjectionEnum::strToProjectionDatumCode(std::string datumStr)
{
    std::string s = toUpper(datumStr);
    if (s == "WGS84" || s == "WGS_84" || s == "WGS 84")return EProjectionEnum::PROJECTION_DATUM_WGS84;
    if (s == "GDA94" || s == "GDA_94" || s == "GDA 94")return EProjectionEnum::PROJECTION_DATUM_GDA94;
    return EProjectionEnum::PROJECTION_DATUM_COUNT;
}

int EProjectionEnum::projectionSpheroidCodeToInt(EProjectionEnum::EProjectionSpheroid spheroid)
{
    return static_cast<int> (spheroid);
}

EProjectionEnum::EProjectionSpheroid EProjectionEnum::intToProjectionSpheroidCode(int spheroidInt)
{
    if (spheroidInt < -1 || spheroidInt >= static_cast<int> (EProjectionEnum::PROJECTION_SPHEROID_COUNT))
        return EProjectionEnum::PROJECTION_SPHEROID_COUNT;
    return static_cast<EProjectionEnum::EProjectionSpheroid> (spheroidInt);
}

std::string EProjectionEnum::projectionUnitsToStr(EProjectionEnum::EProjectionUnits units)
{
    switch (units)
    {
    case EProjectionEnum::PROJECTION_UNIT_RADIAN:return "RADIANS";
        break;
    case EProjectionEnum::PROJECTION_UNIT_FEET:return "FEET";
        break;
    case EProjectionEnum::PROJECTION_UNIT_METER:return "METERS";
        break;
    case EProjectionEnum::PROJECTION_UNIT_SECOND:return "SECONDS";
        break;
    case EProjectionEnum::PROJECTION_UNIT_DEGREE:return "DEGREES";
        break;
    case EProjectionEnum::PROJECTION_UNIT_INT_FEET:return "INT_FEET";
        break;
    }
    return "";
}

std::string EProjectionEnum::projectionUnitsToStrShort(EProjectionEnum::EProjectionUnits units)
{
    switch (units)
    {
    case EProjectionEnum::PROJECTION_UNIT_RADIAN:return "rad";
        break;
    case EProjectionEnum::PROJECTION_UNIT_FEET:return "ft";
        break;
    case EProjectionEnum::PROJECTION_UNIT_METER:return "m";
        break;
    case EProjectionEnum::PROJECTION_UNIT_SECOND:return "s";
        break;
    case EProjectionEnum::PROJECTION_UNIT_DEGREE:return "d";
        break;
    case EProjectionEnum::PROJECTION_UNIT_INT_FEET:return "ftInt";
        break;
    }
    return "";
}

EProjectionEnum::EProjectionUnits EProjectionEnum::strToProjectionUnits(std::string unitsStr)
{
    std::string s = toUpper(unitsStr);
    if (s == "RADIANS" || s == "RADIAN")return EProjectionEnum::PROJECTION_UNIT_RADIAN;
    if (s == "US SURVEY FOOT" || s == "FEET" ||
        s == "FOOT_US")return EProjectionEnum::PROJECTION_UNIT_FEET;
    if (s == "METERS" || s == "METER" || s == "METRE")return EProjectionEnum::PROJECTION_UNIT_METER;
    if (s == "SECONDS" || s == "SECOND")return EProjectionEnum::PROJECTION_UNIT_SECOND;
    if (s == "DEGREES" || s == "DEGREE")return EProjectionEnum::PROJECTION_UNIT_DEGREE;
    if (s == "INT_FEET")return EProjectionEnum::PROJECTION_UNIT_INT_FEET;
    return EProjectionEnum::PROJECTION_UNIT_COUNT;
}

EProjectionEnum::EProjectionUnits EProjectionEnum::intToProjectionUnits(int unitInt)
{
    switch (unitInt)
    {
    case static_cast<int> (EProjectionEnum::PROJECTION_UNIT_RADIAN):return EProjectionEnum::PROJECTION_UNIT_RADIAN;
    case static_cast<int> (EProjectionEnum::PROJECTION_UNIT_FEET):return EProjectionEnum::PROJECTION_UNIT_FEET;
    case static_cast<int> (EProjectionEnum::PROJECTION_UNIT_METER):return EProjectionEnum::PROJECTION_UNIT_METER;
    case static_cast<int> (EProjectionEnum::PROJECTION_UNIT_SECOND):return EProjectionEnum::PROJECTION_UNIT_SECOND;
    case static_cast<int> (EProjectionEnum::PROJECTION_UNIT_DEGREE):return EProjectionEnum::PROJECTION_UNIT_DEGREE;
    case static_cast<int> (EProjectionEnum::PROJECTION_UNIT_INT_FEET):return EProjectionEnum::PROJECTION_UNIT_INT_FEET;
    default: return EProjectionEnum::PROJECTION_UNIT_COUNT;
    }
}

EProjectionEnum::EProjectionOrientation EProjectionEnum::strToProjectionOrientation(std::string orientationStr)
{
    std::string s = toUpper(orientationStr);
    if (s == "NOM") return EProjectionEnum::NOM;
    else if (s == "NUP") return EProjectionEnum::NUP;
    return EProjectionEnum::PROJECTION_ORIENTATION_COUNT;
}

std::string EProjectionEnum::projectionOrientationToStr(EProjectionEnum::EProjectionOrientation orientation)
{
    switch (orientation)
    {
    case EProjectionEnum::NUP:return "NUP";
    case EProjectionEnum::NOM:return "NOM";
    }
    return "";
}

EProjectionEnum::EProjectionOrigin EProjectionEnum::strToProjectionOrigin(std::string projectionOriginStr)
{
    std::string s = toUpper(projectionOriginStr);
    if (s == "UL") return EProjectionEnum::UL;
    else if (s == "CENTER") return EProjectionEnum::CENTER;
    return EProjectionEnum::PROJECTION_ORIGIN_COUNT;
}

std::string EProjectionEnum::projectionOriginToStr(EProjectionEnum::EProjectionOrigin projectionOrigin)
{
    switch (projectionOrigin)
    {
    case EProjectionEnum::UL:return "UL";
    case EProjectionEnum::CENTER:return "CENTER";
    }
    return "";
}

EProjectionEnum::EProjectionSpheroid EProjectionEnum::strToProjectionSpheroidCode(std::string projectionSpheroidStr)
{
    std::string s = toUpper(projectionSpheroidStr);
    if (s == "USER_DEFINED")return EProjectionEnum::PROJECTION_SPHEROID_USER_DEFINED;
    if (s == "CLARKE_1866")return EProjectionEnum::PROJECTION_SPHEROID_CLARKE_1866;
    if (s == "CLARKE_1880")return EProjectionEnum::PROJECTION_SPHEROID_CLARKE_1880;
    if (s == "BESSEL")return EProjectionEnum::PROJECTION_SPHEROID_BESSEL;
    if (s == "INTERNATIONAL_1967")return EProjectionEnum::PROJECTION_SPHEROID_INTERNATIONAL_1967;
    if (s == "INTERNATIONAL_1909")return EProjectionEnum::PROJECTION_SPHEROID_INTERNATIONAL_1909;
    if (s == "WGS_72" || s == "WGS 72" || s == "WGS72")return EProjectionEnum::PROJECTION_SPHEROID_WGS_72;
    if (s == "EVEREST")return EProjectionEnum::PROJECTION_SPHEROID_EVEREST;
    if (s == "WGS_66" || s == "WGS 66" || s == "WGS66")return EProjectionEnum::PROJECTION_SPHEROID_WGS_66;
    if (s == "GRS80" || s == "GRS_80" || s == "GRS 80" ||
        s == "GRS_1980" || s == "GRS 1980" || s == "GRS1980")return EProjectionEnum::PROJECTION_SPHEROID_GRS_1980;
    if (s == "AIRY")return EProjectionEnum::PROJECTION_SPHEROID_AIRY;
    if (s == "MODIFIED_EVEREST")return EProjectionEnum::PROJECTION_SPHEROID_MODIFIED_EVEREST;
    if (s == "MODIFIED_AIRY")return EProjectionEnum::PROJECTION_SPHEROID_MODIFIED_AIRY;
    if (s == "WGS_84" || s == "WGS 84" || s == "WGS84")return EProjectionEnum::PROJECTION_SPHEROID_WGS_84;
    if (s == "SOUTHERN_ASIA")return EProjectionEnum::PROJECTION_SPHEROID_SOUTHERN_ASIA;
    if (s == "AUSTRALIAN_NATIONAL")return EProjectionEnum::PROJECTION_SPHEROID_AUSTRALIAN_NATIONAL;
    if (s == "KRASSOVSKY")return EProjectionEnum::PROJECTION_SPHEROID_KRASSOVSKY;
    if (s == "HOUGH")return EProjectionEnum::PROJECTION_SPHEROID_HOUGH;
    if (s == "MERCURY_1960")return EProjectionEnum::PROJECTION_SPHEROID_MERCURY_1960;
    if (s == "MODIFIED_MERCURY")return EProjectionEnum::PROJECTION_SPHEROID_MODIFIED_MERCURY;
    if (s == "SPHERE_RADIUS")return EProjectionEnum::PROJECTION_SPHEROID_SPHERE_RADIUS;
    if (s == "BESSEL_1841")return EProjectionEnum::PROJECTION_SPHEROID_BESSEL_1841;
    if (s == "EVEREST_SABAH")return EProjectionEnum::PROJECTION_SPHEROID_EVEREST_SABAH;
    if (s == "EVEREST_INDIA_1956")return EProjectionEnum::PROJECTION_SPHEROID_EVEREST_INDIA_1956;
    if (s == "EVEREST_MALAY_1969")return EProjectionEnum::PROJECTION_SPHEROID_EVEREST_MALAY_1969;
    if (s == "EVEREST_MALAY_1948")return EProjectionEnum::PROJECTION_SPHEROID_EVEREST_MALAY_1948;
    if (s == "EVEREST_PAKISTAN")return EProjectionEnum::PROJECTION_SPHEROID_EVEREST_PAKISTAN;
    if (s == "HAYFORD")return EProjectionEnum::PROJECTION_SPHEROID_HAYFORD;
    if (s == "HELMERT_1906")return EProjectionEnum::PROJECTION_SPHEROID_HELMERT_1906;
    if (s == "INDONESIAN_1974")return EProjectionEnum::PROJECTION_SPHEROID_INDONESIAN_1974;
    if (s == "SOUTH_AMERICAN_1969")return EProjectionEnum::PROJECTION_SPHEROID_SOUTH_AMERICAN_1969;
    if (s == "WGS_60" || s == "WGS 60" || s == "WGS60")return EProjectionEnum::PROJECTION_SPHEROID_WGS_60;
    if (s == "MODIS_SPHERE")return EProjectionEnum::PROJECTION_SPHEROID_MODIS_SPHERE;
    return EProjectionEnum::PROJECTION_SPHEROID_COUNT;
}

std::string EProjectionEnum::projectionSpheroidCodeToStr(EProjectionEnum::EProjectionSpheroid projectionSpheroid)
{
    switch (projectionSpheroid)
    {
    case EProjectionEnum::PROJECTION_SPHEROID_USER_DEFINED:return "USER_DEFINED";
    case EProjectionEnum::PROJECTION_SPHEROID_CLARKE_1866:return "CLARKE_1866";
    case EProjectionEnum::PROJECTION_SPHEROID_CLARKE_1880:return "CLARKE_1880";
    case EProjectionEnum::PROJECTION_SPHEROID_BESSEL:return "BESSEL";
    case EProjectionEnum::PROJECTION_SPHEROID_INTERNATIONAL_1967:return "INTERNATIONAL_1967";
    case EProjectionEnum::PROJECTION_SPHEROID_INTERNATIONAL_1909:return "INTERNATIONAL_1909";
    case EProjectionEnum::PROJECTION_SPHEROID_WGS_72:return "WGS_72";
    case EProjectionEnum::PROJECTION_SPHEROID_EVEREST:return "EVEREST";
    case EProjectionEnum::PROJECTION_SPHEROID_WGS_66:return "WGS_66";
    case EProjectionEnum::PROJECTION_SPHEROID_GRS_1980:return "GRS_80";
    case EProjectionEnum::PROJECTION_SPHEROID_AIRY:return "AIRY";
    case EProjectionEnum::PROJECTION_SPHEROID_MODIFIED_EVEREST:return "MODIFIED_EVEREST";
    case EProjectionEnum::PROJECTION_SPHEROID_MODIFIED_AIRY:return "MODIFIED_AIRY";
    case EProjectionEnum::PROJECTION_SPHEROID_WGS_84:return "WGS_84";
    case EProjectionEnum::PROJECTION_SPHEROID_SOUTHERN_ASIA:return "SOUTHERN_ASIA";
    case EProjectionEnum::PROJECTION_SPHEROID_AUSTRALIAN_NATIONAL:return "AUSTRALIAN_NATIONAL";
    case EProjectionEnum::PROJECTION_SPHEROID_KRASSOVSKY:return "KRASSOVSKY";
    case EProjectionEnum::PROJECTION_SPHEROID_HOUGH:return "HOUGH";
    case EProjectionEnum::PROJECTION_SPHEROID_MERCURY_1960:return "MERCURY_1960";
    case EProjectionEnum::PROJECTION_SPHEROID_MODIFIED_MERCURY:return "MODIFIED_MERCURY";
    case EProjectionEnum::PROJECTION_SPHEROID_SPHERE_RADIUS:return "SPHERE_RADIUS";
    case EProjectionEnum::PROJECTION_SPHEROID_BESSEL_1841:return "BESSEL_1841";
    case EProjectionEnum::PROJECTION_SPHEROID_EVEREST_SABAH:return "EVEREST_SABAH";
    case EProjectionEnum::PROJECTION_SPHEROID_EVEREST_INDIA_1956:return "EVEREST_INDIA_1956";
    case EProjectionEnum::PROJECTION_SPHEROID_EVEREST_MALAY_1969:return "EVEREST_MALAY_1969";
    case EProjectionEnum::PROJECTION_SPHEROID_EVEREST_MALAY_1948:return "EVEREST_MALAY_1948";
    case EProjectionEnum::PROJECTION_SPHEROID_EVEREST_PAKISTAN:return "EVEREST_PAKISTAN";
    case EProjectionEnum::PROJECTION_SPHEROID_HAYFORD:return "HAYFORD";
    case EProjectionEnum::PROJECTION_SPHEROID_HELMERT_1906:return "HELMERT_1906";
    case EProjectionEnum::PROJECTION_SPHEROID_INDONESIAN_1974:return "INDONESIAN_1974";
    case EProjectionEnum::PROJECTION_SPHEROID_SOUTH_AMERICAN_1969:return "SOUTH_AMERICAN_1969";
    case EProjectionEnum::PROJECTION_SPHEROID_WGS_60:return "WGS_60";
    case EProjectionEnum::PROJECTION_SPHEROID_MODIS_SPHERE:return "MODIS_SPHERE";

    }
    return "";
}

int EProjectionEnum::GetSpheroidAxis(EProjectionEnum::EProjectionSpheroid projectionSpheroid, double &major, double &minor)
{
    int v = static_cast<int> (projectionSpheroid);
    int max = static_cast<int> (EProjectionEnum::PROJECTION_SPHEROID_COUNT);
    if (v >= 0 && v < max)
    {
        major = EProjectionSpheroidAxis[v][0];
        minor = EProjectionSpheroidAxis[v][1];
    }
    else
    {
        return 1;
    }
    return 0;
}

} // namespace ultra
