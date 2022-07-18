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

#include <ostream>
#include <ul_ProjectionEnum.h>
#include <ul_Pair.h>
#include <ul_3D.h>
#include <ul_2D.h>
#include <ul_ImageEnums.h>
#include <ul_Map.h>
#include <ul_KeyValue.h>
#include <ul_Odl.h>
#include <ul_Square.h>


namespace ultra
{

struct SImageDate
{
    int m_hour;
    int m_minute;
    double m_second;

    int m_year;
    int m_month;
    int m_day;

    SImageDate();
    ~SImageDate();
    SImageDate(const SImageDate & r);

    void set(int year, int month, int day, int hour, int minute, double second);
    SImageDate &operator=(const SImageDate & r);
};

struct SImageMetadata
{
public:

    enum EProjectionParmIndex
    {
        SEMI_MAJOR_AXIS = 0,
        SEMI_MINOR_AXIS,
        LATITUDE_OF_THE_FIRST_STANDARD_PARALLEL,
        LATITUDE_OF_THE_SECOND_STANDARD_PARALLEL,
        LONGITUDE_OF_THE_CENTRAL_MERIDIAN,
        LATITUDE_OF_THE_PROJECTION_ORIGIN,
        FALSE_EASTING,
        FALSE_NORTHING,
        LONGITUDE_OF_FIRST_POINT_ON_CENTER_LINE,
        LATITUDE_OF_FIRST_POINT_ON_CENTER_LINE,
        LONGITUDE_OF_SECOND_POINT_ON_CENTER_LINE,
        LATITUDE_OF_SECOND_POINT_ON_CENTER_LINE,
        EXTRA_1,
        EXTRA_2,
        EXTRA_3,
        EPROJECTION_PARM_INDEX_COUNT
    };

    struct STiePoint
    {
    private:
        S3D<double> m_gcp;
        S2D<double> m_imagePoint;
        std::string m_info;
        std::string m_id;
        friend class CVector<SImageMetadata::STiePoint>;
        STiePoint();
    public:
        STiePoint(const S3D<double> &gcp, const S2D<double> &imagePoint, const std::string &info, const std::string &id);
        STiePoint(const SImageMetadata::STiePoint &r);
        ~STiePoint();

        SImageMetadata::STiePoint &operator=(const SImageMetadata::STiePoint &r);

        S3D<double> getGcp() const;
        std::string getId() const;
        S2D<double> getImagePoint() const;
        std::string getInfo() const;
    };

    struct SStringRpc
    {
    public:

        struct SRpcCoeff
        {
        private:
            std::string m_den, m_num;
            double m_pixelOffSet, m_pixelScale;
            double m_degreeOffSet, m_degreeScale;
            double m_degreeMin, m_degreeMax;
        public:

            SRpcCoeff();
            SRpcCoeff(const std::string &den, const std::string &num,
                      double pixelOffSet, double pixelScale,
                      double degreeOffSet, double degreeScale,
                      double degreeMin, double degreeMax);
            SRpcCoeff(const SImageMetadata::SStringRpc::SRpcCoeff &r);
            ~SRpcCoeff();
            SImageMetadata::SStringRpc::SRpcCoeff &operator=(const SImageMetadata::SStringRpc::SRpcCoeff &r);

            void setDen(std::string den);
            void setNum(std::string num);
            std::string getDen() const;
            std::string getNum() const;

            void setDegreeMax(double degreeMax);
            void setDegreeMin(double degreeMin);
            void setDegreeOffSet(double degreeOffSet);
            void setDegreeScale(double degreeScale);
            void setPixelOffSet(double pixelOffSet);
            void setPixelScale(double pixelScale);
            double getDegreeMax() const;
            double getDegreeMin() const;
            double getDegreeOffSet() const;
            double getDegreeScale() const;
            double getPixelOffSet() const;
            double getPixelScale() const;
        };

