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

namespace ultra
{

struct EProjectionEnum
{
public:

    enum EProjectionOrientation
    {
        NUP = 0,
        NOM,
        PROJECTION_ORIENTATION_COUNT
    };

    enum EProjectionOrigin
    {
        UL = 0,
        CENTER,
        PROJECTION_ORIGIN_COUNT
    };

    enum EProjectionType
    {
        PROJECTION_GEO = 0,
        PROJECTION_UTM,
        PROJECTION_SPCS,
        PROJECTION_ALBERS,
        PROJECTION_LAMCC,
        PROJECTION_MERCAT,
        PROJECTION_PS,
        PROJECTION_POLYC,
        PROJECTION_EQUIDC,
        PROJECTION_TM,
        PROJECTION_STEREO,
        PROJECTION_LAMAZ,
        PROJECTION_AZMEQD,
        PROJECTION_GNOMON,
        PROJECTION_ORTHO,
        PROJECTION_GVNSP,
        PROJECTION_SNSOID,
        PROJECTION_EQRECT,
        PROJECTION_MILLER,
        PROJECTION_VGRINT,
        PROJECTION_HOM,
        PROJECTION_ROBIN,
        PROJECTION_SOM,
        PROJECTION_ALASKA,
        PROJECTION_GOOD,
        PROJECTION_MOLL,
        PROJECTION_IMOLL,
        PROJECTION_HAMMER,
        PROJECTION_WAGIV,
        PROJECTION_WAGVII,
        PROJECTION_OBEQA,
        PROJECTION_TYPE_COUNT
    };

    enum EProjectionUnits
    {
        PROJECTION_UNIT_RADIAN = 0,
        PROJECTION_UNIT_FEET,
        PROJECTION_UNIT_METER,
        PROJECTION_UNIT_SECOND,
        PROJECTION_UNIT_DEGREE,
        PROJECTION_UNIT_INT_FEET,
        PROJECTION_UNIT_COUNT
    };

    enum EProjectionDatum
    {
        PROJECTION_DATUM_GDA94 = 0,
        PROJECTION_DATUM_WGS84,
        PROJECTION_DATUM_COUNT
    };

    enum EProjectionGcsDatum
    {
        PROJECTION_GCS_DATUM_GRS80,
        PROJECTION_GCS_DATUM_WGS84,
        PROJECTION_GCS_DATUM_COUNT
    };

