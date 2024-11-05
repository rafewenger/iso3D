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
#include "iso3D_error.h"

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
     *  @pre icube is a valid cube index.
     *  @pre i < 8.
     *  - Note: Does NOT check that icube is a cube index,
     *    nor that i is less than 8.
     */
    inline VERTEX_INDEX CubeVertex
      (const CUBE_INDEX icube, const int i) const
    { return icube+cube_vertex_increment[i]; }

    /*!
     *  @brief Compute coordinates of vertex iv.
     *  @pre iv < NumVertices().
     */
    template <typename CTYPE>
    void ComputeCoord(const int iv, CTYPE vertex_coord[DIM3]) const;

    /*!
     *  @brief Compute index of vertex with coordinates vertex_coord[].
     *  @pre 0 <= vertex_coord[d] < AxisSize(d).
     */
    template <typename CTYPE>
    VERTEX_INDEX ComputeVertexIndex(CTYPE vertex_coord[DIM3]) const;
    

    // *** Check functions ***

    /*!
     *  @brief Return true if coordinate c is within bounds.
     *  - Return false and set error if coordinate is out of bounds.
     *  - Return true if 0 <= c < AxisSize(d).
     *  @param d Coordinate is on axis d.
     *  @param c Coordinate.
     */
    template <typename CTYPE>
    bool CheckVertexCoord(const int d, const CTYPE c, ERROR & error) const;

    /*!
     *  @brief Return true if cube coordinate c is within bounds.
     *  - Return false and set error if coordinate is out of bounds.
     *  - Return true if 0 <= c < AxisSize(d)-1.
     *  @param d Coordinate is on axis d.
     *  @param c Coordinate.
     */
    template <typename CTYPE>
    bool CheckCubeCoord(const int d, const CTYPE c, ERROR & error) const;

    /*!
     *  @overload
     *  @brief Return true if all cube coordinates are within bounds.
     *  - Check all cube coordinates.
     *  @param cube_coord[] Cube coordinates.
     *  @param error Error messages, if some cube coordinate is out of bounds.
     */
    template <typename CTYPE>
    bool CheckCubeCoord
    (const CTYPE cube_coord[DIM3], ERROR & error) const;
    
    /*!
     *  @brief Return true if cube invex is valid.
     */
    bool CheckCubeIndex(const CUBE_INDEX icube, ERROR & error) const;
    
    
    // *** Output functions - Mainly for debugging ***

    /*!
     *  @brief Output array a[].
     *  @tparam ETYPE Array element type.
     *  @param out Output stream.
     *  @param a[] Array.
     *  @param array_length Array length.
     */
    template <typename OSTREAM_TYPE, typename ETYPE>
    void OutArray
    (OSTREAM_TYPE & out, const ETYPE a[], const int array_length) const;
      
    /// @brief Output axis size.
    template <typename OSTREAM_TYPE>
    void OutAxisSize(OSTREAM_TYPE & out) const
    { OutArray(out, axis_size, Dimension()); }

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
    void OutAxisIncrement(OSTREAM_TYPE & out) const
    { OutArray(out, axis_increment, Dimension()); }

    /*!
     *  @overload
     *  @brief Output axis increment with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutAxisIncrement
    (OSTREAM_TYPE & out, const char * prefix, const char * suffix) const
    { out << prefix; OutAxisIncrement(out); out << suffix; }      

    /// @brief Output cube vertex increment.
    template <typename OSTREAM_TYPE>
    void OutCubeVertexIncrement(OSTREAM_TYPE & out) const
    { OutArray(out, cube_vertex_increment, CUBE3D::NumVertices()); }

    /*!
     *  @overload
     *  @brief Output cube vertex increment with preceding 
     *    and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutCubeVertexIncrement
    (OSTREAM_TYPE & out, const char * prefix, const char * suffix) const
    { out << prefix; OutCubeVertexIncrement(out); out << suffix; }

    /// @brief Output vertex coordinates.
    template <typename OSTREAM_TYPE>
    void OutVertexCoord(OSTREAM_TYPE & out, const int iv) const;

    /// @brief Output vertex index and vertex coordinates.
    template <typename OSTREAM_TYPE>
    void OutVertexIndexAndCoord(OSTREAM_TYPE & out, const int iv) const
    { out << iv << " "; OutVertexCoord(out, iv); }

    /*!
     *  @overload
     *  @brief Output vertex index and vertex coordinates
     *    with preceding and following string.
     */    
    template <typename OSTREAM_TYPE>
    void OutVertexIndexAndCoord
    (OSTREAM_TYPE & out, const char * prefix, const int iv,
     const char * suffix) const
    { out << prefix; OutVertexIndexAndCoord(out,iv); out << suffix; }
      
  };


  // *****************************************************************
  // GRID3D Member functions
  // *****************************************************************  

  template <typename CTYPE>
  void GRID3D::ComputeCoord(const int iv, CTYPE vertex_coord[DIM3]) const
  {
    int k = iv;
    for (int d = 0; d < DIM3; d++) {
      vertex_coord[d] = CTYPE(k % AxisSize(d));
      k = k / AxisSize(d);
    }
  }

  
  /*!
   *  @brief Compute index of vertex with coordinates vertex_coord[].
   *  @pre 0 <= vertex_coord[d] < AxisSize(d).
   */
  template <typename CTYPE>
  VERTEX_INDEX GRID3D::ComputeVertexIndex(CTYPE vertex_coord[DIM3]) const
  {
    PROCEDURE_ERROR error("GRID3D:ComputeVertexIndex");

    VERTEX_INDEX iv = 0;
    
    for (int d = 0; d < DIM3; d++) {
      if (!CheckVertexCoord(d, vertex_coord[d], error))
        { throw error; }

      iv += vertex_coord[d]*AxisIncrement(d);
    }

    return iv;
  }

  
  // *****************************************************************
  // GRID3D Check functions
  // *****************************************************************
  
  // Return true if coordinate c is within bounds.
  template <typename CTYPE>
  bool GRID3D::CheckVertexCoord
  (const int d, const CTYPE c, ERROR & error) const
  {
    if (c < 0) {
      error.AddToMessage("Error. Illegal ", d, "'th vertex coordinate ", c, ".");
      error.AddToMessage("  Vertex coordinates must be non-negative.");
      return false;
    }

    if (c >= AxisSize(d)) {
      error.AddToMessage("Error. Illegal ", d, "'th vertex coordinate ", c, ".");
      error.AddToMessage("  axis_size[", d, "] = ", AxisSize(d), ".");
      error.AddToMessage
        ("  Vertex coordinate must be less than axis size.");
      return false;
    }

    return true;
  }


  // Return true if cube coordinate c is within bounds.
  template <typename CTYPE>
  bool GRID3D::CheckCubeCoord(const int d, const CTYPE c, ERROR & error) const
  {
    if (c < 0) {
      error.AddToMessage("Error. Illegal ", d, "'th cube coordinate ", c, ".");
      error.AddToMessage("  Cube coordinates must be non-negative.");
      return false;
    }

    if (c+1 >= AxisSize(d)) {
      error.AddToMessage("Error. Illegal ", d, "'th cube coordinate ", c, ".");
      error.AddToMessage("  axis_size[", d, "] = ", AxisSize(d), ".");
      error.AddToMessage
        ("  Cube coordinate plus 1 must be less than axis size.");
      return false;
    }

    return true;
  }

    
  // Return true if cube coordinates are all within bounds.
  template <typename CTYPE>
  bool GRID3D::CheckCubeCoord
  (const CTYPE cube_coord[DIM3], ERROR & error) const
  {
    for (int d = 0; d < DIM3; d++) {
      if (!CheckCubeCoord(d, cube_coord[d], error))
        { return false; }
    }

    return true;
  }    


  // *****************************************************************
  // GRID3D Output functions - Mainly for debugging
  // *****************************************************************

  template <typename OSTREAM_TYPE, typename ETYPE>
  void GRID3D::OutArray
  (OSTREAM_TYPE & out, const ETYPE a[], const int array_length) const
  {
    out << "(";
    for (int i = 0; i < array_length; i++) {
      out << a[i];
      if (i+1 < array_length) { out << ","; }
    }
    out << ")";
  }

  // Output vertex coordinates.
  template <typename OSTREAM_TYPE>
  void GRID3D::OutVertexCoord(OSTREAM_TYPE & out, const int iv) const
  {
    GRID_COORD vertex_coord[DIM3];

    ComputeCoord(iv, vertex_coord);
    OutArray(out, vertex_coord, DIM3);
  }
  
}

#endif
