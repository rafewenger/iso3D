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

    /// @brief axis_size[d] Number of grid vertices along axis d,
    ///   i.e., axis in direction d.
    AXIS_SIZE_TYPE axis_size[DIM3];

    /// @brief Add axis_increment[d] to vertex index to get next
    ///   vertex in direction d.
    VERTEX_INDEX_TYPE axis_increment[DIM3];

    /// @brief cube_vertex_increment[i] Increment to add
    ///   to cube_index to get vertex index of i'th cube vertex.
    VERTEX_INDEX_TYPE cube_vertex_increment[CUBE3D::NumVertices()];

    /// @brief facet_vertex_increment[i][j] Increment to add
    ///   to cube_index to get j'th vertex of i'th cube facet.
    VERTEX_INDEX_TYPE facet_vertex_increment
    [CUBE3D::NumFacets()][CUBE3D::NumVerticesPerFacet()];

    /// @brief spacing[d] Length of single grid edge along axis d.
    COORD_TYPE spacing[DIM3];
    
    /// @brief Total number of grid vertices.
    VERTEX_INDEX_TYPE num_vertices;

    /// @brief Total number of grid cubes.
    CUBE_INDEX_TYPE num_cubes;

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


    // ***************************************************************
    //! @name Copy functions
    // ***************************************************************            

    //@{
    
    /// @brief Copy function.
    const GRID3D & Copy(const GRID3D & right);

    /// @brief Copy operator.
    const GRID3D & operator = (const GRID3D & right)
    { return Copy(right); }

    //@}

    
    // ***************************************************************
    //! @name Set functions
    // ***************************************************************            

    //@{

    /// @brief Set axis size.
    void SetAxisSize(const AXIS_SIZE_TYPE axis_size[DIM3]);


    /// @brief Set spacing.
    void SetSpacing(const int d, COORD_TYPE spacing_d)
    { spacing[d] = spacing_d; }

    /*!
     *  @overload
     *  @brief Set spacing. (All 3 axes.)
     *  - Version setting spacing along all 3 axes.
     */
    template <typename CTYPE>
    void SetSpacing(const CTYPE _spacing[DIM3]);
    
    /// @brief Copy axis size from gridB.
    void CopyAxisSize(const GRID3D & gridB);

    //@}


    // ***************************************************************
    //! @name Get functions
    // ***************************************************************        

    //@{
    
    /// @brief Return grid dimension. (Always 3).
    constexpr int Dimension() const
    { return 3; }

    /// @brief Return number of grid facets. (Always 6).
    /// - Equals number of cube facets.
    constexpr int NumFacets() const
    { return CUBE3D::NumFacets(); }
    
    /// @brief Return number of vertices in grid.
    inline const VERTEX_INDEX_TYPE NumVertices() const
    { return num_vertices; }

    /// @brief Return number of cube in grid.
    inline const VERTEX_INDEX_TYPE NumCubes() const
    { return num_cubes; }

    /// @brief Return axis_size[d].
    inline int AxisSize(const int d) const
    { return axis_size[d]; }

    /// @brief Return axis_increment[d].
    inline VERTEX_INDEX_TYPE AxisIncrement(const int d) const
    { return axis_increment[d]; }

    /*!
     *  @brief Return index of next vertex after iv in direction d.
     *  - Note: Does NOT check whether next vertex exists,
     *    i.e., does not check whether iv is on rightmost/topmost
     *    boundary of grid.
     */
    inline VERTEX_INDEX_TYPE NextVertex
    (const VERTEX_INDEX_TYPE iv, const int d) const
    { return iv + axis_increment[d]; }

    /*!
     *  @brief Return index of previous vertex befor iv in direction d.
     *  - Note: Does NOT check whether previous vertex exists,
     *    i.e., does not check whether iv is on leftmost/bottommost
     *    boundary of grid.
     */
    inline VERTEX_INDEX_TYPE PrevVertex
    (const VERTEX_INDEX_TYPE iv, const int d) const
    { return iv - axis_increment[d]; }

    /*!
     *  @brief Return i'th vertex of cube icube.
     *  @pre icube is a valid cube index.
     *  @pre i < 8.
     *  - Note: Does NOT check that icube is a cube index,
     *    nor that i is less than 8.
     */
    inline VERTEX_INDEX_TYPE CubeVertex
      (const CUBE_INDEX_TYPE icube, const int i) const
    { return icube+cube_vertex_increment[i]; }

    /// @brief Return length of single grid edge along axis d.
    inline COORD_TYPE Spacing(const int d) const
    { return spacing[d]; }
    
    //@}
    

    // ***************************************************************
    //! @name Compute functions
    // ***************************************************************    

    //@{
    
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
    VERTEX_INDEX_TYPE ComputeVertexIndex(CTYPE vertex_coord[DIM3]) const;

    /*!
     *  @brief Compute and return the number of vertices in grid facet
     *    orthogonal to orth_dir.
     *  @param orth_dir Direction orthogonal to facet.
     *  @pre orth_dir is 0, 1, or 2.
     */
    int ComputeNumVerticesInGridFacet(const int orth_dir) const;

    /*!
     *  @brief Compute and return the number of cubes supported
     *    by the grid facet orthogonal to orth_dir.
     *  - Equivalent to the number of cubes that intersect 
     *    the grid facet in a cube facet.
     *  - Also equivalent to the number of squares
     *    in a grid facet orthogonal to orth_dir.
     *  @param orth_dir Direction orthogonal to facet.
     *  @pre orth_dir is 0, 1, or 2.
     */
    int ComputeNumCubesSupportedByGridFacet(const int orth_dir) const;

    /*!
     *  @brief Compute the coordinates of the cube center.
     */
    void ComputeCubeCenterCoord
    (const int icube, COORD_TYPE coord[DIM3]) const;

    /*!
     *  @brief Compute bits indicating which boundary grid facet (if any)
     *    contains vertex iv.
     *  -
     */
    void ComputeVertexBoundaryBits
    (const int iv, BOUNDARY_BITS_TYPE & boundary_bits) const;

    /*!
     *  @brief Compute bits indicating which boundary grid facet (if any)
     *    contains cube icube.
     *  -
     */
    void ComputeCubeBoundaryBits
    (const int icube, BOUNDARY_BITS_TYPE & boundary_bits) const;

    //@}
    

    // ***************************************************************
    //! @name Check functions
    // ***************************************************************    

    //@{
    
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
    bool CheckCubeIndex(const CUBE_INDEX_TYPE icube, ERROR & error) const;

    //@}
    

    // ***************************************************************
    //! @name Output functions - Mainly for debugging 
    // ***************************************************************

    //@{
    
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

    /// @brief Output spacing.
    /// - spacing[d] Length of single grid edge along axis d.
    template <typename OSTREAM_TYPE>
    void OutSpacing(OSTREAM_TYPE & out) const
    { OutArray(out, spacing, DIM3); }

    /*!
     *  @overload
     *  @brief Output spacing with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutSpacing
    (OSTREAM_TYPE & out, const char * prefix, const char * suffix) const      
    { out << prefix; OutSpacing(out); out << suffix; }
      
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


    /// @brief Output cube center coordinates.
    template <typename OSTREAM_TYPE>
    void OutCubeCenterCoord(OSTREAM_TYPE & out, const int icube) const;

    /*!
     *  @overload
     *  @brief Output cube center coordinates 
     *     with preceding and following string.
     */    
    template <typename OSTREAM_TYPE>
    void OutCubeCenterCoord
    (OSTREAM_TYPE & out, const char * prefix, const int icube,
     const char * suffix) const
    { out << prefix; OutCubeCenterCoord(out,icube); out << suffix; }

    /// @brief Output boundary bits.
    /// - Output bits from left to right, with boundary_bits[0]
    ///   being the leftmost bit.
    template <typename OSTREAM_TYPE>
    void OutBoundaryBits
    (OSTREAM_TYPE & out, const BOUNDARY_BITS_TYPE & boundary_bits) const;

    /// @brief Output vertex boundary bits.
    /// - Output bits from left to right, with bit 0
    ///   being the leftmost bit.    
    template <typename OSTREAM_TYPE>
    void OutVertexBoundaryBits
    (OSTREAM_TYPE & out, const int iv) const;

    /*!
     *  @overload
     *  @brief Output vertex boundary bits
     *    with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutVertexBoundaryBits
    (OSTREAM_TYPE & out, const char * prefix, const int iv,
     const char * suffix) const
    { out << prefix; OutVertexBoundaryBits(out, iv); out << suffix; }
    
    /// @brief Output cube boundary bits
    /// - Output bits from left to right, with bit 0
    ///   being the leftmost bit.    
    template <typename OSTREAM_TYPE>
    void OutCubeBoundaryBits
    (OSTREAM_TYPE & out, const int icube) const;

    /*!
     *  @overload
     *  @brief Output cube boundary bits
     *    with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutCubeBoundaryBits
    (OSTREAM_TYPE & out, const char * prefix, const int icube,
     const char * suffix) const
    { out << prefix; OutCubeBoundaryBits(out, icube); out << suffix; }
      
    //@}
      
  };


  // *****************************************************************
  // GRID3D Member functions
  // *****************************************************************  

  // Set spacing. (All 3 axes.)
  template <typename CTYPE>
  void GRID3D::SetSpacing(const CTYPE _spacing[DIM3])
  {
    spacing[0] = _spacing[0];
    spacing[1] = _spacing[1];
    spacing[2] = _spacing[2];
  }
  
  
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
  VERTEX_INDEX_TYPE GRID3D::ComputeVertexIndex(CTYPE vertex_coord[DIM3]) const
  {
    PROCEDURE_ERROR error("GRID3D:ComputeVertexIndex");

    VERTEX_INDEX_TYPE iv = 0;
    
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
    GRID_COORD_TYPE vertex_coord[DIM3];

    ComputeCoord(iv, vertex_coord);
    OutArray(out, vertex_coord, DIM3);
  }

    
  // Output cube center coordinates.
  template <typename OSTREAM_TYPE>
  void GRID3D::OutCubeCenterCoord
  (OSTREAM_TYPE & out, const int icube) const
  {
    COORD_TYPE cube_center_coord[DIM3];

    ComputeCubeCenterCoord(icube, cube_center_coord);
    OutArray(out, cube_center_coord, DIM3);
  }    


  // Output boundary bits.
  template <typename OSTREAM_TYPE>
  void GRID3D::OutBoundaryBits
  (OSTREAM_TYPE & out, const BOUNDARY_BITS_TYPE & boundary_bit) const
  {
    out << "(";
    for (int ifacet = 0; ifacet < NumFacets(); ifacet++)
      { out << boundary_bit[ifacet]; }
    out << ")";
  }


  // Output vertex boundary bits.
  template <typename OSTREAM_TYPE>
  void GRID3D::OutVertexBoundaryBits
  (OSTREAM_TYPE & out, const int iv) const
  {
    BOUNDARY_BITS_TYPE vertex_boundary_bit;
    ComputeVertexBoundaryBits(iv, vertex_boundary_bit);
    OutBoundaryBits(out, vertex_boundary_bit);
  }

    
  // Output cube boundary bits.
  template <typename OSTREAM_TYPE>
  void GRID3D::OutCubeBoundaryBits
  (OSTREAM_TYPE & out, const int icube) const
  {
    BOUNDARY_BITS_TYPE cube_boundary_bit;
    ComputeCubeBoundaryBits(icube, cube_boundary_bit);
    OutBoundaryBits(out, cube_boundary_bit);
  }    
    
}

#endif
