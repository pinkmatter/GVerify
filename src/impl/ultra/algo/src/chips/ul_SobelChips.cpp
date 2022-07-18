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

#include "ul_SobelChips.h"

#include "ul_Logger.h"

namespace ultra
{

CSobelChips::CSobelChips() :
CChipsGen<float>("Sobel_ChipsGen")
{
    m_transformer = nullptr;
    m_smoother = nullptr;
    m_smootherCtx = nullptr;
}

CSobelChips::~CSobelChips()
{
    if (m_transformer != nullptr)
    {
        delete m_transformer;
        m_transformer = nullptr;
    }

    if (m_smoother != nullptr)
    {
        delete m_smoother;
        m_smoother = nullptr;
    }

    if (m_smootherCtx != nullptr)
    {
        delete m_smootherCtx;
        m_smootherCtx = nullptr;
    }
}

int CSobelChips::Init(CChipsGen<float>::CChipsGenContext *context)
{
    CSobelChips::CSobelChipsContext *thisContext = dynamic_cast<CSobelChips::CSobelChipsContext *> (context);
    if (thisContext == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to cast context, please pass in CSobelChips::CSobelChipsContext struct");
        return 1;
    }

    m_chipWidth = thisContext->chipWidth;

    if (m_chipWidth % 2 != 0 && m_chipWidth <= 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Chips width must be an positive odd number");
        return 1;
    }
    m_chipOffset = (m_chipWidth - 1) / 2;

    if (m_transformer == nullptr)
    {
        m_transformer = new CSobelFilterTransform<float>();
    }

    if (m_smoother == nullptr)
    {
        m_smoother = new CGaussianFilterTransform<float>();
    }

    if (m_smootherCtx == nullptr)
    {
        m_smootherCtx = new CGaussianFilterTransform<float>::CContext();
    }

    return 0;
}

int CSobelChips::ApplySobel(const CMatrix<float> &im, CMatrix<float> &sobel_grad, CMatrix<float> &sobel_phase)
{
    CVector<CMatrix<float> > imVec;
    CVector<CMatrix<float> > sobelOut;

    imVec.resize(1);
    imVec[0] = im;
    if (imVec[0].getSize() != im.getSize())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed make a copy of the input image");
        return 1;
    }

    if (m_transformer->LoadData(&imVec, &sobelOut) != 0)
    {
        delete m_transformer;
        m_transformer = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadData()");
        return 1;
    }
    if (m_transformer->Transform() != 0)
    {
        delete m_transformer;
        m_transformer = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Transform()");
        return 1;
    }

    if (sobelOut.size() < 4)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Too few images is returned from the Sobel transform");
        return 1;
    }

    sobel_grad = sobelOut[2];
    sobel_phase = sobelOut[3];

    return 0;
}

int CSobelChips::getThreshold(const CMatrix<float>& im, float& th)
{
    SSize min;
    SSize max;
    im.minMaxLoc(min, max);

    float minVal = im[min.row][min.col];
    float maxVal = im[max.row][max.col];

    unsigned long size = 1000;
    CVector<unsigned long> hist = im.getHistogram(size, false);
    if (hist.size() != size)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid histogram calculated");
        return 1;
    }

    unsigned long int sum = im.getSize().col * im.getSize().row;
    unsigned long int cutoff = (unsigned long int) (0.95 * sum);
    sum = 0;
    int i = 0;
    for (; i < size; i++)
    {
        sum += hist[i];
        if (sum >= cutoff)
        {
            break;
        }
    }

    float step = (maxVal - minVal) / ((float) size);
    th = step * i;

    if (!(th > 0))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Invalid threshold calculated.");
        return 1;
    }

    return 0;
}

int CSobelChips::PostSmoothing(const CMatrix<float> &threshed, CMatrix<float> &smoothed)
{
    CVector<CMatrix<float> > gaussOut;
    CVector<CMatrix<float> > gaussIn;
    gaussIn.resize(1);
    gaussIn[0] = threshed;

    if (m_smoother->LoadData(&gaussIn, &gaussOut) != 0)
    {
        delete m_smoother;
        m_smoother = nullptr;
        delete m_smootherCtx;
        m_smootherCtx = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadData()");
        return 1;
    }
    if (m_smoother->Transform(m_smootherCtx) != 0)
    {
        delete m_smoother;
        m_smoother = nullptr;
        delete m_smootherCtx;
        m_smootherCtx = nullptr;
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Transform()");
        return 1;
    }

    smoothed = gaussOut[0];
    return 0;
}

int CSobelChips::isHighestInCenter(const CMatrix<float> &sample, bool &res, unsigned long i, unsigned long j, const SSize &tileSize)
{
    res = false;
    int centerCol = j + (tileSize.col / 2);
    int centerRow = i + (tileSize.row / 2);

    SSize min;
    SSize max;
    const float thres = 200;

    if (sample.minMaxLocSubMatrix(min, max, SSize(i, j), tileSize) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from MinMacLocSubImage()");
        return 1;
    }

    if (max.col == centerCol && max.row == centerRow)
    {
        // Also check if highest value is high enough...
        float val = sample[centerRow][centerCol];
        if (val > thres)
        {
            res = true;
            return 0;
        }
    }

    return 0;
}

