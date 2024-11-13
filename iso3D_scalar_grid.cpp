/*!
 *  @file iso3D_scalar_grid.cpp
 *  @brief Scalar grid class.
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


#include "iso3D_scalar_grid.h"

using namespace ISO3D;

// *** DEBUG ***
#include <iostream>


void SCALAR_GRID3D_BASE::Init()
{
  scalar = NULL;
}


void SCALAR_GRID3D::Init()
{
  if (this->NumVertices() > 0) {
    scalar = new SCALAR_TYPE[this->NumVertices()];
  }
}


void SCALAR_GRID3D::FreeAll()
{
  if (scalar != NULL) {
    delete [] scalar;
    scalar = NULL;
  }
}


/* OBSOLETE
// Set axis size.
// - Reallocate scalar[] with change in number of vertices.
void SCALAR_GRID3D::SetAxisSize(const AXIS_SIZE_TYPE asize[DIM3])
{
  const NUMBER_TYPE old_num_vertices = this->NumVertices();
  
  GRID3D::SetAxisSize(asize);

  if (old_num_vertices != this->NumVertices()) {
    FreeAll();

    if (this->NumVertices() > 0) {
      scalar = new SCALAR_TYPE[this->NumVertices()];
    }
  }
  
}
*/
