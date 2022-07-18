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

#include "ul_Int_ImageLoader.h"

namespace ultra
{
namespace __ultra_internal
{

class CImageLoaderGDAL : public IImageLoader
{
private:
    CImageLoaderGDAL(bool isThreadSafe);

    // wrapped calls
    template<class T>
    int GetImageRasterLines(std::string pathToInputFile, CMatrix<T> &img, int bandNumber,
                            const SSize &ul, const SSize& size);

    // no wrap needed
    template<class T>
    int inner_loadPartialImage(const std::string &pathToInputFile, int bandNumber, CMatrix<T> &img, const SSize &ul, const SSize &size);

    int TranslateImageType(std::string imageDesc, EImage::EImageFormat &imageType);

private:
    //where calls are wrapped
    int innerWrappedLoadProj4Str(std::string pathToImageFile, std::string &proj4Str);
    int innerWrappedLoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt);
    int innerWrappedGetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType);
    bool innerWrappedCanLoadImage(std::string pathToImageFile);

    template<class T>
    int WrappedGetImageRasterLines(std::string pathToInputFile, CMatrix<T> &img, int bandNumber,
                                   const SSize &ul, const SSize& size);
protected:
    // wrapped calls
    virtual int innerLoadProj4Str(std::string pathToImageFile, std::string &proj4Str) override;
    virtual int innerLoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt) override;
    virtual int innerGetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType) override;
    virtual bool innerCanLoadImage(std::string pathToImageFile) override;

    // no wrap needed
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<char> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<short> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<int> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<float> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<double> &image, const SSize & ul, const SSize &size, int bandNumber = 1) override;

    virtual int innerLoadImage_file(FILE *fm, CMatrix<unsigned short> &image, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<short> &image, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<unsigned char> &image, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<char> &image, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<unsigned int> &image, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<int> &image, int bandNumber, const fpos64_t &pos) override;

    virtual int innerLoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber = 1) override;
    virtual int innerLoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber = 1) override;
    virtual int innerLoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImageMetadata(FILE *fm, SImageMetadata &metadata, int bandNumber, const fpos64_t &pos) override;
    virtual int innerLoadImageDimention(std::string pathToImageFile, SSize &size, int bandNumber = 1) override;

    virtual int innerGetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber = 1) override;

public:
    virtual ~CImageLoaderGDAL();
    static IImageLoader *getInstance();
};

} // namespace __ultra_internal
} // namespace ultra
