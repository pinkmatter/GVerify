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

#include "ul_Proj4Projection.h"

#include <ul_Logger.h>
#include <proj.h>

namespace ultra
{

const char *CProj4Projection::BASE_LAT_LONG_PROJ4_STR = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +type=crs";
const char *CProj4Projection::EPSG3857 = "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext  +no_defs";
const double CProj4Projection::DEG2RAD = M_PI / 180.0;
const double CProj4Projection::RAD2DEG = 180.0 / M_PI;

std::string CProj4Projection::UTM(int zone)
{
    bool south = zone < 0;
    zone = getAbs(zone);
    return "+proj=utm +datum=WGS84 +zone=" + toString(zone) + " " + (south ? "+south " : "") + "+no_defs";
}

std::string CProj4Projection::proj2proj4Str(const std::string &anyProjStr)
{
    std::string result = "";
    PJ *P = proj_create(PJ_DEFAULT_CTX, anyProjStr.c_str());
    if (P != nullptr)
    {
        auto retProj4 = proj_as_proj_string(PJ_DEFAULT_CTX, P, PJ_PROJ_4, nullptr);
        result = retProj4 ? retProj4 : "";
        proj_destroy(P);
    }
    return result;
}

std::string CProj4Projection::proj2wkt(const std::string &anyProjStr)
{
    std::string result = "";
    PJ *P = proj_create(PJ_DEFAULT_CTX, anyProjStr.c_str());
    if (P != nullptr)
    {
        auto retProj4 = proj_as_wkt(PJ_DEFAULT_CTX, P, PJ_WKT2_2015_SIMPLIFIED, nullptr);
        result = retProj4 ? retProj4 : "";
        proj_destroy(P);
    }
    return result;
}

CProj4Projection::CProj4Projection(const std::string &inputProjStr, const std::string &outputProjStr)
{
    m_inputProjStr = trimStr(inputProjStr);
    m_outputProjStr = trimStr(outputProjStr);

    m_srcDesData = nullptr;
    PJ *P = proj_create_crs_to_crs(PJ_DEFAULT_CTX, inputProjStr.c_str(), outputProjStr.c_str(), nullptr);

    if (P != nullptr)
    {
        PJ *NP = proj_normalize_for_visualization(PJ_DEFAULT_CTX, P);
        proj_destroy(P);
        P = NP;
        NP = nullptr;
        m_srcDesData = reinterpret_cast<void*> (P);
    }
}

CProj4Projection::~CProj4Projection()
{
    if (m_srcDesData != nullptr)
    {
        proj_destroy(reinterpret_cast<PJ*> (m_srcDesData));
        m_srcDesData = nullptr;
    }
}

int CProj4Projection::CheckSrcDesProjections() const
{
    if (m_srcDesData == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create proj6 transform for projection '" + m_inputProjStr + "' to '" + m_outputProjStr + "'");
        return 1;
    }
    return 0;
}

std::string CProj4Projection::getInputProj4Str() const
{
    return m_inputProjStr;
}

std::string CProj4Projection::getOutputProj4Str() const
{
    return m_outputProjStr;
}

int CProj4Projection::Project(double *x, double *y, unsigned long size, unsigned long strideX, unsigned long strideY) const
{
    if (m_inputProjStr == m_outputProjStr)
    {
        return 0;
    }
    if (CheckSrcDesProjections() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CheckSrcDesProjections()");
        return 1;
    }
    if (proj_trans_generic(reinterpret_cast<PJ*> (m_srcDesData), PJ_FWD,
                           x, strideX, size,
                           y, strideY, size,
                           nullptr, 0, 0,
                           nullptr, 0, 0) != size)
    {
        int code = proj_errno(reinterpret_cast<PJ*> (m_srcDesData));
        const char *errTxt = proj_errno_string(code);
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from proj_trans_generic() '" + toString(errTxt) + "'");
        return 1;
    }

    return 0;
}

int CProj4Projection::Project(const SPair<double> &input, SPair<double> &output) const
{
    output = input;
    static const unsigned long stride = sizeof (SPair<double>);
    return Project(&output.x, &output.y, 1, stride, stride);
}

int CProj4Projection::Project(CVector<double> &x, CVector<double> &y) const
{
    return Project(x, y, getMIN<unsigned long>(x.size(), y.size()));
}

int CProj4Projection::Project(CVector<double> &x, CVector<double> &y, unsigned long itemCount) const
{
    if (itemCount > x.size() ||
        itemCount > y.size())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Cannot project '" + toString(itemCount) + "', it is more than what is contained in the vectors");
        return 1;
    }
    static const unsigned long stride = sizeof (double);
    return Project(x.getDataPointer(), y.getDataPointer(), itemCount, stride, stride);
}

int CProj4Projection::Project(const CVector<SPair<double> > &input, CVector<SPair<double> > &output) const
{
    output = input;
    unsigned long size = output.size();
    if (size == 0)
        return 0;
    double *x = &output[0].x;
    double *y = &output[0].y;
    static const unsigned long stride = sizeof (SPair<double>);
    if (Project(x, y, size, stride, stride) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Project()");
        return 1;
    }
    return 0;
}

int CProj4Projection::Project(const CMatrix<SPair<double> > &in, CMatrix<SPair<double> > &out) const
{
    SSize size = in.getSize();
    out.resize(size);

    for (unsigned long r = 0; r < size.row; r++)
    {
        if (Project(in[r], out[r]) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Project()");
            return 1;
        }
    }
    return 0;
}

} // namespace ultra
