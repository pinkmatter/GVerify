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
namespace __ultra_internal
{
namespace correlate_phase
{

CPhaseCorrelatorImpl::CPhaseCorrelatorImpl()
{
}

CPhaseCorrelatorImpl::~CPhaseCorrelatorImpl()
{
}

void CPhaseCorrelatorImpl::correlateComplexWt(CDft2d *fftOp, const CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > &reference,
                                              const CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > &input, CMatrix<double> &output,
                                              const SComplex<ADftBase2d::MY_WORKING_TYPE> *refNoData, const SComplex<ADftBase2d::MY_WORKING_TYPE> *inputNoData) const
{
    if (refNoData != nullptr || inputNoData != nullptr)
        throw CException(__FILE__, __LINE__, "Phase correlator cannot handle no-data");

    CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > referenceSpec;
    CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > inputSpec;
    fftOp->forward(reference, referenceSpec);
    fftOp->forward(input, inputSpec);

    auto size = inputSpec.getSize();
    CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > ans(size);
    for (unsigned long r = 0; r < size.row; r++)
    {
        auto rDp = referenceSpec[r].getDataPointer();
        auto iDp = inputSpec[r].getDataPointer();
        auto aDp = ans[r].getDataPointer();
        for (unsigned long c = 0; c < size.col; c++)
        {
            auto &v = aDp[c] = rDp[c] * iDp[c].conjugate();
            auto d = v.mod();
            if (d > 0)
                v /= d;
            else
                v.im = v.re = 0;
        }
    }
    CMatrix<SComplex<ADftBase2d::MY_WORKING_TYPE> > result;
    double normValue = size.getProduct();
    fftOp->inverse(ans, result);
    output = result.map<double>([normValue](const SComplex<ADftBase2d::MY_WORKING_TYPE> &v)->double
    {
        return v.mod() / normValue;
    });
    CDft2d::inverseFftMatrixSwapQuadrants(output);
}

} // namespace correlate_phase
} // namespace __ultra_internal
} // namespace ultra
