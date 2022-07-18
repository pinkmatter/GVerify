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

#include "ul_Resampler.h"

namespace ultra
{

std::string EResamplerEnum::resamplingTypeToStr(EResamplerEnum::EResampleType resampler)
{
    switch (resampler)
    {
    case EResamplerEnum::RESAMPLE_TYPE_NN:return "NN";
        break;
    case EResamplerEnum::RESAMPLE_TYPE_BI:return "BI";
        break;
    case EResamplerEnum::RESAMPLE_TYPE_CI:return "CI";
        break;
    case EResamplerEnum::RESAMPLE_TYPE_AVG:return "AVG";
        break;
    }
    return "";
}

EResamplerEnum::EResampleType EResamplerEnum::strToResamplingType(std::string resamplerStr)
{
    std::string s = toUpper(resamplerStr);
    if (s == "NN")
        return EResamplerEnum::RESAMPLE_TYPE_NN;
    if (s == "BI")
        return EResamplerEnum::RESAMPLE_TYPE_BI;
    if (s == "CI")
        return EResamplerEnum::RESAMPLE_TYPE_CI;
    if (s == "AVG")
        return EResamplerEnum::RESAMPLE_TYPE_AVG;

    return EResamplerEnum::RESAMPLE_TYPE_ENUM_COUNT;
}

namespace __ultra_internal
{

CResampleBilinearHelper::CResampleBilinearHelper()
{
}

CResampleBilinearHelper::~CResampleBilinearHelper()
{
}

CResampleBicubicHelper::CResampleBicubicHelper()
{
}

CResampleBicubicHelper::~CResampleBicubicHelper()
{
}

} // namespace __ultra_internal

} //namespace ultra
