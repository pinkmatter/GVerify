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

#include "ul_TiledGaussianPyramidTiePointGenerator.h"

#include "ul_ImageLoader.h"
#include "ul_ImageSaver.h"
#include "ul_ImageOverlap.h"
#include <ul_PaddImage.h>

namespace ultra
{

const unsigned long CTiledGaussianPyramidTiePointGenerator::REF_INDEX = 0;
const unsigned long CTiledGaussianPyramidTiePointGenerator::INPUT_INDEX = 1;

CTiledGaussianPyramidTiePointGenerator::CTiledGaussianPyramidTiePointGenerator(const SContext *context) :
ITiePointGenerator()
{
    m_context = context;
}

CTiledGaussianPyramidTiePointGenerator::~CTiledGaussianPyramidTiePointGenerator()
{
    m_context = nullptr;
}

int CTiledGaussianPyramidTiePointGenerator::ContextOk()
{
    if (m_context == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Null context passed in");
        return 1;
    }

    if (m_context->tileSize.containsZero() ||
        m_context->tileSize.row <= m_context->chipSize ||
        m_context->tileSize.col <= m_context->chipSize)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Tile size cannot be of size 0x0 and must be larger than the chip size");
        return 1;
    }

    if (m_context->amountOfPyramids == 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Requires at least one pyramid level");
        return 1;
    }

