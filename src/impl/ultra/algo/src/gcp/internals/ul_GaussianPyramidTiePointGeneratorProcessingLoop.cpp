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

#include "ul_GaussianPyramidTiePointGenerator.h"

#include "ul_ImageLoader.h"

namespace ultra
{

namespace
{

bool sortSPairFunc(const SPair<double> &l, const SPair<double> &r)
{
    return (l.modSizeSquared() > r.modSizeSquared());
}

}

int CGaussianPyramidTiePointGenerator::CalcAvgShift(
                                                    const CVector<SChipCorrelationResult> &result,
                                                    const SPair<double> &gsd,
                                                    SPair<double> &avgShift,
                                                    bool &skip
                                                    )
{
    avgShift = 0;
    CVector<SPair<double> > tempList;
    long size = result.size();
    long goodCount = 0;

    for (unsigned long t = 0; t < size; t++)
    {
        if (result[t].isGoodResult())
        {
            SPair<double> distance = (result[t].newMidCoordinate - result[t].chip.midCoordinate) / gsd;
            tempList.pushBack(distance);
        }
    }

    tempList = tempList.sort(&sortSPairFunc);

    if (tempList.size() < m_context.publicContex->minimumRequiredGcp)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Found '" + toString(tempList.size()) + "' good GCPs, but requires at least '" + toString(m_context.publicContex->minimumRequiredGcp) + "'");
        skip = true;
        return 0;
    }
    double sizeD = (double) tempList.size();
    long bot = (long) (0.1 * sizeD);
    long top = (long) (0.9 * sizeD);

    for (unsigned long t = bot; t < top; t++)
    {
        avgShift += tempList[t];
        goodCount++;
    }

    if (goodCount > 0)
    {
        avgShift /= (double) goodCount;
    }

    skip = false;
    if (goodCount == 0)
    {
        skip = true;
    }

    return 0;
}

int CGaussianPyramidTiePointGenerator::UpdateResult(
                                                    CVector<SChipCorrelationResult> &result,
                                                    const SPair<double> &avgShift
                                                    )
{
    SPair<double> gsd;
    unsigned long size = result.size();
    SPair<double> totalAddedShift;

    if (m_context.inputPyramids.size() == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector contains no images");
        return 1;
    }

    if (CImageLoader::getInstance()->LoadImageGsd(m_context.inputPyramids[0].filePath, gsd) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageGsd()");
        return 1;
    }

    totalAddedShift = avgShift * gsd;

    for (unsigned long t = 0; t < size; t++)
    {
        if (result[t].isGoodResult())
        {
            result[t].newMidCoordinate += totalAddedShift;
            for (int x = 0; x < 4; x++)
            {
                result[t].newBoundingCoordinate[x] += totalAddedShift;
            }
        }
    }

    return 0;
}

int CGaussianPyramidTiePointGenerator::GenerateGcps(CVector<SChipCorrelationResult> &result)
{
    bool skip = true;
    bool someChipsCorrelatedBefore = false;
    SPair<double> avgShift;
    CTiePointGenerator::SContext context = m_context.publicContex->tiePointGeneratorContext;

    SPair<double> gsd;
    long loops = m_context.inputPyramids.size();

    double baseThreshold = context.correlationThreshold;
    bool baseHullReject = context.outerHullRejectGcps;
    long baseSearchWindowSize = context.searchWindowSize;
    for (long t = loops - 1; t >= 0; t--)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "<<----PYRAMID LOOP (" + toString(loops - t) + "/" + toString(loops) + ")----------------->>");
        context.inputScene.pathToImage = m_context.inputPyramids[t].filePath;
        context.referenceScene.pathToImage = m_context.referencePyramids[t].filePath;
        context.correlationThreshold = baseThreshold / (t + 1);
        gsd = m_context.inputPyramids[t].imageMetadata.getGsd();
        context.outerHullRejectGcps = false;
        if (t == 0)
            context.outerHullRejectGcps = baseHullReject;

        std::unique_ptr<CTiePointGenerator> calculator(new CTiePointGenerator(&context));
        if (calculator.get() == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CSceneShiftCalculator object");
            return 1;
        }

        for (int i = 0; i < 2; i++)
        {
            if (calculator->Calculate(result) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Calculate()");
                return 1;
            }

            if (CalcAvgShift(result, gsd, avgShift, skip) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CalcAvgShift()");
                return 1;
            }
            if (t == 0 || !skip)// on last iter do not decrease correlationThreshold
                break;
            context.correlationThreshold /= 2.0;
        }

        if (t != 0)
        {
            if (skip)
            {
                context.inputOffsetShiftInPixels *= 2.0;
                context.searchWindowSize = baseSearchWindowSize;
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "No good GCPs found skipping processing loop '" + toString(loops - t) + "'");
                continue;
            }
            else
            {
                someChipsCorrelatedBefore = true;
                context.inputOffsetShiftInPixels *= 2.0;
                /*
                 * The round here is very important, the average shift calculated
                 * must only be applied to the next pyramid as an integer value
                 * This helps the sub pixel shift/error introduces which is 
                 * caused by generating the actual pyramids them self.
                 */
                context.inputOffsetShiftInPixels += avgShift.roundValues() * 2.0;
                context.searchWindowSize = 15;
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Average shift for pyramid scene '" + toString(loops - t) + "/" + toString(loops) + "' is '" + toString(avgShift) + "' pixels");
            }
        }
    }

    if (UpdateResult(result, context.inputOffsetShiftInPixels) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from UpdateResult()");
        return 1;
    }

    if (CalcAvgShift(result, gsd, avgShift, skip) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CalcAvgShift()");
        return 1;
    }

    if (!skip)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Average shift in pixels are '" + toString(avgShift) + "'");
    }
    else
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "No good GCPs found");
    }

    return 0;
}

} // namespace ultra