    enum EProjectionSpheroid
    {
        PROJECTION_SPHEROID_USER_DEFINED = -1,
        PROJECTION_SPHEROID_CLARKE_1866 = 0, /* 0: Clarke 1866 */
        PROJECTION_SPHEROID_CLARKE_1880, /* 1: Clarke 1880 */
        PROJECTION_SPHEROID_BESSEL, /* 2: Bessel */
        PROJECTION_SPHEROID_INTERNATIONAL_1967, /* 3: International 1967 */
        PROJECTION_SPHEROID_INTERNATIONAL_1909, /* 4: International 1909 */
        PROJECTION_SPHEROID_WGS_72, /* 5: WGS 72 */
        PROJECTION_SPHEROID_EVEREST, /* 6: Everest */
        PROJECTION_SPHEROID_WGS_66, /* 7: WGS 66 */
        PROJECTION_SPHEROID_GRS_1980, /* 8: GRS 1980 */
        PROJECTION_SPHEROID_AIRY, /* 9: Airy */
        PROJECTION_SPHEROID_MODIFIED_EVEREST, /* 10: Modified Everest */
        PROJECTION_SPHEROID_MODIFIED_AIRY, /* 11: Modified Airy */
        PROJECTION_SPHEROID_WGS_84, /* 12: WGS 84 */
        PROJECTION_SPHEROID_SOUTHERN_ASIA, /* 13: Southeast Asia */
        PROJECTION_SPHEROID_AUSTRALIAN_NATIONAL, /* 14: Australian National */
        PROJECTION_SPHEROID_KRASSOVSKY, /* 15: Krassovsky */
        PROJECTION_SPHEROID_HOUGH, /* 16: Hough */
        PROJECTION_SPHEROID_MERCURY_1960, /* 17: Mercury 1960 */
        PROJECTION_SPHEROID_MODIFIED_MERCURY, /* 18: Modified Mercury 1968 */
        PROJECTION_SPHEROID_SPHERE_RADIUS, /* 19: Sphere of Radius 6370997 meters*/
        PROJECTION_SPHEROID_BESSEL_1841, /* 20: Bessel 1841(Namibia) */
        PROJECTION_SPHEROID_EVEREST_SABAH, /* 21: Everest (Sabah & Sarawak) */
        PROJECTION_SPHEROID_EVEREST_INDIA_1956, /* 22: Everest (India 1956) */
        PROJECTION_SPHEROID_EVEREST_MALAY_1969, /* 23: Everest (Malaysia 1969) */
        PROJECTION_SPHEROID_EVEREST_MALAY_1948, /* 24: Everest (Malay & Singapr 1948)*/
        PROJECTION_SPHEROID_EVEREST_PAKISTAN, /* 25: Everest (Pakistan) */
        PROJECTION_SPHEROID_HAYFORD, /* 26: Hayford */
        PROJECTION_SPHEROID_HELMERT_1906, /* 27: Helmert 1906 */
        PROJECTION_SPHEROID_INDONESIAN_1974, /* 28: Indonesian 1974 */
        PROJECTION_SPHEROID_SOUTH_AMERICAN_1969, /* 29: South American 1969 */
        PROJECTION_SPHEROID_WGS_60, /* 30: WGS 60 */
        PROJECTION_SPHEROID_MODIS_SPHERE, /* 31: MODIS Sphere */
        PROJECTION_SPHEROID_COUNT
    };

private:
    static const char* EProjectionTypeProj4Str[EProjectionEnum::PROJECTION_TYPE_COUNT];
    static const char* EProjectionTypeUltraStr[EProjectionEnum::PROJECTION_TYPE_COUNT];
    static const double EProjectionSpheroidAxis[EProjectionEnum::PROJECTION_SPHEROID_COUNT][2];

public:
    static int GetUltraProjCode(EProjectionEnum::EProjectionType proj, std::string &strName);
    static int ProjCodeToProj4String(EProjectionEnum::EProjectionType proj, std::string &proj4Str);
    static int ProjUnitsToProj4String(EProjectionEnum::EProjectionUnits unit, std::string &proj4Str);
    static int ProjSpheriodToProj4String(EProjectionEnum::EProjectionSpheroid spheriod, std::string &proj4Str);
    static int ProjDatumToProj4String(EProjectionEnum::EProjectionDatum datum, std::string &proj4Str);
    static std::string projectionCodeToStr(EProjectionEnum::EProjectionType proj);
    static int projectionCodeToInt(EProjectionEnum::EProjectionType proj);
    static EProjectionEnum::EProjectionType intToProjectionCode(int projInt);
    static EProjectionEnum::EProjectionType strToProjectionCode(std::string projStr);
    static std::string projectionDatumCodeToStr(EProjectionEnum::EProjectionDatum datum);
    static EProjectionEnum::EProjectionDatum strToProjectionDatumCode(std::string datumStr);
    static std::string projectionUnitsToStr(EProjectionEnum::EProjectionUnits units);
    static std::string projectionUnitsToStrShort(EProjectionEnum::EProjectionUnits units);
    static EProjectionEnum::EProjectionUnits intToProjectionUnits(int unitInt);
    static EProjectionEnum::EProjectionUnits strToProjectionUnits(std::string unitsStr);
    static EProjectionEnum::EProjectionOrientation strToProjectionOrientation(std::string orientationStr);
    static std::string projectionOrientationToStr(EProjectionEnum::EProjectionOrientation orientation);
    static EProjectionEnum::EProjectionOrigin strToProjectionOrigin(std::string projectionOriginStr);
    static std::string projectionOriginToStr(EProjectionEnum::EProjectionOrigin projectionOrigin);
    static EProjectionEnum::EProjectionSpheroid strToProjectionSpheroidCode(std::string projectionSpheroidStr);
    static std::string projectionSpheroidCodeToStr(EProjectionEnum::EProjectionSpheroid projectionSpheroid);
    static EProjectionEnum::EProjectionSpheroid intToProjectionSpheroidCode(int spheroidInt);
    static int projectionSpheroidCodeToInt(EProjectionEnum::EProjectionSpheroid projectionSpheroid);

    static int GetSpheroidAxis(EProjectionEnum::EProjectionSpheroid projectionSpheroid, double &major, double &minor);
};

} // namespace ultra
