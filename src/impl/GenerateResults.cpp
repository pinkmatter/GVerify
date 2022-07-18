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

#include "GenerateResults.h"

#include <ul_ImageLoader.h>
#include <ul_ImageSaver.h>
#include <ul_Logger.h>
#include <ul_Pair.h>
#include <ul_Proj4Projection.h>
#include <ul_File.h>

static std::string changeExt(const ultra::CFile &filePath, std::string ext)
{
    return (filePath.getParentFolderFile()+(filePath.getFileNameWithoutExtension() + "." + ext)).getPath();
}

static int SaveResidualFile(SArgs &args,
                            const std::string &inProj4Str,
                            const std::string &refProj4Str,
                            const ultra::SImageMetadata &metaIn,
                            const ultra::SImageMetadata &metaRef,
                            ultra::SPair<double> &totalXY,
                            ultra::CVector<unsigned int> &residualHistogram,
                            const ultra::SImageMetadata &subSetMetaRef,
                            const ultra::CVector<ultra::SChipCorrelationResult> &result)
{
    ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_INFO, "Residual X = " + ultra::toString(totalXY.x) + " Y = " + ultra::toString(totalXY.y));
    char msg[4096];
    sprintf(msg, "Residual histogram\n");
    for (int x = 0; x < residualHistogram.size(); x++)
    {
        switch (x)
        {
        case 0:
            sprintf(msg, "%s\tGreen %d\n", msg, residualHistogram[x]);
            break;
        case 1:
            sprintf(msg, "%s\tTeal %d\n", msg, residualHistogram[x]);
            break;
        case 2:
            sprintf(msg, "%s\tBlue %d\n", msg, residualHistogram[x]);
            break;
        case 3:
            sprintf(msg, "%s\tYellow %d\n", msg, residualHistogram[x]);
            break;
        case 4:
            sprintf(msg, "%s\tRed %d\n", msg, residualHistogram[x]);
            break;
        }
    }
    ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_INFO, msg);

    std::string outputPath = args.outputPath + "/image-gverify.res";
    FILE *fm = fopen(outputPath.c_str(), "w");
    if (fm == NULL)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_WARN, "Failed to create residual file");
        return 1;
    }

    fprintf(fm, "IMAGE_GVERIFY\n\n");
    fprintf(fm, "Total residuals\n");
    fprintf(fm, "\tResidual x = %lf\n", totalXY.x);
    fprintf(fm, "\tResidual y = %lf\n", totalXY.y);

    fprintf(fm, "Residual histogram\n");
    for (int x = 0; x < residualHistogram.size(); x++)
    {
        switch (x)
        {
        case 0:
            fprintf(fm, "\tGreen %d\n", residualHistogram[x]);
            break;
        case 1:
            fprintf(fm, "\tTeal %d\n", residualHistogram[x]);
            break;
        case 2:
            fprintf(fm, "\tBlue %d\n", residualHistogram[x]);
            break;
        case 3:
            fprintf(fm, "\tYellow %d\n", residualHistogram[x]);
            break;
        case 4:
            fprintf(fm, "\tRed %d\n", residualHistogram[x]);
            break;
        }
    }
    ultra::CVector<double> afx = metaRef.getAffineGeoTransform();
    const double* dp = afx.getDataPointer();
    fprintf(fm, "\n");
    fprintf(fm, "Geo-Affine-Transform [%4.4lf, %4.4lf, %4.4lf, %4.4lf, %4.4lf, %4.4lf]\n", dp[0], dp[1], dp[2], dp[3], dp[4], dp[5]);
    fprintf(fm, "Proj4-Str [%s]\n", refProj4Str.c_str());

    ultra::EProjectionEnum::EProjectionUnits projUnits = subSetMetaRef.projectionUnits;
    std::string resType = ultra::toLower(ultra::EProjectionEnum::projectionUnitsToStr(projUnits));
    if (resType == "")
        resType = "meters";
    fprintf(fm, "-----------------------------\n");
    fprintf(fm, "-------------GCPs------------\n");
    fprintf(fm, "-----------------------------\n");
    fprintf(fm, "Point_ID    Line      Sample         Map-X      Map-Y     Corelation  Residual Residual Outlier \n");
    fprintf(fm, "         (Ref image) (Ref image) (Ref image) (Ref image)  Coefficient  In y     In x     Flag   \n");
    fprintf(fm, "                                                                                        (0=bad  \n");
    fprintf(fm, "                                                                    (%s) (%s)   1=OK)  \n", resType.c_str(), resType.c_str());
    fprintf(fm, "BEGIN\n");

    for (long t = 0; t < result.size(); t++)
    {
        long id = t;
        double mapx = result[t].chip.midCoordinate.x;
        double mapy = result[t].chip.midCoordinate.y;
        ultra::SPair<long> sl = metaRef.getSampleLine(result[t].chip.midCoordinate).roundValues().convertType<long>();
        long line = sl.y;
        long sample = sl.x;
        double corcoef = result[t].corrCoefficient;
        double resx = (mapx - result[t].newMidCoordinate.x);
        double resy = (mapy - result[t].newMidCoordinate.y);
        int good = (result[t].isGoodResult() ? (1) : (0));
        if (result[t].correlationResultIsGood)
            fprintf(fm, "%04d %s\t%05d\t\t%05d\t\t%4.4lf\t\t%4.4lf\t\t%4.4lf\t\t%4.4lf\t\t%4.4lf\t\t%d\n", id, result[t].chip.chipType.c_str(), line, sample, mapx, mapy, corcoef, resy, resx, good);
    }

    fclose(fm);
    return 0;
}