        struct SRpcHeight
        {
        private:
            double m_heightOffset, m_heightScale;
        public:
            SRpcHeight();
            SRpcHeight(double heightOffset, double heightScale);
            SRpcHeight(const SImageMetadata::SStringRpc::SRpcHeight &r);
            ~SRpcHeight();
            SImageMetadata::SStringRpc::SRpcHeight &operator=(const SImageMetadata::SStringRpc::SRpcHeight &r);

            void setHeightOffset(double heightOffset);
            void setHeightScale(double heightScale);
            double getHeightOffset() const;
            double getHeightScale() const;

        };
    private:
        SRpcCoeff m_samples, m_lines;
        SRpcHeight m_height;
        std::string m_polynomial;
    public:
        SStringRpc();
        SStringRpc(const SImageMetadata::SStringRpc &r);
        ~SStringRpc();
        SImageMetadata::SStringRpc &operator=(const SImageMetadata::SStringRpc &r);

        void setPolynomial(std::string poly);
        void setLines(SImageMetadata::SStringRpc::SRpcCoeff lines);
        void setSamples(SImageMetadata::SStringRpc::SRpcCoeff samples);
        void setHeight(SImageMetadata::SStringRpc::SRpcHeight height);
        std::string getPolynomial() const;
        SImageMetadata::SStringRpc::SRpcCoeff getLines() const;
        SImageMetadata::SStringRpc::SRpcCoeff getSamples() const;
        SImageMetadata::SStringRpc::SRpcHeight getHeight() const;
    };

private:

    SStringRpc m_rpc;

    struct SProjParms
    {
        SProjParms();
        ~SProjParms();
        SProjParms(const SProjParms &r);
        SProjParms &operator=(const SProjParms &r);

        double projParm;
        bool gotProjParm;
    };

    SProjParms m_projParms[static_cast<int> (EPROJECTION_PARM_INDEX_COUNT)];

    int getProjParmStr(EProjectionParmIndex index, std::string &parmStr) const;
    CVector<double> m_affineGeoTransform;
    CVector<SImageMetadata::STiePoint> m_tiePoints;

    SSize m_dimension;
    bool m_pixelIsAreaIsSet;
    bool m_pixelIsArea;
    bool m_hasGdalProjectionRefWkt;
    std::string m_gdalProjectionRefWktStr;
    COdl m_metadataTags;
    std::string m_proj4Str;
    bool m_proj4StrSet;
public:
    std::string originId;
    std::string originatorName;
    std::string imageId;
    std::string title;
    std::string imageComment;
    std::string geotifProjectionCode;

    EProjectionEnum::EProjectionType projectionCode;
    EProjectionEnum::EProjectionUnits projectionUnits;

    int utmZone;
    int bpp;
    int bandCount;

    EProjectionEnum::EProjectionSpheroid spheroid;
    EProjectionEnum::EProjectionDatum datum;
    EProjectionEnum::EProjectionGcsDatum gcsDatum;

    SImageDate fileDate;
    SImageDate imageDate;

    SImageMetadata();
    SImageMetadata(const SImageMetadata & r);
    ~SImageMetadata();
    SImageMetadata &operator=(const SImageMetadata & r);

    int Init(const COdl &metadata);
    void setGsd(const SPair<double> &gsd);
    void setGsd(double gsd_x, double gsd_y);
    void setShear(const SPair<double> &shear);
    void setShear(double shear_x, double shear_y);
    void setDimensions(unsigned long height, unsigned long width);
    void setDimensions(const SSize &size);
    SSize getDimensions() const;
    SPair<double> getGsd() const;
    SPair<double> getOrigin() const;
    SPair<double> getUL() const;
    SPair<double> getUR() const;
    SPair<double> getLL() const;
    SPair<double> getLR() const;
    void setOrigin(const SPair<double> &origin);
    void setOrigin(double origin_x, double origin_y);
    bool getPixelIsArea() const;
    void setPixelIsArea(bool pixelIsArea);
    void clearPixelIsArea();
    bool getPixelIsAreaIsSet() const;
    int getCornerCoordinates(EImage::EImageCorner corner, SPair<double> &cornerVal) const;
    int getProjParm(SImageMetadata::EProjectionParmIndex index, double &projVal, bool &wasSet) const;
    int getProjParms(CVector<SKeyValue<int, double> > &parms) const;
    int setProjParm(SImageMetadata::EProjectionParmIndex index, double projVal, bool setUse = true);

