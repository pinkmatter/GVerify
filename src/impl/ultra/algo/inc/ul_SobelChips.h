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
#include "ul_ChipsGen.h"
#include "ul_Transform.h"
#include "ul_GaussianFilterTransform.h"
#include "ul_SobelFilterTransform.h"

namespace ultra
{

class CSobelChips : public CChipsGen<float>
{
public:

    class CSobelChipsContext : public CChipsGen<float>::CChipsGenContext
    {
    public:
        int chipWidth;

        CSobelChipsContext()
        {

        }

        virtual ~CSobelChipsContext()
        {

        }
    };
private:
    int m_chipWidth;
    int m_chipOffset;
    CConvKernel<float> m_gaussianKernel;
    CTransform<float, float> *m_transformer;
    CTransform<float, float> *m_smoother;
    CTransformContext *m_smootherCtx;
    CMatrix<float> m_tempPhase;

    int Init(CChipsGen<float>::CChipsGenContext *context);
    int ApplySobel(const CMatrix<float> &im, CMatrix<float> &sobel_grad, CMatrix<float> &sobel_phase);
    int getThreshold(const CMatrix<float>& im, float& th);
    int PostSmoothing(const CMatrix<float> &threshed, CMatrix<float> &smoothed);
    int isHighestInCenter(const CMatrix<float> &sample, bool &res, unsigned long i, unsigned long j, const SSize &tileSize);
    int isPhaseErratic(const CMatrix<float> &sample, bool &res, unsigned long i, unsigned long j, const SSize &tileSize);
    int isPointUsable(const CMatrix<float> &magSubset, CMatrix<float> &phaseSubset, bool &res, unsigned long i, unsigned long j, const SSize &tileSize);
    int ProcessChips(const CMatrix<float> &im, const CMatrix<float> &smoothed, CMatrix<float> &sobel_phase, CVector<CChip<float> >& output);
protected:
    virtual int innerGenerate(CChipsGen<float>::CChipsGenContext *context) override;
    virtual int SplitImagesForThreads() override;

public:
    CSobelChips();
    virtual ~CSobelChips();
};

} //namespace ultra
