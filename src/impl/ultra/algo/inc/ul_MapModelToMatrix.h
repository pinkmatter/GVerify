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

#include <ul_Matrix.h>
#include <ul_Logger.h>
#include <ul_Pair.h>

#include "ul_DigitalImageCorrelator.h"

namespace ultra
{

template<class T>
class CMapModelToMatrix
{
protected:

    const CMatrix<T> *m_inputMatrix;
    const CMatrix<SPair<double> > *m_worldModel;
    bool m_hasInited;
    SSize m_size;

    int Init()
    {
        if (m_worldModel == nullptr || m_inputMatrix == nullptr)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input matrices may not be nullptr");
            return 1;
        }

        if ((void*) m_worldModel == (void*) m_inputMatrix)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "World and input matrices may not point to the same matrix");
            return 1;
        }

        if (m_inputMatrix->getSize() != m_worldModel->getSize())
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "World and input matrices are not of the same size");
            return 1;
        }

        if (m_inputMatrix->getSize() == SSize(0, 0))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Input and World matrices may not be of size 0x0");
        }

        m_size = m_worldModel->getSize();
        m_hasInited = true;

        return 0;
    }

public:

    CMapModelToMatrix(const CMatrix<SPair<double> > &worldModel, const CMatrix<T> &inputMatrix)
    {
        m_hasInited = false;
        m_worldModel = &worldModel;
        m_inputMatrix = &inputMatrix;
    }

    virtual ~CMapModelToMatrix()
    {
        m_hasInited = false;
        m_worldModel = nullptr;
        m_inputMatrix = nullptr;
    }

    int GetSceneTile(const SPair<double> &midCoordinate, const SSize &windowSize, CMatrix<T> &outputTile, const SPair<double> &groundSamplingDistance, bool &validTileReturned, ECorrelationType &corrMethod)
    {
        validTileReturned = true;
        if (!m_hasInited)
        {
            if (Init() != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from Init()");
                validTileReturned = false;
                return 1;
            }
        }

        if (windowSize == SSize(0, 0))
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Window size must be larger than 0x0");
            return 1;
        }

        if (corrMethod != ECorrelationType::PHASE)
        {
            if (windowSize.col % 2 != 1 ||
                windowSize.row % 2 != 1)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Window size must be odd");
                return 1;
            }
        }
        else
        {
            if (windowSize.col % 2 != 0 ||
                windowSize.row % 2 != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Window size must be even");
                return 1;
            }
        }

        if (windowSize.col > m_size.col ||
            windowSize.row > m_size.row)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "The requested window size may not be larger than the actual input image size");
            validTileReturned = false;
            return 1;
        }

        if (outputTile.getSize() != windowSize)
        {
            try
            {
                outputTile.resize(windowSize);
            }
            catch (CException e)
            {
                validTileReturned = false;
                throw e;
            }
        }

        outputTile.initMat(T(0));

        /*
         * Assuming that the world matrix values will always be square
         */

        //check if the midCoordinate lays between the world coordinates
        // because we assume that the world matrix is square we do this:
        SPair<double> ul = (*m_worldModel)[0][0];
        SPair<double> lr = (*m_worldModel)[m_size.row - 1][m_size.col - 1];

        if (groundSamplingDistance.r < 0 && groundSamplingDistance.c > 0)
        {
            if (midCoordinate.r > ul.r ||
                midCoordinate.r < lr.r ||
                midCoordinate.c < ul.c ||
                midCoordinate.c > lr.c)
            {
                // the requested tile is outside of the world bounds
                validTileReturned = false;
                return 0;
            }
        }
        else if (groundSamplingDistance.r < 0 && groundSamplingDistance.c < 0)
        {
            if (midCoordinate.r > ul.r ||
                midCoordinate.r < lr.r ||
                midCoordinate.c > ul.c ||
                midCoordinate.c < lr.c)
            {
                // the requested tile is outside of the world bounds
                validTileReturned = false;
                return 0;
            }
        }
        else if (groundSamplingDistance.r > 0 && groundSamplingDistance.c > 0)
        {
            if (midCoordinate.r < ul.r ||
                midCoordinate.r > lr.r ||
                midCoordinate.c < ul.c ||
                midCoordinate.c > lr.c)
            {
                // the requested tile is outside of the world bounds
                validTileReturned = false;
                return 0;
            }
        }
        else if (groundSamplingDistance.r > 0 && groundSamplingDistance.c < 0)
        {
            if (midCoordinate.r < ul.r ||
                midCoordinate.r > lr.r ||
                midCoordinate.c > ul.c ||
                midCoordinate.c < lr.c)
            {
                // the requested tile is outside of the world bounds
                validTileReturned = false;
                return 0;
            }
        }
        else
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Ground sampling distance cannot be 0x0");
            return 1;
        }

        // the midCoordinate MAY fall in between a world matrix value
        // thus we need to locate the CLOSEST world matrix value index

        unsigned long row = 0;
        unsigned long col = 0;
        double testVal;
        double absVal;

        testVal = getAbs((*m_worldModel)[0][0].r - midCoordinate.r);
        for (unsigned long r = 1; r < m_size.row; r++)
        {
            absVal = getAbs((*m_worldModel)[r][0].r - midCoordinate.r);
            if (absVal < testVal)
            {
                testVal = absVal;
                row = r;
            }
        }

        testVal = getAbs((*m_worldModel)[0][0].c - midCoordinate.c);
        for (unsigned long c = 1; c < m_size.col; c++)
        {
            absVal = getAbs((*m_worldModel)[0][c].c - midCoordinate.c);
            if (absVal < testVal)
            {
                testVal = absVal;
                col = c;
            }
        }


        // row and col now holds the best match for mapping the world matrix to the input matrix
        // now extra a sub window of size windowSize around this point from the input matrix

        long sr = (long) row - (long) windowSize.row / 2;
        long er = (long) row + (long) windowSize.row / 2;
        long sc = (long) col - (long) windowSize.col / 2;
        long ec = (long) col + (long) windowSize.col / 2;

        if (corrMethod == ECorrelationType::PHASE)
        {
            ec--;
            er--;
        }
        unsigned long rr = 0;
        unsigned long cc = 0;
        for (long r = sr; r <= er; r++)
        {
            cc = 0;
            for (long c = sc; c <= ec; c++)
            {
                if (r < 0 || c < 0 ||
                    r >= m_size.row || c >= m_size.col)
                {
                    //out of bounds
                    validTileReturned = false;
                    return 0;
                }
                outputTile[rr][cc] = (*m_inputMatrix)[r][c];
                cc++;
            }
            rr++;
        }

        return 0;
    }
};

} //namespace ultra
