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

#include "ul_ImageTools.h"

#include "ul_GdalWrapper.h"
#include "../ImageLoaderLocks/ul_ImageLoadSaveLocks.h"

#include <gdal.h>
#include <gdal_priv.h>
#include <ul_File.h>

namespace ultra
{

// this key MUST be the same for all the GDAL loader and saver classes
#ifndef GDAL_LOCK_KEY
#define GDAL_LOCK_KEY 1
#endif

CImageTools::CImageTools() :
m_lock(new CThreadLock(), std::default_delete<CThreadLock>()),
#ifdef GDAL_THREADS
m_isThreadSafe(true)
#else
m_isThreadSafe(false)
#endif
{
    //need to call CGdalWrapper so that GDAL can register all drivers
    __ultra_internal::CGdalWrapper::getInstance()->getInstance();
}

CImageTools::~CImageTools()
{
}

std::shared_ptr<void> CImageTools::getLock()
{
    return __ultra_internal::CImageLoadSaveLocks::getInstance()->getLock(GDAL_LOCK_KEY, m_lock);
}

CImageTools *CImageTools::getInstance()
{
    static CImageTools instance;
    return &instance;
}

template<class T>
std::shared_ptr<void> CImageTools::createColorTableHandle(const CVector<SKeyValue<T, SColor> > &colorTable)
{
    std::shared_ptr<void> cth(new GDALColorTable(GPI_RGB), std::default_delete<GDALColorTable>());
    GDALColorTable *h = (GDALColorTable*) cth.get();
    GDALColorEntry ce;
    for (const auto &kv : colorTable)
    {
        ce.c1 = kv.v.red();
        ce.c2 = kv.v.green();
        ce.c3 = kv.v.blue();
        ce.c4 = kv.v.alpha();
        h->SetColorEntry((int) kv.k, &ce);
    }
    return cth;
}

template<class T>
int CImageTools::LockedSetNoDataValue(std::string pathToImageFile, T noDataValue, int bandNumber)
{
    auto deleter = [](GDALDataset * h)
    {
        if (h != nullptr)
        {
            GDALClose(h);
        }
    };
    std::unique_ptr<GDALDataset, decltype(deleter) > dataset((GDALDataset*) GDALOpen(pathToImageFile.c_str(), GA_Update), deleter);
    if (dataset.get() == nullptr)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to open image at '" + pathToImageFile + "'");
        return 1;
    }
    if (dataset.get() == nullptr)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to open image at '" + pathToImageFile + "'");
        return 1;
    }

    if (bandNumber < 1 || bandNumber > dataset->GetRasterCount())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Band '" + toString(bandNumber) + "' is not found only has '" + toString(dataset->GetRasterCount()) + "' band(s)");
        return 1;
    }
    GDALRasterBand* raster = dataset->GetRasterBand(bandNumber);
    if (raster == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to obtain raster handle for band '" + toString(bandNumber) + "'");
        return 1;
    }
    if (raster->SetNoDataValue(noDataValue) != CE_None)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to set the nodata value for band '" + toString(bandNumber) + "'");
        return 1;
    }

    return 0;
}

template<class T>
int CImageTools::LockedRemoveNoDataValue(std::string pathToImageFile, int bandNumber)
{
    auto deleter = [](GDALDataset * h)
    {
        if (h != nullptr)
        {
            GDALClose(h);
        }
    };
    std::unique_ptr<GDALDataset, decltype(deleter) > dataset((GDALDataset*) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly), deleter);
    if (dataset.get() == nullptr)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to open image at '" + pathToImageFile + "'");
        return 1;
    }
    if (dataset.get() == nullptr)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to open image at '" + pathToImageFile + "'");
        return 1;
    }

    if (bandNumber < 1 || bandNumber > dataset->GetRasterCount())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Band '" + toString(bandNumber) + "' is not found only has '" + toString(dataset->GetRasterCount()) + "' band(s)");
        return 1;
    }
    GDALRasterBand* raster = dataset->GetRasterBand(bandNumber);
    if (raster == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to obtain raster handle for band '" + toString(bandNumber) + "'");
        return 1;
    }
    int error = raster->DeleteNoDataValue();
    if (error != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to remove the nodata value for band '" + toString(bandNumber) + "'");
        return 1;
    }

    return 0;
}

template<class T>
int CImageTools::LockedGetNoDataValue(std::string pathToImageFile, T &noDataValue, int bandNumber)
{
    auto deleter = [](GDALDataset * h)
    {
        if (h != nullptr)
        {
            GDALClose(h);
        }
    };
    std::unique_ptr<GDALDataset, decltype(deleter) > dataset((GDALDataset*) GDALOpen(pathToImageFile.c_str(), GA_ReadOnly), deleter);
    if (dataset.get() == nullptr)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to open image at '" + pathToImageFile + "'");
        return 1;
    }
    if (dataset.get() == nullptr)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to open image at '" + pathToImageFile + "'");
        return 1;
    }

    if (bandNumber < 1 || bandNumber > dataset->GetRasterCount())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Band '" + toString(bandNumber) + "' is not found only has '" + toString(dataset->GetRasterCount()) + "' band(s)");
        return 1;
    }
    GDALRasterBand* raster = dataset->GetRasterBand(bandNumber);
    if (raster == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to obtain raster handle for band '" + toString(bandNumber) + "'");
        return 1;
    }
    int success;
    noDataValue = (T) raster->GetNoDataValue(&success);
    if (!success)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to get the nodata value for band '" + toString(bandNumber) + "'");
        return 1;
    }

    return 0;
}

template<class T>
int CImageTools::SetNoDataValue_Temp(std::string pathToImageFile, T noDataValue, int bandNumber)
{
    int ret = 0;
    if (!m_isThreadSafe)
    {
        AUTO_LOCK(getLock());
        ret = LockedSetNoDataValue<T>(pathToImageFile, noDataValue, bandNumber);
    }
    else
        ret = LockedSetNoDataValue<T>(pathToImageFile, noDataValue, bandNumber);

    CFile delFile = CFile(pathToImageFile + ".aux.xml");
    if (delFile.isFile())
    {
        delFile.remove();
    }
    return ret;
}

template<class T>
int CImageTools::GetNoDataValue_Temp(std::string pathToImageFile, T &noDataValue, int bandNumber)
{
    if (!m_isThreadSafe)
    {
        AUTO_LOCK(getLock());
        return LockedGetNoDataValue<T>(pathToImageFile, noDataValue, bandNumber);
    }
    else
        return LockedGetNoDataValue<T>(pathToImageFile, noDataValue, bandNumber);
}

template<class T>
int CImageTools::RemoveNoDataValue_Temp(std::string pathToImageFile, int bandNumber)
{
    if (!m_isThreadSafe)
    {
        AUTO_LOCK(getLock());
        return LockedRemoveNoDataValue<T>(pathToImageFile, bandNumber);
    }
    else
        return LockedRemoveNoDataValue<T>(pathToImageFile, bandNumber);
}


int CImageTools::SetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber)
{
    return SetNoDataValue_Temp<double>(pathToImageFile, noDataValue, bandNumber);
}

int CImageTools::RemoveNoDataValue(std::string pathToImageFile, int bandNumber)
{
    return RemoveNoDataValue_Temp<double>(pathToImageFile, bandNumber);
}

int CImageTools::GetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber)
{
    return GetNoDataValue_Temp<double>(pathToImageFile, noDataValue, bandNumber);
}

} //namespace ultra
