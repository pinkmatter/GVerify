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

#include <ul_Logger.h>
#include <ul_MatrixArray.h>
#include <ul_Pair.h>
#include <ul_Complex.h>
#include <ul_UltraThreadPool.h>

namespace ultra
{

struct EResamplerEnum
{

    enum EResampleType
    {
        RESAMPLE_TYPE_NN = 0,
        RESAMPLE_TYPE_BI,
        RESAMPLE_TYPE_CI,
        RESAMPLE_TYPE_AVG,
        RESAMPLE_TYPE_ENUM_COUNT
    };

    enum ELinearResampleType
    {
        RESAMPLE_CENTER = 0,
        RESAMPLE_UL,
        RESAMPLE_UR,
        RESAMPLE_LL,
        RESAMPLE_LR,
        RESAMPLE_TYPE_COUNT
    };

    static std::string resamplingTypeToStr(EResamplerEnum::EResampleType resampler);
    static EResamplerEnum::EResampleType strToResamplingType(std::string resamplerStr);
};

namespace __ultra_internal
{

class CResampleBilinearHelper
{
private:

    template<class T>
    void addBi(const CMatrix<T> &input, const T *srcNullValue, const SSize &inputSize, const double &per, const SPair<long> &index, T &val, double &divVal) const
    {
        if (index.r >= 0 && index.r < inputSize.row &&
                index.c >= 0 && index.c < inputSize.col)
        {
            T v = input[index.r][index.c];
            if (srcNullValue == nullptr || v != *srcNullValue)
            {
                v *= per;
                val += v;
                divVal += per;
            }
        }
    }

public:
    CResampleBilinearHelper();
    virtual ~CResampleBilinearHelper();

    template<class T>
    void biLinearInterpolate(const CMatrix<T> &input, const SPair<double> &outputToInput, const T *srcNullValue, const SSize &inputSize, bool &usable, T &val) const
    {
        val = 0;
        double divVal = 0;

        double R_d = outputToInput.y;
        double C_d = outputToInput.x;

        long R_i = (long) floor(R_d);
        long C_i = (long) floor(C_d);

        double R_down = R_d - R_i;
        double C_right = C_d - C_i;
        double R_res = R_down;
        double C_res = C_right;

        double R_up = 1.0 - R_res;
        double C_left = 1.0 - C_res;

        double UL_per = R_up*C_left;
        double LL_per = R_down*C_left;
        double LR_per = R_down*C_right;
        double UR_per = R_up*C_right;

        SPair<long> UL_index((long) floor(R_d), (long) floor(C_d));
        SPair<long> LL_index((long) ceil(R_d), (long) floor(C_d));
        SPair<long> LR_index((long) ceil(R_d), (long) ceil(C_d));
        SPair<long> UR_index((long) floor(R_d), (long) ceil(C_d));

        addBi(input, srcNullValue, inputSize, UL_per, UL_index, val, divVal);
        addBi(input, srcNullValue, inputSize, LL_per, LL_index, val, divVal);
        addBi(input, srcNullValue, inputSize, LR_per, LR_index, val, divVal);
        addBi(input, srcNullValue, inputSize, UR_per, UR_index, val, divVal);

        if (divVal != 0)
        {
            usable = true;
            val /= divVal;
        }
        else
        {
            usable = false;
        }
    }

    template<class T>
    void biLinearInterpolate(const CMatrix<T> &input, const SPair<float> &outputToInput, const T *nullValue, const SSize &inputSize, bool &usable, T &val) const
    {
        biLinearInterpolate(input, outputToInput.convertType<double>(), nullValue, inputSize, usable, val);
    }
};

class CResampleBicubicHelper
{
private:

