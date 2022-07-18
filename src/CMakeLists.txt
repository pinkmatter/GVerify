cmake_minimum_required(VERSION 3.17.5)

set(EIGEN_HOME /path/to/eigen CACHE STRING "Path to EIGEN home")
set(GDAL_HOME /path/to/gdal CACHE STRING "Path to GDAL home")
set(PROJ4_HOME /path/to/proj4 CACHE STRING "Path to PROJ4 home")

project(image-gverify)

add_subdirectory(impl)

install(TARGETS image-gverify)