int CSobelChips::isPhaseErratic(const CMatrix<float> &sample, bool &res, unsigned long i, unsigned long j, const SSize &tileSize)
{
    //check the phase and make sure that NOT all points in the same directions
    for (unsigned long r = 0; r < tileSize.row; r++)
    {
        const float *sampleP = &sample[r + i][0];
        float *gaussian = &m_gaussianKernel[r][0];
        float *phase = &m_tempPhase[r][0];
        for (unsigned long c = 0; c < tileSize.col; c++)
        {
            phase[c] = sampleP[c + j] * gaussian[c];
        }
    }

    double stdDev = m_tempPhase.stdDev();
    if (stdDev < 0.01)
        res = false;

    return 0;
}

int CSobelChips::isPointUsable(const CMatrix<float> &magSubset, CMatrix<float> &phaseSubset, bool &res, unsigned long i, unsigned long j, const SSize &tileSize)
{
    if (isHighestInCenter(magSubset, res, i, j, tileSize) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isHighestInCenter()");
        return 1;
    }

    if (res)
    {
        if (isPhaseErratic(phaseSubset, res, i, j, tileSize) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isPhaseErratic()");
            return 1;
        }
    }
    return 0;
}

int CSobelChips::ProcessChips(const CMatrix<float> &im, const CMatrix<float> &smoothed, CMatrix<float> &sobel_phase, CVector<CChip<float> >& output)
{
    // Process for chips    
    unsigned long sizeY = smoothed.getSize().row;
    unsigned long sizeX = smoothed.getSize().col;
    std::vector<CChip<float> > chipList;
    CChip<float> chip;
    chip.chipType = EChips::chipGenTypeToStr(EChips::CHIP_GEN_SOBEL);
    unsigned long currentId = 0;
    bool test;
    long per = -1;
    long tempPer = 0;

    long difY = sizeY - m_chipWidth;
    long difX = sizeX - m_chipWidth;
    SSize tileSize = SSize(m_chipWidth, m_chipWidth);
    m_tempPhase.resize(tileSize);
    for (unsigned long i = 0; i < difY; i++)
    {
        for (unsigned long j = 0; j < difX; j++)
        {
            if (isPointUsable(smoothed, sobel_phase, test, i, j, tileSize) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from isPointUsable()");
                return 1;
            }

            if (test)
            {
                chip.chipData = im.getSubMatrix(SSize(i, j), tileSize);
                chip.midCoordinate = SPair<double>(i + m_chipOffset, j + m_chipOffset);
                /**
                 * 0 = UL
                 * 1 = LL
                 * 2 = LR
                 * 3 = UR
                 */
                chip.boundingCoordinate[0] = SPair<double>(i, j);
                chip.boundingCoordinate[1] = SPair<double>(i, j + m_chipWidth);
                chip.boundingCoordinate[2] = SPair<double>(i + m_chipWidth, j + m_chipWidth);
                chip.boundingCoordinate[3] = SPair<double>(i + m_chipWidth, j);
                chip.chipId = currentId++;
                chipList.push_back(chip);
            }


        }
        tempPer = (i * 100) / difY;
        if (per != tempPer)
        {
            per = tempPer;
            CLogger::getInstance()->LogNoNewLine(__FILE__, __LINE__, CLogger::LOG_INFO, "Sobel-chips generation: " + toString(per) + "%                   \r");
        }
    }
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Sobel-chips generation: 100%   ");

    output = chipList;

    return 0;
}

int CSobelChips::SplitImagesForThreads()
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Unimplemented call");
    return 1;
}

int CSobelChips::innerGenerate(CChipsGen<float>::CChipsGenContext *context)
{
    if (Init(context) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
        return 1;
    }

    if (CCreateConvKernel::GetKernel<float>(EKernelTypes::GAUSSIAN, SSize(m_chipWidth, m_chipWidth), m_gaussianKernel, 1.6) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getKernel()");
        return 1;
    }

    CMatrix<float> sobel_grad;
    CMatrix<float> sobel_phase;
    if (ApplySobel(*(this->m_inputImage), sobel_grad, sobel_phase) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ApplySobel()");
        return 1;
    }

    // Determine threshold from sobel output
    float th;
    if (getThreshold(sobel_grad, th) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getThreshold()");
        return 1;
    }

    //Apply threshold
    CMatrix<float> threshed = sobel_grad.binaryThreshold(th, 0, 255);
    sobel_grad.clear();

    //Post-smoothing
    CMatrix<float> smoothed;
    if (PostSmoothing(threshed, smoothed) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PostSmoothing()");
        return 1;
    }

    if (ProcessChips(*(this->m_inputImage), smoothed.fit(), sobel_phase, *(this->m_outputChips)) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ProcessChips()");
        return 1;
    }

    return 0;
}

} //namespace ultra
