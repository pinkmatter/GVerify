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

#include "ul_ConvKernel.h"
#include "ul_DrawingTools.h"

namespace ultra
{

struct EKernelTypes
{

    enum EConvKernelType
    {
        GAUSSIAN = 0,
        SOBEL_X,
        SOBEL_Y,
        CONVKERNELTYPE_COUNT
    };
};

class CCreateConvKernel
{
private:

    template<class T>
    struct SInnerCtx
    {
        T stdDevValue;
        bool normalize;
        bool evenRow;
        bool evenCol;
    };

    template<class T>
    int Init(const SSize &size, T stdDevValue, EKernelTypes::EConvKernelType kernType, SInnerCtx<T> &ctx)
    {
        if (size.row <= 1 || size.col <= 1)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Kernel size is smaller or equal to 1");
            return 1;
        }

        ctx.evenRow = size.row % 2 == 0;
        ctx.evenCol = size.col % 2 == 0;

        if (kernType != EKernelTypes::GAUSSIAN)
        {
            if (ctx.evenRow || ctx.evenCol)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Kernel size is not of odd size");
                return 1;
            }
        }

        if (!std::numeric_limits<T>::is_integer)
        {
            if (getAbs(stdDevValue) < std::numeric_limits<T>::epsilon())
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "stdDev cannot be smaller than the numeric limit epsilon");
                return 1;
            }
        }
        else
        {
            if (stdDevValue == 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "stdDev cannot be zero");
                return 1;
            }
        }
        ctx.stdDevValue = stdDevValue;

        return 0;
    }

    /**
     * This will make sure that the kernel values all add up to '1'
     * @param mat
     * @return 
     */
    template<class T>
    CConvKernel<T> Normalize(const CConvKernel<T> &mat)
    {
        CConvKernel<T> ret = CConvKernel<T > (mat);
        double sum = 0;

        for (unsigned int r = 0; r < ret.getSize().row; r++)
        {
            for (unsigned int c = 0; c < ret.getSize().col; c++)
            {
                sum += getAbs((double) (ret[r][c]));
            }
        }

        if (std::numeric_limits<T>::is_integer)
        {
            if (sum == 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Sum of all values is zero");
                return ret;
            }
        }
        else
        {
            if (getAbs(sum) < std::numeric_limits<T>::epsilon())
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Sum of all values is less than epsilon");
                return ret;
            }
        }

        for (unsigned int r = 0; r < ret.getSize().row; r++)
        {
            for (unsigned int c = 0; c < ret.getSize().col; c++)
            {
                ret[r][c] = (T) (ret[r][c] / sum);
            }
        }

        return ret;
    }

    template<class T>
    int getGaussian(const SInnerCtx<T> &ctx, CConvKernel<T> &kernel)
    {
        T first = 1.0 / (2.0 * M_PI * ctx.stdDevValue * ctx.stdDevValue);

        SSize kernSize = kernel.getSize();
        SSize kernAnc = kernel.getAnchor();
        T div = (2.0 * ctx.stdDevValue * ctx.stdDevValue);
        if (ctx.evenRow && ctx.evenCol)
        {
            for (unsigned int r = 0; r < kernSize.row / 2; r++)
            {
                for (unsigned int c = 0; c < kernSize.col / 2; c++)
                {
                    T rr = (T) (r)-(T) (kernAnc.row);
                    T cc = (T) (c)-(T) (kernAnc.col);
                    T second = exp(-1 * (T(rr * rr + cc * cc)) / div);
                    T ans = first*second;

                    kernel[r][c] = ans;
                    kernel[r][kernSize.col - 1 - c] = ans;
                    kernel[kernSize.row - 1 - r][c] = ans;
                    kernel[kernSize.row - 1 - r] [kernSize.col - 1 - c] = ans;
                }
            }
        }
        else if (ctx.evenRow)
        {
            for (unsigned int r = 0; r < kernSize.row / 2; r++)
            {
                for (unsigned int c = 0; c < kernSize.col; c++)
                {
                    T rr = (T) (r)-(T) (kernAnc.row);
                    T cc = (T) (c)-(T) (kernAnc.col);
                    T second = exp(-1 * (T(rr * rr + cc * cc)) / div);
                    T ans = first*second;

                    kernel[r][c] = ans;
                    kernel[kernSize.row - 1 - r][c] = ans;
                }
            }
        }
        else if (ctx.evenCol)
        {
            for (unsigned int r = 0; r < kernSize.row; r++)
            {
                for (unsigned int c = 0; c < kernSize.col / 2; c++)
                {
                    T rr = (T) (r)-(T) (kernAnc.row);
                    T cc = (T) (c)-(T) (kernAnc.col);
                    T second = exp(-1 * (T(rr * rr + cc * cc)) / div);
                    T ans = first*second;

                    kernel[r][c] = ans;
                    kernel[r][kernSize.col - 1 - c] = ans;
                }
            }
        }
        else
        {
            for (unsigned int r = 0; r < kernSize.row; r++)
            {
                for (unsigned int c = 0; c < kernSize.col; c++)
                {
                    T rr = (T) (r)-(T) (kernAnc.row);
                    T cc = (T) (c)-(T) (kernAnc.col);
                    T second = exp(-1 * (T(rr * rr + cc * cc)) / div);
                    T ans = first*second;

                    kernel[r][c] = ans;
                }
            }
        }
        if (ctx.normalize)
        {
            kernel = Normalize<T>(kernel);
        }

        return 0;
    }

    template<class T>
    int getSobelX(const SInnerCtx<T> &ctx, CConvKernel<T> &kernel)
    {
        if (kernel.getSize() != SSize(3, 3))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Sobel kernels must be of size 3x3");
            return 1;
        }

        kernel[0][0] = -1;
        kernel[0][1] = 0;
        kernel[0][2] = 1;
        kernel[1][0] = -2;
        kernel[1][1] = 0;
        kernel[1][2] = 2;
        kernel[2][0] = -1;
        kernel[2][1] = 0;
        kernel[2][2] = 1;

        return 0;
    }

    template<class T>
    int getSobelY(const SInnerCtx<T> &ctx, CConvKernel<T> &kernel)
    {
        if (kernel.getSize() != SSize(3, 3))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Sobel kernels must be of size 3x3");
            return 1;
        }

        kernel[0][0] = 1;
        kernel[0][1] = 2;
        kernel[0][2] = 1;
        kernel[1][0] = 0;
        kernel[1][1] = 0;
        kernel[1][2] = 0;
        kernel[2][0] = -1;
        kernel[2][1] = -2;
        kernel[2][2] = -1;

        return 0;
    }
    
    CCreateConvKernel()
    {

    }

