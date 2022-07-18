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

#include "ul_ImageLoader.h"

namespace ultra
{

int CTiePointGenerator::LoadOneImageFull(int bandNumber)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Loading input image .... '" + m_context.innerContext->inputScene.pathToImage + "' band number '" + toString(m_context.innerContext->referenceScene.bandNumber) + "'");
    if (CImageLoader::getInstance()->LoadImage(m_context.innerContext->inputScene.pathToImage,
                                               m_context.images[INPUT_IMG_INDEX],
                                               m_context.innerContext->inputScene.bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Loading input image .... DONE");

    return 0;
}

int CTiePointGenerator::LoadTwoImages()
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading reference image .... '" + m_context.innerContext->referenceScene.pathToImage + "' band number '" + toString(m_context.innerContext->inputScene.bandNumber) + "'");
    if (CImageLoader::getInstance()->LoadImage(
                                               m_context.innerContext->referenceScene.pathToImage,
                                               m_context.images[REF_IMG_INDEX],
                                               m_context.innerContext->referenceScene.bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading reference image .... DONE");

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading input image .... '" + m_context.innerContext->inputScene.pathToImage + "' band number '" + toString(m_context.innerContext->referenceScene.bandNumber) + "'");
    if (CImageLoader::getInstance()->LoadImage(
                                               m_context.innerContext->inputScene.pathToImage,
                                               m_context.images[INPUT_IMG_INDEX],
                                               m_context.innerContext->inputScene.bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading input image .... DONE");

    return 0;
}

int CTiePointGenerator::LoadCompleteImages(CMatrixArray<float> &refImages, SImageMetadata &refMetadata,
                                           CMatrixArray<float> &inputImages, SImageMetadata &inputMetadata)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading reference image .... '" + m_context.innerContext->referenceScene.pathToImage + "");
    if (CImageLoader::getInstance()->LoadImage(m_context.innerContext->referenceScene.pathToImage, refImages) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }

    if (CImageLoader::getInstance()->LoadImageMetadata(m_context.innerContext->referenceScene.pathToImage, refMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading reference image .... DONE");

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading input image .... '" + m_context.innerContext->inputScene.pathToImage + "'");
    if (CImageLoader::getInstance()->LoadImage(m_context.innerContext->inputScene.pathToImage, inputImages) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
        return 1;
    }

    if (CImageLoader::getInstance()->LoadImageMetadata(m_context.innerContext->inputScene.pathToImage, inputMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Loading input image .... DONE");
    return 0;
}

} // namespace ultra
