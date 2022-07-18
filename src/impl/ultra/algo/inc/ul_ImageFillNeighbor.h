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

#include <queue>

#include <ul_Matrix.h>
#include <ul_Logger.h>
#include <ul_Pair.h>

namespace ultra
{

template<class T>
class CImageFillNeighbor
{
private:

    SPair<long> m_mod[8];
    int m_fillDirections;

    int Init(const CMatrix<T> &inputImage, CMatrix<T> &outputImage, const SSize &startPoint)
    {
        if ((&inputImage) == (&outputImage))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input and output image cannot be the same image");
            return 1;
        }

        if (startPoint.col < 0 || startPoint.col >= inputImage.getSize().col ||
            startPoint.row < 0 || startPoint.row >= inputImage.getSize().row)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Fill start is outside of the image bounds");
            return 1;
        }

        outputImage = inputImage;

        /*
         * It is important to note that the first 4 in m_mod is for UP,LEFT,RIGHT and DOWN
         * The other 4 values are for the diagonal sections
         * This is so that if the fill direction is set to 4 then the first 4 items will be used
         * other wise all 8 are
         */

        m_mod[4] = SPair<long>(-1, -1);
        m_mod[0] = SPair<long>(-1, 0);
        m_mod[5] = SPair<long>(-1, 1);
        m_mod[1] = SPair<long>(0, -1);
        m_mod[2] = SPair<long>(0, 1);
        m_mod[6] = SPair<long>(1, -1);
        m_mod[3] = SPair<long>(1, 0);
        m_mod[7] = SPair<long>(1, 1);

        if (m_fillDirections != 4 && m_fillDirections != 8)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Fill direction must be either 4 or 8");
            return 1;
        }
        return 0;
    }

    int PopulateTempImage(CMatrix<T> &outputImage, T fillColor, T startColor, SSize startPoint)
    {
        std::queue<SSize> Q;
        SPair<long> neighbor;
        SSize newPoint;

        Q.push(startPoint);
        while (!Q.empty())
        {
            SSize point = Q.front();
            Q.pop();
            T &imageValue = outputImage[point.row][point.col];
            if (imageValue == startColor)
            {
                imageValue = fillColor;
                for (int x = 0; x < m_fillDirections; x++)
                {
                    neighbor.i = ((int) point.col) + m_mod[x].i;
                    neighbor.j = ((int) point.row) + m_mod[x].j;
                    if (neighbor.i < 0 || neighbor.i >= outputImage.getSize().col ||
                        neighbor.j < 0 || neighbor.j >= outputImage.getSize().row)
                    {
                        continue; //skip cuz we are outside of image bounds
                    }
                    newPoint.col = (unsigned int) neighbor.i;
                    newPoint.row = (unsigned int) neighbor.j;
                    Q.push(newPoint);
                }
            }
        }

        return 0;
    }

public:

    CImageFillNeighbor()
    {
    }

    virtual ~CImageFillNeighbor()
    {
    }

    int FillNeighbor(
                     const CMatrix<T> &inputImage,
                     CMatrix<T> &outputImage,
                     const SSize &startPoint,
                     T fillColor,
                     int fillDirections = 8
                     )
    {
        m_fillDirections = fillDirections;
        if (Init(inputImage, outputImage, startPoint) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
            return 1;
        }

        T colorAtStartPoint = outputImage[startPoint.row][startPoint.col];

        SSize newStartPoint = startPoint;
        if (PopulateTempImage(outputImage, fillColor, colorAtStartPoint, newStartPoint) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from _FillNeighbor()");
            return 1;
        }

        return 0;
    }

    static void fillNeighbor(
                             const CMatrix<T> &inputImage,
                             CMatrix<T> &outputImage,
                             const SSize &startPoint,
                             T fillColor,
                             int fillDirections = 8
                             )
    {
        CImageFillNeighbor<T> filler;
        if (filler.FillNeighbor(inputImage, outputImage, startPoint, fillColor, fillDirections) != 0)
            throw CException(__FILE__, __LINE__, "Failure returned from CImageFillNeighbor::FillNeighbor()");
    }
};

} //namespace ultra
