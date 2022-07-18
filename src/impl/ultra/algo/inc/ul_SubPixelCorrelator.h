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

#include "ul_DigitalImageCorrelator.h"
#include "ul_LeastSquares.h"

#include "ul_Pair.h"

namespace ultra
{

namespace __ultra_internal
{

enum class EdgeType
{
    CENTER = 0,
    X,
    Y,
    FULL,
    UNKNOWN
};

class ICorrelateInterface
{
public:
    ICorrelateInterface();
    virtual ~ICorrelateInterface();
    virtual int correlate(const CMatrix<double> &scratchImage,
                          const SSize &maxPeakLoc, double maxPeakVal,
                          const double &corrThreshold, bool &success,
                          SPair<double> &templateToInputOffset, EdgeType edgeType) = 0;
};

class CCorrelatePhaseImpl : public ICorrelateInterface
{
private:
    CMatrix<double> m_neighbors;
    SPair<double> m_templateImageSizeHalf;
    SSize m_size3x3;
public:

    CCorrelatePhaseImpl(const SPair<double> &templateImageSizeHalf);
    virtual ~CCorrelatePhaseImpl();
    virtual int correlate(const CMatrix<double> &scratchImage,
                          const SSize &maxPeakLoc, double maxPeakVal,
                          const double &corrThreshold, bool &success,
                          SPair<double> &templateToInputOffset, EdgeType edgeType) override;
};

class CCorrelateSpacePolyImpl : public ICorrelateInterface
{
private:
    SSize m_size3x3;
    SSize m_size3x1;
    SSize m_size1x3;
    CVector<double> m_vec3;
    CMatrix<double> m_A, m_B;
    CVector<double> m_leastSquareAns;
private:

    static CVector<double> genVec(double r, double c);
public:

    CCorrelateSpacePolyImpl();
    virtual ~CCorrelateSpacePolyImpl();
    virtual int correlate(const CMatrix<double> &scratchImage,
                          const SSize &maxPeakLoc, double maxPeakVal,
                          const double &corrThreshold, bool &success,
                          SPair<double> &templateToInputOffset, EdgeType edgeType) override;
};
} // namespace __ultra_internal

template<class T>
class CSubPixelCorrelator
{
private:
    std::shared_ptr<ADigitalImageCorrelator<T> > m_correlator;
    CMatrix<double> m_scratchImage;
    SSize m_maxPeakLoc;
    T m_maxPeakVal;
    CVector<SSize> m_minPeaks;
    CVector<SSize> m_maxPeaks;
    bool m_isPhase;

    std::unique_ptr<__ultra_internal::ICorrelateInterface> m_subCorrelator;
    int (CSubPixelCorrelator<T>::*m_fp_calcCorrCoef)(double &);

    int CalcCorrCoefPhase(double &corrCoefficient)
    {
        unsigned long peakCount = 4;
        if (m_scratchImage.peaks(m_minPeaks, m_maxPeaks, peakCount) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CMatrix::peaks()");
            return 1;
        }

        corrCoefficient = m_scratchImage[m_maxPeaks[0]];
        m_maxPeakLoc = m_maxPeaks[0];
        m_maxPeakVal = m_scratchImage[m_maxPeaks[0]];
        long maxRow = m_maxPeaks[0].row;
        long maxCol = m_maxPeaks[0].col;
        for (unsigned long t = 1; t < peakCount; t++)
        {
            long difRow = getAbs(maxRow - (long) (m_maxPeaks[t].row));
            long difCol = getAbs(maxCol - (long) (m_maxPeaks[t].col));
            if (difRow <= 1 && difCol <= 1)
                corrCoefficient += m_scratchImage[m_maxPeaks[t]];
        }

        return 0;
    }

    int CalcCorrCoefSpace(double &corrCoefficient)
    {
        m_maxPeakLoc = m_scratchImage.maxLoc();
        corrCoefficient = m_maxPeakVal = m_scratchImage[m_maxPeakLoc];
        return 0;
    }

    bool isOnEdge(const CMatrix<double> &img, const SSize &point, __ultra_internal::EdgeType &edgeType) const
    {
        SSize s = img.getSize();
        if (s.col == 1 && s.row == 1)
        {
            edgeType = __ultra_internal::EdgeType::CENTER;
            return false;
        }
        else if (s.col == 1)
        {
            edgeType = __ultra_internal::EdgeType::Y;
            return point.row == 0 || point.row + 1 == s.row;
        }
        else if (s.row == 1)
        {
            edgeType = __ultra_internal::EdgeType::X;
            return point.col == 0 || point.col + 1 == s.col;
        }
        else
        {
            edgeType = __ultra_internal::EdgeType::FULL;
            return img.isOnEdge(point);
        }
    }

