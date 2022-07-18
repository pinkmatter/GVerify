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

#include "ul_Int_ImageSaver.h"

#include "../ImageLoaderLocks/ul_ImageLoadSaveLocks.h"
#include <ul_Exception.h>
#include <ul_UltraThread.h>

namespace ultra
{
namespace __ultra_internal
{

IImageSaver::IImageSaver(bool isThreadSave, int lockKey) :
m_lock(new CThreadLock(), std::default_delete<CThreadLock>())
{
    m_isThreadSafe = isThreadSave;
    m_lockKey = lockKey;
}

IImageSaver::~IImageSaver()
{
}

std::shared_ptr<void> IImageSaver::getLock()
{
    return CImageLoadSaveLocks::getInstance()->getLock(m_lockKey, m_lock);
}

bool IImageSaver::isThreadSafe() const
{
    return m_isThreadSafe;
}

template<class T>
int IImageSaver::SaveImageLocked_T(std::string pathToImageFile, const CMatrix<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    if (innerSaveImage(pathToImageFile, image, imageType, metadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerSaveImage()");
        return 1;
    }
    return 0;
}

template<class T>
int IImageSaver::SaveImageLocked_T(std::string pathToImageFile, const CMatrixArray<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    if (innerSaveImage(pathToImageFile, image, imageType, metadata) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerSaveImage()");
        return 1;
    }
    return 0;
}

template<class T>
int IImageSaver::SaveImage_T(std::string pathToImageFile, const CMatrix<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return SaveImageLocked_T(pathToImageFile, image, imageType, metadata);
    }
    return SaveImageLocked_T(pathToImageFile, image, imageType, metadata);
}

template<class T>
int IImageSaver::SaveImage_T(std::string pathToImageFile, const CMatrixArray<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    if (!isThreadSafe())
    {
        AUTO_LOCK(getLock());
        return SaveImageLocked_T(pathToImageFile, image, imageType, metadata);
    }
    return SaveImageLocked_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrix<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata)
{
    return SaveImage_T(pathToImageFile, image, imageType, metadata);
}

int IImageSaver::SetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber)
{
    return innerSetNoDataValue(pathToImageFile, noDataValue, bandNumber);
}

int IImageSaver::RemoveNoDataValue(std::string pathToImageFile, int bandNumber)
{
    return innerRemoveNoDataValue(pathToImageFile, bandNumber);
}

} // namespace __ultra_internal
} // namespace ultra
