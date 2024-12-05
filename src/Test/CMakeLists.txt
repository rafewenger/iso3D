CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

PROJECT(ISO3D_TEST)

#---------------------------------------------------------

SET(LIBDIR "../../lib")

#---------------------------------------------------------

IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE Release CACHE STRING 
       "Default build type: Release" FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

INCLUDE_DIRECTORIES("../iso3D" "../iso3D_MCtable")
LINK_DIRECTORIES("${LIBDIR}")
LINK_LIBRARIES(expat NrrdIO z)

# Iso3D link libraries
LINK_LIBRARIES("iso3D" "iso3D_MCtable")
  

# Possibly turn on usage of zlib compression (requires linking with libz)
# (i.e., programs compiled with ITKNrrdIO must also be compiled with zlib)
ADD_DEFINITIONS(-DTEEM_ZLIB=1)

# Turn on TEEM_BUILD so that the proper dll export def's are 
# used on windows builds.
ADD_DEFINITIONS(-DTEEM_BUILD=1)

ADD_COMPILE_OPTIONS(-Wall -Wno-unused-function)

SET(CMAKE_CPP_FLAGS "-std=c++17")
SET(CMAKE_CPP_FLAGS_RELEASE "-O3")

ADD_EXECUTABLE(test_error test_error.cpp)
ADD_EXECUTABLE(test_string test_string.cpp)
ADD_EXECUTABLE(test_cube test_cube.cpp)
ADD_EXECUTABLE(test_grid test_grid.cpp)
ADD_EXECUTABLE(test_scalar_grid test_scalar_grid.cpp)
