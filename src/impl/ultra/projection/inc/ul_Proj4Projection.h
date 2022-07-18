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

#include <ul_Pair.h>
#include <ul_Vector.h>
#include <ul_Matrix.h>

namespace ultra
{

class CProj4Projection
{
public:
    static const char *BASE_LAT_LONG_PROJ4_STR;
    static const char *EPSG3857;
    static const double DEG2RAD;
    static const double RAD2DEG;
    static std::string UTM(int zone);
    static std::string proj2proj4Str(const std::string &anyProjStr);
    static std::string proj2wkt(const std::string &anyProjStr);
private:
    std::string m_inputProjStr;
    std::string m_outputProjStr;
    void *m_srcDesData;
    int CheckSrcDesProjections() const;

    int Project(double *x, double *y, unsigned long size, unsigned long strideX, unsigned long strideY) const;
public:
    CProj4Projection(const std::string &inputProjStr, const std::string &outputProjStr);
    virtual ~CProj4Projection();

    std::string getInputProj4Str() const;
    std::string getOutputProj4Str() const;

    int Project(const SPair<double> &input, SPair<double> &output) const;
    int Project(CVector<double> &x, CVector<double> &y) const;
    int Project(CVector<double> &x, CVector<double> &y, unsigned long itemCount) const;
    int Project(const CVector<SPair<double> > &input, CVector<SPair<double> > &output) const;
    int Project(const CMatrix<SPair<double> > &in, CMatrix<SPair<double> > &out) const;
};

} // namespace ultra
