/*!
 *  @file iso3D_const.h
 *  @brief ISO3D global constants
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


#ifndef _ISO3D_CONST_H_
#define _ISO3D_CONST_H_

namespace ISO3D {
  
  // Global variables.
  const int DIM3(3);
  const int NUM_VERTICES_PER_TRIANGLE(3);

  /*!
   *  @brief Maximum number of polytope vertices and facets.
   *  - Should be a power of 2 since this is used to define bitsets.
   *  - Note: Even if number of polytope vertices is less than
   *    or equal to MAX_NUM_POLYTOPE_VERTICES, there may not be
   *    enough memory to store the isosurface table.
   */
  const int MAX_NUM_POLYTOPE_VERTICES(32);
  const int MAX_NUM_POLYTOPE_FACETS(16);
}

#endif

