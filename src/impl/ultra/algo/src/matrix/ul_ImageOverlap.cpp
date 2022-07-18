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

#include "ul_ImageOverlap.h"
#include "ul_PaddImage.h"

#include <ul_ImageSaver.h>
#include <ul_Logger.h>
#include <ul_File.h>
#include <ul_Utility.h>

namespace ultra
{

CImageOverlap::SContext::SContext()
{

}

CImageOverlap::SContext::SContext(const CImageOverlap::SContext &r)
{
    inputOutput = r.inputOutput;
    metadata = r.metadata;
    proj4 = r.proj4;
    subUl = r.subUl;
    subImageSize = r.subImageSize;
    paddUl = r.paddUl;
    paddLr = r.paddLr;
}

CImageOverlap::SContext::~SContext()
{

}

CImageOverlap::SContext &CImageOverlap::SContext::operator=(const CImageOverlap::SContext &r)
{
    if (this == &r)
        return *this;
    inputOutput = r.inputOutput;
    metadata = r.metadata;
    proj4 = r.proj4;
    subUl = r.subUl;
    subImageSize = r.subImageSize;
    paddUl = r.paddUl;
    paddLr = r.paddLr;
    return *this;
}

bool CImageOverlap::SContext::contextMatches(const CImageOverlap::SContext &r)
{
    if (metadata.getGsd() != r.metadata.getGsd())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "GSD codes do not match");
        return false;
    }
    if (metadata.getGsd().containsZero())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "GSD contains zeros");
        return false;
    }
    if (proj4 != r.proj4)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Proj4 strings do not match");
        return false;
    }

    return true;
}

bool CImageOverlap::SContext::checkIfOverlapExists() const
{
    bool outputBounds = subUl.r < 0 || subUl.c < 0 ||
        subImageSize.r <= 0 || subImageSize.c <= 0;
    return !outputBounds;
}

int CImageOverlap::SContext::PopulateSubImageCoordinates(const SKeyValue<SPair<double>, SPair<double> > &ul_lr)
{
    SPair<double> origin = metadata.getOrigin();
    SPair<double> gsd = metadata.getGsd();
    SPair<long> size = metadata.getDimensions();
    SPair<double> tempUl = ((origin / gsd));
    SPair<long> selectUl = (ul_lr.k - tempUl).roundValues().convertType<long>();
    SPair<long> selectSize = (ul_lr.v - ul_lr.k).convertType<long>();

    paddUl = 0;
    paddLr = 0;
    if (selectUl.r < 0)
    {
        paddUl.r = -selectUl.r;
        selectSize.r -= paddUl.r;
        selectUl.r = 0;
    }
    if (selectUl.c < 0)
    {
        paddUl.c = -selectUl.c;
        selectSize.c -= paddUl.c;
        selectUl.c = 0;
    }

    SPair<long> boundSize = selectUl + selectSize;

    if (boundSize.r > size.r)
    {
        paddLr.r = boundSize.r - size.r;
        selectSize.r -= paddLr.r;
        boundSize.r = size.r;
    }
    if (boundSize.c > size.c)
    {
        paddLr.c = boundSize.c - size.c;
        selectSize.c -= paddLr.c;
        boundSize.c = size.c;
    }

    subUl = selectUl;
    subImageSize = selectSize;

    if (boundSize.r > size.r ||
        boundSize.c > size.c)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "There are rounding errors... cannot compute sub image coordinates");
        return 1;
    }

    if (paddUl.c < 0 || paddUl.r < 0 ||
        paddLr.c < 0 || paddLr.r < 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Trying to pad with negative amounts (rounding errors)");
        return 1;
    }

    return 0;
}

CImageOverlap::CImageOverlap()
{
    m_inputOutputImageContext.clear();
    m_type = CImageOverlap::OVERLAP_TYPE_COUNT;
}

CImageOverlap::~CImageOverlap()
{
    m_inputOutputImageContext.clear();
    m_type = CImageOverlap::OVERLAP_TYPE_COUNT;
}

