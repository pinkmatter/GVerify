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

#include <iostream>

#include <ul_Vector.h>
#include <ul_ChipsGen.h>
#include <ul_Resampler.h>

struct SArgs
{
public:

    struct SImageItem
    {
        std::string pathToImage;
        std::string proj4Str;

        SImageItem();
        ~SImageItem();

        friend std::ostream &operator<<(std::ostream &stream, const SArgs::SImageItem & o);
    };

    SImageItem referenceImage;
    SImageItem inputImage;

    std::string originalInputPath;
    std::string originalReferencePath;

    ultra::EResamplerEnum::EResampleType resampleType;
    std::string workingDirectory;

    unsigned int threadCount;
    unsigned long chipGenerationGridSize;
    double correlationThreshold;
    bool usePhaseCorrelation;
    std::string logPath;
    std::string outputPath;
    unsigned long chipSize;
    unsigned long amountOfPyramids;
    ultra::EChips::EChipType chipGenType;
    std::string fixedChipLocationFile;
    float nullValue;
    bool mayContainNullValues;
    bool saveTiff;
    bool saveUnion;

    friend std::ostream &operator<<(std::ostream &stream, const SArgs & o);

    SArgs();
    ~SArgs();
};

int ParseInput(int argc, char **argv, SArgs &args);
void showHelp(int argc, char **argv);
