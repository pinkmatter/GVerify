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

#include "ul_Transform.h"

namespace ultra
{

class CHarrisCornerTransform : public CTransform<float, unsigned char>
{
public:

    class CHarrisCornerContext : public CTransformContext
    {
    public:
        float minHarrisValue;
        CHarrisCornerContext();
        virtual ~CHarrisCornerContext();
    };
private:

    CTransform<float, float> *m_sobelFilter;
    CVector<CMatrix<float> > m_sobelImages;
    CMatrix<CMatrix<float> > m_harrisMat;
    CHarrisCornerContext *m_context;
    bool m_internalContext;

    int DoSobelTransForm();
    int ConstructHarrisMatrix();
    int DetectHarrisCorners();

protected:
    virtual int Init(CTransformContext *context) override;
    virtual int InnerTransform() override;

public:
    CHarrisCornerTransform();
    virtual ~CHarrisCornerTransform();

};

} // namespace ultra
