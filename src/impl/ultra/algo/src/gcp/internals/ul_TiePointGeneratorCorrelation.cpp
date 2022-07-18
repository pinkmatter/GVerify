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

#include "ul_TiePointGenerator.h"

#include "ul_WorldModel.h"
#include "ul_CorrelationHandler.h"
#include "ul_UltraThread.h"
#include "ul_Resampler.h"

namespace ultra
{

int CTiePointGenerator::GenerateWorldMatrix(
                                            CMatrix<SPair<double> > &worldMatrix,
                                            const SSize &size,
                                            const SPair<double> &pixelGroundSamplingDistance,
                                            const SPair<double> &origin,
                                            const SPair<double> &subImageShift
                                            )
{
    SPair<double> newOrigin = origin;
    newOrigin += subImageShift * pixelGroundSamplingDistance;
    CWorldModel *worldModel = new CWorldModel(size, pixelGroundSamplingDistance, newOrigin);
    if (worldModel == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create world model object");
        return 1;
    }

    if (worldModel->GetWorldMatrix(worldMatrix) != 0)
    {
        delete worldModel;
        worldModel = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetWorldMatrix()");
        return 1;
    }

    delete worldModel;
    worldModel = nullptr;

    return 0;
}

int CTiePointGenerator::Correlate(
                                  CMatrix<SPair<double> > &worldMatrix,
                                  CVector<CChip<float> > &chipVector,
                                  CVector<SChipCorrelationResult> &result
                                  )
{
    result.clear();
    if (chipVector.size() == 0)
    {
        //no chips found
        return 0;
    }
    std::shared_ptr<CThreadLock> lock(new CThreadLock());
    unsigned long numberThreadsToUse = m_context.innerContext->threadCount;
    if (chipVector.size() < numberThreadsToUse)
    {
        numberThreadsToUse = chipVector.size();
    }

    std::unique_ptr<CCorrelationHandler> corr(new CCorrelationHandler(
                                                                      numberThreadsToUse,
                                                                      m_context.innerContext->correlationThreshold,
                                                                      m_context.innerContext->useNullValue(),
                                                                      m_context.innerContext->getNullValue(),
                                                                      m_context.innerContext->getMayContainNullValues()
                                                                      ));

    if (corr->LoadData(
                       m_context.images[INPUT_IMG_INDEX],
                       chipVector,
                       worldMatrix,
                       m_context.inputSceneMetadata.gsd,
                       m_context.innerContext->searchWindowSize,
                       m_context.referenceSceneMetadata.origin - m_context.inputSceneMetadata.origin,
                       m_context.innerContext->typeOfChipCorrelationTechnique
                       ) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadData()");
        return 1;
    }

    {
        AUTO_LOCK(lock);
        if (corr->Correlate(lock) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Correlate()");
            return 1;
        }
        if (lock->wait(__FILE__, __LINE__) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to wait on a mutex");
            return 1;
        }
    }


    if (corr->GetResults(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetResults()");
        return 1;
    }

    return 0;
}

} // namespace ultra