    double cubicInterpolate(double *convVec, double indexXVal) const
    {
        double a = (-0.5 * convVec[0]) + (1.5 * convVec[1]) + (-1.5 * convVec[2]) + (0.5 * convVec[3]);
        double b = (convVec[0]) + (-2.5 * convVec[1])+(2.0 * convVec[2])+(-0.5 * convVec[3]);
        double c = (convVec[2] - convVec[0]) / 2.0;
        double d = convVec[1];

        double indexXVal2 = indexXVal*indexXVal;
        double indexXVal3 = indexXVal*indexXVal2;
        //ax^3 + bx^2 + cx +d
        double retVal = a * (indexXVal3) + b * (indexXVal2) + c * (indexXVal) + d;
        return retVal;
    }

    SComplex<double> cubicInterpolate(SComplex<double> *convVec, double indexXVal) const
    {
        SComplex<double> a = (-0.5 * convVec[0]) + (1.5 * convVec[1]) + (-1.5 * convVec[2]) + (0.5 * convVec[3]);
        SComplex<double> b = (convVec[0]) + (-2.5 * convVec[1])+(2.0 * convVec[2])+(-0.5 * convVec[3]);
        SComplex<double> c = (convVec[2] - convVec[0]) / 2.0;
        SComplex<double> d = convVec[1];

        double indexXVal2 = indexXVal*indexXVal;
        double indexXVal3 = indexXVal*indexXVal2;
        //ax^3 + bx^2 + cx +d
        SComplex<double> retVal = a * (indexXVal3) + b * (indexXVal2) + c * (indexXVal) + d;
        return retVal;
    }

    template<class T, class BUF>
    void biCubicInterpolateImpl(const CMatrix<T> &input, const SPair<double> &outputToInput, const T *srcNullValue, const SSize &inputSize, bool &usable, BUF &val,
                                BUF *convVec1, BUF *convVec2) const
    {
        usable = false;
        long inputRMax = inputSize.row;
        long inputCMax = inputSize.col;
        double irIndex, icIndex;
        double irIndexInt, icIndexInt;
        double irIndexRes, icIndexRes;

        for (long ir2 = -1, vecItR = 0; ir2 < 3; ir2++, vecItR++)
        {
            irIndex = ir2 + outputToInput.r;
            if (irIndex < 0 || irIndex >= inputRMax)
                return;
            else
            {
                irIndexInt = floor(irIndex);
                irIndexRes = irIndex - irIndexInt;
            }

            for (long ic2 = -1, vecItC = 0; ic2 < 3; ic2++, vecItC++)
            {
                icIndex = ic2 + outputToInput.c;
                if (icIndex < 0 || icIndex >= inputCMax)
                    return;
                else
                {
                    icIndexInt = floor(icIndex);
                    icIndexRes = icIndex - icIndexInt;
                }

                convVec1[vecItC] = input[(long) irIndexInt][(long) icIndexInt];
                if (srcNullValue != nullptr && convVec1[vecItC] == *srcNullValue)
                    return;
            }
            convVec2[vecItR] = cubicInterpolate(convVec1, icIndexRes);
        }
        usable = true;
        val = cubicInterpolate(convVec2, irIndexRes);
    }

public:

    CResampleBicubicHelper();
    virtual ~CResampleBicubicHelper();

    template<class T>
    void biCubicInterpolate(const CMatrix<SComplex<T> > &input, const SPair<double> &outputToInput, const SComplex<T> *nullValue, const SSize &inputSize, bool &usable, SComplex<T> &val) const
    {
        SComplex<double> convVec1[4];
        SComplex<double> convVec2[4];
        SComplex<double> ans;
        biCubicInterpolateImpl<SComplex<T>, SComplex<double> >(input, outputToInput, nullValue, inputSize, usable, ans, convVec1, convVec2);
        if (std::numeric_limits<T>::is_integer)
            val = ans.round().convertType<T>();
        else
            val = ans.convertType<T>();
    }

    template<class T>
    void biCubicInterpolate(const CMatrix<T> &input, const SPair<double> &outputToInput, const T *nullValue, const SSize &inputSize, bool &usable, T &val) const
    {
        double convVec1[4];
        double convVec2[4];
        double ans;
        biCubicInterpolateImpl<T, double>(input, outputToInput, nullValue, inputSize, usable, ans, convVec1, convVec2);
        if (std::numeric_limits<T>::is_integer)
            val = (T) std::round(ans);
        else
            val = (T) ans;
    }

