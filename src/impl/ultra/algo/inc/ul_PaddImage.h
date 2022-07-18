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

#include <ul_ImageEnums.h>
#include <ul_ImageMetadataObjects.h>
#include <ul_MatrixArray.h>

namespace ultra
{

class CPaddImage
{
private:
    static int UpdateMetadata(const SImageMetadata &oldMetadata, SImageMetadata &newMetadata, const SSize &padSizeUl, const SSize &padSizeLr);
    static int InnerPadd(const CMatrixArray<float>& inputImage, const SSize &padSizeUl, const SSize &padSizeLr, float paddVal, CMatrixArray<float>& outputImage);
    CPaddImage();
public:
    virtual ~CPaddImage();

    static int Padd(const std::string &pathToImage, const SSize &padSizeUl, const SSize &padSizeLr, float paddVal, const std::string pathToImageSaveLocation);
    static int Padd(const std::string &pathToImage, const SSize &padSize, float paddVal, CMatrixArray<float>& outputImage, SImageMetadata &newMetadata, EImage::EImageFormat &imageType, int &bpp);
    static int Padd(const CMatrixArray<float>& inputImage, const SImageMetadata &inputMetadata, const SSize &padSize, float paddVal, CMatrixArray<float>& outputImage, SImageMetadata &newMetadata);
    static int Padd(const CMatrixArray<float>& inputImage, const SImageMetadata &inputMetadata, const SSize &padSizeUl, const SSize &padSizeLr, float paddVal, CMatrixArray<float>& outputImage, SImageMetadata &newMetadata);

    static int Padd(CMatrix<float> &image, const SSize &paddSize);
    static int Padd(CMatrix<float> &image, const SSize &paddSizeUl, const SSize &paddSizeLr, float paddVal);
    static int RemovePadd(CMatrix<float> &image, const SSize &removeSize);
    static int RemovePadd(CMatrix<float> &image, const SSize &removeSizeUl, const SSize &removeSizeLr);
};

} // namespace ultra