static int SaveImage(SArgs &args,
                     ultra::CImage &imgInput,
                     ultra::CImage &imgRef,
                     const ultra::SImageMetadata &subSetMetaIn,
                     const ultra::SImageMetadata &subSetMetaRef)
{
    std::string outputPath = args.outputPath + "/image-gverify_result.jpeg";
    if (ultra::CImageSaver::getInstance()->SaveImage(outputPath, imgInput, ultra::EImage::IMAGE_TYPE_JPEG) != 0 ||
        args.saveTiff && ultra::CImageSaver::getInstance()->SaveImage(changeExt(outputPath, "tif"), imgInput, ultra::EImage::IMAGE_TYPE_GEOTIFF, &subSetMetaIn) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned form SaveImage()");
        return 1;
    }
    outputPath = args.outputPath + "/image-gverify_result.jgw";
    if (subSetMetaIn.GenerateWorldFile(outputPath) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from GenerateWorldFile()");
        return 1;
    }

    outputPath = args.outputPath + "/image-gverify_base.jpeg";
    if (ultra::CImageSaver::getInstance()->SaveImage(outputPath, imgRef, ultra::EImage::IMAGE_TYPE_JPEG) != 0 ||
        args.saveTiff && ultra::CImageSaver::getInstance()->SaveImage(changeExt(outputPath, "tif"), imgRef, ultra::EImage::IMAGE_TYPE_GEOTIFF, &subSetMetaRef) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned form SaveImage()");
        return 1;
    }
    outputPath = args.outputPath + "/image-gverify_base.jgw";
    if (subSetMetaRef.GenerateWorldFile(outputPath) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from GenerateWorldFile()");
        return 1;
    }

    return 0;
}

static int DrawDot(
                   const ultra::SChipCorrelationResult &gcp,
                   ultra::CImage &imgInput,
                   const ultra::SImageMetadata &subSetMeta,
                   ultra::CProj4Projection *proj4,
                   const ultra::SColor &col,
                   bool isRef
                   )
{
    ultra::SPair<double> ptMap;
    if (isRef)
        ptMap = gcp.chip.midCoordinate;
    else
        ptMap = gcp.newMidCoordinate;
    if (proj4 != NULL)
    {
        if (proj4->Project(ptMap, ptMap) != 0)
        {
            ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CProj4Projection::Project()");
            return 1;
        }
    }
    ultra::SPair<long> ptSl = subSetMeta.getSampleLine(ptMap).roundValues().convertType<long>();

    if (ptSl.r < 15 || ptSl.c < 15 ||
        ptSl.r >= imgInput.getSize().row - 15 || ptSl.c >= imgInput.getSize().col - 15)
    {
        return 0;
    }

    for (long r = -14; r <= 14; r++)
    {
        for (long c = -14; c <= 14; c++)
        {
            if (c != 0 || r != 0)
                imgInput[(long) (ptSl.r + r)][(long) (ptSl.c + c)] = col;
        }
    }

    return 0;
}

