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

#include "ul_Chips.h"
#include "ul_UltraThread.h"

namespace ultra
{

template<class T>
class CChipsGen : public IRunnable
{
public:

    class CChipsGenContext
    {
    private:

        std::shared_ptr<CThreadLock> m_ultraSChipsGenContextLock;

        struct SImageBounds
        {
            SSize imageOrigin;
            SSize imageSize;
        };

        CVector<SImageBounds> m_subImages;

        friend class CChipsGen;
    public:

        unsigned int threadCount;
        int errorCode;
        unsigned long gridSize;

        CChipsGenContext() :
        m_ultraSChipsGenContextLock(new CThreadLock())
        {
            threadCount = 1;
            errorCode = 0;
        }

        virtual ~CChipsGenContext()
        {
        }
    };

private:
    std::string m_chipGenName;

    int CheckContext()
    {
        if (m_mainContext == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Please load context");
            return 1;
        }

        m_mainContext->errorCode = 0;

        if (m_mainContext->threadCount != 1 &&
            m_mainContext->threadCount != 2)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context thread count can only be 1 or 2");
            return 1;
        }

        m_mainContext->m_subImages.resize(m_mainContext->threadCount);

        return 0;
    }

    int GenerateThreadedCall()
    {
        if (SplitImagesForThreads() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from SplitImagesForThreads()");
            return 1;
        }
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
        return 1;
    }

protected:
    const CMatrix<T> *m_inputImage;
    CVector<CChip<T> > *m_outputChips;
    CChipsGenContext *m_mainContext;

    virtual int SplitImagesForThreads() = 0;
    virtual int innerGenerate(CChipsGen<T>::CChipsGenContext *context = nullptr) = 0;

public:

    CChipsGen(std::string chipsGenName = "Base_ChipsGen") :
    IRunnable()
    {
        m_chipGenName = chipsGenName;
        m_outputChips = nullptr;
        m_inputImage = nullptr;
    }

    virtual ~CChipsGen()
    {
        m_inputImage = nullptr;
        m_outputChips = nullptr;
        m_chipGenName = "";
    }

    std::string GetChipsGenName()
    {
        return m_chipGenName;
    }

    int LoadData(const CMatrix<T> *inputImage, CVector< CChip<T> > *chips)
    {
        if (inputImage == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector is nullptr");
            return 1;
        }

        if (chips == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Output chips vector is nullptr");
            return 1;
        }

        m_inputImage = inputImage;
        m_outputChips = chips;

        return 0;
    }

    int Generate(CChipsGen<T>::CChipsGenContext *context)
    {
        m_mainContext = context;
        if (CheckContext() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckContext()");
            return 1;
        }

        if (m_mainContext->threadCount == 1)
        {
            run();
        }
        else
        {
            if (GenerateThreadedCall() != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateThreadedCall()");
                return 1;
            }
        }

        if (m_mainContext->errorCode != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from inner class()");
            return 1;
        }

        return 0;
    }

    virtual void run(void *context = nullptr)
    {
        if (innerGenerate(m_mainContext) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerGenerate()");
            {
                AUTO_LOCK(m_mainContext->m_ultraSChipsGenContextLock);
                m_mainContext->errorCode = 1;
            }
        }
    }
};

} // namespace ultra
