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

#include "ul_Matrix.h"
#include "ul_Logger.h"

namespace ultra
{

class CTransformContext
{
public:
    CTransformContext();
    virtual ~CTransformContext();
};

template<class Tin, class Tout>
class CTransform
{
private:

    int IsDataLoaded() const
    {
        if (m_inputImages == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input images is nullptr");
            return 1;
        }

        if (m_inputImages->size() == 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input images contains zero images");
            return 1;
        }

        if (m_outputImages == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Output images is nullptr");
            return 1;
        }

        for (unsigned long t = 0; t < m_inputImages->size(); t++)
        {
            SSize size = m_inputImages->operator[](t).getSize();
            if (size.row == 0 || size.col == 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector contains matrices that have columns or rows of size zero");
                return 1;
            }
        }

        return 0;
    }

    const CVector< CMatrix<Tin> > *m_inputImages;
    CVector< CMatrix<Tout> > *m_outputImages;
    std::string m_transformName;

protected:

    void SetTransformName(std::string &transformName)
    {
        m_transformName = transformName;
    }

    const CVector<CMatrix<Tin> > *getInputImages()
    {
        return m_inputImages;
    }

    CVector<CMatrix<Tout> > *getOutputImages()
    {
        return m_outputImages;
    }

    virtual int Init(CTransformContext *context) = 0;
    virtual int InnerTransform() = 0;

public:

    CTransform(std::string transformName = "Base_Transform")
    {
        m_transformName = transformName;
        m_inputImages = nullptr;
        m_outputImages = nullptr;
    }

    virtual ~CTransform()
    {
        m_inputImages = nullptr;
        m_outputImages = nullptr;
        m_transformName = "";
    }

    std::string GetTransformName() const
    {
        return m_transformName;
    }

    int LoadData(const CVector< CMatrix<Tin> > *inputImages, CVector< CMatrix<Tout> > *outputImages)
    {
        if (inputImages == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector is nullptr");
            return 1;
        }

        if (outputImages == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Output image vector is nullptr");
            return 1;
        }

        m_inputImages = inputImages;
        m_outputImages = outputImages;

        if (m_inputImages->size() == 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector is of size 0");
            return 1;
        }

        return 0;
    }

    int Transform(CTransformContext *context = nullptr)
    {
        if (IsDataLoaded() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from IsDataLoaded()");
            return 1;
        }

        if (Init(context) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
            return 1;
        }

        if (InnerTransform() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from InnerTransform()");
            return 1;
        }

        return 0;
    }
};

} //namespace ultra
