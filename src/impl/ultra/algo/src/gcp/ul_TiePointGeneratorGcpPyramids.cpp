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
#include "ul_ChipsGenWrapper.h"

#include <ul_ImageLoader.h>
#include <ul_ImageSaver.h>
#include <ul_UltraThreadFixedPool.h>

namespace ultra
{

CTiePointGeneratorGcpPyramids::CTiePointGeneratorGcpPyramids(CTiePointGeneratorGcpPyramids::SContext *context) :
ITiePointGenerator()
{
    m_context = context;
}

CTiePointGeneratorGcpPyramids::~CTiePointGeneratorGcpPyramids()
{
    m_context = nullptr;
}

int CTiePointGeneratorGcpPyramids::CreateFolder(const std::string &path)
{
    if (!CFile(path).isDirectory() && CFile(path).mkdir() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + path + "'");
        return 1;
    }
    return 0;
}

int CTiePointGeneratorGcpPyramids::PreProcessInputData(std::string &inImage, SPair<double> &inputOriginAdd)
{
    if (CreateFolder(CFile(m_context->m_pathToWorkingFolder, "Input").getAbsolutePath()) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CreateFolder()");
        return 1;
    }

    inImage = CFile(CFile(m_context->m_pathToWorkingFolder, "Input"), "OriginAdjustedInput.TIF").getAbsolutePath();

    SImageMetadata metaRef, metaIn;
    SPair<double> gsd;
    if (CImageLoader::getInstance()->LoadImageMetadata(m_context->m_pathToRefImage, metaRef) != 0 ||
        CImageLoader::getInstance()->LoadImageMetadata(m_context->m_pathToInputImage, metaIn) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }
    // we already check that both GSDs are the same we can assume that they are when we get here
    gsd = metaRef.getGsd();

    SPair<double> originIn = metaIn.getOrigin();
    SPair<double> originRef = metaRef.getOrigin();

    originIn /= gsd;
    originRef /= gsd;

    SPair<double> normalizedOriginDiff = originRef - originIn;

    double dummy;
    normalizedOriginDiff.r = modf(normalizedOriginDiff.r, &dummy);
    normalizedOriginDiff.c = modf(normalizedOriginDiff.c, &dummy);

    normalizedOriginDiff *= gsd;

    inputOriginAdd = normalizedOriginDiff;
    SPair<double> newOriginIn = metaIn.getOrigin() + normalizedOriginDiff;

    metaIn.setOrigin(newOriginIn);

    //verify that we got the correct offset only
    originIn = metaIn.getOrigin();
    originRef = metaRef.getOrigin();

    originIn /= gsd;
    originRef /= gsd;

    normalizedOriginDiff = originRef - originIn;

    normalizedOriginDiff.r = modf(normalizedOriginDiff.r, &dummy);
    normalizedOriginDiff.c = modf(normalizedOriginDiff.c, &dummy);

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Going to offset input image`s origin with '" + toString(inputOriginAdd) + "'");
    if (!normalizedOriginDiff.containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to calculate the required origin offset for the input image");
        return 1;
    }

    CMatrixArray<float> image;
    if (CImageLoader::getInstance()->LoadImage(m_context->m_pathToInputImage, image) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImage()");
        return 1;
    }

    if (CImageSaver::getInstance()->SaveImage(inImage, image, EImage::IMAGE_TYPE_GEOTIFF, &metaIn) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageSaver::getInstance()->SaveImage()");
        return 1;
    }
    return 0;
}

int CTiePointGeneratorGcpPyramids::GenerateTiePoints(const std::string &imagePath, CVector<SPair<double> > &tiePointCoordinates)
{
    unsigned long size = m_context->m_chipGeneratorMethods.size();
    CVector<CVector<CChip<float> > > chipsVec(size);
    CMatrix<float> image;
    unsigned long totalChipsFound = 0;
    unsigned long counter = 0;
    if (CImageLoader::getInstance()->LoadImage(imagePath, image) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImage()");
        return 1;
    }

    SPair<double> imageSizeMinus1 = image.getSize() - 1;
    SPair<double> zero = 0;
    for (unsigned long t = 0; t < size; t++)
    {
        CChipsGen<float>::CChipsGenContext *chipContext = genDefaultChipGenContext(m_context->m_chipGeneratorMethods[t]);
        if (chipContext == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create a context for CChipsGen object");
            return 1;
        }
        if (CChipsGenWrapper::Generate(m_context->m_chipGeneratorMethods[t], &image, &chipsVec[t], chipContext) != 0)
        {
            delete chipContext;
            chipContext = nullptr;
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CChipsGenWrapper::Generate()");
            return 1;
        }

        unsigned long validSize = 0;
        for (unsigned long i = 0; i < chipsVec[t].size(); i++)
        {
            chipsVec[t][i].chipData.clear();
            chipsVec[t][i].worldMatrix.clear();
            SSize tempCoor = chipsVec[t][i].midCoordinate.roundValues().clipBetween(zero, imageSizeMinus1).getSizeType();
            if (image[tempCoor] != m_context->m_nullValue)
            {
                validSize++;
            }
        }

        totalChipsFound += validSize;
        delete chipContext;
        chipContext = nullptr;
    }

    tiePointCoordinates.resize(totalChipsFound);
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Found '" + toString(totalChipsFound) + "' valid chips");
    counter = 0;
    for (unsigned long t = 0; t < size; t++)
    {
        for (unsigned long i = 0; i < chipsVec[t].size(); i++)
        {
            SSize tempCoor = chipsVec[t][i].midCoordinate.roundValues().clipBetween(zero, imageSizeMinus1).getSizeType();
            if (image[tempCoor] != m_context->m_nullValue)
            {
                if (!tiePointCoordinates.contains(chipsVec[t][i].midCoordinate))
                {
                    tiePointCoordinates[counter] = chipsVec[t][i].midCoordinate;
                    counter++;
                }
            }
        }
    }

    return 0;
}

int CTiePointGeneratorGcpPyramids::PyramidLoop(const std::string &imagePath,
                                               const CVector<SPair<double> > &tiePointCoordinates,
                                               CVector<SPyramid> &pyramids)
{
    SImageMetadata metaIn;
    if (CImageLoader::getInstance()->LoadImageMetadata(imagePath, metaIn) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }
    CVector<SPair<double> > offsets(tiePointCoordinates.size());
    offsets.initVec(SPair<double>(0));

    m_overallGcpShift.resize(tiePointCoordinates.size());
    m_overallGcpShift.initVec(SPair<double>(0));

    pyramids.resize(m_context->m_pyramidCount);
    pyramids[0].pyramidLevel = 1;
    pyramids[0].tiePointCoordinatesOriginal = tiePointCoordinates;
    pyramids[0].tiePointCoordinates = tiePointCoordinates;
    pyramids[0].modVal = 1;
    pyramids[0].tiePointGood.resize(pyramids[0].tiePointCoordinates.size());
    pyramids[0].tiePointGood.initVec(true);
    pyramids[0].selectUlOriginal.resize(pyramids[0].tiePointCoordinates.size());
    pyramids[0].originOriginal = metaIn.getOrigin();
    pyramids[0].gsdOriginal = metaIn.getGsd();
    pyramids[0].gsd = metaIn.getGsd();

    for (unsigned long t = 1; t < pyramids.size(); t++)
    {
        pyramids[t].pyramidLevel = pyramids[t - 1].pyramidLevel * 2;
        pyramids[t].tiePointCoordinatesOriginal = pyramids[t - 1].tiePointCoordinatesOriginal;
        pyramids[t].tiePointCoordinates = pyramids[t - 1].tiePointCoordinates / SPair<double>(2.0);
        pyramids[t].modVal = pyramids[t - 1].modVal * 2.0;
        pyramids[t].tiePointGood = pyramids[t - 1].tiePointGood;
        pyramids[t].selectUlOriginal.resize(pyramids[0].tiePointCoordinates.size());
        pyramids[t].originOriginal = pyramids[t - 1].originOriginal;
        pyramids[t].gsdOriginal = pyramids[t - 1].gsdOriginal;
        pyramids[t].gsd = pyramids[t - 1].gsd * 2.0;
    }

    CMatrix<float> inputImageData;
    if (CImageLoader::getInstance()->LoadImage(imagePath, inputImageData) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImage()");
        return 1;
    }

    SPair<long> inputImageDataSize = inputImageData.getSize();
    for (long t = ((long) pyramids.size()) - 1; t >= 0; t--)
    {
        unsigned long requiredChipSize = (m_context->m_chipSizeMinimum - 1) * pyramids[t].pyramidLevel + 1;
        pyramids[t].selectSizeOriginal = requiredChipSize;
        for (unsigned long i = 0; i < pyramids[t].tiePointCoordinatesOriginal.size(); i++)
        {
            if (!pyramids[t].tiePointGood[i])
                continue;

            SPair<long> chipMid = pyramids[t].tiePointCoordinatesOriginal[i].roundValues().convertType<long>();
            SPair<long> ulSelect = chipMid - requiredChipSize / 2;
            pyramids[t].selectUlOriginal[i] = ulSelect.getSizeType();
            SPair<long> lrSelect = chipMid + requiredChipSize / 2;
            SPair<long> sizeSelect = lrSelect - ulSelect + 1;
            if (sizeSelect.r != requiredChipSize &&
                sizeSelect.c != requiredChipSize)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid chip size calculated");
                return 1;
            }

            if (ulSelect.r < 0 || ulSelect.c < 0 ||
                lrSelect.r >= inputImageDataSize.r || lrSelect.c > inputImageDataSize.c)
            {
                for (unsigned long a = 0; a < pyramids.size(); a++)
                    pyramids[a].tiePointGood[i] = false;
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Out of bounds, going to skip this chip");
                continue;
            }
        }
    }

    if (StartThreads(pyramids, offsets, inputImageData) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from StartThreads()");
        return 1;
    }

    return 0;
}

void CTiePointGeneratorGcpPyramids::cleanUp(CVector<CCorrelationThread *> &correlationThreads, CAtomicBool &runningVar)
{
    runningVar.set(false);
    for (unsigned long t = 0; t < correlationThreads.size(); t++)
    {
        if (correlationThreads[t] != nullptr)
        {
            while (CUltraThread::getInstance()->isBusy(correlationThreads[t]))
            {
                sleep(1);
            }

            delete correlationThreads[t];
            correlationThreads[t] = nullptr;
        }
    }

    correlationThreads.clear();
}

int CTiePointGeneratorGcpPyramids::StartThreads(CVector<SPyramid> &pyramids,
                                                CVector<SPair<double> > &offsets,
                                                CMatrix<float> &inputImageData)
{
    CThreadLock sharedLock;
    CAtomicLong sharedCounter;
    CAtomicLong sharedProcessingPercentage;
    CAtomicLong threadDoneCount;
    CAtomicBool runningVar;
    CVector<CCorrelationThread *> correlationThreads;
    for (long t = ((long) pyramids.size()) - 1; t >= 0; t--)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Starting pyramid correlation level (" + toString(pyramids.size() - t) + "/" + toString(pyramids.size()) + ")");
        sharedCounter.set(0);
        threadDoneCount.set(0);
        sharedProcessingPercentage.set(-1);
        runningVar.set(true);
        correlationThreads.resize(m_context->m_threadCount);
        correlationThreads.initVec(nullptr);
        for (unsigned long a = 0; a < m_context->m_threadCount; a++)
        {
            correlationThreads[a] = new CCorrelationThread();
            if (correlationThreads[a] == nullptr)
            {
                cleanUp(correlationThreads, runningVar);
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CCorrelationThread object");
                return 1;
            }
            if (correlationThreads[a]->Init(m_context, &inputImageData,
                                            &pyramids, &offsets, t,
                                            &m_overallGcpShift, &sharedLock,
                                            &sharedCounter, &sharedProcessingPercentage,
                                            &threadDoneCount, m_context->m_threadCount,
                                            &runningVar) != 0)
            {
                cleanUp(correlationThreads, runningVar);
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CCorrelationThread::Init()");
                return 1;
            }
        }

        CUltraThreadFixedPool *pool = new CUltraThreadFixedPool(m_context->m_threadCount);
        if (pool == nullptr)
        {
            cleanUp(correlationThreads, runningVar);
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CUltraThreadPool object");
            return 1;
        }

        for (unsigned long a = 0; a < m_context->m_threadCount; a++)
        {
            pool->start(correlationThreads[a], nullptr);
        }

        if (pool->JoinAllThreads() != 0)
        {
            cleanUp(correlationThreads, runningVar);
            delete pool;
            pool = nullptr;
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CUltraThreadPool::JoinAllThreads()");
            return 1;
        }

        delete pool;
        pool = nullptr;

        int exitCode = 0;
        for (unsigned long a = 0; a < m_context->m_threadCount; a++)
        {
            exitCode |= correlationThreads[a]->getExitCode();
        }
        cleanUp(correlationThreads, runningVar);

        if (exitCode != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ProcessPyramidLevel()");
            return 1;
        }
    }
    return 0;
}

int CTiePointGeneratorGcpPyramids::GenerateResults(const CVector<SPair<double> > &tiePointCoordinates,
                                                   const SPair<double> &inputOriginAdd,
                                                   const CVector<SPyramid> &pyramids,
                                                   CVector<SChipCorrelationResult> &result)
{
    int counter = 0;
    for (unsigned long t = 0; t < m_overallGcpShift.size(); t++)
    {
        if (pyramids[0].tiePointGood[t])
        {
            counter++;
        }
    }

    SPair<double> origin = pyramids[0].originOriginal;
    SPair<double> gsd = pyramids[0].gsdOriginal;
    result.resize(counter);
    counter = 0;
    for (unsigned long t = 0; t < m_overallGcpShift.size(); t++)
    {
        if (pyramids[0].tiePointGood[t])
        {
            result[counter].correlationResultIsGood = true;
            result[counter].chip.midCoordinate = tiePointCoordinates[t] * gsd + origin;
            result[counter].newMidCoordinate = (result[counter].chip.midCoordinate + inputOriginAdd - m_overallGcpShift[t] * gsd);
            counter++;
        }
    }

    return 0;
}

int CTiePointGeneratorGcpPyramids::Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists)
{
    if (overlapExists != nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented usage of 'overlapExists'");
        return 1;
    }

    m_result = &result;
    m_result->clear();
    if (ContextCheck() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ContextCheck()");
        return 1;
    }

    std::string inImage;
    SPair<double> inputOriginAdd;
    if (PreProcessInputData(inImage, inputOriginAdd) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PreProcessInputData()");
        return 1;
    }

    CVector<SPair<double> > tiePointCoordinates;
    if (GenerateTiePoints(inImage, tiePointCoordinates) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateTiePoints()");
        return 1;
    }

    CVector<SPyramid> pyramids;
    if (PyramidLoop(inImage, tiePointCoordinates, pyramids) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PyramidLoop()");
        return 1;
    }

    if (GenerateResults(tiePointCoordinates, inputOriginAdd, pyramids, result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateResults()");
        return 1;
    }

    return 0;
}

} // namespace ultra
