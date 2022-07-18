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

int CTiePointGenerator::innerLoadImgMetadata(
                                             const CTiePointGenerator::SContext::SScene &context,
                                             CTiePointGenerator::SInnerContext::SSceneMetadata &sceneMetadata
                                             )
{
    if (CImageLoader::getInstance()->LoadImageType(context.pathToImage, sceneMetadata.imageEnumType) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageType()");
        return 1;
    }

    SImageMetadata metadata;
    std::string proj4;
    if (CImageLoader::getInstance()->LoadImageMetadata(context.pathToImage, metadata) != 0 ||
        CImageLoader::getInstance()->LoadProj4Str(context.pathToImage, proj4) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }
    metadata.setProj4String(proj4);
    sceneMetadata.bpp = metadata.bpp;
    sceneMetadata.gsd = metadata.getGsd();
    sceneMetadata.imageSize = metadata.getDimensions();
    sceneMetadata.origin = metadata.getOrigin();

    return 0;
}

int CTiePointGenerator::LoadImgMetadata()
{
    if (innerLoadImgMetadata(m_context.innerContext->referenceScene, m_context.referenceSceneMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImgMetadata() for inputScene");
        return 1;
    }

    if (innerLoadImgMetadata(m_context.innerContext->inputScene, m_context.inputSceneMetadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImgMetadata() for referenceScene");
        return 1;
    }

    return 0;
}

} // namespace ultra
