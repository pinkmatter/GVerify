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

#include <ul_IntegralMatrix.h>

namespace ultra
{
namespace __ultra_internal
{
namespace correlate_coef_norm
{
namespace
{

class CCoefNormTemplateMatching
{
private:

    template<class T>
    static std::function<void(double &outputVal, const T &top, const T &bot) > createFp()
    {
        static constexpr T eps = std::numeric_limits<T>::epsilon();
        static auto fp = [](double &outputVal, const T &top, const T & bot)->void
        {
            if (getAbs(bot) < eps)
            {
                outputVal = 0;
                if (bot < 0)
                    outputVal = top / (-eps);
                else if (bot > 0)
                    outputVal = top / (eps);
                outputVal = (outputVal > 1) ? (1) : ((outputVal<-1) ? (-1) : (0));
            }
            else
            {
                outputVal = top / bot;
            }
        };
        return fp;
    }

    template<class T>
    static std::function<void(double &outputVal, const SComplex<T> &top, const SComplex<T> &bot) > createFpComplex()
    {
        static constexpr T eps = std::numeric_limits<T>::epsilon();
        static auto fpComplex = [](double &outputVal, const SComplex<T> &top, const SComplex<T> &bot)->void
        {
            if (getAbs(bot) < eps)
                outputVal = (top / SComplex<T>::fromMagPhase(eps, bot.phase())).re;
            else
                outputVal = (top / bot).re;
        };
        return fpComplex;
    }
private:

    template<class T>
    static T getMean(const CMatrix<T> &image, const SSize &ul, const SSize &size, const T *noDataVal)
    {
        T divCounter = 0;
        T sum = 0;
        unsigned long stopRow = ul.row + size.row;
        unsigned long stopCol = ul.col + size.col;
        for (unsigned long r = ul.row; r < stopRow; r++)
        {
            auto *dp = image[r].getDataPointer();
            for (unsigned long c = ul.col; c < stopCol; c++)
            {
                T tempVal = dp[c];
                if (noDataVal == nullptr || *noDataVal != tempVal)
                {
                    divCounter++;
                    sum += tempVal;
                }
            }
        }
        return divCounter > 0 ? sum / divCounter : sum;
    }

    template<class T>
    static void coefNormalizedImpl(const CMatrix<T> &searchImage, const CMatrix<T> &tempImage, CMatrix<double> &output,
                                   const std::function<void(double &outputVal, const T &top, const T &bot)> &fp)
    {
        auto searchSize = searchImage.getSize();
        auto tempSize = tempImage.getSize();
        if (tempSize.row > searchSize.row ||
                tempSize.col > searchSize.col)
            throw CException(__FILE__, __LINE__, "Template image is larger than the search image");
        auto outSize = searchSize + 1 - tempSize;
        output.resize(outSize);

        auto tempImageNormRemoved = tempImage - tempImage.mean();
        CIntegralMatrix searchIntegralImage = CIntegralMatrix::createMeanIntegralMatrix<T>(searchImage, tempSize.getProduct());

        T Tval = 0;
        T Sval = 0;
        T top = 0;
        T botT = 0;
        T botS = 0;
        T bot = 0;
        T SMean;

        for (SSize loop(0, 0); loop.row < outSize.row; loop.row++)
        {
            auto outputDp = output[loop.row].getDataPointer();
            for (loop.col = 0; loop.col < outSize.col; loop.col++)
            {
                SMean = static_cast<T> (searchIntegralImage.getSum(loop, tempSize)); //get sum as we already passed in the tempSize.getProduct()
                Tval = 0;
                Sval = 0;
                top = 0;
                botT = 0;
                botS = 0;
                bot = 0;
                for (unsigned long inR = 0; inR < tempSize.row; inR++)
                {
                    auto TvalDp = tempImageNormRemoved[inR].getDataPointer();
                    auto SvalDp = searchImage[loop.row + inR].getDataPointer();
                    for (unsigned long inC = 0; inC < tempSize.col; inC++)
                    {
                        Tval = TvalDp[inC];
                        Sval = SvalDp[loop.col + inC] - SMean;
                        top += Tval * Sval;
                        botT += Tval * Tval;
                        botS += Sval * Sval;
                    }
                }
                bot = sqrt(botT * botS);
                auto &outputVal = outputDp[loop.col];
                fp(outputVal, top, bot);
            }
        }
    }