static int GenUnionBounds(const ultra::SImageMetadata &metaRef,
                          const ultra::SImageMetadata &metaIn,
                          const std::string &refProj4,
                          const std::string &inProj4,
                          ultra::SPair<double> &outRefUl, ultra::SPair<double> &outRefLr,
                          ultra::SPair<double> &outInUl, ultra::SPair<double> &outInLr)
{
    ultra::CProj4Projection proj4ToRef(inProj4, refProj4);
    ultra::CProj4Projection proj4ToIn(refProj4, inProj4);

    ultra::SPair<double> ulRef = metaRef.getOrigin();
    ultra::SPair<double> lrRef = metaRef.getMapCoord(metaRef.getDimensions());
    ultra::SPair<double> ulInOld = metaIn.getOrigin();
    ultra::SPair<double> lrInOld = metaIn.getMapCoord(metaIn.getDimensions());
    ultra::SPair<double> ulIn;
    ultra::SPair<double> lrIn;
    ultra::SSize inSize = metaIn.getDimensions();
    ultra::CVector<ultra::SPair<double> > inBounds(inSize.col * 2 + inSize.row * 2);

    unsigned long t = 0;
    for (unsigned long r = 0; r < inSize.row; r++)
    {
        inBounds[t++] = metaIn.getMapCoord(ultra::SPair<double>(r, 0));
        inBounds[t++] = metaIn.getMapCoord(ultra::SPair<double>(r, inSize.col));
    }
    for (unsigned long c = 0; c < inSize.col; c++)
    {
        inBounds[t++] = metaIn.getMapCoord(ultra::SPair<double>(0, c));
        inBounds[t++] = metaIn.getMapCoord(ultra::SPair<double>(inSize.row, c));
    }

    if (proj4ToRef.Project(inBounds, inBounds) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CProj4Projection::Project()");
        return 1;
    }

    ulIn = lrIn = inBounds[0];

    for (const auto &e : inBounds)
    {
        ulIn.r = ultra::getMAX(ulIn.r, e.r);
        ulIn.c = ultra::getMIN(ulIn.c, e.c);

        lrIn.r = ultra::getMIN(lrIn.r, e.r);
        lrIn.c = ultra::getMAX(lrIn.c, e.c);
    }

    ultra::SPair<double> unionUl;
    ultra::SPair<double> unionLr;

    unionUl.c = ultra::getMAX(ulRef.c, ulIn.c);
    unionUl.r = ultra::getMIN(ulRef.r, ulIn.r);
    unionLr.c = ultra::getMIN(lrRef.c, lrIn.c);
    unionLr.r = ultra::getMAX(lrRef.r, lrIn.r);

    outRefUl = unionUl;
    outRefLr = unionLr;

    if (proj4ToIn.Project(unionUl, outInUl) != 0 ||
        proj4ToIn.Project(unionLr, outInLr) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CProj4Projection::Project()");
        return 1;
    }

    ultra::SPair<double> min = ultra::SPair<double>::getMin(ulInOld, lrInOld);
    ultra::SPair<double> max = ultra::SPair<double>::getMax(ulInOld, lrInOld);
    outInUl = outInUl.clipBetween(min, max);
    outInLr = outInLr.clipBetween(min, max);

    return 0;
}

static int LoadImage(const std::string &imgPath, ultra::CImage &img,
                     const ultra::SPair<double> &ulCoor, const ultra::SPair<double> &lrCoor,
                     const ultra::SImageMetadata &meta, ultra::SImageMetadata &subSetMetaOut)
{
    ultra::SPair<double> ulSl = meta.getSampleLine(ulCoor).ceilValues();
    ultra::SPair<double> lrSl = meta.getSampleLine(lrCoor).floorValues();
    ultra::SPair<double> temp = round(lrSl - ulSl);

    if (temp.r < 1 || temp.c < 1 ||
        ulSl.r < 0 || ulSl.c < 0 ||
        ulSl.r >= meta.getDimensions().row || ulSl.c >= meta.getDimensions().col)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Trying to load an image with a size smaller than 1x1 or outsize of the image bounds");
        return 1;
    }
    ultra::SSize ul = ulSl.getSizeType();
    ultra::SSize outSize = temp.getSizeType();
    ultra::CMatrix<float> tempImage;
    std::string proj4Str;
    if (ultra::CImageLoader::getInstance()->LoadImageWithMetadata(imgPath, tempImage, ul, outSize, subSetMetaOut) != 0 ||
        ultra::CImageLoader::getInstance()->LoadProj4Str(imgPath, proj4Str) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageWithMetadata()");
        return 1;
    }
    subSetMetaOut.setProj4String(proj4Str);

    if (img.SetRed(tempImage.fitConvertType<unsigned char>()) != 0 ||
        img.SetGreen(img.getRed()) != 0 ||
        img.SetBlue(img.getRed()) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failed to set image colors");
        return 1;
    }

    return 0;
}

