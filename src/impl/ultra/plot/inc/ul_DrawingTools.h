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

#include "ul_Image.h"
#include "ul_Pair.h"
#include "ul_Color.h"
#include "ul_Line.h"
#include "ul_SmartVector.h"

namespace ultra
{

class CDrawingTools
{
private:

    template<class T>
    static void drawPixel(CMatrix<T> &image, const SSize &size, const SPair<long> &point, const T &value)
    {
        if (point.r >= 0 && point.r < size.row &&
                point.c >= 0 && point.c < size.col)
        {
            image[point.r][point.c] = value;
        }
    }

    static void calcStraightLineValues(const SPair<long> &point1, const SPair<long> &point2,
                                       double &m, double &c, bool &deltaXIsZero,
                                       const SPair<long> &imageSize, long &minX, long &maxX, long &minY, long &maxY,
                                       bool &canDraw);

public:

    template<class T>
    static void line(CMatrix<T> &image, const SPair<long> &point1, const SPair<long> &point2, const T &value)
    {
        bool draw = false;
        if (point1 == point2)
        {
            if (point1.y < 0 ||
                    point1.x < 0 ||
                    point1.y >= image.getSize().row ||
                    point1.x >= image.getSize().col)
            {
                return; //invalid point
            }
            draw = true;
            image[point1.y][point1.x] = value;
            return;
        }

        double m, c;
        bool deltaXIsZero;
        SPair<long> imageSize = image.getSize();
        long minX, maxX, minY, maxY;
        bool canDraw;
        calcStraightLineValues(point1, point2, m, c, deltaXIsZero, imageSize, minX, maxX, minY, maxY, canDraw);
        if (!canDraw)
            return;

        if (deltaXIsZero)//x=c
        {
            c = std::round(c);
            if (c >= 0 && c < imageSize.c)
            {
                for (long y = minY; y <= maxY; y++)
                {
                    if (y < 0 || y >= imageSize.r)
                        continue;
                    draw = true;
                    image[y][(unsigned int) c] = value;
                }
            }
        }
        else
        {
            if (getAbs(m) <= 1)//y=mx+c
            {
                for (long x = minX; x <= maxX; x++)
                {
                    if (x < 0 || x >= imageSize.c)
                        continue;
                    double y = std::round(m * ((double) x) + c);
                    if (y >= 0 && y < imageSize.r)
                    {
                        draw = true;
                        image[(unsigned int) y][x] = value;
                    }
                }
            }
            else//x=(y-c)/m
            {
                for (long y = minY; y <= maxY; y++)
                {
                    if (y < 0 || y >= imageSize.r)
                        continue;
                    double x = std::round(((double) y - c) / m);
                    if (x >= 0 && x < imageSize.c)
                    {
                        draw = true;
                        image[y][(unsigned int) x] = value;
                    }
                }
            }
        }

        if (!draw)
        {
            drawPixel<T>(image, imageSize.getSizeType(), point1, value);
        }
    }

};

} //namespace ultra
