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

#include "ul_CreateConvKernel.h"

namespace ultra
{

class CConvKernelToImage
{
private:

    template<class T>
    static int Init(const CMatrix<T> &input, const CConvKernel<T> &kernel, CMatrix<T> &output)
    {
        if (input.getSize().row < kernel.getSize().row ||
            input.getSize().col < kernel.getSize().col ||
            kernel.getSize().row < 1 ||
            kernel.getSize().col < 1 ||
            kernel.getSize().row % 2 != 1 ||
            kernel.getSize().col % 2 != 1)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input image and kernel image have incorrect sizes");
            return 1;
        }

        if ((&input) == (&output))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input and output images may not be the same image");
            return 1;
        }

        if (output.getSize() != input.getSize())
        {
            output.resize(input.getSize());
            output.initMat(T(0));
        }

        return 0;
    }

    template<class T>
    static int execute(const CMatrix<T> &input, const CConvKernel<T> &kernel, CMatrix<T> &output, bool normalizeKernel, const T *nodata, bool onlyOverValidData, bool useLogger)
    {
        CMatrix<T> temp;
        CMatrix<T> *outputPtr = &output;
        if (&input == &output)
        {
            outputPtr = &temp;
        }

        if (CConvKernelToImage::Init<T>(input, kernel, *outputPtr) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
            return 1;
        }

        long ar_Size = (long) kernel.getAnchor().row;
        long ac_Size = (long) kernel.getAnchor().col;
        long kr_Size = (long) kernel.getSize().row;
        long kc_Size = (long) kernel.getSize().col;
        long r_Size = (long) input.getSize().row;
        long c_Size = (long) input.getSize().col;

        bool hasShownWarning = false;
        if (!useLogger)
            hasShownWarning = true;

        int per = 0;
        for (long r = 0; r < r_Size; r++)
        {
            for (long c = 0; c < c_Size; c++)
            {
                T total = 0;
                T kernelSum = 0;
                bool gotPixel = false;
                bool mustContinue = false;
                for (long kr = kr_Size - 1; kr >= 0; kr--)
                {
                    for (long kc = kc_Size - 1; kc >= 0; kc--)
                    {
                        long rr = r - kr + ar_Size;
                        long cc = c - kc + ac_Size;

                        //skip if outside bounds
                        if (rr < 0 || cc < 0 || rr >= r_Size || cc >= c_Size ||
                            (nodata != nullptr && *nodata == input[rr][cc]))
                        {
                            if (onlyOverValidData)
                            {
                                if (nodata != nullptr)
                                    (*outputPtr)[r][c] = *nodata;
                                mustContinue = true;
                                goto endLoop;
                            }
                            continue;
                        }

                        kernelSum += kernel[kr][kc];
                        total += input[rr][cc] * kernel[kr][kc];
                        gotPixel = true;
                    }
                }
endLoop:
                if (mustContinue)
                    continue;

                if (!gotPixel)
                {
                    if (nodata != nullptr)
                        (*outputPtr)[r][c] = *nodata;
                }
                else
                {
                    if (!normalizeKernel)
                    {
                        (*outputPtr)[r][c] = total;
                    }
                    else
                    {
                        if (std::numeric_limits<T>::is_integer)
                        {
                            if (kernelSum == 0)
                            {
                                if (!hasShownWarning)
                                {
                                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Kernel sum is zero");
                                    hasShownWarning = true;
                                }
                                (*outputPtr)[r][c] = total;
                            }
                            else
                            {
                                (*outputPtr)[r][c] = total / kernelSum;
                            }
                        }
                        else//floating points
                        {
                            if (getAbs(kernelSum) < std::numeric_limits<T>::epsilon())
                            {
                                if (!hasShownWarning)
                                {
                                    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Kernel sum is less than epsilon");
                                    hasShownWarning = true;
                                }
                                (*outputPtr)[r][c] = total;
                            }
                            else
                            {
                                (*outputPtr)[r][c] = total / kernelSum;
                            }
                        }
                    }
                }
            }
            if (useLogger)
            {
                if (per != (int) ((r * 100) / input.getSize().row + 1))
                {
                    per = (int) ((r * 100) / input.getSize().row + 1);
                    CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, toString(per) + "%     \r");
                }
            }
        }
        if (useLogger)
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "100%   ");

        if (&input == &output)
        {
            output = temp;
        }
        return 0;
    }

public:

    CConvKernelToImage()
    {

    }

    virtual ~CConvKernelToImage()
    {

    }

    template<class T>
    static int Convolve(const CMatrix<T> &input, const CConvKernel<T> &kernel, CMatrix<T> &output, bool normalizeKernel = true, const T *nodata = nullptr, bool enableLogger = false)
    {
        return execute(input, kernel, output, normalizeKernel, nodata, false, enableLogger);
    }

    /**
     * If a nodata value is given, will only convolve if the input window contain only valid data. 
     * @return 
     */
    template<class T>
    static int ConvolveOverValidData(const CMatrix<T> &input, const CConvKernel<T> &kernel, CMatrix<T> &output, bool normalizeKernel = true, const T *nodata = nullptr, bool enableLogger = false)
    {
        return execute(input, kernel, output, normalizeKernel, nodata, true, enableLogger);
    }


};

} //namespace ultra
