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


// Compute and return the number of vertices in grid facet.
int GRID3D::ComputeNumVerticesInGridFacet(const int orth_dir) const
{
  const int d1 = (orth_dir+1)%DIM3;
  const int d2 = (orth_dir+1)%DIM3;

  return (AxisSize(d1)*AxisSize(d2)); 
}


// Compute and return the number of rectangles in grid facet.
int GRID3D::ComputeNumRectanglesInGridFacet(const int orth_dir) const
{
  const int d1 = (orth_dir+1)%DIM3;
  const int d2 = (orth_dir+1)%DIM3;

  if ((d1 == 0) || (d2 == 0)) { return 0; }

  return ((AxisSize(d1)-1)*(AxisSize(d2)-1));
}


// Compute the coordinates of the cube center.
void GRID3D::ComputeCubeCenterCoord
(const int icube, COORD_TYPE coord[DIM3]) const
{
  ComputeCoord(icube, coord);
  coord[0] += 0.5;
}



// Compute bits indicating which boundary grid facet (if any)
void GRID3D::ComputeVertexBoundaryBits
(const int iv, BOUNDARY_BITS_TYPE & boundary_bits) const
{
  boundary_bits.reset();
  VERTEX_INDEX k = iv;
  for (int d = 0; d < DIM3; d++) {
    const GRID_COORD c = (k % AxisSize(d));
    k = k / AxisSize(d);
    
    if (c == 0) { boundary_bits.set(d); }
    if ((c+1) >= axis_size[d]) { boundary_bits.set(d+DIM3); }
  }
}


// Compute bits indicating which boundary grid facet (if any)
void GRID3D::ComputeCubeBoundaryBits
(const int icube, BOUNDARY_BITS_TYPE & boundary_bits) const
{
  boundary_bits.reset();
  VERTEX_INDEX k = icube;
  for (int d = 0; d < DIM3; d++) {
    const GRID_COORD c = (k % AxisSize(d));
    k = k / AxisSize(d);

    if (c == 0) { boundary_bits.set(d); }
    if ((c+2) >= axis_size[d]) { boundary_bits.set(d+DIM3); }
  }
}


// Return true if cube invex is valid.
bool GRID3D::CheckCubeIndex(const CUBE_INDEX icube, ERROR & error) const
{
  CUBE_COORD cube_coord[DIM3];

  ComputeCoord(icube, cube_coord);
  return CheckCubeCoord(cube_coord, error);
}
  
