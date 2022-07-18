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

#include <ul_MatrixArray.h>
#include <ul_ImageMetadataObjects.h>
#include <ul_Color.h>

namespace ultra
{
namespace __ultra_internal
{

class IImageLoader
{
private:

    int m_lockKey;
    std::shared_ptr<void> m_lock;

    bool m_isThreadSafe;

protected:
    virtual int innerLoadProj4Str(std::string pathToImageFile, std::string &proj4Str) = 0;
    virtual int innerLoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt) = 0;
    virtual int innerGetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType) = 0;
    virtual bool innerCanLoadImage(std::string pathToImageFile) = 0;
    virtual int innerGetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber = 1) = 0;

    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<char> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<short> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<int> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<float> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;
    virtual int innerLoadImage(std::string pathToImageFile, CMatrix<double> &image, const SSize & ul, const SSize &size, int bandNumber = 1) = 0;

    virtual int innerLoadImage_file(FILE *fm, CMatrix<unsigned char> &image, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<char> &image, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<unsigned short> &image, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<short> &image, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<unsigned int> &image, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImage_file(FILE *fm, CMatrix<int> &image, int bandNumber, const fpos64_t &pos) = 0;

    virtual int innerLoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber = 1) = 0;
    virtual int innerLoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber = 1) = 0;
    virtual int innerLoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImageMetadata(FILE *fm, SImageMetadata &metadata, int bandNumber, const fpos64_t &pos) = 0;
    virtual int innerLoadImageDimention(std::string pathToImageFile, SSize &size, int bandNumber = 1) = 0;

    std::shared_ptr<void> getLock();

    IImageLoader(bool isThreadSafe, int lockKey);
private:
    template<class T>
    int LoadImageLocked_T(std::string pathToImageFile, CMatrix<T> &image, int bandNumber);
    template<class T>
    int LoadImageLocked_T(std::string pathToImageFile, CMatrix<T> &image, const SSize &ul, const SSize &size, int bandNumber);

    template<class T>
    int LoadImage_T(std::string pathToImageFile, CMatrix<T> &image, int bandNumber);
    template<class T>
    int LoadImage_T(std::string pathToImageFile, CMatrix<T> &image, const SSize &ul, const SSize &size, int bandNumber);
public:
    virtual ~IImageLoader();

    int LoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<char> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<short> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<int> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<float> &image, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<double> &image, int bandNumber = 1);

    int LoadImage(std::string pathToImageFile, CMatrix<unsigned char> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<char> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<unsigned short> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<short> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<unsigned int> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<int> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<float> &image, const SSize & ul, const SSize &size, int bandNumber = 1);
    int LoadImage(std::string pathToImageFile, CMatrix<double> &image, const SSize & ul, const SSize &size, int bandNumber = 1);

    template<class T>
    int LoadImage(std::string pathToImageFile, CMatrixArray<T> &image)
    {
        int bandCount = 0;
        if (GetBandCount(pathToImageFile, bandCount) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to resize a CMatrixArray()");
            return 1;
        }
        image.resize(bandCount);
        for (int t = 0; t < bandCount; t++)
        {
            if (LoadImage(pathToImageFile, image[t], t + 1) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
                return 1;
            }
        }

        return 0;
    }

    template<class T>
    int LoadImage(std::string pathToImageFile, CMatrixArray<T> &image, const SSize &ul, const SSize &size)
    {
        int bandCount = 0;
        if (GetBandCount(pathToImageFile, bandCount) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to resize a CMatrixArray()");
            return 1;
        }
        image.resize(bandCount);
        for (int t = 0; t < bandCount; t++)
        {
            if (LoadImage(pathToImageFile, image[t], ul, size, t + 1) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadImage()");
                return 1;
            }
        }

        return 0;
    }

    int LoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber = 1);
    int LoadImageMetadata(std::string pathToImageFile, SImageMetadata &metadata, int bandNumber = 1);
    int LoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos);
    int LoadImageMetadata(FILE *fm, SImageMetadata &metadata, int bandNumber, const fpos64_t &pos);
    int GetBandCount(std::string pathToImageFile, int &bandCount);
    int GetImageType(std::string pathToImageFile, EImage::EImageFormat &imageType);
    int LoadProj4Str(std::string pathToImageFile, std::string &proj4Str);
    int LoadGdalProjectionRefWkt(std::string pathToImageFile, std::string &gdalProjectionRefWkt);
    bool isThreadSafe() const;
    bool canLoadImage(std::string pathToImageFile);
    int GetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber = 1);
};

} // namespace __ultra_internal
} // namespace ultra
