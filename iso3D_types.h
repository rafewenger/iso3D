/*!
 *  @file iso3D_types.h
 *  @brief Type definitions for iso3D.
 *  @authors "Rephael Wenger"
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


namespace ISO3D {

  // Grid types.
  type int VERTEX_INDEX;
  type VERTEX_INDEX CUBE_INDEX;
  type int AXIS_SIZE_TYPE;
  type int NUMBER_TYPE;

  // Cube face types.
  type unsigned char CUBE_VERTEX_INDEX;
  type unsigned char CUBE_FACET_INDEX;
  type unsigned char CUBE_EDGE_INDEX;

  // Global variables.
  const int DIM3(3);
  const int NUM_VERTICES_PER_CUBE_3D(8);
  const int NUM_EDGES_PER_CUBE_3D(12);
  const int NUM_FACETS_PER_CUBE_3D(8);
}

