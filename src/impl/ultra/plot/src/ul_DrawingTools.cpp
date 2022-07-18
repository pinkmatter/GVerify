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

#include "ul_DrawingTools.h"
#include "ul_SmartVector.h"
#include "ul_Logger.h"

namespace ultra
{

void CDrawingTools::calcStraightLineValues(const SPair<long> &point1, const SPair<long> &point2,
                                           double &m, double &c, bool &deltaXIsZero,
                                           const SPair<long> &imageSize, long &minX, long &maxX, long &minY, long &maxY,
                                           bool &canDraw)
{

    canDraw = true;
    if ((point1.r < 0 && point2.r < 0) ||
            (point1.r >= imageSize.r && point2.r < imageSize.r) ||
            (point1.c < 0 && point2.c < 0) ||
            (point1.c >= imageSize.c && point2.c < imageSize.c))
    {
        canDraw = false;
    }
    long deltaY = point1.y - point2.y;
    long deltaX = point1.x - point2.x;

    deltaXIsZero = false;
    if (deltaX == 0)
    {
        deltaXIsZero = true;
        c = point1.x;
    }
    else
    {
        m = ((double) deltaY) / ((double) deltaX);
        c = point1.y - m * point1.x;
    }

    minX = ((point1.x < point2.x) ? (point1.x) : (point2.x));
    maxX = ((point1.x > point2.x) ? (point1.x) : (point2.x));

    if (minX < 0)
        minX = 0;
    else if (minX + 1 > imageSize.c)
        minX = imageSize.c;
    if (maxX < 0)
        maxX = 0;
    else if (maxX + 1 > imageSize.c)
        maxX = imageSize.c;

    minY = ((point1.y < point2.y) ? (point1.y) : (point2.y));
    maxY = ((point1.y > point2.y) ? (point1.y) : (point2.y));

    if (minY < 0)
        minY = 0;
    else if (minY + 1 > imageSize.r)
        minY = imageSize.r;
    if (maxY < 0)
        maxY = 0;
    else if (maxY + 1 > imageSize.r)
        maxY = imageSize.r;
}

} //namespace ultra
