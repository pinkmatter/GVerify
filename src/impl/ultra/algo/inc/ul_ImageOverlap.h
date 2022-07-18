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

#include <ul_Vector.h>
#include <ul_KeyValue.h>
#include <ul_ImageLoader.h>
#include <ul_Size.h>
namespace ultra
{

class CImageOverlap
{
public:

    enum EOverlapType
    {
        MIN_BOX = 0,
        MAX_BOX,
        OVERLAP_TYPE_COUNT
    };
private:

    struct SContext
    {
        SKeyValue<std::string, std::string> inputOutput;
        SImageMetadata metadata;
        std::string proj4;
        SPair<long> subUl, subImageSize;
        SPair<long> paddUl, paddLr;

        SContext();
        SContext(const CImageOverlap::SContext &r);
        ~SContext();
        CImageOverlap::SContext &operator=(const CImageOverlap::SContext &r);
        bool contextMatches(const CImageOverlap::SContext &r);
        int PopulateSubImageCoordinates(const SKeyValue<SPair<double>, SPair<double> > &ul_lr);
        bool checkIfOverlapExists() const;
    };
    CVector<SContext> m_inputOutputImageContext;
    CImageOverlap::EOverlapType m_type;

    CImageOverlap();
    int DoPadding(const CImageOverlap::SContext &context, float paddVal, CMatrixArray<float> &image);
    int innerOverlapImages(CVector<SKeyValue<std::string, std::string> > &inputOutputImagePaths, CImageOverlap::EOverlapType type, bool &overlapExists, float paddVal);
    int Init(CVector<SKeyValue<std::string, std::string> > &inputOutputImagePaths, CImageOverlap::EOverlapType type);
    int MinBox();
    int MaxBox();

public:
    virtual ~CImageOverlap();
    static int OverlapImages(CVector<SKeyValue<std::string, std::string> > &inputOutputImagePaths, CImageOverlap::EOverlapType type, bool &overlapExists, float paddVal = 0);
};

} // namespace ultra