    template<class N>
    int CorrelateImpl(const CMatrix<N> &inputImage,
                      const CMatrix<N> &templateImage,
                      const double &corrThreshold,
                      bool &success,
                      SPair<double> &templateToInputOffset,
                      double &corrCoefficient,
                      const N* inputNullValue,
                      const N* templateNullValue,
                      bool mayInputContainNullValues,
                      bool mayTemplateContainNullValues)
    {
        success = false;
        if (m_isPhase)
            mayInputContainNullValues = mayTemplateContainNullValues = false;
        if (!mayTemplateContainNullValues && templateNullValue != nullptr && templateImage.contains(*templateNullValue))
            return 0;

        if (!mayInputContainNullValues && inputNullValue != nullptr && inputImage.contains(*inputNullValue))
            return 0;

        if (m_isPhase)
            m_correlator->correlate(inputImage, templateImage, m_scratchImage);
        else
            m_correlator->correlate(inputImage, templateImage, m_scratchImage, inputNullValue, templateNullValue);

        if ((this->*m_fp_calcCorrCoef)(corrCoefficient) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ::m_fp_calcCorrCoef()");
            return 1;
        }

        if (corrCoefficient < corrThreshold)
            return 0;

        __ultra_internal::EdgeType edgeType = __ultra_internal::EdgeType::UNKNOWN;
        if (isOnEdge(m_scratchImage, m_maxPeakLoc, edgeType))
            return 0;

        if (m_subCorrelator->correlate(m_scratchImage, m_maxPeakLoc, m_maxPeakVal, corrThreshold, success, templateToInputOffset, edgeType) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SubCorrelator()");
            return 1;
        }
        return 0;
    }

public:

    CSubPixelCorrelator(const SSize &templateImageSize,
                        ECorrelationType corrType,
                        ESubPixelCorrelationType subPixelCorrType)
    {
        SPair<double> templateImageSizeD = templateImageSize;
        SPair<double> templateImageSizeHalf = templateImageSizeD / 2.0;
        m_correlator = CDigitalImageCorrelatorFactory::create<T>(corrType, templateImageSize);

        m_fp_calcCorrCoef = nullptr;
        m_isPhase = false;
        if (corrType == ECorrelationType::PHASE)
        {
            m_isPhase = true;
            m_fp_calcCorrCoef = &CSubPixelCorrelator<T>::CalcCorrCoefPhase;
            if (subPixelCorrType == ESubPixelCorrelationType::PHASE_SUB_PIXEL)
                m_subCorrelator.reset(new __ultra_internal::CCorrelatePhaseImpl(templateImageSizeHalf));
            else
            {
                std::string corrName = CCorrelationHelper::typeToStr(corrType);
                std::string expected = CCorrelationHelper::typeToStr(ESubPixelCorrelationType::PHASE_SUB_PIXEL);
                throw CException(__FILE__, __LINE__, corrName + " correlator requires sub pixel correlation type(s) '" + expected + "'");
            }
        }
        else
        {
            m_fp_calcCorrCoef = &CSubPixelCorrelator<T>::CalcCorrCoefSpace;
            if (subPixelCorrType == ESubPixelCorrelationType::LEAST_SQUARE_SUB_PIXEL)
                m_subCorrelator.reset(new __ultra_internal::CCorrelateSpacePolyImpl());
            else
            {
                std::string corrName = CCorrelationHelper::typeToStr(corrType);
                std::string expected = CCorrelationHelper::typeToStr(ESubPixelCorrelationType::LEAST_SQUARE_SUB_PIXEL);
                throw CException(__FILE__, __LINE__, corrName + " correlator requires sub pixel correlation type(s) '" + expected + "'");
            }
        }
        if (!m_subCorrelator)
            throw CException(__FILE__, __LINE__, "Failed to set function pointer to correct correlator");
        if (m_fp_calcCorrCoef == nullptr)
            throw CException(__FILE__, __LINE__, "Failed to set function point to correct correlation coefficient calculator");
    }

    virtual ~CSubPixelCorrelator()
    {
    }

    int Correlate(const CMatrix<T> &inputImage,
                  const CMatrix<T> &templateImage,
                  const double &corrThreshold,
                  bool &success,
                  SPair<double> &templateToInputOffset,
                  double &corrCoefficient,
                  const T* inputNullValue = nullptr,
                  const T* templateNullValue = nullptr,
                  bool mayInputContainNullValues = true,
                  bool mayTemplateContainNullValues = true)
    {
        return CorrelateImpl<T>(inputImage, templateImage,
            corrThreshold,
            success,
            templateToInputOffset,
            corrCoefficient,
            inputNullValue,
            templateNullValue,
            mayInputContainNullValues,
            mayTemplateContainNullValues);
    }

    int Correlate(const CMatrix<SComplex<T> > &inputImage,
                  const CMatrix<SComplex<T> > &templateImage,
                  const double &corrThreshold,
                  bool &success,
                  SPair<double> &templateToInputOffset,
                  double &corrCoefficient,
                  const SComplex<T>* inputNullValue = nullptr,
                  const SComplex<T>* templateNullValue = nullptr,
                  bool mayInputContainNullValues = true,
                  bool mayTemplateContainNullValues = true)
    {
        return CorrelateImpl<SComplex<T> >(inputImage, templateImage,
            corrThreshold,
            success,
            templateToInputOffset,
            corrCoefficient,
            inputNullValue,
            templateNullValue,
            mayInputContainNullValues,
            mayTemplateContainNullValues);
    }
};

} // namespace ultra
