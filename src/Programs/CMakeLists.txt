CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

#---------------------------------------------------------

SET(NRRD_LIBDIR "../../lib")

#---------------------------------------------------------

IF (NOT CMAKE_BUILD_TYPE)
  SET (CMAKE_BUILD_TYPE Release CACHE STRING 
       "Default build type: Release" FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

LINK_DIRECTORIES("${NRRD_LIBDIR}")
LINK_LIBRARIES(expat NrrdIO z)

# Possibly turn on usage of zlib compression (requires linking with libz)
# (i.e., programs compiled with ITKNrrdIO must also be compiled with zlib)
ADD_DEFINITIONS(-DTEEM_ZLIB=1)

# Turn on TEEM_BUILD so that the proper dll export def's are 
# used on windows builds.
ADD_DEFINITIONS(-DTEEM_BUILD=1)

ADD_COMPILE_OPTIONS(-Wall -Wno-unused-function)

# Include random library.
SET(CMAKE_CPP_FLAGS "-std=c++17")
SET(CMAKE_CPP_FLAGS_RELEASE "-O3")

ADD_EXECUTABLE(iso3D_nrrd_info iso3D_nrrd_info.cpp)

ADD_EXECUTABLE(iso3D_MCtable_info iso3D_MCtable_info.cpp)

ADD_EXECUTABLE(iso3D_scalar iso3D_scalar.cpp)

TARGET_LINK_LIBRARIES(iso3D_nrrd_info PUBLIC iso3D)
TARGET_INCLUDE_DIRECTORIES(iso3D_nrrd_info PUBLIC ../iso3D)

TARGET_LINK_LIBRARIES(iso3D_MCtable_info PUBLIC iso3D iso3D_MCtable)
TARGET_INCLUDE_DIRECTORIES(iso3D_MCtable_info PUBLIC ../iso3D ../iso3D_MCtable)

TARGET_LINK_LIBRARIES(iso3D_scalar PUBLIC iso3D)
TARGET_INCLUDE_DIRECTORIES(iso3D_scalar PUBLIC ../iso3D)


