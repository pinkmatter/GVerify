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

#include "ul_Dft.h"

namespace ultra
{

enum class ECorrelationType
{
    CCOEFF_NORM = 0,
    PHASE,
    CORRELATION_TYPE_COUNT
};

enum class ESubPixelCorrelationType
{
    LEAST_SQUARE_SUB_PIXEL = 0,
    PHASE_SUB_PIXEL,
    SUB_PIXEL_CORRELATION_TYPE_COUNT
};

class CCorrelationHelper
{
public:
    CCorrelationHelper() = delete;
    ~CCorrelationHelper() = delete;
    static std::string typeToStr(ECorrelationType type);
    static std::string typeToStr(ESubPixelCorrelationType type);
};

template<class T>
class ADigitalImageCorrelator
{
protected:
    const SSize m_inputSize;
public:

    ADigitalImageCorrelator(const SSize &inputSize) :
    m_inputSize(inputSize)
    {
    }

    virtual ~ADigitalImageCorrelator()
    {
    }

    virtual void correlate(const CMatrix<T> &reference, const CMatrix<T> &input, CMatrix<double> &output,
                           const T *refNoData = nullptr, const T *inputNoData = nullptr) const = 0;
    virtual void correlate(const CMatrix<SComplex<T> > &reference, const CMatrix<SComplex<T> > &input, CMatrix<double> &output,
                           const SComplex<T> *refNoData = nullptr, const SComplex<T> *inputNoData = nullptr) const = 0;
};

namespace __ultra_internal
{

namespace correlate_phase
{

class CPhaseCorrelatorImpl
{
private:
    void correlateComplexWt(CDft2d *fftOp, const CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > &reference,
                            const CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > &input, CMatrix<double> &output,
                            const SComplex<ADftBase2d::MY_WORKING_TYPE> *refNoData, const SComplex<ADftBase2d::MY_WORKING_TYPE> *inputNoData) const;
public:
    CPhaseCorrelatorImpl();
    virtual ~CPhaseCorrelatorImpl();

    template<class T>
    void correlateNormal(CDft2d *fftOp, const CMatrix<T> &reference, const CMatrix<T> &input, CMatrix<double> &output,
                         const T *refNoData, const T *inputNoData) const
    {
        auto fp = [](const T & v)->SComplex<ADftBase2d::MY_WORKING_TYPE>
        {
            return SComplex<ADftBase2d::MY_WORKING_TYPE>(v, 0);
        };
        auto nRef = reference.template map<SComplex<ADftBase2d::MY_WORKING_TYPE> > (fp);
        auto nIn = input.template map<SComplex<ADftBase2d::MY_WORKING_TYPE> > (fp);
        SComplex<ADftBase2d::MY_WORKING_TYPE> nRefNd;
        SComplex<ADftBase2d::MY_WORKING_TYPE> *nRefNdPtr = nullptr;
        if (refNoData != nullptr)
        {
            nRefNd = fp(*refNoData);
            nRefNdPtr = &nRefNd;
        }
        SComplex<ADftBase2d::MY_WORKING_TYPE> nInNd;
        SComplex<ADftBase2d::MY_WORKING_TYPE> *nInNdPtr = nullptr;
        if (inputNoData != nullptr)
        {
            nInNd = fp(*inputNoData);
            nInNdPtr = &nInNd;
        }
        correlateComplexWt(fftOp, nRef, nIn, output, nRefNdPtr, nInNdPtr);
    }