    int GenProj4String(std::string &proj4Str) const;
    std::string getProj4String() const;
    void setProj4String(const std::string &proj4Str);
    void clearProj4String();
    bool isProj4StringSet() const;

    SImageMetadata::SStringRpc getRpc() const;
    void setRpc(SImageMetadata::SStringRpc rpc);

    int SetAffineGeoTransform(const CVector<double> &vec);
    CVector<double> getAffineGeoTransform() const;

    static CVector<double> createAffineTransform(const SPair<double> &gsd, const SPair<double> &origin, const SPair<double> &shear);

    ///////
    /////// getSampleLine
    ///////

    static inline SPair<double> getSampleLine(const CVector<double> &affineGeoTransform, const double &mapCoord_x, const double &mapCoord_y)
    {
        double y = (mapCoord_y - affineGeoTransform[3] - affineGeoTransform[4]*((mapCoord_x - affineGeoTransform[0]) / affineGeoTransform[1])) / (affineGeoTransform[5] - affineGeoTransform[4] * affineGeoTransform[2] / affineGeoTransform[1]);
        return SPair<double>
            (
            y,
            (mapCoord_x - affineGeoTransform[0] - y * affineGeoTransform[2]) / affineGeoTransform[1]
            );
    }

    static inline SPair<double> getSampleLine(const CVector<double> &affineGeoTransform, const SPair<double> &mapCoord)
    {
        return getSampleLine(affineGeoTransform, mapCoord.x, mapCoord.y);
    }

    inline SPair<double> getSampleLine(const SPair<double> &mapCoord) const
    {
        return getSampleLine(m_affineGeoTransform, mapCoord.x, mapCoord.y);
    }

    static inline CVector<SPair<double> > getSampleLine(const CVector<double> &affineGeoTransform, const CVector<SPair<double> > &mapCoords)
    {
        unsigned long size = mapCoords.size();
        CVector<SPair<double> > ret(size);
        const SPair<double> *inDp = mapCoords.getDataPointer();
        SPair<double> *outDp = ret.getDataPointer();

        for (unsigned long t = 0; t < size; t++)
        {
            outDp[t] = getSampleLine(affineGeoTransform, inDp[t]);
        }

        return ret;
    }

    inline CVector<SPair<double> > getSampleLine(const CVector<SPair<double> > &mapCoords) const
    {
        return getSampleLine(m_affineGeoTransform, mapCoords);
    }

    ///////
    /////// getMapCoor
    ///////

    static inline SPair<double> getMapCoord(const CVector<double> &affineGeoTransform, const double &sampleLine_x, const double &sampleLine_y)
    {
        return SPair<double>
            (
            affineGeoTransform[3] + sampleLine_x * affineGeoTransform[4] + sampleLine_y * affineGeoTransform[5],
            affineGeoTransform[0] + sampleLine_x * affineGeoTransform[1] + sampleLine_y * affineGeoTransform[2]
            );
    }

    static inline SPair<double> getMapCoord(const CVector<double> &affineGeoTransform, const SPair<double> &sampleLine)
    {
        return getMapCoord(affineGeoTransform, sampleLine.x, sampleLine.y);
    }

    inline SPair<double> getMapCoord(const SPair<double> &sampleLine) const
    {
        return getMapCoord(m_affineGeoTransform, sampleLine.x, sampleLine.y);
    }

    static inline CVector<SPair<double> > getMapCoord(const CVector<double> &affineGeoTransform, const CVector<SPair<double> > &sampleLines)
    {
        unsigned long size = sampleLines.size();
        CVector<SPair<double> > ret(size);
        const SPair<double> *inDp = sampleLines.getDataPointer();
        SPair<double> *outDp = ret.getDataPointer();

        for (unsigned long t = 0; t < size; t++)
        {
            outDp[t] = getMapCoord(affineGeoTransform, inDp[t]);
        }

        return ret;
    }