int CImageOverlap::Init(CVector<SKeyValue<std::string, std::string> > &inputOutputImagePaths, CImageOverlap::EOverlapType type)
{
    m_type = type;

    if (static_cast<int> (m_type) < 0 || static_cast<int> (m_type) >= static_cast<int> (CImageOverlap::OVERLAP_TYPE_COUNT))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid overlap type");
        return 1;
    }

    if (inputOutputImagePaths.size() <= 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image vector must at least contain 2 or more items");
        return 1;
    }

    m_inputOutputImageContext.resize(inputOutputImagePaths.size());

    for (long x = 0; x < inputOutputImagePaths.size(); x++)
    {
        SKeyValue<std::string, std::string> item = inputOutputImagePaths[x];
        std::string inPath = item.k;
        std::string outPath = item.v;
        if (!CFile(inPath).isFile())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot find a file at '" + inPath + "'");
            return 1;
        }

        if (!CFile(outPath).getParentFolderFile().isDirectoryWritable())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot save output file '" + outPath + "'");
            return 1;
        }

        SContext context;
        context.inputOutput = item;
        if (CImageLoader::getInstance()->LoadImageMetadata(item.k, context.metadata) != 0 ||
            CImageLoader::getInstance()->LoadProj4Str(item.k, context.proj4) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
            return 1;
        }
        context.metadata.setProj4String(context.proj4);

        m_inputOutputImageContext[x] = context;
    }

    SContext context_1 = m_inputOutputImageContext[0];
    for (long x = 1; x < m_inputOutputImageContext.size(); x++)
    {
        if (!context_1.contextMatches(m_inputOutputImageContext[x]))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "There is a image metadata mismatch, cannot continue");
            return 1;
        }
    }

    return 0;
}

int CImageOverlap::MinBox()
{
    SKeyValue<SPair<double>, SPair<double> > ul_lr;
    SPair<double> temp;
    SPair<double> gsd = m_inputOutputImageContext[0].metadata.getGsd();
    ul_lr.k = (m_inputOutputImageContext[0].metadata.getOrigin() / gsd);
    ul_lr.v = ul_lr.k + m_inputOutputImageContext[0].metadata.getDimensions();

    for (long x = 1; x < m_inputOutputImageContext.size(); x++)
    {
        temp = (m_inputOutputImageContext[x].metadata.getOrigin() / gsd);
        ul_lr.k.c = max_of<double>(2, ceil(ul_lr.k.c), ceil(temp.c));
        ul_lr.k.r = max_of<double>(2, ceil(ul_lr.k.r), ceil(temp.r));

        temp += m_inputOutputImageContext[x].metadata.getDimensions();
        ul_lr.v.c = min_of<double>(2, floor(ul_lr.v.c), floor(temp.c));
        ul_lr.v.r = min_of<double>(2, floor(ul_lr.v.r), floor(temp.r));
    }

    for (long x = 0; x < m_inputOutputImageContext.size(); x++)
    {
        if (m_inputOutputImageContext[x].PopulateSubImageCoordinates(ul_lr) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PopulateSubImageCoordinates()");
            return 1;
        }
    }
    return 0;
}

int CImageOverlap::MaxBox()
{
    SKeyValue<SPair<double>, SPair<double> > ul_lr;
    SPair<double> temp;
    SPair<double> gsd = m_inputOutputImageContext[0].metadata.getGsd();
    ul_lr.k = (m_inputOutputImageContext[0].metadata.getOrigin() / gsd);
    ul_lr.v = ul_lr.k + m_inputOutputImageContext[0].metadata.getDimensions();

    for (long x = 1; x < m_inputOutputImageContext.size(); x++)
    {
        temp = (m_inputOutputImageContext[x].metadata.getOrigin() / gsd);
        ul_lr.k.c = min_of<double>(2, ceil(ul_lr.k.c), ceil(temp.c));
        ul_lr.k.r = min_of<double>(2, ceil(ul_lr.k.r), ceil(temp.r));

        temp += m_inputOutputImageContext[x].metadata.getDimensions();
        ul_lr.v.c = max_of<double>(2, floor(ul_lr.v.c), floor(temp.c));
        ul_lr.v.r = max_of<double>(2, floor(ul_lr.v.r), floor(temp.r));
    }

    for (long x = 0; x < m_inputOutputImageContext.size(); x++)
    {
        if (m_inputOutputImageContext[x].PopulateSubImageCoordinates(ul_lr) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PopulateSubImageCoordinates()");
            return 1;
        }
    }
    return 0;
}