    template<class T>
    void correlateComplex(CDft2d *fftOp, const CMatrix<SComplex<T> > &reference, const CMatrix<SComplex<T> > &input,
                          CMatrix<double> &output, const SComplex<T> *refNoData, const SComplex<T> *inputNoData) const
    {
        auto fp = [](const SComplex<T> & v)->SComplex<ADftBase2d::MY_WORKING_TYPE>
        {
            return v.template convertType<ADftBase2d::MY_WORKING_TYPE>();
        };
        auto nRef = reference.template map<SComplex<ADftBase2d::MY_WORKING_TYPE> > (fp);
        auto nIn = input.template map<SComplex<ADftBase2d::MY_WORKING_TYPE> > (fp);
        SComplex<ADftBase2d::MY_WORKING_TYPE> nRefNd;
        SComplex<ADftBase2d::MY_WORKING_TYPE> *nRefNdPtr = nullptr;
        if (refNoData != nullptr)
        {
            nRefNd = fp(*refNoData);
            nRefNdPtr = &nRefNd;
        }
        SComplex<ADftBase2d::MY_WORKING_TYPE> nInNd;
        SComplex<ADftBase2d::MY_WORKING_TYPE> *nInNdPtr = nullptr;
        if (inputNoData != nullptr)
        {
            nInNd = fp(*inputNoData);
            nInNdPtr = &nInNd;
        }
        correlateComplexWt(fftOp, nRef, nIn, output, nRefNdPtr, nInNdPtr);
    }
};

template<class T>
class CPhaseDigitalImageCorrelator : public ADigitalImageCorrelator<T>
{
private:
    std::shared_ptr<CDft2d> m_fft2d;
    CDft2d *m_fft2dPtr;
    CPhaseCorrelatorImpl m_corImpl;
public:

    CPhaseDigitalImageCorrelator(const SSize &inputSize) :
    ADigitalImageCorrelator<T>(inputSize),
    m_fft2d(std::make_shared<CDft2d>(inputSize)),
    m_fft2dPtr(m_fft2d.get())
    {
        if (inputSize.row % 2 != 0 || inputSize.col % 2 != 0)
            throw CException(__FILE__, __LINE__, "Phase correlation requires that the matrix sizes be of even size");
    }

    virtual ~CPhaseDigitalImageCorrelator()
    {
    }

    virtual void correlate(const CMatrix<T> &reference, const CMatrix<T> &input, CMatrix<double> &output,
                           const T *refNoData = nullptr, const T *inputNoData = nullptr) const override
    {
        m_corImpl.template correlateNormal<T>(m_fft2dPtr, reference, input, output, refNoData, inputNoData);
    }

    virtual void correlate(const CMatrix<SComplex<T> > &reference, const CMatrix<SComplex<T> > &input, CMatrix<double> &output,
                           const SComplex<T> *refNoData = nullptr, const SComplex<T> *inputNoData = nullptr) const override
    {
        m_corImpl.template correlateComplex<T>(m_fft2dPtr, reference, input, output, refNoData, inputNoData);
    }
};

} // namespace correlate_phase

namespace correlate_coef_norm
{

class CCoefNormCorrelatorImpl
{
private:
    using MY_WORKING_TYPE = double;

    void correlateNormalWt(const CMatrix<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> &reference,
                           const CMatrix<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> &input,
                           CMatrix<double> &output,
                           const CCoefNormCorrelatorImpl::MY_WORKING_TYPE *refNoData,
                           const CCoefNormCorrelatorImpl::MY_WORKING_TYPE *inputNoData) const;
    void correlateComplexWt(const CMatrix<SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> > &reference,
                            const CMatrix<SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> > &input,
                            CMatrix<double> &output,
                            const SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> *refNoData,
                            const SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> *inputNoData) const;
public:
    CCoefNormCorrelatorImpl();
    virtual ~CCoefNormCorrelatorImpl();

    template<class T>
    void correlateNormal(const CMatrix<T> &reference, const CMatrix<T> &input, CMatrix<double> &output,
                         const T *refNoData, const T *inputNoData) const
    {
        auto nRef = reference.template convertType<MY_WORKING_TYPE>();
        auto nIn = input.template convertType<MY_WORKING_TYPE>();
        MY_WORKING_TYPE nRefNd;
        MY_WORKING_TYPE *nRefNdPtr = nullptr;
        if (refNoData != nullptr)
        {
            nRefNd = (MY_WORKING_TYPE) (*refNoData);
            nRefNdPtr = &nRefNd;
        }
        MY_WORKING_TYPE nInNd;
        MY_WORKING_TYPE *nInNdPtr = nullptr;
        if (inputNoData != nullptr)
        {
            nInNd = (MY_WORKING_TYPE) (*inputNoData);
            nInNdPtr = &nInNd;
        }
        correlateNormalWt(nRef, nIn, output, nRefNdPtr, nInNdPtr);
    }

