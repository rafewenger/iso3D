/*!
 *  @file iso3D_types.h
 *  @brief Type definitions for iso3D.
 *  @authors "Rephael Wenger"
 *  @version 0.0.2
 */

/*
  ISO3D: 3D isosurface generation code.
  Copyright (C) 2024 Rephael Wenger

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _ISO3D_TYPES_H_
#define _ISO3D_TYPES_H_

#include <bitset>

#include "iso3D_const.h"

namespace ISO3D {

  //! @name Grid types.
  typedef int VERTEX_INDEX_TYPE;
  typedef VERTEX_INDEX_TYPE CUBE_INDEX_TYPE;
  typedef int AXIS_SIZE_TYPE;
  typedef int NUMBER_TYPE;
  typedef VERTEX_INDEX_TYPE GRID_COORD_TYPE;
  typedef GRID_COORD_TYPE CUBE_COORD_TYPE;

  //! @name Cube face types.
  typedef unsigned char CUBE_VERTEX_INDEX_TYPE;
  typedef unsigned char CUBE_FACET_INDEX_TYPE;
  typedef unsigned char CUBE_EDGE_INDEX_TYPE;
  typedef std::bitset<MAX_NUM_POLYTOPE_VERTICES> CUBE_VERTEX_BITSET;
  typedef std::bitset<MAX_NUM_POLYTOPE_FACETS> CUBE_FACET_BITSET;

  //! @name Deprecated types. (Types should end in INDEX.)
  typedef CUBE_VERTEX_INDEX_TYPE CUBE_VERTEX_INDEX;
  typedef CUBE_FACET_INDEX_TYPE CUBE_FACET_INDEX;
  typedef CUBE_EDGE_INDEX_TYPE CUBE_EDGE_INDEX;
  
  //! Bitset types
  typedef std::bitset<16> BOUNDARY_BITS_TYPE;
    
  //! Non-integer types
  typedef float SCALAR_TYPE;
  typedef float COORD_TYPE;

  //! Isosurface lookup table types.
  typedef int TABLE_INDEX_TYPE;
  
  //! Isosurface types
  typedef int ISO_VERTEX_INDEX_TYPE;
  
}

#endif

