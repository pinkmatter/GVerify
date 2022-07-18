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

#include "ul_TiePointGenerator.h"

#include "ul_File.h"

namespace ultra
{

bool CTiePointGenerator::isContextSceneOK(const CTiePointGenerator::SContext::SScene &context)
{
    if (context.bandNumber == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid band number");
        return false;
    }

    if (!pathExists(context.pathToImage))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Path '" + context.pathToImage + "' does not exist");
        return false;
    }

    return true;
}

bool CTiePointGenerator::isContextOK(const CTiePointGenerator::SContext *context)
{
    if (context == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Please load context");
        return false;
    }

    if (context->chipGenerationGridSize <= 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "'gridSize' must be larger than zero");
        return false;
    }

    // min chip size is 1 and must be odd
    if (context->chipSizeMinimum == 0 || context->chipSizeMinimum % 2 != 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::minimumChipSize must be larger than zero and be odd");
        return false;
    }

    // a min of 1 thread MUST be used
    if (context->threadCount == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::numberThreadsToUse must be larger than zero");
        return false;
    }

    // Check that enum falls in correct range
    if (static_cast<int> (context->typeOfChipCorrelationTechnique) < 0 ||
        static_cast<int> (context->typeOfChipCorrelationTechnique) >= static_cast<int> (ECorrelationType::CORRELATION_TYPE_COUNT))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::typeOfChipCorrelationTechnique is invalid");
        return false;
    }

    if (context->chipGeneratorMethods.size() == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "No chip generation technique specified");
        return 1;
    }
    for (int x = 0; x < context->chipGeneratorMethods.size(); x++)
    {
        if (static_cast<int> (context->chipGeneratorMethods[x]) < 0 ||
            static_cast<int> (context->chipGeneratorMethods[x]) >= static_cast<int> (EChips::CHIP_GEN_TYPE_COUNT))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::typeOfChipGenerator is invalid");
            return false;
        }
    }

    if (context->searchWindowSize <= 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Context::searchWindowSize must be larger than zero");
        return false;
    }

    if (!isContextSceneOK(context->inputScene))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isContextSceneOK() for Context::inputScene");
        return false;
    }

    if (!isContextSceneOK(context->referenceScene))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isContextSceneOK() for Context::referenceScene");
        return false;
    }

    return true;
}

} //namespace ultra
