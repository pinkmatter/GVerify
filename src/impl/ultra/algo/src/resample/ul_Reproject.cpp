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

#include "ul_Reproject.h"

#include <ul_Proj4Projection.h>
#include <ul_ImageMetadataObjects.h>

namespace ultra
{

CReproject::CReproject()
{

}

CReproject::~CReproject()
{

}

template<class T>
int CReproject::InputOkTest(const CMatrixArray<T> &inputImages) const
{
    if (inputImages.size() == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image array is of size 0");
        return 1;
    }
    SSize size = inputImages[0].getSize();

    if (size.containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input images contain sizes of zero");
        return 1;
    }

    for (unsigned long t = 1; t < inputImages.size(); t++)
    {
        if (size != inputImages[t].getSize())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image array contains different size images");
            return 1;
        }
    }
    return 0;
}

template<class T>
int CReproject::innerGetTranslationMap(const SSize &inputImageSize,
                                       const std::string &inputProj4, const std::string &targetProj4,
                                       const CVector<double> &affineGeoTransform,
                                       const SPair<double> &targetGsd, SPair<double> &outputOrigin,
                                       CMatrix<SPair<T> > &outputTranslationMap, bool enableLogger)
{
    std::unique_ptr<CProj4Projection> proj4InToOut(new CProj4Projection(inputProj4, targetProj4));
    std::unique_ptr<CProj4Projection> proj4OutToIn(new CProj4Projection(targetProj4, inputProj4));
    SPair<double> inPt, outPt;
    SPair<double> ul, lr;

    inPt.x = affineGeoTransform[0];
    inPt.y = affineGeoTransform[3];

    if (proj4InToOut->Project(inPt, outPt) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CProj4Projection::Project()");
        return 1;
    }

    lr.r = ul.r = outPt.r;
    lr.c = ul.c = outPt.c;

    CVector<double> vecX;
    CVector<double> vecY;

    vecX.resize(inputImageSize.col);
    vecY.resize(inputImageSize.col);

    int per = -1;
    SPair<double> sampleLine;
    for (unsigned long r = 0; r < inputImageSize.row; r++)
    {
        for (unsigned long c = 0; c < inputImageSize.col; c++)
        {
            sampleLine.r = r;
            sampleLine.c = c;
            sampleLine = SImageMetadata::getMapCoord(affineGeoTransform, sampleLine);
            vecX[c] = sampleLine.x;
            vecY[c] = sampleLine.y;
        }

        if (proj4InToOut->Project(vecX, vecY) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CProj4Projection::Project()");
            return 1;
        }

        for (unsigned long c = 0; c < inputImageSize.col; c++)
        {
            if (vecX[c] < ul.c)
                ul.c = vecX[c];
            if (vecX[c] > lr.c)
                lr.c = vecX[c];

            if (vecY[c] > ul.r)
                ul.r = vecY[c];
            if (vecY[c] < lr.r)
                lr.r = vecY[c];
        }

        if (enableLogger)
        {
            if (per != (r * 100) / inputImageSize.row)
            {
                per = (r * 100) / inputImageSize.row;
                CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Projecting " + toString(per) + "%     \r");
            }
        }
    }
    if (enableLogger)
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Projecting 100%   ");

    outputOrigin = ul;
    SPair<double> outSizeD = (((lr - ul) / targetGsd) + 1).roundValues();

    outputTranslationMap.resize(outSizeD.getSizeType());
    SSize outSize = outputTranslationMap.getSize();
    per = -1;
    vecX.resize(outSize.col);
    vecY.resize(outSize.col);
    for (unsigned long r = 0; r < outSize.row; r++)
    {
        for (unsigned long c = 0; c < outSize.col; c++)
        {
            vecX[c] = targetGsd.c * c + outputOrigin.c;
            vecY[c] = targetGsd.r * r + outputOrigin.r;
        }

        if (proj4OutToIn->Project(vecX, vecY) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CProj4Projection::Project()");
            return 1;
        }

        // to get sample line
        for (unsigned long c = 0; c < outSize.col; c++)
        {
            sampleLine.x = vecX[c];
            sampleLine.y = vecY[c];
            sampleLine = SImageMetadata::getSampleLine(affineGeoTransform, sampleLine);
            outputTranslationMap[r][c].x = (T) sampleLine.x;
            outputTranslationMap[r][c].y = (T) sampleLine.y;
        }
        if (enableLogger)
        {
            if (per != (r * 100) / outSize.row)
            {
                per = (r * 100) / outSize.row;
                CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Generate translation map " + toString(per) + "%     \r");
            }
        }
    }
    if (enableLogger)
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Generate translation map 100%   ");

    return 0;
}

template<class T>
int CReproject::innerProject(const CMatrixArray<T> &inputImages,
                             const std::string &inputProj4, const std::string &targetProj4,
                             const CVector<double> &affineGeoTransform,
                             const SPair<double> &targetGsd, SPair<double> &outputOrigin,
                             CMatrixArray<T> &outputImages, EResamplerEnum::EResampleType resampleType,
                             T srcNullValue, T trgNullValue, bool enableLogger)
{
    if (InputOkTest(inputImages) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from InputOkTest()");
        return 1;
    }

    outputImages.resize(inputImages.size());
    SSize inSize = inputImages[0].getSize();

    CMatrix<SPair<float> > outputTranslationMap;
    if (innerGetTranslationMap<float>(inSize, inputProj4, targetProj4, affineGeoTransform,
        targetGsd, outputOrigin, outputTranslationMap, enableLogger) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerGetTranslationMap()");
        return 1;
    }

    for (unsigned long t = 0; t < inputImages.size(); t++)
    {
        if (enableLogger)
        {
            if (CResampler<true>::Resample<T, float>(inputImages[t], outputImages[t], outputTranslationMap, resampleType, &srcNullValue, &trgNullValue) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CResampler::Resample");
                return 1;
            }
        }
        else
        {
            if (CResampler<false>::Resample<T, float>(inputImages[t], outputImages[t], outputTranslationMap, resampleType, &srcNullValue, &trgNullValue) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CResampler::Resample");
                return 1;
            }
        }
    }

    return 0;
}


int CReproject::Project(const CMatrixArray<float> &inputImages,
                        const std::string &inputProj4, const std::string &targetProj4,
                        const CVector<double> &affineGeoTransform,
                        const SPair<double> &targetGsd, SPair<double> &outputOrigin,
                        CMatrixArray<float> &outputImages, EResamplerEnum::EResampleType resampleType,
                        float srcNullValue, float trgNullValue, bool enableLogger)
{
    std::unique_ptr<CReproject> pr(new CReproject());
    return pr->innerProject<float>(inputImages, inputProj4, targetProj4, affineGeoTransform, targetGsd, outputOrigin, outputImages, resampleType, srcNullValue, trgNullValue, enableLogger);
}

} // namespace ultra
