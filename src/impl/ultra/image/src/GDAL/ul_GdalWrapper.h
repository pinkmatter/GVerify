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

#include "ul_ImageEnums.h"
#include "ul_ImageMetadataObjects.h"
#include <ul_Odl.h>
#include <ul_AtomicBool.h>
#include <ul_Matrix.h>
#include <ul_Map.h>

namespace ultra
{
namespace __ultra_internal
{

class CGdalWrapper
{
private:
    CGdalWrapper();
    CMap<EImage::EImageFormat, SSize> m_defaultBlockSize;
    CMap<EImage::EImageFormat, std::shared_ptr<char*> > m_defaultPapszOptions;
private:
    //image
    int SetBpp(void *raster, COdl &metadata, int bandNumber);
    void addMetadataDomain(void *dataset, COdl &metadata, char *domain);
    int SetGeoLocation(void *dataset, COdl &metadata);
    int AddTiePoints(COdl &metadata, void *dataset);
    void SetProjectionParmsItem(void *hSRSptr, COdl &metadata, const char *key);
    int SetProjectionParms(void *hSRSptr, COdl &metadata);
    int UpdateMetadataUsingProj4Str(const std::string &proj4Str, COdl &metadata);
    SImageMetadata::EProjectionParmIndex setParmIndex(std::string key);
    void SetProjection(COdl &metadata, std::string projection, int zone);
    void SetDatum(COdl &metadata, std::string datum);
    void SetSpheroid(COdl &metadata, std::string spheroid);
    int SetProjectionParmsToGeo(COdl &metadata);
    int SetExtraMetadata(void *dataset, COdl &metadata);

private:

    int innerLoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber = 1);
    int innerLoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos);

public:
    virtual ~CGdalWrapper();
    static CGdalWrapper *getInstance();

    int LoadImageMetadata(std::string pathToImageFile, COdl &metadata, int bandNumber = 1);
    int LoadImageMetadata(FILE *fm, COdl &metadata, int bandNumber, const fpos64_t &pos);

    const SSize *getDefaultBlockSize(EImage::EImageFormat imageType) const;
    char** getGdalDefaultPapszOptions(EImage::EImageFormat imageType) const;
};

} // namespace __ultra_internal
} // namespace ultra