    template<class T>
    void biCubicInterpolate(const CMatrix<T> &input, const SPair<float> &outputToInput, const T *nullValue, const SSize &inputSize, bool &usable, T &val) const
    {
        biCubicInterpolate<T>(input, outputToInput.convertType<double>(), nullValue, inputSize, usable, val);
    }

    template<class T>
    void biCubicInterpolate(const CMatrix<SComplex<T> > &input, const SPair<float> &outputToInput, const SComplex<T> *nullValue, const SSize &inputSize, bool &usable, SComplex<T> &val) const
    {
        biCubicInterpolate<T>(input, outputToInput.convertType<double>(), nullValue, inputSize, usable, val);
    }
};

} // namespace __ultra_internal

template<bool ENABLE_PERCENTAGE_LOGGER = true >
class CResampler
{
private:

    template <class T, class N>
    struct SResamplerCtx
    {
        std::function<int( const CMatrix<T> &, CMatrix<T> &, const CMatrix<SPair<N> > *, const T *, const T *) > fp_resampler;
    };

    template<class T, class N>
    int Init(
             const CMatrix<T> &input,
             CMatrix<T> &output,
             const CMatrix<SPair<N> > &outputToInputMap,
             EResamplerEnum::EResampleType resType,
             SResamplerCtx<T, N> &ctx,
             const T *srcNullValue,
             const T *trgNullValue
             )
    {
        if (&input == &output)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input and output image may not point to the same image");
            return 1;
        }
        output.resize(outputToInputMap.getSize());
        if (srcNullValue == nullptr && trgNullValue == nullptr)
            output.initMat(T(0));
        else if (trgNullValue != nullptr)
            output.initMat(*trgNullValue);
        else
            output.initMat(*srcNullValue);
        ctx.fp_resampler = nullptr;

