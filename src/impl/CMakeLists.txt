add_subdirectory(ultra)

file(GLOB SOURCES "*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_executable(image-gverify ${SOURCES})

target_link_libraries(image-gverify PUBLIC 
				ultra-algo 
				ultra-image
				ultra-data 
				ultra-util
				ultra-io 
                                ultra-plot 
                                ultra-projection
                                -ljpeg -lz -lm -lpthread
                                -L${PROJ4_HOME}/lib -lproj
                                -L${GDAL_HOME}/lib -lgdal 
                                -lrt)

target_include_directories(image-gverify PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/io/inc"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/data/inc"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/util/inc"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/algo/inc"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/image/inc"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/projection/inc"
                          "${PROJECT_SOURCE_DIR}/impl/ultra/plot/inc"
                          )
                          
