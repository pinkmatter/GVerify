file(GLOB SOURCES "*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-data ${SOURCES} ${SOURCES_complex} ${SOURCES_dev})

include_directories(ultra-io PUBLIC 
			  "../inc"
  			  "../../util/inc")