        switch (resType)
        {
        case EResamplerEnum::RESAMPLE_TYPE_NN:
            ctx.fp_resampler = CResampler::innerExecuteNN<T, N>;
            if (ENABLE_PERCENTAGE_LOGGER)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampler is nearest neighbor");
            }
            break;
        case EResamplerEnum::RESAMPLE_TYPE_BI:
            ctx.fp_resampler = CResampler::innerExecuteBI<T, N>;
            if (ENABLE_PERCENTAGE_LOGGER)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampler is bilinear");
            }
            break;
        case EResamplerEnum::RESAMPLE_TYPE_CI:
            ctx.fp_resampler = CResampler::innerExecuteCI<T, N>;
            if (ENABLE_PERCENTAGE_LOGGER)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampler is bicubic");
            }
            break;
        case EResamplerEnum::RESAMPLE_TYPE_AVG:
            ctx.fp_resampler = CResampler::innerExecuteAVG<T, N>;
            if (ENABLE_PERCENTAGE_LOGGER)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampler is averaging");
            }
            break;
        default:
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented resampler '" + EResamplerEnum::resamplingTypeToStr(resType) + "'");
            return 1;
            break;
        }

        if (!ctx.fp_resampler)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unknown resampling technique");
            return 1;
        }

        return 0;
    }

    template<class T, class N>
    static int innerExecuteNN(
                              const CMatrix<T> &input,
                              CMatrix<T> &output,
                              const CMatrix<SPair<N> > *outputToInputMap,
                              const T *srcNullValue, const T *trgNullValue
                              )
    {
        return ExecuteNN<T, N>(input, output, *outputToInputMap, srcNullValue, trgNullValue);
    }

    template<class T, class N>
    static int innerExecuteBI(
                              const CMatrix<T> &input,
                              CMatrix<T> &output,
                              const CMatrix<SPair<N> > *outputToInputMap,
                              const T *srcNullValue, const T *trgNullValue
                              )
    {
        return ExecuteBI<T, N>(input, output, *outputToInputMap, srcNullValue, trgNullValue);
    }

    template<class T, class N>
    static int innerExecuteCI(
                              const CMatrix<T> &input,
                              CMatrix<T> &output,
                              const CMatrix<SPair<N> > *outputToInputMap,
                              const T *srcNullValue, const T *trgNullValue
                              )
    {
        return ExecuteCI<T, N>(input, output, *outputToInputMap, srcNullValue, trgNullValue);
    }

    template<class T, class N>
    static int innerExecuteAVG(
                               const CMatrix<T> &input,
                               CMatrix<T> &output,
                               const CMatrix<SPair<N> > *outputToInputMap,
                               const T *srcNullValue, const T *trgNullValue
                               )
    {
        return ExecuteAVG<T, N>(input, output, *outputToInputMap, srcNullValue, trgNullValue);
    }

    template<class T, class I>
    static int ExecuteNN(
                         const CMatrix<T> &input,
                         CMatrix<T> &output,
                         const CMatrix<SPair<I> > &outputToInputMap,
                         const T *srcNullValue, const T *trgNullValue
                         )
    {
        SSize inputSize = input.getSize();
        SSize outputSize = output.getSize();
        long r, c;
        long newR, newC;
        int per = -1;

        for (r = 0; r < outputSize.row; r++)
        {
            if (per != (int) ((r * 100.0) / outputSize.row))
            {
                per = (int) ((r * 100.0) / outputSize.row);
                if (ENABLE_PERCENTAGE_LOGGER)
                {
                    CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling " + toString(per) + "%     \r");
                }
            }
            for (c = 0; c < outputSize.col; c++)
            {
                if (outputToInputMap[r][c].containsNaN())
                    continue;
                newR = (outputToInputMap[r][c].y >= 0) ? ((long) (outputToInputMap[r][c].y + 0.5)) : ((long) (outputToInputMap[r][c].y - 0.5));
                newC = (outputToInputMap[r][c].x >= 0) ? ((long) (outputToInputMap[r][c].x + 0.5)) : ((long) (outputToInputMap[r][c].x - 0.5));

                if (newR >= 0 && newR < inputSize.row &&
                        newC >= 0 && newC < inputSize.col)
                {
                    if (srcNullValue == nullptr || input[newR][newC] != *srcNullValue)
                        output[r][c] = input[newR][newC];
                }
            }
        }
        if (ENABLE_PERCENTAGE_LOGGER)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling 100%     ");
        }

        return 0;
    }

    template<class T, class I>
    static int ExecuteBI(
                         const CMatrix<T> &input,
                         CMatrix<T> &output,
                         const CMatrix<SPair<I> > &outputToInputMap,
                         const T *srcNullValue, const T *trgNullValue
                         )
    {
        __ultra_internal::CResampleBilinearHelper resamp;
        int per = -1;
        SSize outputSize = output.getSize();
        SSize inputSize = input.getSize();
        bool usable = false;
        for (long r = 0, sr = (long) outputSize.row; r < sr; r++)
        {
            if (per != (int) ((r * 100.0) / outputSize.row))
            {
                per = (int) ((r * 100.0) / outputSize.row);
                if (ENABLE_PERCENTAGE_LOGGER)
                {
                    CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling " + toString(per) + "%     \r");
                }
            }
            for (long c = 0, sc = (long) outputSize.col; c < sc; c++)
            {
                if (outputToInputMap[r][c].containsNaN())
                    continue;

                T val = 0;
                resamp.biLinearInterpolate<T>(input, outputToInputMap[r][c], srcNullValue, inputSize, usable, val);
                if (usable)
                    output[r][c] = val;
            }
        }
        if (ENABLE_PERCENTAGE_LOGGER)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling 100%     ");
        }

        return 0;
    }

    template< class I>
    static void orderAndClampIndices(const SPair<I> &inputSizeInc, SPair<I> &ulIndex, SPair<I> &lrIndex)
    {
        static const SPair<I> zero(0);
        if (ulIndex.r > lrIndex.r)
            swapValues<I>(ulIndex.r, lrIndex.r);
        if (ulIndex.c > lrIndex.c)
            swapValues<I>(ulIndex.c, lrIndex.c);

        ulIndex = (ulIndex + 1e-10).floorValues().clipBetween(zero, inputSizeInc);
        lrIndex = (lrIndex - 1e-10).ceilValues().clipBetween(zero, inputSizeInc);
    }

    template<class T, class I>
    static int ExecuteAVG(
                          const CMatrix<T> &input,
                          CMatrix<T> &output,
                          const CMatrix<SPair<I> > &outputToInputMap,
                          const T *srcNullValue, const T *trgNullValue
                          )

    {
        __ultra_internal::CResampleBilinearHelper resampBi;
        bool usable = false;
        SSize inputSizeSize = input.getSize();
        SPair<I> inputSize = inputSizeSize;
        SPair<I> inputSizeInc = inputSize - 1; //inclusive
        int per = -1;
        SPair<long> outputSize = output.getSize();
        SPair<I> ulIndex, lrIndex; //inclusive
        T sum;
        double count = 0;
        for (long r = 0; r < outputSize.r; r++)
        {
            if (per != (int) ((r * 100.0) / outputSize.r))
            {
                per = (int) ((r * 100.0) / outputSize.r);
                if (ENABLE_PERCENTAGE_LOGGER)
                {
                    CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling " + toString(per) + "%     \r");
                }
            }
            auto *outDp = output[r].getDataPointer();
            long lrR = clip<long>(r + 1, 0, outputSize.r - 1);
            for (long c = 0; c < outputSize.c; c++)
            {
                long lrC = clip<long>(c + 1, 0, outputSize.c - 1);
                ulIndex = outputToInputMap[r][c];
                lrIndex = outputToInputMap[lrR][lrC] - 1;

                if (ulIndex == lrIndex)
                {
                    resampBi.biLinearInterpolate<T>(input, ulIndex, srcNullValue, inputSizeSize, usable, sum);
                    if (usable)
                        output[r][c] = sum;
                }
                else
                {
                    orderAndClampIndices(inputSizeInc, ulIndex, lrIndex);
                    count = 0;
                    for (long rr = ulIndex.r, err = lrIndex.r; rr <= err; rr++)
                    {
                        const auto *inDp = input[rr].getDataPointer();
                        for (long cc = ulIndex.c, ecc = lrIndex.c; cc <= ecc; cc++)
                        {
                            const auto &inVal = inDp[cc];
                            if (srcNullValue == nullptr || inVal != *srcNullValue)
                            {
                                if (count == 0)
                                    sum = inVal;
                                else
                                    sum += inVal;
                                count++;
                            }
                        }
                    }
                    if (count)
                        outDp[c] = sum / count;
                }
            }
        }

        if (ENABLE_PERCENTAGE_LOGGER)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling 100%     ");
        }
        return 0;
    }

    template<class T, class I>
    static int ExecuteCI(
                         const CMatrix<T> &input,
                         CMatrix<T> &output,
                         const CMatrix<SPair<I> > &outputToInputMap,
                         const T *srcNullValue, const T *trgNullValue
                         )
    {
        __ultra_internal::CResampleBicubicHelper resamp;
        SSize inputSize = input.getSize();
        SSize outputSize = output.getSize();
        int per = -1;
        long rMax = outputSize.row;
        long cMax = outputSize.col;
        bool usable = false;
        for (long r = 0; r < rMax; r++)
        {
            if (per != (int) ((r * 100.0) / rMax))
            {
                per = (int) ((r * 100.0) / rMax);
                if (ENABLE_PERCENTAGE_LOGGER)
                {
                    CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling " + toString(per) + "%     \r");
                }
            }
            for (long c = 0; c < cMax; c++)
            {
                if (outputToInputMap[r][c].containsNaN())
                    continue;
                T val = 0;
                resamp.biCubicInterpolate(input, outputToInputMap[r][c], srcNullValue, inputSize, usable, val);
                if (usable)
                    output[r][c] = val;
            }
        }
        if (ENABLE_PERCENTAGE_LOGGER)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Resampling 100%     ");
        }

        return 0;
    }

    template<class T, class N>
    int innerResample(
                      const CMatrix<T> &input,
                      CMatrix<T> &output,
                      const CMatrix<SPair<N> > &outputToInputMap,
                      EResamplerEnum::EResampleType resType,
                      const T *srcNullValue, const T *trgNullValue
                      )
    {
        SResamplerCtx<T, N> ctx;
        if (Init<T, N>(input, output, outputToInputMap, resType, ctx, srcNullValue, trgNullValue) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
            return 1;
        }

        if (ctx.fp_resampler(input, output, &outputToInputMap, srcNullValue, trgNullValue) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from fp_resampler()");
            return 1;
        }

        return 0;
    }

    template<class T>
    int innerResizeImpl(const CMatrix<T> &input,
                        CMatrix<T> &output,
                        const SSize &outputImageSize,
                        EResamplerEnum::EResampleType resType,
                        const T *srcNullValue, const T *trgNullValue)
    {
        CMatrix<SPair<float> > outputToInputMap;
        SPair<float> inputSize = input.getSize();
        SPair<float> step;
        outputToInputMap.resize(outputImageSize);

        step.r = inputSize.r / (float) outputImageSize.row;
        step.c = inputSize.c / (float) outputImageSize.col;

        for (unsigned long r = 0; r < outputImageSize.row; r++)
        {
            float rV = step.r * (float) r;
            for (unsigned long c = 0; c < outputImageSize.col; c++)
            {
                outputToInputMap[r][c].r = rV;
                outputToInputMap[r][c].c = step.c * (float) c;
            }
        }

        if (innerResample<T, float>(input, output, outputToInputMap, resType, srcNullValue, trgNullValue) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from innerResample()");
            return 1;
        }
        return 0;
    }

    template<class T>
    int innerResize(
                    const CMatrix<T> &input,
                    CMatrix<T> &output,
                    const SSize &outputImageSize,
                    EResamplerEnum::EResampleType resType,
                    const T *srcNullValue, const T *trgNullValue
                    )
    {
        if (outputImageSize.containsZero())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Output image size is too small");
            return 1;
        }

        if (input.getSize().containsZero())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image size is too small");
            return 1;
        }

        if (outputImageSize == input.getSize())
        {
            output = input;
            if (output.getSize() != input.getSize())
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to generate output matrix");
                return 1;
            }
            return 0;
        }
        return innerResizeImpl(input, output, outputImageSize, resType, srcNullValue, trgNullValue);
    }

    CResampler()
    {
    }

public:

    virtual ~CResampler()
    {
    }

    template<class T, class N>
    static int Resample(
                        const CMatrix<T> &input,
                        CMatrix<T> &output,
                        const CMatrix<SPair<N> > &outputToInputMap,
                        EResamplerEnum::EResampleType resType = EResamplerEnum::RESAMPLE_TYPE_NN,
                        const T *srcNullValue = nullptr, const T *trgNullValue = nullptr
                        )
    {
        std::unique_ptr<CResampler> res(new CResampler());
        return res->template innerResample<T, N>(input, output, outputToInputMap, resType, srcNullValue, trgNullValue);
    }

    template<class T>
    static int Resize(
                      const CMatrix<T> &input,
                      CMatrix<T> &output,
                      const SSize &outputImageSize,
                      EResamplerEnum::EResampleType resType = EResamplerEnum::RESAMPLE_TYPE_NN,
                      const T *srcNullValue = nullptr, const T *trgNullValue = nullptr
                      )
    {
        std::unique_ptr<CResampler> res(new CResampler());
        return res->template innerResize<T>(input, output, outputImageSize, resType, srcNullValue, trgNullValue);
    }

};

} //namespace ultra
