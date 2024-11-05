/*!
 *  @file iso3D_grid.h
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


#ifndef _ISO3D_GRID_H_
#define _ISO3D_GRID_H_

#include "iso3D_const.h"
#include "iso3D_types.h"
#include "iso3D_cube.h"

namespace ISO3D {

  /// @brief Class representing a 3D grid.
  class GRID3D {

  protected:
    AXIS_SIZE_TYPE axis_size[DIM3];
    VERTEX_INDEX axis_increment[DIM3];
    VERTEX_INDEX cube_vertex_increment[CUBE3D::NumVertices()];
    VERTEX_INDEX num_vertices;
    CUBE_INDEX num_cubes;

    /// @brief Initialize data structure.
    void Init(const AXIS_SIZE_TYPE asize[DIM3]);

    /// @overload
    /// @brief Initialize data structure with axis size [0,0,0].
    void Init();
    
  public:
    /// @brief Constructor.
    GRID3D() { Init(); }
    GRID3D(const AXIS_SIZE_TYPE asize[DIM3])
      { Init(asize); }

    // *** Copy functions ***
    
    /// @brief Copy function.
    const GRID3D & Copy(const GRID3D & right);
    template <typename GTYPE2>

    /// @brief Copy operator.
    const GRID3D & operator = (const GRID3D & right)
    { return Copy(right); }

    
    // *** Set functions ***

    /// @brief Set axis size.
    void SetAxisSize(const AXIS_SIZE_TYPE axis_size[DIM3]);

    /// @brief Copy axis size from gridB.
    void CopyAxisSize(const GRID3D & gridB);


    // *** Get functions ***

    /// @brief Return grid dimension. (Always 3).
    constexpr int Dimension() const
    { return 3; }
    
    /// @brief Return number of vertices in grid.
    inline const VERTEX_INDEX NumVertices() const
    { return num_vertices; }

    /// @brief Return number of cube in grid.
    inline const VERTEX_INDEX NumCubes() const
    { return num_cubes; }

    /// @brief Return axis_size[d].
    inline int AxisSize(const int d) const
    { return axis_size[d]; }

    /// @brief Return axis_increment[d].
    inline VERTEX_INDEX AxisIncrement(const int d) const
    { return axis_increment[d]; }

    /*!
     *  @brief Return index of next vertex after iv in direction d.
     *  - Note: Does NOT check whether next vertex exists,
     *    i.e., does not check whether iv is on rightmost/topmost
     *    boundary of grid.
     */
    inline VERTEX_INDEX NextVertex(const VERTEX_INDEX iv, const int d) const
    { return iv + axis_increment[d]; }

    /*!
     *  @brief Return index of previous vertex befor iv in direction d.
     *  - Note: Does NOT check whether previous vertex exists,
     *    i.e., does not check whether iv is on leftmost/bottommost
     *    boundary of grid.
     */
    inline VERTEX_INDEX PrevVertex(const VERTEX_INDEX iv, const int d) const
    { return iv - axis_increment[d]; }

    /*!
     *  @brief Return i'th vertex of cube icube.
     *  - Note: Does NOT check that icube is a cube index,
     *    nor that i is less than 8.
     */
    inline VERTEX_INDEX CubeVertex
      (const CUBE_INDEX icube, const int i) const
    { return icube+cube_vertex_increment[i]; }

    
    // *** Output functions - Mainly for debugging ***

    /// @brief Output axis size.
    template <typename OSTREAM_TYPE>
    void OutAxisSize(OSTREAM_TYPE & out) const;

    /*!
     *  @overload
     *  @brief Output axis size with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutAxisSize
    (OSTREAM_TYPE & out, const char * prefix, const char * suffix) const
    { out << prefix; OutAxisSize(out); out << suffix; }

    /// @brief Output axis increment.
    template <typename OSTREAM_TYPE>
    void OutAxisIncrement(OSTREAM_TYPE & out) const;

    /*!
     *  @overload
     *  @brief Output axis increment with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutAxisIncrement
    (OSTREAM_TYPE & out, const char * prefix, const char * suffix) const
    { out << prefix; OutAxisIncrement(out); out << suffix; }      
    
  };



  // *****************************************************************
  // GRID3D Output functions - Mainly for debugging
  // *****************************************************************

  template <typename OSTREAM_TYPE>  
  void GRID3D::OutAxisSize(OSTREAM_TYPE & out) const
  {
    out << "(";
    for (int d = 0; d < Dimension(); d++) {
      out << axis_size[d];
      if (d+1 < Dimension())
        { out << ","; }
    }
    out << ")";
  }

    
  template <typename OSTREAM_TYPE>  
  void GRID3D::OutAxisIncrement(OSTREAM_TYPE & out) const
  {
    out << "(";
    for (int d = 0; d < Dimension(); d++) {
      out << axis_increment[d];
      if (d+1 < Dimension())
        { out << ","; }
    }
    out << ")";
  }
    
}

#endif
