file(GLOB SOURCES_chips "chips/*.cpp")
file(GLOB SOURCES_convolve "convolve/*.cpp")
file(GLOB SOURCES_correlation "correlation/*.cpp")
file(GLOB SOURCES_correlation_impl "correlation/impl/*.cpp")
file(GLOB SOURCES_correlation_impl_concurrency "correlation/impl/concurrency/*.cpp")
file(GLOB SOURCES_frequencyDomain "frequencyDomain/*.cpp")
file(GLOB SOURCES_frequencyDomain_impl "frequencyDomain/impl/*.cpp")
file(GLOB SOURCES_gcp "gcp/*.cpp")
file(GLOB SOURCES_gcp_internals "gcp/internals/*.cpp")
file(GLOB SOURCES_matrix "matrix/*.cpp")
file(GLOB SOURCES_modelling "modelling/*.cpp")
file(GLOB SOURCES_resample "resample/*.cpp")
file(GLOB SOURCES_transforms "transforms/*.cpp")

set (CMAKE_CXX_STANDARD 11)
add_library(ultra-algo ${SOURCES_chips} ${SOURCES_convolve} ${SOURCES_correlation} ${SOURCES_correlation_impl} ${SOURCES_correlation_impl_concurrency} ${SOURCES_frequencyDomain} ${SOURCES_frequencyDomain_impl} ${SOURCES_gcp} ${SOURCES_gcp_internals} ${SOURCES_matrix} ${SOURCES_modelling} ${SOURCES_resample}  ${SOURCES_transforms})

include_directories(ultra-io PUBLIC 
			  "../inc"
  			  "../../util/inc"
  			  "../../data/inc"
  			  "../../io/inc"
  			  "../../image/inc"
  			  "../../projection/inc"
  			  "../../plot/inc"
  			  "${EIGEN_HOME}/include/eigen3")
