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

#include "ul_Transform.h"
#include "ul_ConvKernel.h"
#include "ul_ConvKernelToImage.h"
#include "ul_CreateConvKernel.h"

namespace ultra
{

template<class T>
class CSobelFilterTransform : public CTransform<T, T>
{
public:

    class CContext : public CTransformContext
    {
    private:
        bool m_genMod;
        bool m_genPhase;
        bool m_smoothInput;
    public:

        CContext() : CTransformContext()
        {
            m_genMod = true;
            m_genPhase = true;
            m_smoothInput = true;
        }

        virtual ~CContext()
        {

        }

        bool getSmoothInput() const
        {
            return m_smoothInput;
        }

        void setSmoothInput(bool smoothInput)
        {
            m_smoothInput = smoothInput;
        }

        bool getGenMod() const
        {
            return m_genMod;
        }

        void setGenMod(bool genMod)
        {
            m_genMod = genMod;
        }

        bool getGenPhase() const
        {
            return m_genPhase;
        }

        void setGenPhase(bool genPhase)
        {
            m_genPhase = genPhase;
        }

    };
private:
    CVector<CMatrix<T> > m_smoothedImage;
    CConvKernel<T> m_gaussianKernel;
    CConvKernel<T> m_sobelX;
    CConvKernel<T> m_sobelY;
    CContext *m_ctx;
    unsigned long m_outBandSize;

    int GaussInput(unsigned long it)
    {
        const CVector<CMatrix<T> > *inputImages = CTransform<T, T>::getInputImages();

        if (m_ctx->getSmoothInput())
        {
            if (CConvKernelToImage::Convolve<T>((*inputImages)[it], m_gaussianKernel, m_smoothedImage[it], true) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Convolve()");
                return 1;
            }
        }
        else
        {
            m_smoothedImage[it] = (*inputImages)[it];
        }

        return 0;
    }

    int ApplySobel(unsigned long it)
    {
        CVector<CMatrix<T> > *outputImages = CTransform<T, T>::getOutputImages();

        if (CConvKernelToImage::Convolve<T>(m_smoothedImage[it], m_sobelX, (*outputImages)[0 + m_outBandSize * it], true) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Convolve()");
            return 1;
        }

        if (CConvKernelToImage::Convolve<T>(m_smoothedImage[it], m_sobelY, (*outputImages)[1 + m_outBandSize * it], true) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Convolve()");
            return 1;
        }
        SSize size = (*outputImages)[0 + m_outBandSize * it].getSize();

        int modIndex = 2;
        int phaseIndex = 3;
        bool genMod = m_ctx->getGenMod();
        bool genPhase = m_ctx->getGenPhase();
        if (genMod)
            (*outputImages)[modIndex + m_outBandSize * it].resize(size);
        else
            phaseIndex = modIndex; // as we are generating the mod we need to make adjust the phase index
        if (genPhase)
            (*outputImages)[phaseIndex + m_outBandSize * it].resize(size);

        if (genMod || genPhase)
        {
            unsigned long r, c;
            for (r = 0; r < size.row; r++)
            {
                T *xDp = (*outputImages)[0 + m_outBandSize * it][r].getDataPointer();
                T *yDp = (*outputImages)[1 + m_outBandSize * it][r].getDataPointer();
                for (c = 0; c < size.col; c++)
                {
                    if (genMod)
                        (*outputImages)[modIndex + m_outBandSize * it][r][c] = std::sqrt(xDp[c] * xDp[c] + yDp[c] * yDp[c]);
                    if (genPhase)
                        (*outputImages)[phaseIndex + m_outBandSize * it][r][c] = atan2((double) yDp[c], (double) xDp[c]);
                }
            }
        }

        return 0;
    }

protected:

    virtual int Init(CTransformContext *context) override
    {
        if (context == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context may not be nullptr");
            return 1;
        }
        m_ctx = dynamic_cast<CContext*> (context);
        if (m_ctx == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input context is not of type CSobelFilterTransform::CContext");
            return 1;
        }
        m_outBandSize = 2;
        if (m_ctx->getGenMod())
            m_outBandSize++;
        if (m_ctx->getGenPhase())
            m_outBandSize++;
        const CVector<CMatrix<T> > *inputImages = CTransform<T, T>::getInputImages();
        CVector<CMatrix<T> > *outputImages = CTransform<T, T>::getOutputImages();

        unsigned long size = (*inputImages).size();
        for (unsigned long it = 0; it < size; it++)
        {
            if ((*inputImages)[it].getSize() == SSize(0, 0))
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image may not be of size 0x0");
                return 1;
            }
        }

        m_smoothedImage.resize(size);

        for (unsigned long t = 0; t < size; t++)
        {
            if (m_smoothedImage[t].getSize() != (*inputImages)[t].getSize())
            {
                m_smoothedImage[t].resize((*inputImages)[t].getSize());
            }
        }

        outputImages->resize(m_outBandSize * size);

        if (CCreateConvKernel::GetKernel<T>(EKernelTypes::GAUSSIAN, SSize(5, 5), m_gaussianKernel, 1.6) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create getKernel()");
            return 1;
        }

        if (CCreateConvKernel::GetKernel<T>(EKernelTypes::SOBEL_X, SSize(3, 3), m_sobelX, 1.6) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create getKernel()");
            return 1;
        }

        if (CCreateConvKernel::GetKernel<T>(EKernelTypes::SOBEL_Y, SSize(3, 3), m_sobelY, 1.6) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create getKernel()");
            return 1;
        }

        return 0;
    }

    virtual int InnerTransform() override
    {
        const CVector<CMatrix<T> > *inputImages = CTransform<T, T>::getInputImages();

        unsigned long size = inputImages->size();
        for (unsigned long it = 0; it < size; it++)
        {
            if (GaussInput(it) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GaussInput()");
                return 1;
            }

            if (ApplySobel(it) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ApplySobel()");
                return 1;
            }

            m_smoothedImage[it].clear();
        }

        return 0;
    }

public:

    CSobelFilterTransform() :
    CTransform<T, T>("SobelFilter_Transform")
    {
    }

    virtual ~CSobelFilterTransform()
    {
    }
};

} // namespace ultra
