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

#include <limits>

#include "ul_Vector.h"
#include "ul_Matrix.h"
#include "ul_Logger.h"

namespace ultra
{

class CLeastSquares
{
private:
    CLeastSquares();

    template<class T>
    static void constructMatrices(T eps, const CMatrix<T> *A, const CMatrix<T> *B, CMatrix<T> &C, CVector<T> &values)
    {
        T tReg;
        CMatrix<T> At;
        CMatrix<T> AtA;
        CMatrix<T> AtB;
        //calc needed matrix values
        // At*A*C=At*B
        //we need to determine C so for now this can be clear
        At = A->transpose();
        AtA = At * (*A);
        AtB = At * (*B);

        for (unsigned long r = 0; r < AtA.getSize().row; r++)
        {
            for (unsigned long c = 0; c < AtA.getSize().col; c++)
            {
                // using abs on both sized to make sure this will work if T = SComplex<N>
                if (getAbs(AtA[r][c]) <= getAbs(eps))
                    AtA[r][c] = 0;
            }
            // using abs on both sized to make sure this will work if T = SComplex<N>
            if (getAbs(AtB[r][0]) <= getAbs(eps))
                AtB[r][0] = 0;
        }

        // this will hold the values returned it is important
        // that the values be set to zero
        values.resize(AtB.getSize().row);
        values.initVec(0);

        //all this does is populate m_C so that it holds both
        // AtA|AtB
        C = CMatrix<T > (AtB.getSize().row, AtA.getSize().col + 1);
        C.initMat(0);
        for (unsigned long r = 0; r < C.getSize().row; r++)
        {
            for (unsigned long c = 0; c < C.getSize().col - 1; c++)
            {
                C[r][c] = AtA[r][c];
            }
            C[r] [C.getSize().col - 1] = AtB[r][0];
        }
    }

    template<class T>
    static void solveTriangularMatrix(T eps, CMatrix<T> &C, CVector<T> &values)
    {
        //this calculates the results using simple back propagation
        for (long it = values.size() - 1; it >= 0; it--)
        {
            // using abs on both sized to make sure this will work if T = SComplex<N>
            if (getAbs(C[it][it]) < getAbs(eps))
            {
                values[it] = 0;
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_DEBUG, "Going do divide by zero at row'" + toString(it) + "', assume ans will be zero (as in not used)");
                continue;
            }

            T val = T(0);
            for (long c = values.size() - 1; c > it; c--)
            {
                val = val + C[it][c] * values[c];
            }

            values[it] = (C[it][C.getSize().col - 1] - val) / C[it][it];
        }
    }

public:

    virtual ~CLeastSquares();

    /**
     * This will throw CException if the upper
     * triangular matrix is singular
     * @param A
     * @param B
     * @return 
     */
    template <class T>
    static CVector<T> calc(const CMatrix<T> *A, const CMatrix<T> *B)
    {
        CMatrix<T> C;
        CVector<T> values;
        unsigned int rowsSwapped = 0;
        static const T eps = (T) std::numeric_limits<double>::epsilon();
        constructMatrices(eps, A, B, C, values);
        C = C.getUpperTriangularMatrix(&rowsSwapped);
        SSize cSize = C.getSize();
        for (unsigned long r = 0; r < cSize.row; r++)
        {
            double d = C[r][r];
            if (getAbs(d) < getAbs(eps))
                throw CException(__FILE__, __LINE__, "Matrix is singular cannot compute least squares");
            for (unsigned long c = r; c < cSize.col; c++)
                C[r][c] = C[r][c] / d;
        }
        solveTriangularMatrix(eps, C, values);
        return values;
    }

};

} //namespace ultra
