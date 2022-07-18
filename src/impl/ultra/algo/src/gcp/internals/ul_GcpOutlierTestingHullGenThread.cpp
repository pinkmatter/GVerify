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

#include "ul_GcpHullRejection.h"

#include <ul_ConvKernelToImage.h>
#include <ul_ConvexHull.h>
#include <ul_DrawingTools.h>
#include <ul_ImageFillNeighbor.h>
#include <ul_Logger.h>
#include <ul_PaddImage.h>

namespace ultra
{

CGcpHullRejection::CHullGenThread::CHullGenThread(
                                                  CMatrix<float> *image, CMatrix<float> *hull,
                                                  float nullValue,
                                                  float nonNullValue,
                                                  unsigned long trimAmount,
                                                  unsigned long paddSize
                                                  )
{
    m_exitCode = 0;
    m_image = image;
    m_hull = hull;
    m_nullValue = nullValue;
    m_nonNullValue = nonNullValue;
    m_trimAmount = trimAmount;
    m_paddSize = paddSize;
}

CGcpHullRejection::CHullGenThread::~CHullGenThread()
{
    m_image = nullptr;
    m_hull = nullptr;
}

int CGcpHullRejection::CHullGenThread::FillPixelDataArea(const CMatrix<float> *image, CMatrix<float> &hull) const
{
    CImageFillNeighbor<float> *filler = nullptr;
    CMatrix<float> outputImage;
    SSize dunkPoint;
    bool found = false;
    SSize size = hull.getSize();
    int fillDirections = 4;
    for (unsigned long r = 0; r < size.row && !found; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            if (hull[r][c] == m_nullValue && (*image)[r][c] != m_nullValue)
            {
                dunkPoint.row = r;
                dunkPoint.col = c;
                found = true;
                break;
            }
        }
    }
    if (!found)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failed to locate original scene data area");
        return 0;
    }

    filler = new CImageFillNeighbor<float>();
    if (filler == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CImageFillNeighbor object");
        return 1;
    }

    if (filler->FillNeighbor(hull, outputImage, dunkPoint,
                             m_nonNullValue, fillDirections) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from FillNeighbor()");
        return 1;
    }

    delete filler;
    filler = nullptr;

    hull = outputImage;

    return 0;
}

template<class IO_TYPE, class WORKING_TYPE>
int CGcpHullRejection::CHullGenThread::GenOuterHull(const CMatrix<IO_TYPE> &inMat, CMatrix<IO_TYPE> &outMat, IO_TYPE clipVal, IO_TYPE insertVal) const
{
    CMatrix<WORKING_TYPE> out;
    CConvKernel<WORKING_TYPE> kernel;
    CMatrix<WORKING_TYPE> imgf;

    SSize size = inMat.getSize();
    imgf.resize(size);
    outMat.resize(size);

    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            imgf[r][c] = (WORKING_TYPE) inMat[r][c];
        }
    }

    imgf = imgf.binaryThreshold(clipVal, 0, insertVal);

    kernel.initMat(0);
    kernel.setAnchor(SSize(1, 1));
    kernel[0][0] = (0);
    kernel[1][0] = (-1);
    kernel[2][0] = (0);
    kernel[0][1] = (-1);
    kernel[1][1] = (4);
    kernel[2][1] = (-1);
    kernel[0][2] = (0);
    kernel[1][2] = (-1);
    kernel[2][2] = (0);

    if (CConvKernelToImage::Convolve<WORKING_TYPE>(imgf, kernel, out, false) != 0)
    {
        return 1;
    }

    out = out.binaryThreshold((WORKING_TYPE) clipVal, 0, (WORKING_TYPE) insertVal);

    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            outMat[r][c] = (IO_TYPE) out[r][c];
        }
    }

    return 0;
}