    template<class T>
    void correlateComplex(const CMatrix<SComplex<T> > &reference, const CMatrix<SComplex<T> > &input,
                          CMatrix<double> &output, const SComplex<T> *refNoData, const SComplex<T> *inputNoData) const
    {
        auto fp = [](const SComplex<T> & v)->SComplex<MY_WORKING_TYPE>
        {
            return v.template convertType<MY_WORKING_TYPE>();
        };
        auto nRef = reference.template map<SComplex<MY_WORKING_TYPE> >(fp);
        auto nIn = input.template map<SComplex<MY_WORKING_TYPE> >(fp);

        SComplex<MY_WORKING_TYPE> nRefNd;
        SComplex<MY_WORKING_TYPE> *nRefNdPtr = nullptr;
        if (refNoData != nullptr)
        {
            nRefNd = refNoData->template convertType<MY_WORKING_TYPE>();
            nRefNdPtr = &nRefNd;
        }
        SComplex<MY_WORKING_TYPE> nInNd;
        SComplex<MY_WORKING_TYPE> *nInNdPtr = nullptr;
        if (inputNoData != nullptr)
        {
            nInNd = inputNoData->template convertType<MY_WORKING_TYPE>();
            nInNdPtr = &nInNd;
        }
        correlateComplexWt(nRef, nIn, output, nRefNdPtr, nInNdPtr);
    }
};

template<class T>
class CCoefNormDigitalImageCorrelator : public ADigitalImageCorrelator<T>
{
private:
    CCoefNormCorrelatorImpl m_corImpl;
public:

    CCoefNormDigitalImageCorrelator(const SSize &inputSize) :
    ADigitalImageCorrelator<T>(inputSize)
    {
    }

    virtual ~CCoefNormDigitalImageCorrelator()
    {
    }

    virtual void correlate(const CMatrix<T> &reference, const CMatrix<T> &input, CMatrix<double> &output,
                           const T *refNoData = nullptr, const T *inputNoData = nullptr) const override
    {
        m_corImpl.template correlateNormal<T>(reference, input, output, refNoData, inputNoData);
    }

    virtual void correlate(const CMatrix<SComplex<T> > &reference, const CMatrix<SComplex<T> > &input, CMatrix<double> &output,
                           const SComplex<T> *refNoData = nullptr, const SComplex<T> *inputNoData = nullptr) const override
    {
        m_corImpl.template correlateComplex<T>(reference, input, output, refNoData, inputNoData);
    }
};

} // namespace correlate_coef_norm

} // namespace __ultra_internal

class CDigitalImageCorrelatorFactory
{
public:
    CDigitalImageCorrelatorFactory() = delete;
    virtual ~CDigitalImageCorrelatorFactory() = delete;

    template<class T>
    static std::shared_ptr<ADigitalImageCorrelator<T> > create(ECorrelationType correlationMethod, const SSize &inputSize)
    {
        std::shared_ptr<ADigitalImageCorrelator<T> > correlator;
        switch (correlationMethod)
        {
        case ECorrelationType::CCOEFF_NORM:
            correlator = std::make_shared<__ultra_internal::correlate_coef_norm::CCoefNormDigitalImageCorrelator<T> >(inputSize);
            break;
        case ECorrelationType::PHASE:
            correlator = std::make_shared<__ultra_internal::correlate_phase::CPhaseDigitalImageCorrelator<T> >(inputSize);
            break;
        default:
            throw CException(__FILE__, __LINE__, "No implementation found");
        }
        return correlator;
    }
};

} // namespace ultra
