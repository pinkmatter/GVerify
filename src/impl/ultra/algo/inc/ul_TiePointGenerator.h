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

#include "ul_MatrixArray.h"
#include "ul_ChipsGen.h"
#include "ul_DigitalImageCorrelator.h"
#include "ul_Int_TiePointGenerator.h"

namespace ultra
{

/*
 * This class is spread over a couple of cpp files, this was done for ease of use, debug and reading
 * View the internals folder
 */

class CTiePointGenerator : public ITiePointGenerator
{
public:

    struct SContext
    {

        struct SScene
        {
            std::string pathToImage;
            int bandNumber;

            //methods
            SScene();
            SScene(const CTiePointGenerator::SContext::SScene & r);
            ~SScene();

            CTiePointGenerator::SContext::SScene &operator=(const CTiePointGenerator::SContext::SScene & r);
        };
    private:
        bool m_useNullValue;
        float m_nullValue;
        bool m_mayContainNullValues;

    public:

        unsigned long threadCount;
        unsigned long chipSizeMinimum;
        unsigned long searchWindowSize;

        ECorrelationType typeOfChipCorrelationTechnique;
        // this must usually be twice the 'chipSizeMinimum' value
        unsigned long chipGenerationGridSize;
        CVector<EChips::EChipType> chipGeneratorMethods;
        CVector<SPair<double> > fixedLocationChips;
        std::string fixedLocationChipProj4Str;

        // value between 0~1 default is 0.75
        double correlationThreshold;

        bool outerHullRejectGcps;

        SPair<double> inputOffsetShiftInPixels;

        SScene inputScene;
        SScene referenceScene;

        //methods
        SContext();
        SContext(const CTiePointGenerator::SContext & r);
        ~SContext();

        // All these relate to both reference and input, the null value is ignored by
        // the correlators
        void setNullValue(float val);
        float getNullValue() const;
        void disableNullValue();
        bool useNullValue() const;
        bool getMayContainNullValues() const;
        void setMayContainNullValues(bool mayContainNullValues);

        CTiePointGenerator::SContext &operator=(const CTiePointGenerator::SContext & r);
    };

private:
    // private data containers

    struct SInnerContext
    {

        struct SSceneMetadata
        {
            SPair<double> origin;
            SPair<double> gsd;
            EImage::EImageFormat imageEnumType;
            SSize imageSize;
            int bpp;

            SSceneMetadata();
            SSceneMetadata(const CTiePointGenerator::SInnerContext::SSceneMetadata & r);
            ~SSceneMetadata();

            CTiePointGenerator::SInnerContext::SSceneMetadata &operator=(const CTiePointGenerator::SInnerContext::SSceneMetadata &r);
        };

        const SContext *innerContext;

        CMatrix<float> images[2];

        SSceneMetadata referenceSceneMetadata;
        SSceneMetadata inputSceneMetadata;

        SInnerContext();
        ~SInnerContext();
        SInnerContext(const SInnerContext & r);
    };

private:
    static const int REF_IMG_INDEX;
    static const int INPUT_IMG_INDEX;
    SInnerContext m_context;

    bool isContextSceneOK(const CTiePointGenerator::SContext::SScene &context);

    int innerLoadImgMetadata(
                             const CTiePointGenerator::SContext::SScene &context,
                             CTiePointGenerator::SInnerContext::SSceneMetadata &sceneMetadata
                             );
    int LoadImgMetadata();
    int LoadTwoImages();
    int LoadCompleteImages(CMatrixArray<float> &refImages, SImageMetadata &refMetadata,
                           CMatrixArray<float> &inputImages, SImageMetadata &inputMetadata);
    int LoadOneImageFull(int bandNumber);
    int GenerateChips(const std::string pathToImageWhereChipsAreLoadedFrom,
                      CMatrix<float> &image, CVector<CChip<float> > &chipVector,
                      EChips::EChipType chipType);
    //chip generation systems START
    int (CTiePointGenerator::*m_fp_generateChips)(CMatrix<float> &, CVector<CChip<float> > &);
    int GenerateSobelChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector);
    int GenerateEvenChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector);
    int GenerateHarrisChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector);
    int GenerateFixedLocationChips(CMatrix<float> &image, CVector<CChip<float> > &chipVector);
    int GenerateChips(CChipsGen<float> *chipGenner, CChipsGen<float>::CChipsGenContext *context,
                      CMatrix<float> &image, CVector<CChip<float> > &chipVector);
    int TranslateChipCoordinatesToMap(CVector<CChip<float> > &chipVector);
    //chip generation systems END

    //correlation START
    int GenerateWorldMatrix(
                            CMatrix<SPair<double> > &worldMatrix,
                            const SSize &size,
                            const SPair<double> &pixelGroundSamplingDistance,
                            const SPair<double> &origin,
                            const SPair<double> &subImageShift
                            );
    int Correlate(
                  CMatrix<SPair<double> > &worldMatrix,
                  CVector<CChip<float> > &chipVector,
                  CVector<SChipCorrelationResult> &result
                  );
    int StartCorrelation(CVector<CChip<float> > &chipVector, CVector<SChipCorrelationResult> &result);
    //correlation END

    int MainLoop(CVector<SChipCorrelationResult> &finalResult);
    void RemoveDuplicateGcps(CVector<SChipCorrelationResult> &finalResult);
    int HullRejectGcps(CVector<SChipCorrelationResult> &result);
public:

    CTiePointGenerator(const CTiePointGenerator::SContext *context);
    virtual ~CTiePointGenerator();

    bool isContextOK(const CTiePointGenerator::SContext *context);

    virtual int Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists = nullptr) override;
};

} //namespace ultra
