# Pinkmatter Image GVerify

A tool that measures the geometric accuracy of an satellite orthorectified image.

The input image is matched to a reference image, using either spatial or phase correlation. This tool makes use of a Gaussian pyramid search mechanism to increase performance.

## Build from sources
GVerify depends on three third party libraries, and uses cmake for compilation.

### Dependencies:
Third party libraries:

* GDAL v3.3.0 [link](https://github.com/OSGeo/gdal/releases/download/v3.3.0/gdal-3.3.0.tar.gz)
* PROJ v6.1.1 [link](https://download.osgeo.org/proj/proj-6.1.1.tar.gz)
* EigenLib v3.4.0 [link](https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz)

Install GDAL, PROJ and EigenLib before building GVerify.

### GVerify

```bash
cd $DEV/GVerify
mkdir build
cd build
# EIGEN_HOME_PATH needs to point to EigenLib home
# GDAL_HOME_PATH needs to point to GDAL home
# PROJ_HOME_PATH needs to point to PROJ home

cmake3 -DCMAKE_BUILD_TYPE=Release -DEIGEN_HOME=EIGEN_HOME_PATH -DGDAL_HOME=GDAL_HOME_PATH -DPROJ4_HOME=PROJ_HOME_PATH ../src/
make -j
```

## Command line

```
[user@localhost build]./image-gverify
2022-06-15T09:54:43:758 ul_Logger.cpp 509: LOGGER: Log level is: INFO
Usable arguments:

	-b <reference image>
	-m <input image>
	-w <path to working directory>
	-o <output folder>
	-nv <null value>
	-t [chip gen type (EVEN / FIXED_LOCATION / SOBEL / HARRIS)]
	-t_file [path to fixed-chip-location file]
	-p [pyramid levels(1)]
	-n [thread count(1)]
	-l [log folder]
	-c [correlation coefficient(0.75)]
	-r [resample technique (NN/BI/CI)]
	-cs [chip size (33)]
	-st [save geotiff (false)]
	-su [save union images between reference and input (true)]
	-g [grid size (chip size * 2)]
	-b_p [reference image proj4 string (example "+proj=utm +ellps=WGS84 +units=m +no_defs +zone=36"')]
	-m_p [input image proj4 string (example "+proj=eqc +ellps=sphere +R=5729577.951308 +units=m +no_defs"')]
	-usePhaseCorrelation [true/false (false)]

Note:
	'-b_p' and '-m_p' do not need to be specified, these are only used if you wish to override
	the detected Proj4 projection strings

fixed-chip-location structure:
  Is a plain text file on which each line must contain a latitude,longitude decimal degrees pair in WGS84
  Example:
  -25.32,132.3
  -23.23,133.343
  ...
  -25.342,134.33

```

# License

* Refer to [LICENSE](LICENSE.txt)