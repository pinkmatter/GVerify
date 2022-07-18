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

#include <ul_Vector.h>
#include <ul_Size.h>
#include <ul_KeyValue.h>

namespace ultra
{

class CConvexHull
{
private:
    static bool sortFunction(const SKeyValue<unsigned long, SSize> &l, const SKeyValue<unsigned long, SSize> &r);
    static int sortFunctionVoid(const void *ll, const void *rr);
    static bool hullFuncCompFuncTop(float a1, float a2);
    static bool hullFuncCompFuncBot(float a1, float a2);

    void hullFunc(
                  const CVector<SKeyValue<unsigned long, SSize> > &pts,
                  std::vector<SKeyValue<unsigned long, SSize> > &topBot,
                  bool (*func)(float, float)
                  ) const;
public:
    CConvexHull();
    virtual ~CConvexHull();

    int GenerateHullPoint(const CVector<SSize> &points, CVector<unsigned long> &hullSequence) const;
};

} // namespace ultra
