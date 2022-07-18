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

#include <ul_Color.h>
#include <ul_Vector.h>
#include <ul_KeyValue.h>

namespace ultra
{

class CImageTools
{
private:
    std::shared_ptr<void> m_lock;
    bool m_isThreadSafe;
private:
    CImageTools();
    std::shared_ptr<void> getLock();

    template<class T>
    std::shared_ptr<void> createColorTableHandle(const CVector<SKeyValue<T, SColor> > &colorTable);
    template<class T>
    int LockedAddPct(std::string pathToImageFile, const CVector<SKeyValue<T, SColor> > &colorTable, int bandNumber);
    template<class T>
    int LockedSetNoDataValue(std::string pathToImageFile, T noDataValue, int bandNumber);
    template<class T>
    int LockedGetNoDataValue(std::string pathToImageFile, T &noDataValue, int bandNumber);
    template<class T>
    int LockedRemoveNoDataValue(std::string pathToImageFile, int bandNumber);

    template<class T>
    int SetNoDataValue_Temp(std::string pathToImageFile, T noDataValue, int bandNumber);
    template<class T>
    int GetNoDataValue_Temp(std::string pathToImageFile, T &noDataValue, int bandNumber);
    template<class T>
    int RemoveNoDataValue_Temp(std::string pathToImageFile, int bandNumber);
public:
    virtual ~CImageTools();
    static CImageTools *getInstance();

    int SetNoDataValue(std::string pathToImageFile, double noDataValue, int bandNumber);
    int GetNoDataValue(std::string pathToImageFile, double &noDataValue, int bandNumber);
    int RemoveNoDataValue(std::string pathToImageFile, int bandNumber);
};

} // namespace ultra
