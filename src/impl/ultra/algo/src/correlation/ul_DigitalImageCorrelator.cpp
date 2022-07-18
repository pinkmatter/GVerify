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

#include "ul_DigitalImageCorrelator.h"

namespace ultra
{

std::string CCorrelationHelper::typeToStr(ECorrelationType type)
{
    switch (type)
    {
    case ECorrelationType::CCOEFF_NORM:return"CCOEFF_NORM";
    case ECorrelationType::PHASE:return"PHASE";
    }
    return "UNKOWN";
}

std::string CCorrelationHelper::typeToStr(ESubPixelCorrelationType type)
{
    switch (type)
    {
    case ESubPixelCorrelationType::LEAST_SQUARE_SUB_PIXEL:return "LEAST_SQUARE_SUB_PIXEL";
    case ESubPixelCorrelationType::PHASE_SUB_PIXEL:return "PHASE_SUB_PIXEL";
    }
    return "UNKOWN";
}

} // namespace ultra
