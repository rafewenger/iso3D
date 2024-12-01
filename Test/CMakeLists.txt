PROJECT(ISO3D)

#---------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# *** Change this to be the location of the directory containing libnrrdIO.a.
SET(NRRD_LIBDIR "../../ijkPublic/lib")

#---------------------------------------------------------

IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE Release CACHE STRING 
       "Default build type: Release" FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

INCLUDE_DIRECTORIES("..")
LINK_DIRECTORIES("${NRRD_LIBDIR}")
LINK_LIBRARIES(NrrdIO z)

# Possibly turn on usage of zlib compression (requires linking with libz)
# (i.e., programs compiled with ITKNrrdIO must also be compiled with zlib)
ADD_DEFINITIONS(-DTEEM_ZLIB=1)

# Turn on TEEM_BUILD so that the proper dll export def's are 
# used on windows builds.
ADD_DEFINITIONS(-DTEEM_BUILD=1)

ADD_COMPILE_OPTIONS(-Wall -Wno-unused-function)

SET(CMAKE_CPP_FLAGS "-std=c++17")
SET(CMAKE_CPP_FLAGS_RELEASE "-O3")


ADD_EXECUTABLE(test_error test_error.cpp ../iso3D_error.cpp)
ADD_EXECUTABLE(test_string test_string.cpp ../iso3D_string.cpp ../iso3D_error.cpp)
ADD_EXECUTABLE(test_cube test_cube.cpp ../iso3D_error.cpp)
ADD_EXECUTABLE(test_grid test_grid.cpp ../iso3D_grid.cpp ../iso3D_error.cpp)
ADD_EXECUTABLE(test_scalar_grid test_scalar_grid.cpp
                ../iso3D_grid.cpp ../iso3D_scalar_grid.cpp
                ../iso3D_error.cpp)
