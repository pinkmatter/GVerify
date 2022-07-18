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

#include "ul_GaussianPyramidTiePointGenerator.h"

#include "ul_ImageLoader.h"
#include "ul_File.h"

namespace ultra
{

int CGaussianPyramidTiePointGenerator::checkImageSize(const std::string &pathToImage, unsigned long modVal)
{
    SSize size;

    if (CImageLoader::getInstance()->LoadImageDimensions(pathToImage, size) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageDimensions()");
        return 1;
    }

    if (size.row == 0 || size.col == 0 ||
        size.row % modVal != 0 || size.col % modVal != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Loaded image's size must be diviable by '" + toString(modVal) + "'");
        return 1;
    }

    return 0;
}

bool CGaussianPyramidTiePointGenerator::isContextOk()
{
    if (m_context.publicContex->amountOfPyramids <= 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "CGaussianPyramidShiftCalculator::SContext->amountOfPyramids must be 1 or largers");
        return false;
    }

    unsigned long mod = 1;
    m_context.pyramidDivLevels.pushBack(1);
    for (unsigned long t = 0; t < m_context.publicContex->amountOfPyramids - 1; t++)
    {
        mod *= 2;
        m_context.pyramidDivLevels.pushBack(mod);
    }

    CTiePointGenerator *calc = new CTiePointGenerator(&m_context.publicContex->tiePointGeneratorContext);
    if (calc == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CSceneShiftCalculator object");
        return false;
    }

    if (!calc->isContextOK(&m_context.publicContex->tiePointGeneratorContext))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CSceneShiftCalculator::isContextOK()");
        return false;
    }

    delete calc;
    calc = nullptr;

    if (checkImageSize(m_context.publicContex->tiePointGeneratorContext.inputScene.pathToImage, mod) != 0 ||
        checkImageSize(m_context.publicContex->tiePointGeneratorContext.referenceScene.pathToImage, mod) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from checkImageSize()");
        return false;
    }

    if (!CFile(m_context.publicContex->workingFolder).isDirectoryWritable())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Working folder is not a writable folder '" + m_context.publicContex->workingFolder + "'");
        return false;
    }

    return true;
}

} // namespace ultra
