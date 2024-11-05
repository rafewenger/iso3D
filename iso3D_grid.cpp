/*!
 *  @file iso3D_grid.cpp
 *  @brief Grid class.
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


#include "iso3D_grid.h"

using namespace ISO3D;

void GRID3D::Init(const AXIS_SIZE_TYPE asize[DIM3])
{
  CUBE3D cube;
  
  // Initialize.
  num_vertices = 1;
  num_cubes = 1;
  
  for (int d = 0; d < DIM3; d++) {
    axis_size[d] = asize[d];
    num_vertices = num_vertices * asize[d];
    
    if (asize[d] > 0)
      { num_cubes = num_cubes * (asize[d]-1); }
    else
      { num_cubes = 0; }
  }

  axis_increment[0] = 1;
  for (int d = 1; d < DIM3; d++) {
    axis_increment[d] = axis_increment[d-1]*asize[d-1];
  }

  for (int i = 0; i < CUBE3D::NumVertices(); i++) {
    VERTEX_INDEX increment = 0;
    for (int d = 0; d < Dimension(); d++) {
      increment += axis_increment[d] * cube.VertexCoord(i, d);
    }
    cube_vertex_increment[i] = increment;
  }
  
}


void GRID3D::Init()
{
  for (int d = 0; d < DIM3; d++) {
    axis_size[d] = 0;
    axis_increment[d] = 0;
  }

  num_vertices = 0;
  num_cubes = 0;

  for (int i = 0; i < CUBE3D::NumVertices(); i++)
    { cube_vertex_increment[i] = 0; }
}

