file(GLOB SOURCES "*.cpp")
file(GLOB SOURCES_GDAL "GDAL/*.cpp")
file(GLOB SOURCES_ImageLoaderLocks "ImageLoaderLocks/*.cpp")
file(GLOB SOURCES_Interface "Interface/*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-image ${SOURCES} ${SOURCES_GDAL} ${SOURCES_ImageLoaderLocks} ${SOURCES_Interface} ${SOURCES_RPC})

include_directories(ultra-image PUBLIC 
			  "../inc"
  			  "../../util/inc"
  			  "../../data/inc"
  			  "../../io/inc"
  			  "../../projection/inc"
  			  "../../plot/inc"
  			  "${GDAL_HOME}/include")