    if (!CFile(m_context->workingFolder).isDirectoryWritable())
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Working folder '" + m_context->workingFolder + "' does not point to a writable folder");
        return 1;
    }

    if (m_context->chipSize % 2 != 1 || m_context->chipSize < 15)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Chip size must be odd and at least 15");
        return 1;
    }

    if (m_context->threadCount < 1)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Thread count be larger than zero");
        return 1;
    }

    m_paddedInputScene = m_context->inputScene;
    m_paddedRefScene = m_context->referenceScene;

    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::MinMaxBoxImages(bool &overlapExists)
{
    CFile minMaxBox = CFile(m_context->workingFolder, "MinMaxBox");
    if (!minMaxBox.exists())
    {
        if (minMaxBox.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + minMaxBox.getPath() + "'");
            return 1;
        }
    }
    CFile maxBoxIn = CFile(minMaxBox, "Input");
    CFile maxBoxRef = CFile(minMaxBox, "Reference");
    if (!maxBoxIn.exists())
    {
        if (maxBoxIn.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + maxBoxIn.getPath() + "'");
            return 1;
        }
    }
    if (!maxBoxRef.exists())
    {
        if (maxBoxRef.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + maxBoxRef.getPath() + "'");
            return 1;
        }
    }

    CVector<SKeyValue<std::string, std::string> > inputOutputImagePaths;
    std::string inputOut = CFile(maxBoxIn, CFile(m_paddedInputScene).getFileName()).getPath();
    std::string referenceOut = CFile(maxBoxRef, CFile(m_paddedRefScene).getFileName()).getPath();
    inputOutputImagePaths.pushBack(SKeyValue<std::string, std::string>(m_paddedInputScene, inputOut));
    inputOutputImagePaths.pushBack(SKeyValue<std::string, std::string>(m_paddedRefScene, referenceOut));
    CImageOverlap::EOverlapType type = CImageOverlap::MIN_BOX;
    float paddVal = m_context->nullValue;
    if (CImageOverlap::OverlapImages(inputOutputImagePaths, type, overlapExists, paddVal) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageOverlap::OverlapImages()");
        return 1;
    }

    m_paddedInputScene = inputOut;
    m_paddedRefScene = referenceOut;
    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::PaddImagesToSameSize()
{
    int div = 1;
    for (int t = 1; t < m_context->amountOfPyramids; t++)
    {
        div *= 2;
    }

    SImageMetadata metaIn;
    SImageMetadata metaRef;
    if (CImageLoader::getInstance()->LoadImageMetadata(m_paddedInputScene, metaIn) != 0 ||
        CImageLoader::getInstance()->LoadImageMetadata(m_paddedRefScene, metaRef) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }

    SSize outSize = metaIn.getDimensions();
    outSize.col = max_of<unsigned long>(2, outSize.col, metaRef.getDimensions().col);
    outSize.row = max_of<unsigned long>(2, outSize.row, metaRef.getDimensions().row);
    SSize paddingAddedLr;
    paddingAddedLr.row = (div - outSize.row % div) % div;
    paddingAddedLr.col = (div - outSize.col % div) % div;
    outSize += paddingAddedLr;
    SSize inPadd = outSize - metaIn.getDimensions();
    SSize refPadd = outSize - metaRef.getDimensions();

    CFile paddedInput = CFile(m_context->workingFolder, "Padded");
    if (!paddedInput.exists())
    {
        if (paddedInput.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + paddedInput.getPath() + "'");
            return 1;
        }
    }

    CFile newInPath = CFile(paddedInput, CFile(m_paddedInputScene).getFileNameWithoutExtension() + "_in.TIF");
    CFile newRefPath = CFile(paddedInput, CFile(m_paddedRefScene).getFileNameWithoutExtension() + "_ref.TIF");

    if (CPaddImage::Padd(m_paddedInputScene, 0, inPadd, m_context->nullValue, newInPath.getPath()) != 0 ||
        CPaddImage::Padd(m_paddedRefScene, 0, refPadd, m_context->nullValue, newRefPath.getPath()) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CPaddImage::Padd()");
        return 1;
    }

    m_paddedInputScene = newInPath.getPath();
    m_paddedRefScene = newRefPath.getPath();
    m_paddedImageSize = outSize;

    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::GenerateTilesIndices()
{
    unsigned long fullRowCounter = m_paddedImageSize.row / m_context->tileSize.row;
    unsigned long fullColCounter = m_paddedImageSize.col / m_context->tileSize.col;
    unsigned long partialRowCounter = m_paddedImageSize.row % m_context->tileSize.row;
    unsigned long partialColCounter = m_paddedImageSize.col % m_context->tileSize.col;

    unsigned long totalRowBlocks = fullRowCounter;
    unsigned long totalColBlocks = fullColCounter;

    if (partialRowCounter != 0)
    {
        if (fullRowCounter == 0)
            totalRowBlocks++;
    }

    if (partialColCounter != 0)
    {
        if (fullColCounter == 0)
            totalColBlocks++;
    }

    m_subImageUl.resize(SSize(totalRowBlocks, totalColBlocks));
    m_subImageSize.resize(SSize(totalRowBlocks, totalColBlocks));
    long totalRowOffset = 0;
    long totalColOffset = 0;
    long overlapSize = m_context->chipSize * 2;
    for (unsigned long r = 0; r < totalRowBlocks; r++)
    {
        totalColOffset = 0;
        long rowSize = m_context->tileSize.row;
        if (r == totalRowBlocks - 1)
        {
            rowSize += partialRowCounter;
        }
        for (unsigned long c = 0; c < totalColBlocks; c++)
        {
            long colSize = m_context->tileSize.col;
            if (c == totalColBlocks - 1)
            {
                colSize += partialColCounter;
            }

            long overlappingTotalRowOffset = totalRowOffset;
            long overlappingTotalColOffset = totalColOffset;
            long overlappingRowSize = rowSize;
            long overlappingColSize = colSize;

            overlappingTotalRowOffset -= overlapSize;
            overlappingTotalColOffset -= overlapSize;
            overlappingRowSize += 2 * overlapSize;
            overlappingColSize += 2 * overlapSize;

            if (overlappingTotalRowOffset < 0)
            {
                overlappingTotalRowOffset = 0;
                overlappingRowSize -= overlapSize;
            }
            if (overlappingTotalColOffset < 0)
            {
                overlappingTotalColOffset = 0;
                overlappingColSize -= overlapSize;
            }

            if (c == totalColBlocks - 1)
            {
                overlappingColSize -= overlapSize;
            }

            if (r == totalRowBlocks - 1)
            {
                overlappingRowSize -= overlapSize;
            }

            long overlappingRowExtent = overlappingTotalRowOffset + overlappingRowSize;
            if (overlappingRowExtent > m_paddedImageSize.row)
            {
                overlappingRowSize = m_paddedImageSize.row - overlappingTotalRowOffset;
            }

            long overlappingColExtent = overlappingTotalColOffset + overlappingColSize;
            if (overlappingColExtent > m_paddedImageSize.col)
            {
                overlappingColSize = m_paddedImageSize.col - overlappingTotalColOffset;
            }


            m_subImageUl[r][c] = SSize(overlappingTotalRowOffset, overlappingTotalColOffset);
            m_subImageSize[r][c] = SSize(overlappingRowSize, overlappingColSize);
            totalColOffset += colSize;
        }

        totalRowOffset += rowSize;
    }

    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::LoadAndSaveTile(const std::string &baseScenePath, const std::string &tileScenePath,
                                                            const SSize &selectUl, const SSize &selectSize)
{
    int div = 1;
    for (int t = 1; t < m_context->amountOfPyramids; t++)
    {
        div *= 2;
    }

    CMatrixArray<float> image;
    SImageMetadata meta;
    std::string proj4str;
    if (CImageLoader::getInstance()->LoadImage(baseScenePath, image, selectUl, selectSize) != 0 ||
        CImageLoader::getInstance()->LoadImageMetadata(baseScenePath, meta) ||
        CImageLoader::getInstance()->LoadProj4Str(baseScenePath, proj4str))
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from getInstance()->LoadImage()");
        return 1;
    }
    meta.setProj4String(proj4str);

    meta.setOrigin(meta.getOrigin() + meta.getGsd() * selectUl);
    std::string saveTemp = CFile(CFile(tileScenePath).getParentFolderFile(), CFile(tileScenePath).getFileNameWithoutExtension() + "_temp.TIF").getPath();
    if (CImageSaver::getInstance()->SaveImage(saveTemp, image, EImage::IMAGE_TYPE_GEOTIFF, &meta) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CImageSaver::SaveImage()");
        return 1;
    }

    unsigned long add = m_context->chipSize * 2;
    SSize outSize = image[0].getSize() + add * 2;
    SSize paddingAddedLr = add;
    SSize paddingAddedUl = add;
    paddingAddedLr.row += (div - outSize.row % div) % div;
    paddingAddedLr.col += (div - outSize.col % div) % div;

    if (CPaddImage::Padd(saveTemp, paddingAddedUl, paddingAddedLr, m_context->nullValue, tileScenePath) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CPaddImage::Padd()");
        return 1;
    }

    if (CFile::remove(saveTemp) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to remove file '" + saveTemp + "'");
        return 1;
    }
    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::GenerateTiles()
{
    CFile tilesInput = CFile(m_context->workingFolder, "Tiles");
    CFile refTiles = CFile(tilesInput, "Reference");
    CFile inputTiles = CFile(tilesInput, "Input");
    if (!tilesInput.exists())
    {
        if (tilesInput.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + tilesInput.getPath() + "'");
            return 1;
        }
    }
    if (!refTiles.exists())
    {
        if (refTiles.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + refTiles.getPath() + "'");
            return 1;
        }
    }
    if (!inputTiles.exists())
    {
        if (inputTiles.mkdir() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + inputTiles.getPath() + "'");
            return 1;
        }
    }

    SSize size = m_subImageUl.getSize();
    m_tilePaths.resize(size.getProduct());
    int loop = 0;
    for (unsigned long r = 0; r < size.row; r++)
    {
        for (unsigned long c = 0; c < size.col; c++, loop++)
        {
            std::string refTile = CFile(refTiles, "Reference_" + toString(r + 1) + "_" + toString(c + 1) + ".TIF").getPath();
            std::string inTile = CFile(inputTiles, "Input_" + toString(r + 1) + "_" + toString(c + 1) + ".TIF").getPath();

            SSize selectUl = m_subImageUl[r][c];
            SSize selectSize = m_subImageSize[r][c];

            if (LoadAndSaveTile(m_paddedInputScene, inTile, selectUl, selectSize) != 0 ||
                LoadAndSaveTile(m_paddedRefScene, refTile, selectUl, selectSize) != 0)
            {
                CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from LoadAndSaveTile()");
                return 1;
            }

            m_tilePaths[loop].resize(2);
            m_tilePaths[loop][REF_INDEX] = refTile;
            m_tilePaths[loop][INPUT_INDEX] = inTile;
        }
    }
    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::CleanSubItems(const std::string &path)
{
    CFile filePath = path;
    CVector<CFile> subItems;
    if (filePath.list(subItems) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CFile::List");
        return 1;
    }

    for (unsigned long t = 0; t < subItems.size(); t++)
    {
        if (subItems[t].remove() != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "Failed to remove '" + subItems[t].getPath() + "'");
        }
    }

    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::ProcessTile(const CVector<std::string> &scenePaths,
                                                        const std::string &workingFolder,
                                                        CVector<SChipCorrelationResult> &result)
{
    CGaussianPyramidTiePointGenerator::SContext gContext;
    gContext.amountOfPyramids = m_context->amountOfPyramids;
    gContext.resampleType = m_context->resampleType;
    gContext.workingFolder = workingFolder;
    gContext.minimumRequiredGcp = 3; //becomes stupid if it is less than 3


    gContext.tiePointGeneratorContext.setNullValue(m_context->nullValue);
    gContext.tiePointGeneratorContext.setMayContainNullValues(m_context->mayContainNullValues);
    gContext.tiePointGeneratorContext.chipGenerationGridSize = m_context->chipGenerationGridSize;
    gContext.tiePointGeneratorContext.chipGeneratorMethods = m_context->chipGeneratorMethods;
    gContext.tiePointGeneratorContext.fixedLocationChips = m_context->fixedLocationChips;
    gContext.tiePointGeneratorContext.fixedLocationChipProj4Str = m_context->fixedLocationChipProj4Str;

    gContext.tiePointGeneratorContext.chipSizeMinimum = m_context->chipSize;
    gContext.tiePointGeneratorContext.correlationThreshold = m_context->correlationThreshold;
    gContext.tiePointGeneratorContext.inputOffsetShiftInPixels = 0;
    gContext.tiePointGeneratorContext.searchWindowSize = m_context->chipSize / 1.5 + 1;
    gContext.tiePointGeneratorContext.threadCount = m_context->threadCount;
    gContext.tiePointGeneratorContext.typeOfChipCorrelationTechnique = m_context->typeOfChipCorrelationTechnique;
    gContext.tiePointGeneratorContext.outerHullRejectGcps = true;

    gContext.tiePointGeneratorContext.inputScene.pathToImage = scenePaths[INPUT_INDEX];
    gContext.tiePointGeneratorContext.inputScene.bandNumber = m_context->bandNumberToUse;

    gContext.tiePointGeneratorContext.referenceScene.pathToImage = scenePaths[REF_INDEX];
    gContext.tiePointGeneratorContext.referenceScene.bandNumber = m_context->bandNumberToUse;

    std::unique_ptr<CGaussianPyramidTiePointGenerator> generator(new CGaussianPyramidTiePointGenerator(&gContext));
    if (generator.get() == nullptr)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create CGaussianPyramidTiePointGenerator()");
        return 1;
    }

    if (generator->Calculate(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CGaussianPyramidTiePointGenerator::Calculate()");
        return 1;
    }
    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::RemoveDuplicateGcps(CVector<SChipCorrelationResult> &result)
{
    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Removing duplicate GCPs");
    unsigned long resultSize = result.size();
    long counter = 0;
    if (resultSize > 0)
    {
        SChipCorrelationResult *dp = result.getDataPointer();
        for (unsigned long t = 0; t < resultSize - 1; t++)
        {
            if (dp[t].correlationResultIsGood)
            {
                unsigned long start = t + 1;
                for (unsigned long i = start; i < resultSize; i++)
                {
                    if (dp[i].correlationResultIsGood &&
                        dp[t].chip.midCoordinate == dp[i].chip.midCoordinate)
                    {
                        dp[i].correlationResultIsGood = false;
                        counter++;
                    }
                }
            }
        }
    }

    CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "Removed '" + toString(counter) + "' duplicate GCPs");
    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::MergeAndCleanOutput(const CVector<CVector<SChipCorrelationResult> > &gcps,
                                                                CVector<SChipCorrelationResult> &result)
{
    unsigned long resultSize = 0;
    for (unsigned long t = 0; t < m_tilePaths.size(); t++)
    {
        unsigned long innerSize = gcps[t].size();
        for (unsigned long i = 0; i < innerSize; i++)
        {
            if (gcps[t][i].isGoodResult())
            {
                resultSize++;
            }
        }
    }

    result.resize(resultSize);

    unsigned long counter = 0;
    for (unsigned long t = 0; t < m_tilePaths.size(); t++)
    {
        unsigned long innerSize = gcps[t].size();
        for (unsigned long i = 0; i < innerSize; i++)
        {
            if (gcps[t][i].isGoodResult())
                result[counter++] = gcps[t][i];
        }
    }

    if (RemoveDuplicateGcps(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from RemoveDuplicateGcps()");
        return 1;
    }
    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::GenerateTiePoints(CVector<SChipCorrelationResult> &result)
{
    CFile pyramidPath = CFile(m_context->workingFolder, "PyramidProcessing");
    if (pyramidPath.mkdirIfNotExists() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failed to create folder '" + pyramidPath.getPath() + "'");
        return 1;
    }

    CVector<CVector<SChipCorrelationResult> > gcps(m_tilePaths.size());
    for (unsigned long t = 0; t < m_tilePaths.size(); t++)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "<<----------------------------------------------------------->>");
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, " <----TILE LOOP (" + toString(t + 1) + "/" + toString(m_tilePaths.size()) + ")----------------->");
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_INFO, "<<----------------------------------------------------------->>");
        if (CleanSubItems(pyramidPath.getPath()) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from CleanSubItems()");
            return 1;
        }

        if (ProcessTile(m_tilePaths[t], pyramidPath.getPath(), gcps[t]) != 0)
        {
            CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ProcessTile()");
            return 1;
        }
    }

    if (MergeAndCleanOutput(gcps, result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from MergeAndCleanOutput()");
        return 1;
    }

    return 0;
}

int CTiledGaussianPyramidTiePointGenerator::Calculate(CVector<SChipCorrelationResult> &result, bool *overlapExists)
{
    if (ContextOk() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from ContextOk()");
        return 1;
    }
    bool overlapExistsInner = true;
    if (MinMaxBoxImages(overlapExistsInner) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from MinMaxBoxImages()");
        return 1;
    }

    if (overlapExists != nullptr)
    {
        *overlapExists = overlapExistsInner;
    }

    if (!overlapExistsInner)
    {
        result.clear();
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_WARN, "No overlap returned from MinMaxBoxImages()");
        return 0;
    }

    if (PaddImagesToSameSize() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from PaddImagesToSameSize()");
        return 1;
    }

    if (GenerateTilesIndices() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateTilesIndices()");
        return 1;
    }

    if (GenerateTiles() != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateTiles()");
        return 1;
    }

    if (GenerateTiePoints(result) != 0)
    {
        CLogger::getInstance()->Log(__FILE__, __LINE__, CLogger::LOG_ERROR, "Failure returned from GenerateTiePoints()");
        return 1;
    }

    return 0;
}

} // namespace ultra
