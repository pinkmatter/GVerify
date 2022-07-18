file(GLOB SOURCES "*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-plot ${SOURCES})

include_directories(ultra-plot PUBLIC 
			  "../inc"
  			  "../../data/inc"
  			  "../../util/inc")
