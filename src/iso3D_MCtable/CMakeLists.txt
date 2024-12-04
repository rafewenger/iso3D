CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

INCLUDE_DIRECTORIES( ../iso3D )

ADD_LIBRARY(iso3D_MCtable iso3D_MCtable.cpp iso3D_MCtable_poly.cpp
  iso3D_MCtable_xitIO.cpp)

  
#ADD_LIBRARY(iso3D_MCtable iso3D_MCtable_orient.cpp)

TARGET_INCLUDE_DIRECTORIES(iso3D_MCtable PUBLIC ,./iso3D )

