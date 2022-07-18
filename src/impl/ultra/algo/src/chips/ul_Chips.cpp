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

#include "ul_Chips.h"

namespace ultra
{

std::string EChips::chipGenTypeToStr(EChips::EChipType type)
{
    switch (type)
    {
    case EChips::CHIP_GEN_SOBEL:return "SOBEL";
    case EChips::CHIP_GEN_EVEN:return "EVEN";
    case EChips::CHIP_GEN_HARRIS:return "HARRIS";
    case EChips::CHIP_GEN_FIXED_LOCATION: return "FIXED_LOCATION";
    }
    return "";
}

EChips::EChipType EChips::strToChipGenType(const std::string &type)
{
    std::string s = toUpper(type);

    if (s == "SOBEL")return EChips::CHIP_GEN_SOBEL;
    if (s == "EVEN")return EChips::CHIP_GEN_EVEN;
    if (s == "HARRIS")return EChips::CHIP_GEN_HARRIS;
    if (s == "FIXED_LOCATION")return EChips::CHIP_GEN_FIXED_LOCATION;

    return EChips::CHIP_GEN_TYPE_COUNT;
}

SChipCorrelationResult::SChipCorrelationResult()
{
    corrCoefficient = 0;
    correlationResultIsGood = false;
}

SChipCorrelationResult::SChipCorrelationResult(const SChipCorrelationResult &r)
{
    correlationResultIsGood = r.correlationResultIsGood;
    min = r.min;
    max = r.max;
    chip = r.chip;
    corrCoefficient = r.corrCoefficient;
    for (int x = 0; x < 4; x++)
        newBoundingCoordinate[x] = r.newBoundingCoordinate[x];
    newMidCoordinate = r.newMidCoordinate;
}

SChipCorrelationResult::~SChipCorrelationResult()
{

}

SChipCorrelationResult &SChipCorrelationResult::operator=(const SChipCorrelationResult &r)
{
    if (this == &r)
        return *this;
    correlationResultIsGood = r.correlationResultIsGood;
    min = r.min;
    max = r.max;
    chip = r.chip;
    corrCoefficient = r.corrCoefficient;

    for (int x = 0; x < 4; x++)
        newBoundingCoordinate[x] = r.newBoundingCoordinate[x];
    newMidCoordinate = r.newMidCoordinate;

    return *this;
}

bool SChipCorrelationResult::isGoodResult() const
{
    return correlationResultIsGood;
}

SDisparityPoint::SDisparityPoint()
{

}

SDisparityPoint::SDisparityPoint(SPair<double> point1, SPair<double> point2)
{
    p1 = point1;
    p2 = point2;
}

SDisparityPoint::~SDisparityPoint()
{

}

SDisparityPoint &SDisparityPoint::operator=(const SDisparityPoint &r)
{
    if (this == &r)
        return *this;
    p1 = r.p1;
    p2 = r.p2;

    return *this;
}

int SDisparityPoint::getBoundingArea(SPair<double> &min, SPair<double> &max)
{
    min = p1;
    max = p1;

    if (p2.c < min.c)
        min.c = p2.c;
    if (p2.r < min.r)
        min.r = p2.r;

    if (p2.c > max.c)
        max.c = p2.c;
    if (p2.r > max.r)
        max.r = p2.r;

    return 0;
}

SDisparityInfo::SDisparityInfo()
{

}

SDisparityInfo::~SDisparityInfo()
{

}

SDisparityInfo &SDisparityInfo::operator=(const SDisparityInfo &r)
{
    if (this == &r)
        return *this;
    disparityPairs = r.disparityPairs;
    zone = r.zone;
    PCScode = r.PCScode;

    return *this;
}

} //namespace utlra
