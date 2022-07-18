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

#include <ul_Chips.h>
#include <ul_UltraThread.h>

namespace ultra
{

class CGcpHullRejection
{
private:

    class CHullGenThread : public IRunnable
    {
    private:
        CMatrix<float> *m_image;
        CMatrix<float> *m_hull;
        float m_nullValue;
        float m_nonNullValue;
        int m_exitCode;
        unsigned long m_trimAmount;
        unsigned long m_paddSize;

        int GenerateConvexHullImage(const CMatrix<float> *image, CMatrix<float> &hull) const;
        int FillPixelDataArea(const CMatrix<float> *image, CMatrix<float> &hull) const;
        int TrimHullArea(CMatrix<float> &hull) const;
        template<class IO_TYPE, class WORKING_TYPE>
        int GenOuterHull(const CMatrix<IO_TYPE> &inMat, CMatrix<IO_TYPE> &outMat, IO_TYPE clipVal, IO_TYPE insertVal) const;
    public:
        CHullGenThread(
                       CMatrix<float> *image, CMatrix<float> *hull,
                       float nullValue,
                       float nonNullValue,
                       unsigned long trimAmount,
                       unsigned long paddSize
                       );
        virtual ~CHullGenThread();

        virtual void run(void *context = nullptr) override;
        int GetExitCode() const;
    };
private:
    CMatrix<float> m_refImage;
    CMatrix<float> m_inImage;
    CMatrix<float> m_refHull;
    CMatrix<float> m_inHull;
    float m_nullValue;
    float m_nonNullValue;
    unsigned long m_trimAmount;
    unsigned long m_paddSize;
    CVector<SChipCorrelationResult> *m_gcps;
    CVector<CHullGenThread *> m_threads;

    void clean();
    void removeThread(CHullGenThread *thread);
    int GetSceneCoordinates(
                            const SChipCorrelationResult &gcp,
                            const SPair<double> &refUl,
                            const SPair<double> &refGsd,
                            const SPair<double> &refSize,
                            const SPair<double> &inUl,
                            const SPair<double> &inGsd,
                            const SPair<double> &inSize,
                            SSize &refPoint, SSize &inputPoint,
                            bool &canUse
                            );
    int RejectGcps(
                   const SImageMetadata &refMetadata,
                   const SImageMetadata &inMetadata,
                   long &rejectionCount
                   );
    int InnerReject(
                    const CMatrix<float> &refImage,
                    const SImageMetadata &refMetadata,
                    const CMatrix<float> &inImage,
                    const SImageMetadata &inMetadata,
                    float nullValue,
                    unsigned long hullTrimAmount,
                    CVector<SChipCorrelationResult> *gcps,
                    long &rejectionCount
                    );

public:
    CGcpHullRejection();
    virtual ~CGcpHullRejection();

    int Reject(
               const CVector<CMatrix<float> > &refImage,
               const SImageMetadata &refMetadata,
               const CVector<CMatrix<float> > &inImage,
               const SImageMetadata &inMetadata,
               float nullValue,
               unsigned long hullTrimAmount,
               CVector<SChipCorrelationResult> *gcps
               );
};

} // namespace ultra
