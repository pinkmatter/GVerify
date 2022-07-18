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

#include "ul_DftBase.h"

namespace ultra
{

ADftBase2d::ADftBase2d(const SSize &size, const std::string &fftLibName) :
m_size(size),
m_fftLibName(fftLibName)
{
}

ADftBase2d::~ADftBase2d()
{
}

std::string ADftBase2d::getFftLibraryName() const
{
    return m_fftLibName;
}

void ADftBase2d::forwardWt(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const
{
    if (&input == &output)
        throw CException(__FILE__, __LINE__, "Input and output cannot point to the same matrix");
    if (input.getSize() != m_size)
        throw CException(__FILE__, __LINE__, "Input matrix is of incorrect size");
    forwardImpl(input, output);
}

void ADftBase2d::inverseWt(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const
{
    if (&input == &output)
        throw CException(__FILE__, __LINE__, "Input and output cannot point to the same matrix");
    if (input.getSize() != m_size)
        throw CException(__FILE__, __LINE__, "Input matrix is of incorrect size");
    inverseImpl(input, output);
}


} // namespace ultra
