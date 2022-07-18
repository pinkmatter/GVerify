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

#include "ul_ParallelChipCorrelatorThread.h"


namespace ultra
{
namespace __ultra_internal
{

CParallelChipCorrelatorThread::CParallelChipCorrelatorThread(CAtomicLong *threadCompletedCount, unsigned long threadsSpawned, std::shared_ptr<CThreadLock> lock) :
m_lock(lock)
{
    m_mapper = nullptr;

    m_inputImage = nullptr;
    m_inputChips = nullptr;
    m_results = nullptr;
    m_worldMatrix = nullptr;

    m_useNullValue = false;
    m_mayContainNullValues = true;

    m_threadCompletedCount = threadCompletedCount;
    m_threadsSpawned = threadsSpawned;
}

CParallelChipCorrelatorThread::~CParallelChipCorrelatorThread()
{
    if (m_mapper != nullptr)
    {
        delete m_mapper;
        m_mapper = nullptr;
    }
}

int CParallelChipCorrelatorThread::innerInit()
{
    if (m_inputImage == nullptr || m_inputChips == nullptr || m_results == nullptr || m_worldMatrix == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image, world-matrix and or chip are not loaded, or results set is not loaded, please call LoadData");
        return 1;
    }

    unsigned long size = m_inputChips->size();
    if (size == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Nothing to correlate input chip vector is of size zero");
        return 0;
    }

    m_results->resize(size);

    long sizeIndex = -1;
    for (unsigned long t = 0; t < m_inputChips->size(); t++)
    {
        if ((*m_inputChips)[t].chipData.getSize() != SSize(0, 0))
        {
            sizeIndex = t;
            break;
        }
    }
    if (sizeIndex < 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "No valid chips found skipping,");
        return 0;
    }
    m_phaseImageSize = (*m_inputChips)[sizeIndex].chipData.getSize();
    if (m_phaseImageSize.row <= 1 ||
        m_phaseImageSize.col <= 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Phase correlation image size is too small");
        return 1;
    }

    if (m_phaseImageSize.col % 2 != 0)
        m_phaseImageSize.col--;
    if (m_phaseImageSize.row % 2 != 0)
        m_phaseImageSize.row--;

    if (!m_subCorrelator)
    {
        ESubPixelCorrelationType subType = ESubPixelCorrelationType::SUB_PIXEL_CORRELATION_TYPE_COUNT;
        if (m_correlationMethod == ECorrelationType::PHASE)
            subType = ESubPixelCorrelationType::PHASE_SUB_PIXEL;
        else
            subType = ESubPixelCorrelationType::LEAST_SQUARE_SUB_PIXEL;

        m_subCorrelator.reset(new CSubPixelCorrelator<float>(m_phaseImageSize, m_correlationMethod, subType));
    }

    return 0;
}

int CParallelChipCorrelatorThread::LoadData(const CMatrix<float> *inputImage,
                                            const CVector<CChip<float> > *inputChips,
                                            CVector<SChipCorrelationResult> *results,
                                            const CMatrix<SPair<double> > *worldMatrix,
                                            const SPair<double> &pixelGroundSamplingDistance,
                                            unsigned long spatialCorrelationSearchWindowSize,
                                            float correlationThreshold,
                                            ECorrelationType correlationMethod,
                                            bool useNullValue, float nullValue, bool mayContainNullValues)
{
    m_inputImage = inputImage;
    m_inputChips = inputChips;
    m_results = results;
    m_worldMatrix = worldMatrix;
    m_pixelGSD = pixelGroundSamplingDistance;
    m_spatialCorrelationSearchWindowSize = spatialCorrelationSearchWindowSize;
    m_correlationThreshold = correlationThreshold;
    m_correlationMethod = correlationMethod;
    m_useNullValue = useNullValue;
    m_nullValue = nullValue;
    m_mayContainNullValues = mayContainNullValues;

    if (m_spatialCorrelationSearchWindowSize < 3)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Search windows size must be larger than 3");
        return 1;
    }

    if (m_inputImage == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid conversion for input float image");
        return 1;
    }

    if (m_inputChips == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid conversion for input float chips");
        return 1;
    }

    if (m_results == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid conversion for output results");
        return 1;
    }

    if (m_worldMatrix == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid conversion for world matrix");
        return 1;
    }