    inline CVector<SPair<double> > getMapCoord(const CVector<SPair<double> > &sampleLines) const
    {
        return getMapCoord(m_affineGeoTransform, sampleLines);
    }

    inline CSquare<double> getImageBounds()
    {
        return getImageBounds(m_affineGeoTransform, m_dimension);
    }

    ///////
    /////// helpers
    ///////

    static inline SPair<double> getGsd(const CVector<double> &affineGeoTransform)
    {
        return SPair<double>(affineGeoTransform[5], affineGeoTransform[1]);
    }

    static inline SPair<double> getOrigin(const CVector<double> &affineGeoTransform)
    {
        return getMapCoord(affineGeoTransform, SPair<double>(0, 0));
    }

    static inline SPair<double> getShear(const CVector<double> &affineGeoTransform)
    {
        return SPair<double>(affineGeoTransform[4], affineGeoTransform[2]);
    }

    static inline CSquare<double> getImageBounds(const CVector<double> &affineGeoTransform, const ultra::SSize imageDims)
    {
        SPair<double> ul, lr;
        ul = lr = getOrigin(affineGeoTransform);

        ultra::SPair<double> max = imageDims;
        max--;
        ultra::SPair<double> sl, ans1, ans2;
        for (sl.r = 0; sl.r <= max.r; sl.r++)
        {
            sl.c = 0;
            ans1 = getMapCoord(affineGeoTransform, sl);
            sl.c = max.c;
            ans2 = getMapCoord(affineGeoTransform, sl);

            ul.c = getMIN(ul.c, getMIN(ans1.c, ans2.c));
            lr.c = getMAX(lr.c, getMAX(ans1.c, ans2.c));

            ul.r = getMAX(ul.r, getMAX(ans1.r, ans2.r));
            lr.r = getMIN(lr.r, getMIN(ans1.r, ans2.r));
        }
        for (sl.c = 0; sl.c <= max.c; sl.c++)
        {
            sl.r = 0;
            ans1 = getMapCoord(affineGeoTransform, sl);
            sl.r = max.r;
            ans2 = getMapCoord(affineGeoTransform, sl);

            ul.c = getMIN(ul.c, getMIN(ans1.c, ans2.c));
            lr.c = getMAX(lr.c, getMAX(ans1.c, ans2.c));

            ul.r = getMAX(ul.r, getMAX(ans1.r, ans2.r));
            lr.r = getMIN(lr.r, getMIN(ans1.r, ans2.r));
        }
        CSquare<double> bounds(ul, lr);
        return bounds;
    }

    static CVector<double> setOrigin(const CVector<double> &affineGeoTransform, const SPair<double> &origin)
    {
        CVector<double> result = affineGeoTransform;
        result[0] = origin.x;
        result[3] = origin.y;
        return result;
    }

    static CVector<double> setGsd(const CVector<double> &affineGeoTransform, const SPair<double> &gsd)
    {
        CVector<double> result = affineGeoTransform;
        result[1] = gsd.c;
        result[5] = gsd.r;
        return result;
    }

    static CVector<double> setShear(const CVector<double> &affineGeoTransform, const SPair<double> &shear)
    {
        CVector<double> result = affineGeoTransform;
        result[2] = shear.c;
        result[4] = shear.r;
        return result;
    }

    CVector<SImageMetadata::STiePoint> getTiePoints() const;
    int GenerateWorldFile(std::string pathToWorldFile) const;
    static int GenerateWorldFile(std::string pathToWorldFile, const CVector<double> &geoAffineTransform);

    bool hasGdalProjectionRefWkt() const;
    std::string getGdalProjectionRefWkt() const;
    void setGdalProjectionRefWkt(const std::string &gdalProjectionRefWkt);
    void cleanGdalProjectionRefWkt();

    const COdl &getExtraMetadata() const;
};

}
