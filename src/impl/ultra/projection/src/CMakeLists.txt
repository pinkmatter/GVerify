file(GLOB SOURCES "*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-projection ${SOURCES})

include_directories(ultra-projection PUBLIC 
			  "../inc"
  			  "../../data/inc"
  			  "../../util/inc"
  			  "${PROJ4_HOME}/include")
