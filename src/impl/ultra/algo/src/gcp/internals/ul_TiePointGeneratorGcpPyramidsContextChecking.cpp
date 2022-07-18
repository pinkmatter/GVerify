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

#include "ul_TiePointGeneratorGcpPyramids.h"
#include "ul_SubPixelCorrelator.h"

#include <ul_ImageLoader.h>

namespace ultra
{

int CTiePointGeneratorGcpPyramids::ContextCheck()
{
    if (m_context == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context is nullptr");
        return 1;
    }

    // working folder must be writable
    if (!CFile(m_context->m_pathToWorkingFolder).isDirectoryWritable())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Working folder '" + m_context->m_pathToWorkingFolder + "' does not point to a writable folder");
        return 1;
    }

    // min chip size is 1 and must be odd
    if (m_context->m_chipSizeMinimum == 0 || m_context->m_chipSizeMinimum % 2 != 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::minimumChipSize must be larger than zero and be odd");
        return 1;
    }

    if (m_context->m_chipGenerationGridSize < 3)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Chip generation grid size needs to be at least 3");
        return 1;
    }

    // a min of 1 thread MUST be used
    if (m_context->m_threadCount == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::numberThreadsToUse must be larger than zero");
        return 1;
    }

    SImageMetadata metaIn, metaRef;
    std::string proj4In, proj4Ref;
    if (CImageLoader::getInstance()->LoadImageMetadata(m_context->m_pathToInputImage, metaIn) != 0 ||
        CImageLoader::getInstance()->LoadImageMetadata(m_context->m_pathToRefImage, metaRef) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }

    if (CImageLoader::getInstance()->LoadProj4Str(m_context->m_pathToInputImage, proj4In) != 0 ||
        CImageLoader::getInstance()->LoadProj4Str(m_context->m_pathToRefImage, proj4Ref) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }

    if (proj4In != proj4Ref)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Both reference and input images require the same proj4 strings '" + proj4In + "' != '" + proj4Ref + "'");
        return 1;
    }

    SPair<double> gsdDiv = metaIn.getGsd() - metaRef.getGsd();

    if (!gsdDiv.containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input and reference images must have the same GSD");
        return 1;
    }

    return 0;
}

} // namespace ultra
