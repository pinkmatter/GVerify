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

#include "ul_GcpHullRejection.h"

namespace ultra
{

int CTiePointGenerator::HullRejectGcps(CVector<SChipCorrelationResult> &result)
{
    CMatrixArray<float> refImage, inImage;
    SImageMetadata refMetadata, inputMetadata;
    unsigned long hullTrimAmount = 0;

    // can only use hull rejection if a nullptr value is supplied
    bool hullReject = m_context.innerContext->outerHullRejectGcps;
    if (!m_context.innerContext->useNullValue())
        hullReject = false;

    if (!hullReject)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Not going to reject GCPs based on the image outer hulls");
        return 0;
    }

    if (!m_context.innerContext->useNullValue())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "nullptr value needs to be specified if GCP hull rejection is to be executed");
        return 1;
    }

    if (LoadCompleteImages(refImage, refMetadata, inImage, inputMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadCompleteImages()");
        return 1;
    }

    CGcpHullRejection *reject = new CGcpHullRejection();
    if (reject == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CGcpHullRejection object");
        return 1;
    }

    for (unsigned long t = 0; t < result.size(); t++)
    {
        if (result[t].chip.chipData.getSize().row > hullTrimAmount)
            hullTrimAmount = result[t].chip.chipData.getSize().row;
        if (result[t].chip.chipData.getSize().col > hullTrimAmount)
            hullTrimAmount = result[t].chip.chipData.getSize().col;
    }

    if (reject->Reject(refImage, refMetadata, inImage, inputMetadata, m_context.innerContext->getNullValue(), hullTrimAmount, &result) != 0)
    {
        delete reject;
        reject = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Reject()");
        return 1;
    }

    delete reject;
    reject = nullptr;
    return 0;
}

} // namespace ultra
