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

#include <ul_Complex.h>
#include <ul_MatrixArray.h>
#include <ul_ImageMetadataObjects.h>
#include <ul_Color.h>

namespace ultra
{
namespace __ultra_internal
{

class IImageSaver
{
private:

    int m_lockKey;
    std::shared_ptr<void> m_lock;
    bool m_isThreadSafe;

protected:

    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;

    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) = 0;

    virtual int innerSetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber) = 0;
    virtual int innerRemoveNoDataValue(std::string pathToImageFile, int bandNumber) = 0;

    std::shared_ptr<void> getLock();

    IImageSaver(bool isThreadSave, int lockKey);
private:
    template<class T>
    int SaveImageLocked_T(std::string pathToImageFile, const CMatrix<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata);
    template<class T>
    int SaveImageLocked_T(std::string pathToImageFile, const CMatrixArray<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata);

    template<class T>
    int SaveImage_T(std::string pathToImageFile, const CMatrix<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata);
    template<class T>
    int SaveImage_T(std::string pathToImageFile, const CMatrixArray<T> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata);
public:
    virtual ~IImageSaver();

    int SaveImage(std::string pathToImageFile, const CMatrix<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrix<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);

    int SaveImage(std::string pathToImageFile, const CMatrixArray<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);
    int SaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata = nullptr);

    int SetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber = 1);
    int RemoveNoDataValue(std::string pathToImageFile, int bandNumber = 1);

    bool isThreadSafe() const;
};

} // namespace __ultra_internal
} // namespace ultra
