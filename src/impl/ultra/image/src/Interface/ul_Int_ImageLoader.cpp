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

#include "ul_Int_ImageLoader.h"

#include "../ImageLoaderLocks/ul_ImageLoadSaveLocks.h"
#include <ul_Exception.h>
#include <ul_UltraThread.h>

namespace ultra
{
namespace __ultra_internal
{

IImageLoader::IImageLoader(bool isThreadSafe, int lockKey)
: m_lock(new CThreadLock(), std::default_delete<CThreadLock>())
{
    m_isThreadSafe = isThreadSafe;
    m_lockKey = lockKey;
}

IImageLoader::~IImageLoader()
{
}

std::shared_ptr<void> IImageLoader::getLock()
{
    return CImageLoadSaveLocks::getInstance()->getLock(m_lockKey, m_lock);
}

bool IImageLoader::isThreadSafe() const
{
    return m_isThreadSafe;
}

template<class T>
int IImageLoader::LoadImageLocked_T(std::string pathToImageFile, CMatrix<T> &image, int bandNumber)
{
    SSize ul = 0;
    SSize size;
    if (innerLoadImageDimention(pathToImageFile, size, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImageDimention()");
        return 1;
    }
    if (innerLoadImage(pathToImageFile, image, ul, size, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImage()");
        return 1;
    }
    return 0;
}

template<class T>
int IImageLoader::LoadImageLocked_T(std::string pathToImageFile, CMatrix<T> &image, const SSize &ul, const SSize &size, int bandNumber)
{
    if (innerLoadImage(pathToImageFile, image, ul, size, bandNumber) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerLoadImage()");
        return 1;
    }
    return 0;
}

template<class T>
int IImageLoader::LoadImage_T(std::string pathToImageFile, CMatrix<T> &image, int bandNumber)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return LoadImageLocked_T(pathToImageFile, image, bandNumber);
    }
    return LoadImageLocked_T(pathToImageFile, image, bandNumber);
}

template<class T>
int IImageLoader::LoadImage_T(std::string pathToImageFile, CMatrix<T> &image, const SSize &ul, const SSize &size, int bandNumber)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return LoadImageLocked_T(pathToImageFile, image, ul, size, bandNumber);
    }
    return LoadImageLocked_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadProj4Str(std::string pathToImageFile, std::string &proj4Str)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerLoadProj4Str(pathToImageFile, proj4Str);
    }
    return innerLoadProj4Str(pathToImageFile, proj4Str);
}

int IImageLoader::LoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerLoadGdalProjectionRefWkt(pathToImageFile, gdalProjectionRefWkt);
    }
    return innerLoadGdalProjectionRefWkt(pathToImageFile, gdalProjectionRefWkt);
}

int IImageLoader::GetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerGetImageType(pathToImageFile, imageType);
    }
    return innerGetImageType(pathToImageFile, imageType);
}

bool IImageLoader::canLoadImage(std::string pathToImageFile)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerCanLoadImage(pathToImageFile);
    }
    return innerCanLoadImage(pathToImageFile);
}

int IImageLoader::GetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerGetNoDataValue(pathToImageFile, noDataValue, bandNumber);
    }
    return innerGetNoDataValue(pathToImageFile, noDataValue, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<char> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<short> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<int> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<float> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<double> &image, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, bandNumber);
}

int IImageLoader::LoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerLoadImageMetadata(pathToImageFile, metadata, bandNumber);
    }
    return innerLoadImageMetadata(pathToImageFile, metadata, bandNumber);
}

int IImageLoader::LoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerLoadImageMetadata(pathToImageFile, metadata, bandNumber);
    }
    return innerLoadImageMetadata(pathToImageFile, metadata, bandNumber);
}

int IImageLoader::LoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerLoadImageMetadata(fm, metadata, bandNumber, pos);
    }
    return innerLoadImageMetadata(fm, metadata, bandNumber, pos);
}

int IImageLoader::LoadImageMetadata(FILE *fm, SImageMetadata &metadata, int bandNumber, const fpos64_t &pos)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return innerLoadImageMetadata(fm, metadata, bandNumber, pos);
    }
    return innerLoadImageMetadata(fm, metadata, bandNumber, pos);
}

int IImageLoader::GetBandCount(std::string pathToImageFile, int &bandCount)
{
    SImageMetadata metadata;
    if (LoadImageMetadata(pathToImageFile, metadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImageMetadata()");
        return 1;
    }
    bandCount = metadata.bandCount;
    return 0;
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, const SSize &ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<char> &image, const SSize &ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, const SSize &ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<short> &image, const SSize &ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<int> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<float> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

int IImageLoader::LoadImage(std::string pathToImageFile, CMatrix<double> &image, const SSize & ul, const SSize &size, int bandNumber)
{
    return LoadImage_T(pathToImageFile, image, ul, size, bandNumber);
}

} // namespace __ultra_internal
} // namespace ultra