    if (m_worldMatrix->getSize().containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "World matrix is of size 0x0");
        return 1;
    }
    m_origin = (*m_worldMatrix)[0][0];

    if (m_mapper != nullptr)
    {
        delete []m_mapper;
        m_mapper = nullptr;
    }

    m_mapper = new CMapModelToMatrix<float>(*m_worldMatrix, *m_inputImage);

    if (innerInit() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerInit()");
        return 1;
    }

    return 0;
}

int CParallelChipCorrelatorThread::populateSubImage(const CChip<float> &chip, bool &validTileReturned)
{
    if (m_correlationMethod != ECorrelationType::PHASE)
    {
        SSize windowSize = chip.chipData.getSize() + m_spatialCorrelationSearchWindowSize;
        if (windowSize.col % 2 != 1)
            windowSize.col++;
        if (windowSize.row % 2 != 1)
            windowSize.row++;

        if (m_mapper->GetSceneTile(chip.midCoordinate, windowSize, m_corrSubImage, m_pixelGSD, validTileReturned, m_correlationMethod) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetSceneTile()");
            return 1;
        }
    }
    else
    {
        if (m_mapper->GetSceneTile(chip.midCoordinate, m_phaseImageSize, m_corrSubImage, m_pixelGSD, validTileReturned, m_correlationMethod) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetSceneTile()");
            return 1;
        }
    }

    return 0;
}

int CParallelChipCorrelatorThread::RunCorrelation(const CChip<float> &chip, SChipCorrelationResult &result)
{
    bool successful = false;
    bool validTileReturned;

    result.correlationResultIsGood = false; //set as failed, we might skip this chip so default to failed
    result.chip = chip;

    if (populateSubImage(chip, validTileReturned) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failure returned from populateSubImage(), going to skip this chip");
        return 0;
    }

    if (!validTileReturned)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "An invalid sub image tile was returned");
        return 0;
    }
    SPair<double> templateToInputOffset;

    if (m_correlationMethod == ECorrelationType::PHASE)
    {
        m_chipData = chip.chipData.getSubMatrix(0, m_phaseImageSize);
    }
    else
    {
        m_chipData = chip.chipData;
    }

    float *inputNullValuePtr = m_useNullValue ? &m_nullValue : nullptr;
    float *templateNullValuePtr = m_useNullValue ? &m_nullValue : nullptr;
    if (m_subCorrelator->Correlate(m_corrSubImage,
                                   m_chipData,
                                   m_correlationThreshold,
                                   successful,
                                   templateToInputOffset,
                                   result.corrCoefficient,
                                   inputNullValuePtr,
                                   templateNullValuePtr,
                                   m_mayContainNullValues,
                                   m_mayContainNullValues) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Correlate()");
        return 1;
    }

    result.correlationResultIsGood = successful;
    if (result.correlationResultIsGood)
    {
        SPair<double> inputToTemplateImageSizeHalf = (SPair<double> (m_corrSubImage.getSize()) - SPair<double> (m_chipData.getSize())) / 2;
        SPair<double> relativeOffset = inputToTemplateImageSizeHalf - templateToInputOffset;
        result.newMidCoordinate = result.chip.midCoordinate;
        result.newMidCoordinate += relativeOffset * m_pixelGSD;
        for (int x = 0; x < 4; x++)
        {
            result.newBoundingCoordinate[x] = result.chip.boundingCoordinate[x];
            result.newBoundingCoordinate[x] += relativeOffset * (m_pixelGSD);
        }
    }

    return 0;
}

void CParallelChipCorrelatorThread::run(void* context)
{
    unsigned long size = m_inputChips->size();
    for (unsigned long it = 0; it < size; it++)
    {
        if (RunCorrelation(m_inputChips->operator[](it), m_results->operator[](it)) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from RunCorrelation()");
            break;
        }
    }

    //reset these back to nullptr, detach pointers
    m_inputImage = nullptr;
    m_inputChips = nullptr;
    m_results = nullptr;
    m_worldMatrix = nullptr;

    m_threadCompletedCount->inc();
    if (m_threadCompletedCount->get() == m_threadsSpawned)
    {
        if (m_lock.get() != nullptr)
        {
            AUTO_LOCK(m_lock);
            if (m_lock->broadcast(__FILE__, __LINE__) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to broadcast on a mutex");
                return;
            }
        }
    }
}

} // namespace __ultra_internal
} // namespace ultra