static int SaveResults(SArgs &args, const ultra::CVector<ultra::SChipCorrelationResult> &result, bool overlapExists,
                       const std::string &inProj4Str, const std::string &refProj4Str,
                       ultra::CImage &imgRef, ultra::CImage &imgIn,
                       const ultra::SImageMetadata &metaRef, const ultra::SImageMetadata &metaIn,
                       const ultra::SImageMetadata &subSetMetaRef, const ultra::SImageMetadata &subSetMetaIn)
{
    unsigned long counter = 0;
    ultra::SPair<double> totalXY(0, 0);
    ultra::CVector<unsigned int> residualHistogram;
    residualHistogram.resize(5);
    residualHistogram.initVec(0);
    ultra::CProj4Projection proj4RefToIn(args.referenceImage.proj4Str, args.inputImage.proj4Str);

    ultra::SPair<double> ulRef = subSetMetaRef.getOrigin();
    ultra::SPair<double> gsdRef = subSetMetaRef.getGsd();
    for (unsigned long t = 0; t < result.size(); t++)
    {
        if (result[t].isGoodResult())
        {
            counter++;
            ultra::SPair<double> coorRef = (result[t].chip.midCoordinate - ulRef) / gsdRef;
            ultra::SPair<double> coorInput = (result[t].newMidCoordinate - ulRef) / gsdRef;
            double disparity = coorRef.distance(coorInput);
            totalXY.x += ultra::getAbs(coorRef.x - coorInput.x);
            totalXY.y += ultra::getAbs(coorRef.y - coorInput.y);
            ultra::SColor col;
            if (disparity < 0.5)
            {
                col = 0xff00; //green
                residualHistogram[0]++;
            }
            else if (disparity < 1.0)
            {
                col = 0xffff; //teal
                residualHistogram[1]++;
            }
            else if (disparity < 2)
            {
                col = 0xff; //blue
                residualHistogram[2]++;
            }
            else if (disparity < 5)
            {
                col = 0xffff00; //yellow
                residualHistogram[3]++;
            }
            else
            {
                col = 0xff0000; //red
                residualHistogram[4]++;
            }

            if (DrawDot(result[t], imgIn, subSetMetaIn, &proj4RefToIn, col, false) != 0 ||
                DrawDot(result[t], imgRef, subSetMetaRef, NULL, col, true) != 0)
            {
                ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from DrawDot()");
                return 1;
            }
        }
    }

    if (counter == 0)
    {
        totalXY.x = NAN;
        totalXY.y = NAN;
    }
    else
    {
        totalXY /= (double) counter;
    }

    if (overlapExists)
    {
        if (SaveImage(args, imgIn, imgRef, subSetMetaIn, subSetMetaRef) != 0)
        {
            ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned form SaveImage()");
            return 1;
        }
    }

    if (SaveResidualFile(args, inProj4Str, refProj4Str, metaIn, metaRef, totalXY, residualHistogram, subSetMetaRef, result) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned form SaveResidualFile()");
        return 1;
    }

    return 0;
}

int GenerateResults(SArgs &args, const ultra::CVector<ultra::SChipCorrelationResult> &result, bool overlapExists)
{
    ultra::CImage imgRef, imgIn;
    ultra::SImageMetadata metaRef, metaIn;
    ultra::SImageMetadata subSetMetaRef2, subSetMetaIn2;
    std::string inProj4, refProj4;
    ultra::SPair<double> outRefUl, outRefLr;
    ultra::SPair<double> outInUl, outInLr;

    if (ultra::CImageLoader::getInstance()->LoadImageMetadata(args.originalReferencePath, metaRef) != 0 ||
        ultra::CImageLoader::getInstance()->LoadImageMetadata(args.originalInputPath, metaIn) != 0 ||
        ultra::CImageLoader::getInstance()->LoadProj4Str(args.originalReferencePath, refProj4) != 0 ||
        ultra::CImageLoader::getInstance()->LoadProj4Str(args.originalInputPath, inProj4) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from CImageLoader::LoadImageMetadata()");
        return 1;
    }
    metaRef.setProj4String(refProj4);
    metaIn.setProj4String(inProj4);

    if (overlapExists)
    {
        if (args.saveUnion)
        {
            if (GenUnionBounds(metaRef, metaIn,
                               refProj4, inProj4,
                               outRefUl, outRefLr,
                               outInUl, outInLr) != 0)
            {
                ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from GenUnionBounds()");
                return 1;
            }
        }
        else
        {
            outRefUl = metaRef.getMapCoord(ultra::SPair<double>{0, 0});
            outRefLr = metaRef.getMapCoord(metaRef.getDimensions());
            outInUl = metaIn.getMapCoord(ultra::SPair<double>{0, 0});
            outInLr = metaIn.getMapCoord(metaIn.getDimensions());
        }

        if (LoadImage(args.originalReferencePath, imgRef, outRefUl, outRefLr, metaRef, subSetMetaRef2) != 0 ||
            LoadImage(args.originalInputPath, imgIn, outInUl, outInLr, metaIn, subSetMetaIn2) != 0)
        {
            ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from LoadImage()");
            return 1;
        }
    }

    if (SaveResults(args, result, overlapExists, inProj4, refProj4, imgRef, imgIn, metaRef, metaIn, subSetMetaRef2, subSetMetaIn2) != 0)
    {
        ultra::CLogger::getInstance()->Log(__FILE__, __LINE__, ultra::CLogger::LOG_ERROR, "Failure returned from SaveResults()");
        return 1;
    }

    return 0;
}
