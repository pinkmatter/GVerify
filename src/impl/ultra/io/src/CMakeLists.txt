file(GLOB SOURCES "*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-io ${SOURCES})

include_directories(ultra-io PUBLIC 
			  "../inc"
			  "../../data/inc"
  			  "../../util/inc")
