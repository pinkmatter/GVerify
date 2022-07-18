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

#include "ul_HostDft.h"
#include "impl/ul_EigenFft.h"

namespace ultra
{
namespace
{

std::string getOpName()
{
    SSize s(2, 2);
    std::shared_ptr<ADftBase2d> op = std::shared_ptr<ADftBase2d>(new __ultra_internal_fft::CEigenFft2d(s));
    if (!op)
        throw CException(__FILE__, __LINE__, "No FFT library found");
    return op->getFftLibraryName();
}
} // namespace

CHostDft2d::CHostDft2d(const SSize &size) :
ADftBase2d(size, getOpName())
{
    m_op = std::shared_ptr<ADftBase2d>(new __ultra_internal_fft::CEigenFft2d(size));
    if (!m_op)
        throw CException(__FILE__, __LINE__, "No FFT library found");
}

CHostDft2d::~CHostDft2d()
{
}

void CHostDft2d::forwardImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const
{
    if (m_op)
        m_op->forward(input, output);
    else
        throw CException(__FILE__, __LINE__, "No FFT library found");
}

void CHostDft2d::inverseImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const
{
    if (m_op)
        m_op->inverse(input, output);
    else
        throw CException(__FILE__, __LINE__, "No FFT library found");
}

} // namespace ultra
