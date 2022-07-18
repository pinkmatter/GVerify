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
class CGaussianFilterTransform : public CTransform<T, T>
{
public:

    class CContext : public CTransformContext
    {
    private:
        T m_stdDev;
        SSize m_kernSize;
        const T *m_noDataValue;
    public:

        CContext() :
        m_stdDev(1.6), m_kernSize(5, 5), m_noDataValue(nullptr)
        {
        }

        virtual ~CContext()
        {
        }

        T getStdDev() const
        {
            return m_stdDev;
        }

        void setStdDev(const T &stdDev)
        {
            m_stdDev = stdDev;
        }

        const T* getNoDataValue() const
        {
            return m_noDataValue;
        }

        void setNoDataValue(const T* noDataValue)
        {
            m_noDataValue = noDataValue;
        }

        SSize getKernSize() const
        {
            return m_kernSize;
        }

        void setKernSize(const SSize &kernSize)
        {
            m_kernSize = kernSize;
        }

    };
private:
    CConvKernel<T> m_gaussianKernel;
    CContext * m_context;

    int GaussInput()
    {
        const CVector<CMatrix<T> > *inputImages = CTransform<T, T>::getInputImages();
        CVector<CMatrix<T> > *outputImages = CTransform<T, T>::getOutputImages();

        for (unsigned long t = 0; t < inputImages->size(); t++)
        {
            if (CConvKernelToImage::Convolve<T>((*inputImages)[t], m_gaussianKernel, (*outputImages)[t], true, m_context->getNoDataValue()) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Convolve()");
                return 1;
            }
        }

        return 0;
    }

protected:

    virtual int Init(CTransformContext *context) override
    {
        const CVector<CMatrix<T> > *inputImages = CTransform<T, T>::getInputImages();
        CVector<CMatrix<T> > *outputImages = CTransform<T, T>::getOutputImages();

        if (inputImages->size() == 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector is of size zero");
            return 1;
        }

        for (unsigned long t = 0; t < inputImages->size(); t++)
        {
            if ((*inputImages)[t].getSize() == SSize(0, 0))
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image may not be of size 0x0");
                return 1;
            }
        }

        if (outputImages->size() != inputImages->size())
        {
            outputImages->resize(inputImages->size());
        }

        m_context = dynamic_cast<CGaussianFilterTransform<T>::CContext *> (context);
        if (m_context == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context is not of the correct type");
            return 1;
        }

        if (CCreateConvKernel::GetKernel<T>(EKernelTypes::GAUSSIAN, m_context->getKernSize(), m_gaussianKernel, m_context->getStdDev()) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create getKernel()");
            return 1;
        }

        return 0;
    }

    virtual int InnerTransform() override
    {
        if (GaussInput() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GaussInput()");
            return 1;
        }

        return 0;
    }

public:

    CGaussianFilterTransform() :
    CTransform<T, T>("GaussianFilter_Transform")
    {
    }

    virtual ~CGaussianFilterTransform()
    {
    }
};

} // namespace ultra