int CGcpHullRejection::CHullGenThread::GenerateConvexHullImage(const CMatrix<float> *image, CMatrix<float> &hull) const
{
    std::vector<SSize> pts;
    SSize size;
    CVector<unsigned long> hullSequence;
    SPair<long> p1, p2;
    unsigned long seqSize;

    std::unique_ptr<CConvexHull> hullGen(new CConvexHull());

    size = image->getSize();
    pts.clear();
    if (GenOuterHull<float, unsigned char>(*image, hull, m_nullValue, m_nonNullValue) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenOuterHull()");
        return 1;
    }

    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++)
        {
            if (hull[r][c] != m_nullValue)
            {
                pts.push_back(SSize(r, c));
            }
        }
    }

    if (pts.size() < 3)
    {
        // this means the tile is almost all null values thus just skip this rejection
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Trying to reject gcps on a blank tile");
        return 0;
    }

    if (hullGen->GenerateHullPoint(pts, hullSequence) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateHullPoint()");
        return 1;
    }

    hull.initMatMemset(m_nullValue);

    seqSize = hullSequence.size();
    if (seqSize == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Zero hull sequence values found");
        return 1;
    }
    seqSize--;
    for (unsigned long i = 0; i < seqSize; i++)
    {
        p1 = pts[hullSequence[i]];
        p2 = pts[hullSequence[i + 1]];
        CDrawingTools::line<float>(hull, p1, p2, m_nonNullValue);
    }
    return 0;
}

int CGcpHullRejection::CHullGenThread::TrimHullArea(CMatrix<float> &hull) const
{
    SPair<long> size = hull.getSize();
    long trim = (long) m_trimAmount;
    int per = 0;
    long tmp;
    long cc, rr;

    for (long r = 0; r < size.r; r++)
    {
        for (long c = 0; c < size.c; c++)
        {
            if (hull[r][c] != m_nullValue)
            {
                tmp = c + trim;
                tmp = (tmp > size.c) ? (tmp = size.c) : (tmp);
                for (cc = c; cc < tmp; cc++)
                    hull[r][cc] = m_nullValue;
                break;
            }
        }
        for (long c = size.c - 1; c >= 0; c--)
        {
            if (hull[r][c] != m_nullValue)
            {
                tmp = c - trim;
                tmp = (tmp < 0) ? (0) : (tmp);
                for (cc = tmp; cc <= c; cc++)
                    hull[r][cc] = m_nullValue;
                break;
            }
        }

        if (per != (int) ((r * 50) / size.r))
        {
            per = (int) ((r * 50) / size.r);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "TrimHullArea " + toString(per) + "%     \r");
        }
    }


    for (long c = 0; c < size.c; c++)
    {
        for (long r = 0; r < size.r; r++)
        {
            if (hull[r][c] != m_nullValue)
            {
                tmp = r + trim;
                tmp = (tmp > size.r) ? (tmp = size.r) : (tmp);
                for (rr = r; rr < tmp; rr++)
                    hull[rr][c] = m_nullValue;
                break;
            }
        }

        for (long r = size.r - 1; r >= 0; r--)
        {
            if (hull[r][c] != m_nullValue)
            {
                tmp = r - trim;
                tmp = (tmp < 0) ? (0) : (tmp);
                for (rr = r; rr >= tmp; rr--)
                    hull[rr][c] = m_nullValue;
                break;
            }
        }

        if (per != 50 + (int) ((c * 50) / size.c))
        {
            per = 50 + (int) ((c * 50) / size.c);
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "TrimHullArea " + toString(per) + "%     \r");
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "TrimHullArea 100%     ");
    return 0;
}

int CGcpHullRejection::CHullGenThread::GetExitCode() const
{
    return m_exitCode;
}

void CGcpHullRejection::CHullGenThread::run(void *context)
{
    if (CPaddImage::Padd(*m_image, m_paddSize, m_paddSize, m_nullValue) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CPaddImage::Padd");
        m_exitCode = 1;
        return;
    }

    if (GenerateConvexHullImage(m_image, *m_hull) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateConvexHullImage()");
        m_exitCode = 1;
        return;
    }

    if (FillPixelDataArea(m_image, *m_hull) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from FillPixelDataArea()");
        m_exitCode = 1;
        return;
    }

    if (TrimHullArea(*m_hull) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from TrimHullArea()");
        m_exitCode = 1;
        return;
    }

    if (CPaddImage::RemovePadd(*m_hull, m_paddSize, m_paddSize) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CPaddImage::RemovePadd");
        m_exitCode = 1;
        return;
    }
}

} //namespace ultra
