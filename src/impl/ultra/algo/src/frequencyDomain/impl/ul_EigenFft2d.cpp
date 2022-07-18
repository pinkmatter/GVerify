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

#include "ul_EigenFft.h"

#include <unsupported/Eigen/FFT>

namespace ultra
{
namespace __ultra_internal_fft
{

CEigenFft2d::CEigenFft2d(const SSize &size) :
ADftBase2d(size, "Eigen"),
m_fftOpRows(new Eigen::FFT<MY_WORKING_TYPE>(), [](void *p)->void
{
    if (p != nullptr)
    {
        Eigen::FFT<MY_WORKING_TYPE> *ptr = (Eigen::FFT<MY_WORKING_TYPE>*)p;
        delete ptr;
    }
}),
m_fftOpCols(new Eigen::FFT<MY_WORKING_TYPE>(), [](void *p)->void
{
    if (p != nullptr)
    {
        Eigen::FFT<MY_WORKING_TYPE> *ptr = (Eigen::FFT<MY_WORKING_TYPE>*)p;
        delete ptr;
    }
})
{
    ((Eigen::FFT<MY_WORKING_TYPE>*)m_fftOpRows.get())->SetFlag(Eigen::FFT<MY_WORKING_TYPE>::Speedy);
    ((Eigen::FFT<MY_WORKING_TYPE>*)m_fftOpCols.get())->SetFlag(Eigen::FFT<MY_WORKING_TYPE>::Speedy);
}

CEigenFft2d::~CEigenFft2d()
{
}

void CEigenFft2d::forwardImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const
{
    output.resize(m_size);
    Eigen::FFT<MY_WORKING_TYPE> *opRow = (Eigen::FFT<MY_WORKING_TYPE>*)m_fftOpRows.get();
    for (unsigned long r = 0; r < m_size.row; r++)
    {
        const auto *src = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)input[r].getDataPointer();
        auto *des = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)output[r].getDataPointer();
        opRow->fwd(des, src, m_size.col);
    }

    Eigen::FFT<MY_WORKING_TYPE> *opCol = (Eigen::FFT<MY_WORKING_TYPE>*)m_fftOpCols.get();
    CVector<SComplex<MY_WORKING_TYPE> > tempCol(m_size.row);
    auto *des = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)tempCol.getDataPointer();
    for (unsigned long c = 0; c < m_size.col; c++)
    {
        auto col = output.getColumn(c);
        auto *src = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)col.getDataPointer();
        opCol->fwd(des, src, m_size.row);
        output.setColumn(c, tempCol);
    }

    output /= SComplex<MY_WORKING_TYPE>(m_size.getProduct());
}

void CEigenFft2d::inverseImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const
{
    output.resize(m_size);

    Eigen::FFT<MY_WORKING_TYPE> *opRow = (Eigen::FFT<MY_WORKING_TYPE>*)m_fftOpRows.get();
    for (unsigned long r = 0; r < m_size.row; r++)
    {
        const auto *src = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)input[r].getDataPointer();
        auto *des = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)output[r].getDataPointer();
        opRow->inv(des, src, m_size.col);
    }

    Eigen::FFT<MY_WORKING_TYPE> *opCol = (Eigen::FFT<MY_WORKING_TYPE>*)m_fftOpCols.get();
    CVector<SComplex<MY_WORKING_TYPE> > tempCol(m_size.row);
    auto *des = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)tempCol.getDataPointer();
    for (unsigned long c = 0; c < m_size.col; c++)
    {
        auto col = output.getColumn(c);
        auto *src = (Eigen::FFT<MY_WORKING_TYPE>::Complex*)col.getDataPointer();
        opCol->inv(des, src, m_size.row);
        output.setColumn(c, tempCol);
    }
}

} // namespace __ultra_internal_fft
} // namespace ultra
