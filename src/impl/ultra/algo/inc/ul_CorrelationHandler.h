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

#include "ul_Chips.h"
#include "ul_DigitalImageCorrelator.h"
#include <ul_Pair.h>
#include <ul_UltraThread.h>
#include <ul_AtomicLong.h>

namespace ultra
{

class CCorrelationHandler
{
private:
    void *m_CCorrelationHandler_corrVecThreads;
    const CMatrix<float> *m_inputImage;
    const CMatrix<SPair<double> > *m_worldMatrix;
    SPair<double> m_pixelGroundSamplingDistance;
    SPair<double> m_originChipToInputDiv;
    unsigned long m_searchWindowSize;
    ECorrelationType m_correlationMethod;

    CVector<CVector<CChip<float> > > m_inputChips;
    CVector<CVector<SChipCorrelationResult> > m_results;
    unsigned int m_maxThreads;
    bool m_canStart;
    float m_correlationThreshold;
    CAtomicLong m_atLong;

    int Init(const CVector<CChip<float> > &inputChips);
    int cleanup();
    int StartThreads(std::shared_ptr<CThreadLock> lock);

    bool m_useNullValue;
    float m_nullValue;
    bool m_mayContainNullValues;

public:
    CCorrelationHandler(unsigned int threadCount, float correlationThreshold, bool useNullValue, float nullValue, bool mayContainNullValues);
    virtual ~CCorrelationHandler();

    int LoadData(
                 const CMatrix<float> &inputImage,
                 const CVector<CChip<float> > &inputChips,
                 const CMatrix<SPair<double> > &worldMatrix,
                 const SPair<double> &pixelGroundSamplingDistance,
                 unsigned long searchWindowSize,
                 const SPair<double> &originChipToInputDiv,
                 ECorrelationType correlationMethod = ECorrelationType::CCOEFF_NORM
                 );
    int Correlate(std::shared_ptr<CThreadLock> lock);
    bool isBusy();
    int GetResults(CVector<SChipCorrelationResult> &results);
};

} // namespace ultra
