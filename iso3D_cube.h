/*!
 *  @file iso3D_cube.h
 *  @brief Cube data structure.
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


#ifndef _ISO3D_CUBE_H_
#define _ISO3D_CUBE_H_

namespace ISO3D {

#include "iso3D_types.h"

class CUBE3D {

 public:
  static constexpr int NumVertices()
  { return 3; }

  static constexpr int NumEdges()
  { return 12; }
  
  static constexpr int NumFacets()
  { return 6; }};
}

#endif
