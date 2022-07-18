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

#include <ul_Resampler.h>
#include <ul_MatrixArray.h>
#include <ul_KeyValue.h>

namespace ultra
{

class CReproject
{
private:

    template<class T>
    struct SResampleGridData
    {
        SSize size;
        CVector<double> vecX, vecY;
        CMatrix<SPair<double> > resampleGrid;
        CMatrix<T> outDataBlock;
    };

    template<class T>
    struct SSrcPixelBlock
    {
        SSize size;
        CMatrix<T> inDataBlock;
    };

private:
    CReproject();

    template<class T>
    int InputOkTest(const CMatrixArray<T> &inputImages) const;
    template<class T>
    int innerProject(const CMatrixArray<T> &inputImages,
                     const std::string &inputProj4, const std::string &targetProj4,
                     const CVector<double> &affineGeoTransform,
                     const SPair<double> &targetGsd, SPair<double> &outputOrigin,
                     CMatrixArray<T> &outputImages, EResamplerEnum::EResampleType resampleType,
                     T srcNullValue, T trgNullValue, bool enableLogger);
    template<class T>
    int innerGetTranslationMap(const SSize &inputImageSize,
                               const std::string &inputProj4, const std::string &targetProj4,
                               const CVector<double> &affineGeoTransform,
                               const SPair<double> &targetGsd, SPair<double> &outputOrigin,
                               CMatrix<SPair<T> > &outputTranslationMap, bool enableLogger);

public:
    virtual ~CReproject();

    static int Project(const CMatrixArray<float> &inputImages,
                       const std::string &inputProj4, const std::string &targetProj4,
                       const CVector<double> &affineGeoTransform,
                       const SPair<double> &targetGsd, SPair<double> &outputOrigin,
                       CMatrixArray<float> &outputImages, EResamplerEnum::EResampleType resampleType = EResamplerEnum::RESAMPLE_TYPE_CI,
                       float srcNullValue = 0, float trgNullValue = 0, bool enableLogger = true);

};

} // namespace ultra
