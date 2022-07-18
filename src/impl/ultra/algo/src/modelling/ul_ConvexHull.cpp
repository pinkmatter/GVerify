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

#include "ul_ConvexHull.h"

#include <ul_Logger.h>
#include <ul_Pair.h>

namespace ultra
{

CConvexHull::CConvexHull()
{

}

CConvexHull::~CConvexHull()
{

}

bool CConvexHull::sortFunction(const SKeyValue<unsigned long, SSize> &l, const SKeyValue<unsigned long, SSize> &r)
{
    long temp = (long(l.v.col))-(long(r.v.col));
    if (temp != 0)
    {
        if (temp < 0)
            return true;
        return false;
    }

    if ((long(l.v.row))-(long(r.v.row)) < 0)
        return true;

    return false;
}

int CConvexHull::sortFunctionVoid(const void *ll, const void *rr)
{
    const SKeyValue<unsigned long, SSize> *l = (const SKeyValue<unsigned long, SSize> *)ll;
    const SKeyValue<unsigned long, SSize> *r = (const SKeyValue<unsigned long, SSize> *)rr;

    long temp = (long(l->v.col))-(long(r->v.col));
    if (temp < 0) return -1;
    if (temp > 0) return 1;


    temp = (long(l->v.row))-(long(r->v.row));
    if (temp < 0) return -1;
    if (temp > 0) return 1;

    return 0;
}

bool CConvexHull::hullFuncCompFuncTop(float a1, float a2)
{

    return a2 <= a1;
}

bool CConvexHull::hullFuncCompFuncBot(float a1, float a2)
{

    return a2 >= a1;
}

void CConvexHull::hullFunc(
                           const CVector<SKeyValue<unsigned long, SSize> > &pts,
                           std::vector<SKeyValue<unsigned long, SSize> > &topBot,
                           bool (*func)(float, float)
                           ) const
{
    long size = pts.size();
    SPair<float> A, B, C;
    long aIndex = 0;
    long bIndex = 1;
    float a1, a2;
    long x, y;
    bool canUse;
    topBot.push_back(pts[0]);
    int per = 0;
    for (x = 0; x < size - 2; x++)
    {
        canUse = true;
        A = pts[aIndex].v;
        B = pts[bIndex].v;
        a1 = (B.y - A.y) / (B.x - A.x);

        for (y = bIndex + 1; y < size; y++)
        {
            C = pts[y].v;
            a2 = (C.y - B.y) / (C.x - B.x);
            if (func(a2, a1))
            {
                canUse = false;
                break;
            }
        }
        if (canUse)
        {
            aIndex = bIndex;
            topBot.push_back(pts[bIndex]);
        }
        bIndex++;

        if (per != (int) ((x * 100) / size))
        {

            per = (int) ((x * 100) / size);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "CConvexHull " + toString(per) + "%     \r");
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "CConvexHull 100%      ");
}

int CConvexHull::GenerateHullPoint(const CVector<SSize> &points, CVector<unsigned long> &hullSequence) const
{
    long size = points.size();
    if (size < 3)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Hull point vector must contain at least 3 points");
        return 1;
    }
    CVector<SKeyValue<unsigned long, SSize> > pts;
    std::vector<SKeyValue<unsigned long, SSize> > ptsToSort;
    ptsToSort.resize(size);
    pts.resize(size);

    for (unsigned long t = 0; t < size; t++)
    {
        ptsToSort[t].k = pts[t].k = t;
        ptsToSort[t].v = pts[t].v = points[t];

    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Sorting '" + toString(size) + "' data points");
    qsort(&ptsToSort[0], size, sizeof (SKeyValue<unsigned long, SSize>), CConvexHull::sortFunctionVoid);

    pts = ptsToSort;
    ptsToSort.clear();

    std::vector<SKeyValue<unsigned long, SSize> > top;
    std::vector<SKeyValue<unsigned long, SSize> > bot;

    hullFunc(pts, top, CConvexHull::hullFuncCompFuncBot);

    top.push_back(pts[size - 1]);

    hullFunc(pts, bot, CConvexHull::hullFuncCompFuncTop);

    size = top.size();
    if (bot.size() >= 2)
    {
        size += bot.size();
    }
    hullSequence.resize(size);
    unsigned long t;
    for (t = 0; t < top.size(); t++)
        hullSequence[t] = top[t].k;

    unsigned long s2 = bot.size() - 1;
    for (unsigned long a = s2; t < size; t++, a--)
        hullSequence[t] = bot[a].k;

    return 0;
}

} // namespace ultra