    template<class T>
    static void coefNormalizedNullValuedImpl(const CMatrix<T> &searchImage, const CMatrix<T> &tempImage, CMatrix<double> &output,
                                             const T *searchNoData, const T *tempNoData,
                                             const std::function<void(double &outputVal, const T &top, const T &bot)> &fp)
    {
        auto searchSize = searchImage.getSize();
        auto tempSize = tempImage.getSize();
        if (tempSize.row > searchSize.row ||
                tempSize.col > searchSize.col)
            throw CException(__FILE__, __LINE__, "Template image is larger than the search image");
        auto outSize = searchSize + 1 - tempSize;
        output.resize(outSize);

        CMatrix<T> tempImageNormRemoved;
        if (tempNoData == nullptr)
            tempImageNormRemoved = tempImage - tempImage.mean();
        else
        {
            T tNoData = *tempNoData;
            T m = tempImage.mean(tNoData);
            tempImageNormRemoved = tempImage.template map<T>([m, tNoData](const T & v)->T
            {
                return v == tNoData ? v : v - m;
            });
        }

        T Tval = 0;
        T Sval = 0;
        T top = 0;
        T botT = 0;
        T botS = 0;
        T bot = 0;
        T SMean;

        for (SSize loop(0, 0); loop.row < outSize.row; loop.row++)
        {
            auto outputDp = output[loop.row].getDataPointer();
            for (loop.col = 0; loop.col < outSize.col; loop.col++)
            {
                SMean = getMean<T>(searchImage, loop, tempSize, searchNoData);
                Tval = 0;
                Sval = 0;
                top = 0;
                botT = 0;
                botS = 0;
                bot = 0;
                for (unsigned long inR = 0; inR < tempSize.row; inR++)
                {
                    auto TvalDpOriginal = tempImage[inR].getDataPointer();
                    auto TvalDp = tempImageNormRemoved[inR].getDataPointer();
                    auto SvalDp = searchImage[loop.row + inR].getDataPointer();
                    for (unsigned long inC = 0; inC < tempSize.col; inC++)
                    {
                        if (tempNoData != nullptr && *tempNoData == TvalDpOriginal[inC])
                            continue;
                        Tval = TvalDp[inC];

                        Sval = SvalDp[loop.col + inC];
                        if (searchNoData != nullptr && *searchNoData == Sval)
                            continue;
                        Sval -= SMean;

                        top += Tval * Sval;
                        botT += Tval * Tval;
                        botS += Sval * Sval;
                    }
                }
                bot = sqrt(botT * botS);
                auto &outputVal = outputDp[loop.col];
                fp(outputVal, top, bot);
            }
        }
    }
public:

    template<class T>
    static void coefNormalizedComplex(const CMatrix<SComplex<T> > &searchImage, const CMatrix<SComplex<T> > &tempImage, CMatrix<double> &output)
    {
        coefNormalizedImpl<SComplex<T> > (searchImage, tempImage, output, createFpComplex<T>());
    }

    template<class T>
    static void coefNormalized(const CMatrix<T> &searchImage, const CMatrix<T> &tempImage, CMatrix<double> &output)
    {
        coefNormalizedImpl<T>(searchImage, tempImage, output, createFp<T>());
    }

    template<class T>
    static void coefNormalizedNullValuedComplex(const CMatrix<SComplex<T> > &searchImage, const CMatrix<SComplex<T> > &tempImage, CMatrix<double> &output,
                                                const SComplex<T> *searchNoData, const SComplex<T> *tempNoData)
    {
        coefNormalizedNullValuedImpl<SComplex<T> >(searchImage, tempImage, output, searchNoData, tempNoData, createFpComplex<T>());
    }

    template<class T>
    static void coefNormalizedNullValued(const CMatrix<T> &searchImage, const CMatrix<T> &tempImage, CMatrix<double> &output,
                                         const T *searchNoData, const T *tempNoData)
    {
        coefNormalizedNullValuedImpl<T>(searchImage, tempImage, output, searchNoData, tempNoData, createFp<T>());
    }


};

} // namespace

CCoefNormCorrelatorImpl::CCoefNormCorrelatorImpl()
{
}

CCoefNormCorrelatorImpl::~CCoefNormCorrelatorImpl()
{
}

void CCoefNormCorrelatorImpl::correlateNormalWt(const CMatrix<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> &reference,
                                                const CMatrix<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> &input,
                                                CMatrix<double> &output,
                                                const CCoefNormCorrelatorImpl::MY_WORKING_TYPE *refNoData,
                                                const CCoefNormCorrelatorImpl::MY_WORKING_TYPE *inputNoData) const
{
    bool refHasNoData = refNoData == nullptr ? false : reference.contains(*refNoData);
    bool inHasNoData = inputNoData == nullptr ? false : input.contains(*inputNoData);
    bool canUseNormalMethod = !refHasNoData && !inHasNoData;
    if (canUseNormalMethod)
        CCoefNormTemplateMatching::coefNormalized<CCoefNormCorrelatorImpl::MY_WORKING_TYPE>(reference, input, output);
    else
        CCoefNormTemplateMatching::coefNormalizedNullValued<CCoefNormCorrelatorImpl::MY_WORKING_TYPE>(reference, input, output, refNoData, inputNoData);
}

void CCoefNormCorrelatorImpl::correlateComplexWt(const CMatrix<SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> > &reference,
                                                 const CMatrix<SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> > &input,
                                                 CMatrix<double> &output,
                                                 const SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> *refNoData,
                                                 const SComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE> *inputNoData) const
{
    bool refHasNoData = refNoData == nullptr ? false : reference.contains(*refNoData);
    bool inHasNoData = inputNoData == nullptr ? false : input.contains(*inputNoData);
    bool canUseNormalMethod = !refHasNoData && !inHasNoData;
    if (canUseNormalMethod)
        CCoefNormTemplateMatching::coefNormalizedComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE>(reference, input, output);
    else
        CCoefNormTemplateMatching::coefNormalizedNullValuedComplex<CCoefNormCorrelatorImpl::MY_WORKING_TYPE>(reference, input, output, refNoData, inputNoData);
}

} // namespace correlate_coef_norm
} // namespace __ultra_internal
} // namespace ultra
