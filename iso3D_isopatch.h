/*!
 *  @file iso3D_isopatch.h
 *  @brief Routines for extracting an isosurface patch from a cube.
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


#ifndef _ISO3D_ISOPATCH_H_
#define _ISO3D_ISOPATCH_H_

#include "iso3D_const.h"
#include "iso3D_cube.h"
#include "iso3D_types.h"


namespace ISO3D {

  /*!
   *  @brief Return true if isosurface intersects cube.
   *  @param icube Cube index.
   *    @pre Must be a valid cube index.
   *    - The cube index equals the index of the leftmost/bottommost 
   *      vertex of the cube.
   *    - A cube index CANNOT equal the index of some vertex
   *      on the top/right facet of the grid.
   */
  inline bool intersects_cube
  (const SCALAR_GRID3D & scalar_grid, const SCALAR_TYPE isovalue,
   const CUBE_INDEX_TYPE icube)
  {
    const VERTEX_INDEX_TYPE iv0 = scalar_grid.CubeVertex(icube, 0);
    if (scalar_grid.Scalar(iv0) < isovalue) {
      for (int j = 1; j < CUBE3D::NumVertices(); j++) {
        const VERTEX_INDEX_TYPE iv1 =
          scalar_grid.CubeVertex(icube, j);
        if (scalar_grid.Scalar(iv1) >= isovalue)
          { return true; };
      }
    }
    else {
      // scalar_grid.Scalar(iv1) >= isovalue
      for (int j = 1; j < CUBE3D::NumVertices(); j++) {
        const VERTEX_INDEX_TYPE iv1 =
          scalar_grid.CubeVertex(icube, j);
        if (scalar_grid.Scalar(iv1) < isovalue)
          { return true; };
      }
    }

    return(false);
  }

}

#endif
