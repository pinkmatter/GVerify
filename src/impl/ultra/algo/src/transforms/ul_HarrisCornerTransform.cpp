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

#include "ul_HarrisCornerTransform.h"

#include "ul_Logger.h"
#include "ul_SobelFilterTransform.h"

namespace ultra
{

CHarrisCornerTransform::CHarrisCornerContext::CHarrisCornerContext()
{

}

CHarrisCornerTransform::CHarrisCornerContext::~CHarrisCornerContext()
{

}

CHarrisCornerTransform::CHarrisCornerTransform() :
CTransform<float, unsigned char>("HarrisCorner_Transform")
{
    m_sobelFilter = nullptr;
    m_internalContext = false;
}

CHarrisCornerTransform::~CHarrisCornerTransform()
{
    if (m_sobelFilter != nullptr)
    {
        delete m_sobelFilter;
        m_sobelFilter = nullptr;
    }

    if (m_internalContext)
    {
        if (m_context != nullptr)
        {
            delete m_context;
            m_context = nullptr;
        }
    }
}

int CHarrisCornerTransform::Init(CTransformContext *context)
{
    const CVector<CMatrix<float> > *inputImages = CTransform<float, unsigned char>::getInputImages();
    CVector<CMatrix<unsigned char> > *outputImages = CTransform<float, unsigned char>::getOutputImages();

    if (context != nullptr)
    {
        if (m_internalContext)
        {
            if (m_context != nullptr)
            {
                delete m_context;
                m_context = nullptr;
            }
        }

        m_context = dynamic_cast<CHarrisCornerContext*> (context);

        m_internalContext = false;
    }
    else
    {
        if (m_internalContext)
        {
            if (m_context != nullptr)
            {
                delete m_context;
                m_context = nullptr;
            }
        }

        m_context = new CHarrisCornerContext();
        if (m_context == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create internal context");
            return 1;
        }

        m_context->minHarrisValue = 1;

        m_internalContext = true;
    }

    if (inputImages->size() != 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector must contain exactly 1 image");
        return 1;
    }

    if ((*inputImages)[0].getSize() == SSize(0, 0))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image may not be of size 0x0");
        return 1;
    }

    outputImages->resize(1);

    if ((*outputImages)[0].getSize() != (*inputImages)[0].getSize())
    {
        (*outputImages)[0].resize((*inputImages)[0].getSize());
    }

    if (m_sobelFilter == nullptr)
    {
        m_sobelFilter = new CSobelFilterTransform<float>();
        if (m_sobelFilter == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create SobelFilterTransform object");
            return 1;
        }
    }

    return 0;
}

int CHarrisCornerTransform::DoSobelTransForm()
{
    const CVector<CMatrix<float> > *inputImages = CTransform<float, unsigned char>::getInputImages();

    if (m_sobelFilter->LoadData(inputImages, &m_sobelImages) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SobelFilterTransform::LoadData()");
        return 1;
    }

    CSobelFilterTransform<float>::CContext sobelCtx;
    sobelCtx.setGenMod(false);
    sobelCtx.setGenPhase(false);
    if (m_sobelFilter->Transform(&sobelCtx) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SobelFilterTransform::Transform()");
        return 1;
    }

    if (m_sobelImages.size() != 2)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Sobel filter returned incorrect amount of images");
        return 1;
    }

    return 0;
}

int CHarrisCornerTransform::ConstructHarrisMatrix()
{
    m_harrisMat.resize(m_sobelImages[0].getSize());
    SSize size = m_harrisMat.getSize();
    int per = 0;

    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            m_harrisMat[r][c].resize(SSize(2, 2));
            m_harrisMat[r][c][0][0] = m_sobelImages[0][r][c] * m_sobelImages[0][r][c];
            m_harrisMat[r][c][0][1] = m_sobelImages[0][r][c] * m_sobelImages[1][r][c];
            m_harrisMat[r][c][1][0] = m_sobelImages[0][r][c] * m_sobelImages[1][r][c];
            m_harrisMat[r][c][1][1] = m_sobelImages[1][r][c] * m_sobelImages[1][r][c];
        }
        if (per != (int) ((r * 100) / size.row))
        {
            per = (int) ((r * 100) / size.row);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing " + toString(per) + "%      \r");
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Processing 100%      ");

    return 0;
}

int CHarrisCornerTransform::DetectHarrisCorners()
{
    CVector<CMatrix<unsigned char> > *outputImages = CTransform<float, unsigned char>::getOutputImages();

    outputImages->resize(1);
    (*outputImages)[0].resize(m_harrisMat.getSize());

    SSize size = m_harrisMat.getSize();
    (*outputImages)[0].initMatMemset(0);
    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            float D = m_harrisMat[r][c][0][0] * m_harrisMat[r][c][1][1] - m_harrisMat[r][c][0][1] * m_harrisMat[r][c][1][0];
            float T = m_harrisMat[r][c][0][0] + m_harrisMat[r][c][1][1];
            float root = T * T - 4.0f * D;
            if (root < 0)
            {
                continue;
            }

            root = std::sqrt(root);

            float l1 = (T - root) / 2.0f;
            float l2 = (T + root) / 2.0f;

            if (l1 > m_context->minHarrisValue &&
                l2 > m_context->minHarrisValue)
            {
                (*outputImages)[0][r][c] = 255;
            }
        }
    }

    return 0;
}

int CHarrisCornerTransform::InnerTransform()
{
    if (DoSobelTransForm() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from DoSobelTransForm()");
        return 1;
    }

    if (ConstructHarrisMatrix() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ConstructHarrisMatrix()");
        return 1;
    }

    if (DetectHarrisCorners() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from DetectHarrisCorners()");
        return 1;
    }

    return 0;
}

} // namespace ultra