int CImageOverlap::DoPadding(const CImageOverlap::SContext &context, float paddVal, CMatrixArray<float> &image)
{
    for (unsigned long t = 0; t < image.size(); t++)
    {
        if (CPaddImage::Padd(image[t], context.paddUl.getSizeType(), context.paddLr.getSizeType(), paddVal) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CPaddImage::Padd()");
            return 1;
        }
    }
    return 0;
}

int CImageOverlap::innerOverlapImages(CVector<SKeyValue<std::string, std::string> > &inputOutputImagePaths, CImageOverlap::EOverlapType type, bool &overlapExists, float paddVal)
{
    overlapExists = true;
    if (Init(inputOutputImagePaths, type) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
        return 1;
    }

    int (CImageOverlap::*m_fp_overlap)();
    m_fp_overlap = nullptr;
    switch (m_type)
    {
    case CImageOverlap::MIN_BOX:
        m_fp_overlap = &CImageOverlap::MinBox;
        break;
    case CImageOverlap::MAX_BOX:
        m_fp_overlap = &CImageOverlap::MaxBox;
        break;
    }

    if (m_fp_overlap == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown overlap type");
        return 1;
    }

    if ((this->*m_fp_overlap)() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from m_fp_overlap()");
        return 1;
    }

    EImage::EImageFormat imageType;
    CMatrixArray<float> tempImage;

    for (long x = 0; x < m_inputOutputImageContext.size(); x++)
    {
        std::string savePath = m_inputOutputImageContext[x].inputOutput.v;
        if (!m_inputOutputImageContext[x].checkIfOverlapExists())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Images do not overlap");
            overlapExists = false;
            return 0;
        }
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Save new overlapped image to '" + savePath + "'");
        if (CImageLoader::getInstance()->LoadImage(m_inputOutputImageContext[x].inputOutput.k, tempImage,
                                                   m_inputOutputImageContext[x].subUl.getSizeType(),
                                                   m_inputOutputImageContext[x].subImageSize.getSizeType()) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImage()");
            return 1;
        }
        if (CImageLoader::getInstance()->LoadImageType(m_inputOutputImageContext[x].inputOutput.k, imageType) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageType()");
            return 1;
        }

        if (DoPadding(m_inputOutputImageContext[x], paddVal, tempImage) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from DoPadding()");
            return 1;
        }


        m_inputOutputImageContext[x].metadata.setOrigin(m_inputOutputImageContext[x].metadata.getOrigin() +
                                                        (m_inputOutputImageContext[x].subUl - m_inputOutputImageContext[x].paddUl).convertType<double>()
                                                        * m_inputOutputImageContext[x].metadata.getGsd());

        if (CImageSaver::getInstance()->SaveImage(savePath, tempImage, imageType, &m_inputOutputImageContext[x].metadata) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageSaver::SaveImage()");
            return 1;
        }
    }
    tempImage.clear();

    return 0;
}

int CImageOverlap::OverlapImages(CVector<SKeyValue<std::string, std::string> > &inputOutputImagePaths, CImageOverlap::EOverlapType type, bool &overlapExists, float paddVal)
{
    std::unique_ptr<CImageOverlap> ptr(new CImageOverlap());
    return ptr->innerOverlapImages(inputOutputImagePaths, type, overlapExists, paddVal);
}

} // namespace ultra