public:

    virtual ~CCreateConvKernel()
    {

    }

    template<class T>
    static CConvKernel<T> getKernel(EKernelTypes::EConvKernelType kernelType, const SSize &size, T stdDevValue = std::numeric_limits<T>::epsilon(), bool normalize = true)
    {
        CConvKernel<T> kernel;
        if (GetKernel<T>(kernelType, size, kernel, stdDevValue, normalize) != 0)
        {
            throw CException(__FILE__, __LINE__, "Failure returned from GetKernel()");
        }
        return kernel;
    }

    /**
     * 
     * @param kernelType
     * @param size The size of the kernel the anchor point will always be the center point, the size must be odd
     * @return 
     */
    template<class T>
    static int GetKernel(EKernelTypes::EConvKernelType kernelType, const SSize &size, CConvKernel<T> &kernel, T stdDevValue = std::numeric_limits<T>::epsilon(), bool normalize = true)
    {
        std::unique_ptr<CCreateConvKernel> ptr(new CCreateConvKernel());
        SInnerCtx<T> ctx;
        ctx.normalize = normalize;
        if (ptr->Init<T>(size, stdDevValue, kernelType, ctx) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
            return 1;
        }

        SSize anchor = SSize(size.row / 2, size.col / 2);
        if (kernel.getSize() != size)
        {
            kernel.resize(size);
        }
        if (kernel.getAnchor() != anchor)
        {
            kernel.setAnchor(anchor);
        }

        switch (kernelType)
        {
        case EKernelTypes::GAUSSIAN:
            if (ptr->getGaussian<T>(ctx, kernel) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getGaussian()");
                return 1;
            }
            break;
        case EKernelTypes::SOBEL_X:
            if (ptr->getSobelX<T>(ctx, kernel) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getSobelX()");
                return 1;
            }
            break;
        case EKernelTypes::SOBEL_Y:
            if (ptr->getSobelY<T>(ctx, kernel) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getSobelY()");
                return 1;
            }
            break;
        default:
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid/Unsupported kernel type");
            return 1;
        }

        return 0;
    }

};

} // namespace ultra
