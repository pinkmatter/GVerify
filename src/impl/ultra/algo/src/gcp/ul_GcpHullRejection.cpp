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

#include "ul_GcpHullRejection.h"

#include <ul_Logger.h>

namespace ultra
{

CGcpHullRejection::CGcpHullRejection()
{
    m_paddSize = 5;
    m_threads.clear();
}

CGcpHullRejection::~CGcpHullRejection()
{
    clean();
}

void CGcpHullRejection::removeThread(CHullGenThread *thread)
{
    if (thread == nullptr)
        return;

    if (CUltraThread::getInstance()->join(thread))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CUltraThread::join()");
    }
}

void CGcpHullRejection::clean()
{
    for (unsigned long t = 0; t < m_threads.size(); t++)
    {
        if (m_threads[t] != nullptr)
        {
            delete m_threads[t];
            m_threads[t] = nullptr;
        }
    }

    m_threads.clear();
}

int CGcpHullRejection::GetSceneCoordinates(
                                           const SChipCorrelationResult &gcp,
                                           const SPair<double> &refUl,
                                           const SPair<double> &refGsd,
                                           const SPair<double> &refSize,
                                           const SPair<double> &inUl,
                                           const SPair<double> &inGsd,
                                           const SPair<double> &inSize,
                                           SSize &refPoint, SSize &inputPoint,
                                           bool &canUse
                                           )
{
    SPair<double> refPointPair = (gcp.chip.midCoordinate - refUl) / refGsd;
    SPair<double> inPointPair = (gcp.newMidCoordinate - inUl) / inGsd;
    canUse = true;

    refPointPair = refPointPair.roundValues();
    inPointPair = inPointPair.roundValues();

    if (refPointPair.r < 0 || refPointPair.c < 0 ||
        refPointPair.r >= refSize.r || refPointPair.c >= refSize.c)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Calculated reference GCP coordinate is outside of scene bounds");
        canUse = false;
        return 0;
    }

    if (inPointPair.r < 0 || inPointPair.c < 0 ||
        inPointPair.r >= inSize.r || inPointPair.c >= inSize.c)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Calculated input GCP coordinate is outside of scene bounds");
        canUse = false;
        return 0;
    }

    refPoint = refPointPair.roundValues().getSizeType();
    inputPoint = inPointPair.roundValues().getSizeType();

    return 0;
}

int CGcpHullRejection::RejectGcps(
                                  const SImageMetadata &refMetadata,
                                  const SImageMetadata &inMetadata,
                                  long &rejectionCount
                                  )
{
    unsigned long size = m_gcps->size();
    SSize refPoint, inPoint;
    SPair<double> refUl = refMetadata.getOrigin();
    SPair<double> refGsd = refMetadata.getGsd();
    SPair<double> refSize = refMetadata.getDimensions();
    SPair<double> inUl = inMetadata.getOrigin();
    SPair<double> inGsd = inMetadata.getGsd();
    SPair<double> inSize = refMetadata.getDimensions();
    bool canUse;

    if (m_refHull.getSize() != refMetadata.getDimensions())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Reference image and reference hull image have different dimensions");
        return 1;
    }

    if (m_inHull.getSize() != inMetadata.getDimensions())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Reference image and reference hull image have different dimensions");
        return 1;
    }

    int per = 0;
    for (unsigned long t = 0; t < size; t++)
    {
        if (GetSceneCoordinates((*m_gcps)[t], refUl, refGsd, refSize, inUl, inGsd, inSize, refPoint, inPoint, canUse) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GetSceneCoordinates()");
            return 1;
        }

        bool rejected = false;
        if (!canUse)
        {
            (*m_gcps)[t].correlationResultIsGood = false;
            rejected = true;
        }

        if (!(rejected) &&
            (m_refHull[refPoint] == m_nullValue ||
            m_inHull[inPoint] == m_nullValue))
        {
            rejectionCount++;
            (*m_gcps)[t].correlationResultIsGood = false;
        }

        if (per != (int) ((t * 100) / size))
        {
            per = (int) ((t * 100) / size);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "RejectGcps " + toString(per) + "%     \r");
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "RejectGcps 100%     ");
    return 0;
}

int CGcpHullRejection::InnerReject(
                                   const CMatrix<float> &refImage,
                                   const SImageMetadata &refMetadata,
                                   const CMatrix<float> &inImage,
                                   const SImageMetadata &inMetadata,
                                   float nullValue,
                                   unsigned long hullTrimAmount,
                                   CVector<SChipCorrelationResult> *gcps,
                                   long &rejectionCount
                                   )
{
    m_refImage = refImage;
    m_inImage = inImage;
    m_nullValue = nullValue;
    m_gcps = gcps;
    m_trimAmount = hullTrimAmount;
    m_nonNullValue = m_nullValue + 1;

    if (m_gcps == nullptr || m_refImage.getSize().containsZero() ||
        m_inImage.getSize().containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input parameters are nullptr or empty");
        return 1;
    }

    if (m_trimAmount == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Nothing to trim off hull this exercise is useless");
        return 0;
    }

    clean();

    m_threads.resize(2);

    m_threads[0] = new CHullGenThread(&m_refImage, &m_refHull, m_nullValue, m_nonNullValue, m_trimAmount, m_paddSize);
    m_threads[1] = new CHullGenThread(&m_inImage, &m_inHull, m_nullValue, m_nonNullValue, m_trimAmount, m_paddSize);

    if (m_threads[0] == nullptr || m_threads[1] == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CHullGenThread object");
        return 1;
    }

    if (CUltraThread::getInstance()->start(m_threads[0], nullptr) != 0 ||
        CUltraThread::getInstance()->start(m_threads[1], nullptr) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CUltraThread::start()");
        return 1;
    }

    if (CUltraThread::getInstance()->join(m_threads[0]) != 0 ||
        CUltraThread::getInstance()->join(m_threads[1]) != 0)
    {
        removeThread(m_threads[0]);
        removeThread(m_threads[1]);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CUltraThread::join()");
        return 1;
    }

    if (m_threads[0]->GetExitCode() != 0 ||
        m_threads[1]->GetExitCode() != 0)
    {
        removeThread(m_threads[0]);
        removeThread(m_threads[1]);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to generate convex hulls");
        return 1;
    }

    removeThread(m_threads[0]);
    removeThread(m_threads[1]);

    if (RejectGcps(refMetadata, inMetadata, rejectionCount) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from RejectGcps()");
        return 1;
    }

    return 0;
}

int CGcpHullRejection::Reject(
                              const CVector<CMatrix<float> > &refImage,
                              const SImageMetadata &refMetadata,
                              const CVector<CMatrix<float> > &inImage,
                              const SImageMetadata &inMetadata,
                              float nullValue,
                              unsigned long hullTrimAmount,
                              CVector<SChipCorrelationResult> *gcps
                              )
{
    long rejectionCount = 0;
    unsigned long size = refImage.size();
    if (size == 0 || inImage.size() == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input matrix vectors are empty");
        return 1;
    }

    if (size != inImage.size())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Reference and input matrix vectors must contain the same amount of images");
        return 1;
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Hull trim size is '" + toString(hullTrimAmount) + "'");

    for (int t = 0; t < size; t++)
    {
        if (InnerReject(refImage[t], refMetadata, inImage[t], inMetadata, nullValue, hullTrimAmount, gcps, rejectionCount) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from InnerReject()");
            return 1;
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Rejected a total of '" + toString(rejectionCount) + "' GCP on the outer hull edges");

    return 0;
}

} // namespace ultra
