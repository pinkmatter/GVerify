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

#include "ul_Int_ImageSaver.h"

namespace ultra
{
namespace __ultra_internal
{

class CImageSaverGDAL : public IImageSaver
{
private:
    const static EImage::EImageFormat DEFAULT_IMAGE_TYPE;
private:
    CImageSaverGDAL(bool isThreadSafe);
    // no wrap needed
    void genGeoTransform(const SPair<double> &origin, const SPair<double> &gsd, double output[6], const SImageMetadata *metadata);

    template<class T>
    int inner_saveImage(const std::string &pathToImageFile, const CMatrix<T> &img, EImage::EImageFormat imageType, const SImageMetadata *metadata);

    template<class T>
    int inner_saveImage(const std::string &pathToImageFile, const CMatrixArray<T> &img, EImage::EImageFormat imageType, const SImageMetadata *metadata);

    int PopulateOSrs(const SImageMetadata *metadata, void *osrs, void *dataset);

    template<class T>
    int DetermineType(int &gdalImageType);

    template<class T>
    int inner_saveImageVecLineForLine(void *PoDriver, const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata);

    template<class T>
    int inner_saveImageVecFromMemory(void *PoDriver, const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata);

    // wrapped calls
    template<class T>
    int inner_saveImageVec(const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata);

private:
    //where calls are wrapped
    template<class T>
    int inner_wrappedSaveImageVec(const std::string &pathToImageFile, const CVector<CMatrix<T>*> &imgVec, EImage::EImageFormat imageType, const SImageMetadata *metadata);
protected:

    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrix<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;

    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<char> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<short> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<unsigned int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<int> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<float> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<double> &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<short> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<int> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<float> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;
    virtual int innerSaveImage(std::string pathToImageFile, const CMatrixArray<SComplex<double> > &image, EImage::EImageFormat imageType, const SImageMetadata *metadata) override;

    virtual int innerSetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber) override;
    virtual int innerRemoveNoDataValue(std::string pathToImageFile, int bandNumber) override;

public:
    virtual ~CImageSaverGDAL();
    static IImageSaver *getInstance();

};

} // namespace __ultra_internal
} // namespace ultra
