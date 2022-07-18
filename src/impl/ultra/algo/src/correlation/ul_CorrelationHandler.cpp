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

#include "ul_CorrelationHandler.h"
#include "impl/concurrency/ul_ParallelChipCorrelatorThread.h"
#include "ul_LeastSquares.h"

namespace ultra
{

CCorrelationHandler::CCorrelationHandler(unsigned int threadCount, float correlationThreshold, bool useNullValue, float nullValue, bool mayContainNullValues)
{
    m_canStart = false;
    m_maxThreads = threadCount;
    m_correlationThreshold = correlationThreshold;
    m_CCorrelationHandler_corrVecThreads = nullptr;
    m_useNullValue = useNullValue;
    m_nullValue = nullValue;
    m_mayContainNullValues = mayContainNullValues;
}

CCorrelationHandler::~CCorrelationHandler()
{
    if (m_CCorrelationHandler_corrVecThreads != nullptr)
    {
        CVector<__ultra_internal::CParallelChipCorrelatorThread *> * vec = (CVector<__ultra_internal::CParallelChipCorrelatorThread *> *)m_CCorrelationHandler_corrVecThreads;
        for (unsigned long x = 0; x < vec->size(); x++)
        {
            if (vec->operator[](x) != nullptr)
            {
                if (CUltraThread::getInstance()->join((vec->operator[](x))) != 0)
                {
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failed to join a thread");
                }
                delete vec->operator[](x);
                vec->operator[](x) = nullptr;
            }
        }
        vec->clear();
        delete vec;
        m_CCorrelationHandler_corrVecThreads = vec = nullptr;
    }
}

int CCorrelationHandler::Init(const CVector<CChip<float> > &inputChips)
{
    if (m_maxThreads < 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Thread count must at least be 1");
        return 1;
    }

    m_inputChips.resize(m_maxThreads);
    m_results.resize(m_maxThreads);

    unsigned long intCount = inputChips.size() / m_maxThreads;
    unsigned long modCount = inputChips.size() % m_maxThreads;

    for (unsigned long x = 0; x < m_maxThreads; x++)
    {
        unsigned long buffSize = intCount;
        if (modCount != 0)
        {
            buffSize++;
            modCount--;
        }

        if (buffSize != 0)
        {
            m_inputChips[x].resize(buffSize);
        }
    }

    unsigned long actualCount = 0;
    for (unsigned long x = 0; x < m_maxThreads; x++)
    {
        for (unsigned long y = 0; y < m_inputChips[x].size(); y++, actualCount++)
        {
            m_inputChips[x][y] = inputChips[actualCount];
        }
    }

    if (m_correlationMethod == ECorrelationType::PHASE)
    {
        if (inputChips.size() > 0)
        {
            SSize firstChipSize = inputChips[0].chipData.getSize();

            for (unsigned long x = 1; x < inputChips.size(); x++)
            {
                if (firstChipSize != inputChips[x].chipData.getSize())
                {
                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Chips must be of the same size if a PHASE correlation is used");
                    return 1;
                }
            }
        }
    }

    return 0;
}

int CCorrelationHandler::LoadData(
                                  const CMatrix<float> &inputImage,
                                  const CVector<CChip<float> > &inputChips,
                                  const CMatrix<SPair<double> > &worldMatrix,
                                  const SPair<double> &pixelGroundSamplingDistance,
                                  unsigned long searchWindowSize,
                                  const SPair<double> &originChipToInputDiv,
                                  ECorrelationType correlationMethod
                                  )
{
    m_canStart = false;
    if (Init(inputChips) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
        return 1;
    }

    m_inputImage = &inputImage;
    m_worldMatrix = &worldMatrix;
    m_pixelGroundSamplingDistance = pixelGroundSamplingDistance;
    m_searchWindowSize = searchWindowSize;
    m_correlationMethod = correlationMethod;
    m_originChipToInputDiv = originChipToInputDiv;
    m_canStart = true;
    return 0;
}

int CCorrelationHandler::StartThreads(std::shared_ptr<CThreadLock> lock)
{
    CVector<__ultra_internal::CParallelChipCorrelatorThread *> * vec = new CVector<__ultra_internal::CParallelChipCorrelatorThread *>();
    m_CCorrelationHandler_corrVecThreads = vec;
    vec->resize(m_maxThreads);
    vec->initVec(nullptr);
    m_atLong.set(0);
    for (unsigned long t = 0; t < vec->size(); t++)
    {
        vec->operator[](t) = new __ultra_internal::CParallelChipCorrelatorThread(&m_atLong, vec->size(), lock);

        if (vec->operator[](t) == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CParallelChipCorrelatorSpacialThread object");
            for (unsigned long tt = 0; tt < t; tt++)
            {
                delete vec->operator[](t);
                vec->operator[](t) = nullptr;
            }
            delete vec;
            m_CCorrelationHandler_corrVecThreads = vec = nullptr;
            return 1;
        }

        if ((*vec)[t]->LoadData(m_inputImage, &m_inputChips[t], &m_results[t], m_worldMatrix,
                                m_pixelGroundSamplingDistance, m_searchWindowSize, m_correlationThreshold,
                                m_correlationMethod, m_useNullValue, m_nullValue, m_mayContainNullValues) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadData()");
            for (unsigned long tt = 0; tt <= t; tt++)
            {
                delete vec->operator[](t);
                vec->operator[](t) = nullptr;
            }
            delete vec;
            m_CCorrelationHandler_corrVecThreads = vec = nullptr;
            return 1;
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Starting chip correlation");
    for (unsigned long t = 0; t < vec->size(); t++)
    {
        if (CUltraThread::getInstance()->start(vec->operator[](t), nullptr) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to start thread, oops here comes a memleak");
            return 1;
        }
    }

    return 0;
}

int CCorrelationHandler::Correlate(std::shared_ptr<CThreadLock> lock)
{
    if (!m_canStart)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot start correlation, please call load data first");
        return 1;
    }
    m_canStart = false;

    if (StartThreads(lock) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from StartThreads()");
        return 1;
    }

    return 0;
}

bool CCorrelationHandler::isBusy()
{
    CVector<__ultra_internal::CParallelChipCorrelatorThread *> * vec = (CVector<__ultra_internal::CParallelChipCorrelatorThread *> *)m_CCorrelationHandler_corrVecThreads;
    for (unsigned long t = 0; t < vec->size(); t++)
    {
        if (CUltraThread::getInstance()->isBusy(vec->operator[](t)))
        {
            return true;
        }
    }

    return false;
}

int CCorrelationHandler::cleanup()
{
    CVector<__ultra_internal::CParallelChipCorrelatorThread *> * vec = (CVector<__ultra_internal::CParallelChipCorrelatorThread *> *)m_CCorrelationHandler_corrVecThreads;
    for (unsigned long t = 0; t < vec->size(); t++)
    {
        if (vec->operator[](t) != nullptr)
        {
            if (CUltraThread::getInstance()->join((vec->operator[](t))))
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to join a thread");
                return 1;
            }
            delete vec->operator[](t);
            vec->operator[](t) = nullptr;
        }
    }
    vec->clear();
    delete vec;
    m_CCorrelationHandler_corrVecThreads = vec = nullptr;
    return 0;
}

int CCorrelationHandler::GetResults(CVector<SChipCorrelationResult> &results)
{
    int tempLoop = 0;
    while (isBusy())
    {
        usleep(100);
        tempLoop++;
        if (tempLoop == 50)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Processing is still busy will try and get results");
            break;
        }
    }

    unsigned long size = 0;

    for (unsigned long t = 0; t < m_results.size(); t++)
    {
        size += m_results[t].size();
    }

    results.resize(size);

    unsigned long counter = 0;
    for (unsigned long t = 0; t < m_results.size(); t++)
    {
        for (unsigned long x = 0; x < m_results[t].size(); x++, counter++)
        {
            results[counter] = m_results[t][x];
            if (results[counter].isGoodResult())
            {
                results[counter].newMidCoordinate += m_originChipToInputDiv;
                results[counter].newBoundingCoordinate[0] += m_originChipToInputDiv;
                results[counter].newBoundingCoordinate[1] += m_originChipToInputDiv;
                results[counter].newBoundingCoordinate[2] += m_originChipToInputDiv;
                results[counter].newBoundingCoordinate[3] += m_originChipToInputDiv;
            }
        }
    }

    if (cleanup() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from cleanup()");
        return 1;
    }

    return 0;
}

} // namespace ultra
