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

#include "ul_Matrix.h"
#include "ul_Logger.h"
#include "ul_ImageSaver.h"
#include <ul_2D.h>
#include <ul_Pair.h>

namespace ultra
{

struct EChips
{

    enum EChipType
    {
        CHIP_GEN_SOBEL = 0,
        CHIP_GEN_EVEN,
        CHIP_GEN_HARRIS,
        CHIP_GEN_FIXED_LOCATION,
        CHIP_GEN_TYPE_COUNT
    };

    static std::string chipGenTypeToStr(EChips::EChipType type);
    static EChips::EChipType strToChipGenType(const std::string &type);
};

template<class T>
class CChip
{
public:
    /**
     * 0 = UL
     * 1 = LL
     * 2 = LR
     * 3 = UR
     */

    std::string chipType;

    unsigned long chipId;
    SPair<double> boundingCoordinate[4];
    SPair<double> midCoordinate;
    CMatrix<T> chipData;
    CMatrix<SPair<double> > worldMatrix;

    CChip()
    {
        chipType = "BASE_CHIP_TYPE";
    }

    CChip(const CChip<T> &r)
    {
        chipType = r.chipType;
        chipId = r.chipId;
        for (unsigned long t = 0; t < 4; t++)
            boundingCoordinate[t] = r.boundingCoordinate[t];
        midCoordinate = r.midCoordinate;
        chipData = r.chipData;
        worldMatrix = r.worldMatrix;
    }

    virtual ~CChip()
    {
        worldMatrix.clear();
        chipData.clear();
    }

    CChip<T> &operator=(const CChip<T> &r)
    {
        chipType = r.chipType;
        chipId = r.chipId;
        for (unsigned long t = 0; t < 4; t++)
            boundingCoordinate[t] = r.boundingCoordinate[t];
        midCoordinate = r.midCoordinate;
        chipData = r.chipData;
        worldMatrix = r.worldMatrix;

        return *this;
    }

    bool operator==(const CChip<T> &r) const
    {
        if (this == &r)
            return true;
        return chipType == r.chipType && chipId == r.chipId &&
                boundingCoordinate[0] == r.boundingCoordinate[0] && boundingCoordinate[1] == r.boundingCoordinate[1]
                && boundingCoordinate[2] == r.boundingCoordinate[2] && boundingCoordinate[3] == r.boundingCoordinate[3] &&
                midCoordinate == r.midCoordinate && chipData == r.chipData && worldMatrix == r.worldMatrix;
    }
};

struct SChipCorrelationResult
{
public:
    CChip<float> chip;

    SPair<double> newBoundingCoordinate[4];
    SPair<double> newMidCoordinate;

    bool correlationResultIsGood;
    double corrCoefficient;
    SSize min, max;

    SChipCorrelationResult();
    SChipCorrelationResult(const SChipCorrelationResult & r);
    ~SChipCorrelationResult();
    SChipCorrelationResult &operator=(const SChipCorrelationResult & r);
    bool isGoodResult() const;
};

struct SDisparityPoint
{
    SPair<double> p1, p2;

    SDisparityPoint();
    SDisparityPoint(SPair<double> point1, SPair<double> point2);
    ~SDisparityPoint();
    SDisparityPoint &operator=(const SDisparityPoint & r);
    int getBoundingArea(SPair<double> &min, SPair<double> &max);

};

struct SDisparityInfo
{
    CVector<SDisparityPoint> disparityPairs;
    int zone;
    int PCScode;

    SDisparityInfo();
    ~SDisparityInfo();
    SDisparityInfo &operator=(const SDisparityInfo & r);
};

} //namespace ultra
