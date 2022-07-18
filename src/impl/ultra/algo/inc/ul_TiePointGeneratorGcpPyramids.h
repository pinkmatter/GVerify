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

#include "ul_Int_TiePointGenerator.h"
#include "ul_DigitalImageCorrelator.h"
#include "ul_ChipsGen.h"
#include "ul_SubPixelCorrelator.h"
#include "ul_AtomicLong.h"
#include "ul_AtomicBool.h"

namespace ultra
{

class CTiePointGeneratorGcpPyramids : public ITiePointGenerator
{
public:

    struct SContext
    {
    private:
        friend class CTiePointGeneratorGcpPyramids;
        std::string m_pathToRefImage;
        std::string m_pathToInputImage;
        std::string m_pathToWorkingFolder;
        CVector<EChips::EChipType> m_chipGeneratorMethods;
        // value between 0~1 default is 0.75
        double m_correlationThreshold;
        unsigned long m_threadCount;
        unsigned long m_chipSizeMinimum;
        unsigned long m_chipGenerationGridSize;
        unsigned long m_pyramidCount;
        unsigned long m_searchWindowSize;
        bool m_useNullValue;
        float m_nullValue;
        ECorrelationType m_typeOfChipCorrelationTechnique;
        ESubPixelCorrelationType m_typeOfSubPixelChipCorrelationTechnique;
    public:

        SContext();
        SContext(const CTiePointGeneratorGcpPyramids::SContext &r);
        ~SContext();
        CTiePointGeneratorGcpPyramids::SContext &operator=(const CTiePointGeneratorGcpPyramids::SContext &r);

        void setUseNullValue(bool useNullValue, float nullValue);
        void setThreadCount(unsigned long threadCount);
        void setCorrelationThreshold(double correlationThreshold);
        void setChipSize(unsigned long chipSize, unsigned long chipGenerationGridSize);
        void setCorrelationTechnique(ECorrelationType chipCorrelationTechnique,
                                     ESubPixelCorrelationType subPixelChipCorrelationTechnique,
                                     unsigned long searchWindowSize);
        void setWorkingFolder(const std::string &workingFolderPath);
        void setInputImage(const std::string &inputImagePath);
        void setReferenceImage(const std::string &referenceImagePath);
        void addChipGenerationMethod(EChips::EChipType chipGenMethod);
        void setChipGenerationMethod(const CVector<EChips::EChipType> &chipGenMethods);
        void clearChipGenerationMethod();
        void setPyramidCount(unsigned long pyramidCount);
    };
private:

    struct SPyramid
    {
        unsigned long pyramidLevel;
        CVector<SPair<double> > tiePointCoordinatesOriginal;
        CVector<bool> tiePointGood;
        CVector<SSize> selectUlOriginal;
        CVector<SPair<double> > tiePointCoordinates;
        double modVal;
        SSize selectSizeOriginal;
        SPair<double> originOriginal;
        SPair<double> gsdOriginal;
        SPair<double> gsd;
    };

    class CCorrelationThread : public IRunnable
    {
    private:
        CTiePointGeneratorGcpPyramids::SContext *m_context;
        std::unique_ptr<CSubPixelCorrelator<float> > m_correlator;
        int m_exitCode;
        SImageMetadata m_refMetadata;
        CMatrix<float> *m_inputImageData;
        CVector<CTiePointGeneratorGcpPyramids::SPyramid> *m_pyramids;
        CVector<SPair<double> > *m_offsets;
        unsigned long m_pyramidIndex;
        CVector<SPair<double> > *m_overallGcpShift;
        CThreadLock *m_sharedLock;
        CAtomicLong *m_sharedCounter;
        CAtomicLong *m_sharedProcessingPercentage;
        CAtomicBool *m_runningVar;
        CAtomicLong *m_threadDoneCount;
        unsigned long m_totalRunningThreads;
        unsigned long m_gcpTotalCount;

        int ResampleData(const CMatrix<float> &originalData,
                         CMatrix<float> &resampledData,
                         const CMatrix<SPair<float> > &resampleMap,
                         CTiePointGeneratorGcpPyramids::SPyramid &pyramid);
        int ExtractRefImageData(const SImageMetadata &refMetadata,
                                const SSize &tileSize,
                                CTiePointGeneratorGcpPyramids::SPyramid &pyramid,
                                CMatrix<float> &refDataOriginal,
                                bool &refDataFound,
                                unsigned long pyramidIndex,
                                const CVector<SPair<double> > &offsets,
                                SPair<double> &tileOrigin);
        int SelectSubImage(CMatrix<float> &image, bool imageMustBeEven) const;
        int ProcessPyramidLevel(const SImageMetadata &refMetadata,
                                const CMatrix<float> &inputImageData,
                                CVector<CTiePointGeneratorGcpPyramids::SPyramid> &pyramids,
                                CVector<SPair<double> > &offsets,
                                unsigned long pyramidIndex);
        int CorrelateData(const CMatrix<float> &refDataResampled, const CMatrix<float> &chipDataResampled,
                          CVector<CTiePointGeneratorGcpPyramids::SPyramid> &pyramid, unsigned long pyramidIndex,
                          CVector<SPair<double> > &offsets, unsigned long subPyramidIndex);
    public:
        CCorrelationThread();
        virtual ~CCorrelationThread();
        int Init(CTiePointGeneratorGcpPyramids::SContext *context,
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
                 CAtomicBool *runningVar);
        int getExitCode() const;
        virtual void run(void *context = nullptr);
    };
private:
    CTiePointGeneratorGcpPyramids::SContext *m_context;
    CVector<SChipCorrelationResult> *m_result;
    CVector<SPair<double> > m_overallGcpShift;

    int ContextCheck();
    int CreateFolder(const std::string &path);
    int PreProcessInputData(std::string &inImage, SPair<double> &inputOriginAdd);

    CChipsGen<float>::CChipsGenContext *createEvenChipContext();
    CChipsGen<float>::CChipsGenContext *genDefaultChipGenContext(EChips::EChipType genType);
    int GenerateTiePoints(const std::string &imagePath, CVector<SPair<double> > &tiePointCoordinates);
    int PyramidLoop(const std::string &imagePath,
                    const CVector<SPair<double> > &tiePointCoordinates,
                    CVector<SPyramid> &pyramids);
    int GenerateResults(const CVector<SPair<double> > &tiePointCoordinates,
                        const SPair<double> &inputOriginAdd,
                        const CVector<SPyramid> &pyramids,
                        CVector<SChipCorrelationResult> &result);
    void cleanUp(CVector<CCorrelationThread *> &correlationThreads, CAtomicBool &runningVar);
    int StartThreads(CVector<SPyramid> &pyramids,
                     CVector<SPair<double> > &offsets,
                     CMatrix<float> &inputImageData);
public:
    CTiePointGeneratorGcpPyramids(CTiePointGeneratorGcpPyramids::SContext *context);
    virtual ~CTiePointGeneratorGcpPyramids();

    virtual int Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists = nullptr) override;
};

} // namespace ultra
