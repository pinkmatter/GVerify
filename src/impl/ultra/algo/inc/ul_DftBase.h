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
#include <ul_Complex.h>
#include <ul_UltraThread.h>

namespace ultra
{

namespace __ultra_internal_fft
{

class CFftInitLock
{
public:
    static std::shared_ptr<CThreadLock> getLock();
};

} // namespace __ultra_internal_fft

class ADftBase2d
{
public:
    using MY_WORKING_TYPE = double;
protected:
    const SSize m_size;
    const std::string m_fftLibName;
    ADftBase2d(const SSize &size, const std::string &fftLibName);

    virtual void forwardImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const = 0;
    virtual void inverseImpl(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const = 0;
    
    void forwardWt(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const;
    void inverseWt(const CMatrix<SComplex<MY_WORKING_TYPE> > &input, CMatrix<SComplex<MY_WORKING_TYPE> > &output) const;
public:
    virtual ~ADftBase2d();

    template<class T>
    void forward(const CMatrix<SComplex<T> > &input, CMatrix<SComplex<T> > &output) const
    {
        auto inputN = input.template map<SComplex<MY_WORKING_TYPE> >([](const SComplex<T> &i)->SComplex<MY_WORKING_TYPE>
        {
            return i.template convertType<MY_WORKING_TYPE>();
        });
        CMatrix<SComplex<MY_WORKING_TYPE> > outputN;
        forwardWt(inputN, outputN);
        output = outputN.template map<SComplex<T> >([](const SComplex<MY_WORKING_TYPE> &o)->SComplex<T>
        {
            return o.template convertType<T>();
        });
    }

    template<class T>
    void inverse(const CMatrix<SComplex<T> > &input, CMatrix<SComplex<T> > &output) const
    {
        auto inputN = input.template map<SComplex<MY_WORKING_TYPE> >([](const SComplex<T> &i)->SComplex<MY_WORKING_TYPE>
        {
            return i.template convertType<MY_WORKING_TYPE>();
        });
        CMatrix<SComplex<MY_WORKING_TYPE> > outputN;
        inverseWt(inputN, outputN);
        output = outputN.template map<SComplex<T> >([](const SComplex<MY_WORKING_TYPE> &o)->SComplex<T>
        {
            return o.template convertType<T>();
        });
    }

    template<class T>
    CMatrix<SComplex<T> > forward(const CMatrix<SComplex<T> > &input) const
    {
        CMatrix<SComplex<T> > output;
        forward<T>(input, output);
        return output;
    }

    template<class T>
    CMatrix<SComplex<T> > inverse(const CMatrix<SComplex<T> > &input) const
    {
        CMatrix<SComplex<T> > output;
        inverse<T>(input, output);
        return output;
    }

    std::string getFftLibraryName() const;

    template<class T>
    static void forwardFftMatrixSwapQuadrants(CMatrix<T> &inputOutput)
    {
        SSize max = inputOutput.getSize();
        SSize mid = SSize(max.row / 2, max.col / 2);
        SSize add = SSize(max.row % 2, max.col % 2);


        CMatrix<T> q1, q2, q3, q4;

        /*
         *  q1 | q2
         *  ---+---
         *  q3 | q4
         */

        SSize ul;
        SSize size;

        ul.row = 0;
        ul.col = 0;
        size.row = mid.row + add.row;
        size.col = mid.col + add.col;
        q1 = inputOutput.getSubMatrix(ul, size);

        ul.row = 0;
        ul.col = mid.col + add.col;
        size.row = mid.row + add.row;
        size.col = mid.col;
        q2 = inputOutput.getSubMatrix(ul, size);

        ul.row = mid.row + add.row;
        ul.col = 0;
        size.row = mid.row;
        size.col = mid.col + add.col;
        q3 = inputOutput.getSubMatrix(ul, size);


        ul.row = mid.row + add.row;
        ul.col = mid.col + add.col;
        size.row = mid.row;
        size.col = mid.col;
        q4 = inputOutput.getSubMatrix(ul, size);

        ///////////////copy in data
        ul.row = 0;
        ul.col = 0;
        int ret = inputOutput.CopyFrom(ul, q4);

        ul.row = mid.row;
        ul.col = mid.col;
        ret |= inputOutput.CopyFrom(ul, q1);

        ul.row = mid.row;
        ul.col = 0;
        ret |= inputOutput.CopyFrom(ul, q2);

        ul.row = 0;
        ul.col = mid.col;
        ret |= inputOutput.CopyFrom(ul, q3);

        if (ret != 0)
            throw CException(__FILE__, __LINE__, "Failed to swap FFT matrix quadrants");
    }

    template<class T>
    static void inverseFftMatrixSwapQuadrants(CMatrix<T> &inputOutput)
    {
        SSize max = inputOutput.getSize();
        SSize mid = SSize(max.row / 2, max.col / 2);
        SSize add = SSize(max.row % 2, max.col % 2);


        CMatrix<T> q1, q2, q3, q4;

        /*
         *  q1 | q2
         *  ---+---
         *  q3 | q4
         */

        SSize ul;
        SSize size;

        ul.row = 0;
        ul.col = 0;
        size.row = mid.row;
        size.col = mid.col;
        q1 = inputOutput.getSubMatrix(ul, size);

        ul.row = 0;
        ul.col = mid.col;
        size.row = mid.row;
        size.col = mid.col + add.col;
        q2 = inputOutput.getSubMatrix(ul, size);

        ul.row = mid.row;
        ul.col = 0;
        size.row = mid.row + add.row;
        size.col = mid.col;
        q3 = inputOutput.getSubMatrix(ul, size);


        ul.row = mid.row;
        ul.col = mid.col;
        size.row = mid.row + add.row;
        size.col = mid.col + add.col;
        q4 = inputOutput.getSubMatrix(ul, size);

        ///////////////copy in data
        ul.row = 0;
        ul.col = 0;
        int ret = inputOutput.CopyFrom(ul, q4);

        ul.row = mid.row + add.row;
        ul.col = mid.col + add.col;
        ret |= inputOutput.CopyFrom(ul, q1);

        ul.row = mid.row + add.row;
        ul.col = 0;
        ret |= inputOutput.CopyFrom(ul, q2);

        ul.row = 0;
        ul.col = mid.col + add.col;
        ret |= inputOutput.CopyFrom(ul, q3);

        if (ret != 0)
            throw CException(__FILE__, __LINE__, "Failed to swap FFT matrix quadrants");
    }
};

} // namespace ultra
