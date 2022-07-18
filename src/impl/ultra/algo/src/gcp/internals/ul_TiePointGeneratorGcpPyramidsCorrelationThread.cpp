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

#include "ul_TiePointGeneratorGcpPyramids.h"

#include <ul_ImageLoader.h>
#include "ul_Resampler.h"

namespace ultra
{

CTiePointGeneratorGcpPyramids::CCorrelationThread::CCorrelationThread()
{
}

CTiePointGeneratorGcpPyramids::CCorrelationThread::~CCorrelationThread()
{
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::Init(CTiePointGeneratorGcpPyramids::SContext *context,
                                                            CMatrix<float> *inputImageData,
                                                            CVector<CTiePointGeneratorGcpPyramids::SPyramid> *pyramids,
                                                            CVector<SPair<double> > *offsets,
                                                            unsigned long pyramidIndex,
                                                            CVector<SPair<double> > *overallGcpShift,
                                                            CThreadLock *sharedLock,
                                                            CAtomicLong *sharedCounter,
                                                            CAtomicLong *sharedProcessingPercentage,
                                                            CAtomicLong *threadDoneCount,
                                                            unsigned long totalRunningThreads,
                                                            CAtomicBool *runningVar)
{
    m_context = context;
    SSize corrSize = context->m_chipSizeMinimum;
    if (m_context->m_typeOfChipCorrelationTechnique == ECorrelationType::PHASE)
    {
        corrSize.row -= corrSize.row % 2;
        corrSize.col -= corrSize.col % 2;
    }

    m_correlator.reset(new CSubPixelCorrelator<float>(corrSize, m_context->m_typeOfChipCorrelationTechnique, m_context->m_typeOfSubPixelChipCorrelationTechnique));

    m_exitCode = 0;

    if (CImageLoader::getInstance()->LoadImageMetadata(context->m_pathToRefImage, m_refMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }

    m_inputImageData = inputImageData;
    m_pyramids = pyramids;
    m_offsets = offsets;
    m_pyramidIndex = pyramidIndex;
    m_overallGcpShift = overallGcpShift;
    m_gcpTotalCount = (*pyramids)[0].selectUlOriginal.size();
    m_sharedLock = sharedLock;
    m_sharedCounter = sharedCounter;
    m_sharedProcessingPercentage = sharedProcessingPercentage;
    m_threadDoneCount = threadDoneCount;
    m_totalRunningThreads = totalRunningThreads;
    m_runningVar = runningVar;
    return 0;
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::getExitCode() const
{
    return m_exitCode;
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::ResampleData(const CMatrix<float> &originalData,
                                                                    CMatrix<float> &resampledData,
                                                                    const CMatrix<SPair<float> > &resampleMap,
                                                                    CTiePointGeneratorGcpPyramids::SPyramid &pyramid)
{
    EResamplerEnum::EResampleType resampler = EResamplerEnum::RESAMPLE_TYPE_BI;
    if (pyramid.pyramidLevel == 1)
    {
        resampledData = originalData;
        return 0;
    }

    SSize targetSize = resampleMap.getSize();
    if (CResampler<false>::Resize<float>(originalData, resampledData, targetSize, EResamplerEnum::RESAMPLE_TYPE_AVG, &m_context->m_nullValue) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CResampler::Resize()");
        return 1;
    }

    return 0;
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::ExtractRefImageData(const SImageMetadata &refMetadata,
                                                                           const SSize &tileSize,
                                                                           CTiePointGeneratorGcpPyramids::SPyramid &pyramid,
                                                                           CMatrix<float> &refDataOriginal,
                                                                           bool &refDataFound,
                                                                           unsigned long pyramidIndex,
                                                                           const CVector<SPair<double> > &offsets,
                                                                           SPair<double> &tileOrigin)
{
    double addUl = 1.0;
    refDataFound = false;
    SPair<long> tileSizeLong = tileSize;
    tileSizeLong *= pyramid.modVal;
    SPair<double> tileSizeMapCoor = tileSizeLong.convertType<double>() * pyramid.gsdOriginal;
    SPair<double> midPoint = pyramid.originOriginal + pyramid.tiePointCoordinatesOriginal[pyramidIndex] * pyramid.gsdOriginal;
    midPoint -= offsets[pyramidIndex] * pyramid.gsdOriginal;
    SPair<double> selectUl = midPoint - (tileSizeMapCoor / 2.0);
    SPair<double> selectLr = midPoint + (tileSizeMapCoor / 2.0);

    SPair<double> refOrigin = refMetadata.getOrigin();
    SPair<double> refDims = refMetadata.getDimensions();
    selectUl -= refOrigin;
    selectLr -= refOrigin;
    selectUl /= pyramid.gsdOriginal;
    selectLr /= pyramid.gsdOriginal;

    selectLr += addUl;
    selectUl += addUl;
    SPair<double> selectSize = (selectLr - selectUl).roundValues();

    if (selectSize.r != tileSizeLong.r ||
        selectSize.c != tileSizeLong.c)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to select correct image bound sizes");
        return 1;
    }

    if (selectUl.r < 0 || selectUl.c < 0 ||
        selectLr.r >= refDims.r || selectLr.c >= refDims.c)
    {
        return 0;
    }
    refDataFound = true;

    SSize selectUlLong = selectUl.getSizeType();
    SSize selectSizeLong = selectSize.getSizeType();

    SImageMetadata loadedMeta;
    if (CImageLoader::getInstance()->LoadImageWithMetadata(m_context->m_pathToRefImage, refDataOriginal, selectUlLong, selectSizeLong, loadedMeta) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageWithMetadata()");
        return 1;
    }

    tileOrigin = loadedMeta.getOrigin();

    return 0;
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::CorrelateData(const CMatrix<float> &refDataResampled, const CMatrix<float> &chipDataResampled,
                                                                     CVector<CTiePointGeneratorGcpPyramids::SPyramid> &pyramid, unsigned long pyramidIndex,
                                                                     CVector<SPair<double> > &offsets, unsigned long subPyramidIndex)
{
    bool isLast = pyramidIndex == 0;
    bool corrSuccess;
    SPair<double> templateToInputOffset;
    double corrCoefficient;
    float *nodataPtr = m_context->m_useNullValue ? &m_context->m_nullValue : nullptr;
    if (m_correlator->Correlate(refDataResampled, chipDataResampled, m_context->m_correlationThreshold, corrSuccess,
                                templateToInputOffset, corrCoefficient, nodataPtr, nodataPtr) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CSubPixelCorrelator::Correlate()");
        return 1;
    }

    pyramid[pyramidIndex].tiePointGood[subPyramidIndex] = corrSuccess;
    if (corrSuccess)
    {
        SPair<double> refSize = refDataResampled.getSize() / 2;
        SPair<double> inSize = chipDataResampled.getSize() / 2;
        SPair<double> shift = inSize - refSize - templateToInputOffset;
        offsets[subPyramidIndex] += shift.roundValues() * pyramid[pyramidIndex].modVal;
        if (isLast)
        {
            (*m_overallGcpShift)[subPyramidIndex] += shift;
        }
        else
        {
            (*m_overallGcpShift)[subPyramidIndex] += shift.roundValues() * pyramid[pyramidIndex].modVal;
        }
    }
    else
    {
        for (unsigned long a = 0; a < pyramid.size(); a++)
            pyramid[a].tiePointGood[subPyramidIndex] = false;
    }

    return 0;
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::SelectSubImage(CMatrix<float> &image, bool imageMustBeEven) const
{
    SSize orSize = image.getSize();
    SSize size = orSize;
    unsigned long add = (imageMustBeEven ? 0 : 1);
    size.col -= (size.col + add) % 2;
    size.row -= (size.row + add) % 2;

    if (orSize != size)
    {
        image = image.getSubMatrix(0, size);
    }
    return 0;
}

int CTiePointGeneratorGcpPyramids::CCorrelationThread::ProcessPyramidLevel(const SImageMetadata &refMetadata,
                                                                           const CMatrix<float> &inputImageData,
                                                                           CVector<CTiePointGeneratorGcpPyramids::SPyramid> &pyramids,
                                                                           CVector<SPair<double> > &offsets,
                                                                           unsigned long pyramidIndex)
{
    CMatrix<float> chipDataOriginal;
    CMatrix<float> chipDataResampled;
    CMatrix<float> refDataOriginal;
    CMatrix<float> refDataResampled;
    SPair<double> refTileOrigin;
    bool refDataFound;

    SSize outChipSize = m_context->m_chipSizeMinimum;
    SSize tileSize = m_context->m_chipSizeMinimum;

    if (m_context->m_typeOfChipCorrelationTechnique != ECorrelationType::PHASE)
    {
        tileSize += m_context->m_searchWindowSize;
        tileSize.row = (tileSize.row % 2 != 0) ? (tileSize.row) : (tileSize.row + 1);
        tileSize.col = (tileSize.col % 2 != 0) ? (tileSize.col) : (tileSize.col + 1);
    }

    CMatrix<SPair<float> > chipResampleMap, refResampleMap;
    chipResampleMap.resize(outChipSize);
    refResampleMap.resize(tileSize);
    for (unsigned long r = 0; r < outChipSize.row; r++)
    {
        for (unsigned long c = 0; c < outChipSize.row; c++)
        {
            chipResampleMap[r][c].r = r * pyramids[pyramidIndex].modVal;
            chipResampleMap[r][c].c = c * pyramids[pyramidIndex].modVal;
        }
    }

    for (unsigned long r = 0; r < tileSize.row; r++)
    {
        for (unsigned long c = 0; c < tileSize.row; c++)
        {
            refResampleMap[r][c].r = r * pyramids[pyramidIndex].modVal;
            refResampleMap[r][c].c = c * pyramids[pyramidIndex].modVal;
        }
    }

    unsigned long gcpItem = 0;
    while ((gcpItem = (unsigned long) m_sharedCounter->getAndInc()) < m_gcpTotalCount && m_runningVar->get())
    {
        if (!pyramids[pyramidIndex].tiePointGood[gcpItem])
            continue;

        if (ExtractRefImageData(refMetadata, tileSize, pyramids[pyramidIndex], refDataOriginal, refDataFound, gcpItem, offsets, refTileOrigin) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ExtractRefImageData()");
            return 1;
        }

        if (!refDataFound)
        {
            for (unsigned long a = 0; a < pyramids.size(); a++)
                pyramids[a].tiePointGood[gcpItem] = false;
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Out of bounds, going to skip this chip, could not find matching reference data");
            continue;
        }

        chipDataOriginal = inputImageData.getSubMatrix(pyramids[pyramidIndex].selectUlOriginal[gcpItem], pyramids[pyramidIndex].selectSizeOriginal);
        if (chipDataOriginal.getSize() != pyramids[pyramidIndex].selectSizeOriginal)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to select chip pixel data");
            return 1;
        }

        if (ResampleData(chipDataOriginal, chipDataResampled, chipResampleMap, pyramids[pyramidIndex]) != 0 ||
            ResampleData(refDataOriginal, refDataResampled, refResampleMap, pyramids[pyramidIndex]) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ResampleData()");
            return 1;
        }

        if (m_context->m_typeOfChipCorrelationTechnique == ECorrelationType::PHASE)
        {
            if (SelectSubImage(chipDataResampled, true) != 0 ||
                SelectSubImage(refDataResampled, true) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SelectSubImage()");
                return 1;
            }
        }

        if (CorrelateData(refDataResampled, chipDataResampled,
                          pyramids, pyramidIndex, offsets, gcpItem) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CorrelateData()");
            return 1;
        }

        if (m_sharedProcessingPercentage->get() < (((long) gcpItem * 100) / (long) m_gcpTotalCount))
        {
            long per = ((long) gcpItem * 100) / (long) m_gcpTotalCount;
            m_sharedProcessingPercentage->set(per);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Correlating " + toString(per) + "%    \r");
        }
    }
    if (m_threadDoneCount->incAndGet() == m_totalRunningThreads)
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Correlating 100%    ");
    return 0;
}

void CTiePointGeneratorGcpPyramids::CCorrelationThread::run(void *context)
{
    m_exitCode = 0;
    if (ProcessPyramidLevel(m_refMetadata, *m_inputImageData, *m_pyramids, *m_offsets, m_pyramidIndex) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ProcessPyramidLevel()");
        m_exitCode = 1;
    }
}

} // namespace ultra
