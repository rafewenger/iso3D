CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

#---------------------------------------------------------

SET(NRRD_LIBDIR "../../lib")

#---------------------------------------------------------

LINK_DIRECTORIES("${NRRD_LIBDIR}")
LINK_LIBRARIES(expat NrrdIO z)

# Possibly turn on usage of zlib compression (requires linking with libz)
# (i.e., programs compiled with ITKNrrdIO must also be compiled with zlib)
ADD_DEFINITIONS(-DTEEM_ZLIB=1)

# Turn on TEEM_BUILD so that the proper dll export def's are 
# used on windows builds.
ADD_DEFINITIONS(-DTEEM_BUILD=1)

ADD_LIBRARY(iso3D iso3D_command_line.cpp iso3D_error.cpp iso3D_grid.cpp
  iso3D_grid_nrrd.cpp iso3D_scalar_grid.cpp iso3D_string.cpp)
