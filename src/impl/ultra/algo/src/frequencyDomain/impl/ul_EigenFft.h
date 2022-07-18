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

#include "ul_DftBase.h"

namespace ultra
{
namespace __ultra_internal_fft
{

class CEigenFft2d : public ADftBase2d
{
private:
    std::shared_ptr<void> m_fftOpRows;
    std::shared_ptr<void> m_fftOpCols;
    virtual void forwardImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const override;
    virtual void inverseImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const override;
public:
    CEigenFft2d(const SSize &size);
    virtual ~CEigenFft2d();
    using ADftBase2d::forward;
    using ADftBase2d::inverse;
};

} // namespace __ultra_internal_fft
} // namespace ultra
