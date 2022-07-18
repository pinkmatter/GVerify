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

#include "GenerateGcps.h"

#include <ul_TiledGaussianPyramidTiePointGenerator.h>
#include <ul_Logger.h>
#include <ul_ImageLoader.h>
#include <ul_Proj4Projection.h>
#include <ul_SmartVector.h>

static int LoadLocations(const std::string &fixedChipLocationFile, ultra::CTiledGaussianPyramidTiePointGenerator::SContext &context)
{
    context.fixedLocationChipProj4Str = ultra::CProj4Projection::BASE_LAT_LONG_PROJ4_STR;
    std::vector<std::string> lines;
    if (ultra::ReadAllLinesFromFile(fixedChipLocationFile, lines, false) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from ReadAllLinesFromFile('" + fixedChipLocationFile + "')");
        return 1;
    }
    ultra::CSmartVector<ultra::SPair<double> > q;
    for (unsigned long t = 0; t < lines.size(); t++)
    {
        ultra::SPair<double> latLong;
        std::string line = lines[t];
        if (sscanf(line.c_str(), "%lf,%lf", &latLong.lat, &latLong.lon) == 2)
        {
            q.pushBack(latLong);
        }
    }
    context.fixedLocationChips = q.toVector();
    return 0;
}

int GenerateGcps(SArgs &args, ultra::CVector<ultra::SChipCorrelationResult> &result, bool &overlapExists)
{
    ultra::CTiledGaussianPyramidTiePointGenerator::SContext context;

    ultra::SImageMetadata meta;
    if (ultra::CImageLoader::getInstance()->LoadImageMetadata(args.inputImage.pathToImage, meta) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }
    context.inputScene = args.inputImage.pathToImage;
    context.referenceScene = args.referenceImage.pathToImage;
    context.tileSize = meta.getDimensions() / 2;
    if (context.tileSize.row < 3000)
        context.tileSize.row = 3000;
    if (context.tileSize.col < 3000)
        context.tileSize.col = 3000;
    if (context.tileSize.row > 6000)
        context.tileSize.row = 6000;
    if (context.tileSize.col > 6000)
        context.tileSize.col = 6000;
    context.workingFolder = args.workingDirectory;
    context.amountOfPyramids = args.amountOfPyramids;
    context.resampleType = args.resampleType;
    context.threadCount = args.threadCount;
    context.bandNumberToUse = 1;
    context.nullValue = args.nullValue;
    context.mayContainNullValues = args.mayContainNullValues;
    context.chipSize = args.chipSize;
    context.chipGenerationGridSize = args.chipGenerationGridSize;
    if (args.usePhaseCorrelation)
        context.typeOfChipCorrelationTechnique = ultra::ECorrelationType::PHASE;
    else
        context.typeOfChipCorrelationTechnique = ultra::ECorrelationType::CCOEFF_NORM;
    context.chipGeneratorMethods.clear();
    context.chipGeneratorMethods.pushBack(args.chipGenType);
    context.correlationThreshold = args.correlationThreshold;

    if (args.chipGenType == ultra::EChips::CHIP_GEN_FIXED_LOCATION)
    {
        if (LoadLocations(args.fixedChipLocationFile, context) != 0)
        {
            ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from LoadLocations()");
            return 1;
        }
    }

    std::unique_ptr<ultra::CTiledGaussianPyramidTiePointGenerator> generator(new ultra::CTiledGaussianPyramidTiePointGenerator(&context));

    if (generator->Calculate(result, &overlapExists) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CTiledGaussianPyramidTiePointGenerator::Calculate()");
        return 1;
    }
    return 0;
}
