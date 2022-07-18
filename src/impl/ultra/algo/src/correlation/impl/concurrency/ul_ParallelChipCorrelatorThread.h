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
#include "ul_UltraThread.h"
#include "ul_MapModelToMatrix.h"
#include "ul_AtomicLong.h"
#include "ul_SubPixelCorrelator.h"

namespace ultra
{
namespace __ultra_internal
{

class CParallelChipCorrelatorThread : public IRunnable
{
private:
    double m_correlationThreshold;
    std::unique_ptr<CSubPixelCorrelator<float> > m_subCorrelator;
    const CMatrix<float> *m_inputImage;
    const CVector<CChip<float> > *m_inputChips;
    const CMatrix<SPair<double> > *m_worldMatrix;
    CVector<SChipCorrelationResult> *m_results;
    CMapModelToMatrix<float> *m_mapper;
    SPair<double> m_pixelGSD;
    ECorrelationType m_correlationMethod;
    unsigned long m_spatialCorrelationSearchWindowSize;
    CMatrix<float> m_corrSubImage;
    CMatrix<float> m_chipData;
    SPair<double> m_origin;
    SSize m_phaseImageSize;
    bool m_mayContainNullValues;
    bool m_useNullValue;
    float m_nullValue;
    CAtomicLong *m_threadCompletedCount;
    unsigned long m_threadsSpawned;
    std::shared_ptr<CThreadLock> m_lock;

    int innerInit();
    int populateSubImage(const CChip<float> &chip, bool &validTileReturned);
    int RunCorrelation(const CChip<float> &chip, SChipCorrelationResult &result);

public:
    CParallelChipCorrelatorThread(CAtomicLong *threadCompletedCount, unsigned long threadsSpawned, std::shared_ptr<CThreadLock> lock);
    virtual ~CParallelChipCorrelatorThread();

    int LoadData(const CMatrix<float> *inputImage,
                 const CVector<CChip<float> > *inputChips,
                 CVector<SChipCorrelationResult> *results,
                 const CMatrix<SPair<double> > *worldMatrix,
                 const SPair<double> &pixelGroundSamplingDistance,
                 unsigned long spatialCorrelationSearchWindowSize,
                 float correlationThreshold,
                 ECorrelationType correlationMethod,
                 bool useNullValue, float nullValue, bool mayContainNullValues);
    virtual void run(void *context = nullptr);
};

} // namespace __ultra_internal
} // namespace ultra
