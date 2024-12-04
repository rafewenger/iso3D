/*!
 *  @file iso3D_MCtable.h
 *  @brief Marching cubes lookup table.
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


#ifndef _ISO3D_MC_TABLE_H_
#define _ISO3D_MC_TABLE_H_

#include <bitset>

#include "iso3D_const.h"
#include "iso3D_types.h"
#include "iso3D_cube.h"
#include "iso3D_error.h"
#include "iso3D_MCtable_poly.h"
#include "iso3D_MCtable_properties.h"
#include "iso3D_templates.h"
#include "iso3D_types.h"

namespace ISO3D {

  // ***************************************************************
  // ISOSURFACE VERTEX
  // ***************************************************************

  /// Isosurface vertex class.
  class ISOSURFACE_VERTEX {

  public:
    typedef enum {VERTEX, EDGE, FACET, POINT} ISOSURFACE_VERTEX_TYPE;
    typedef float COORD_TYPE;

  protected:
    ISOSURFACE_VERTEX_TYPE vtype;
    int face;
    COORD_TYPE coord[DIM3];
    SET_VALUE<std::string> label;

  public:
    ISOSURFACE_VERTEX(){};        //< Constructor
    ~ISOSURFACE_VERTEX();         ///< Destructor.

    // Get Functions

    /// @brief Return isosurface vertex type.
    ISOSURFACE_VERTEX_TYPE Type() const { return(vtype); };

    /// @brief Return index of face (vertex, edge, facet)
    ///   containing isosurface.
    /// - Defined only if vertex type is VERTEX, EDGE or FACET.
    int Face() const { return face; };

    /// @brief Return coordinate of isosurface vertex.
    /// - Defined only if vertex type is POINT.
    COORD_TYPE Coord(const int d) const
    { return coord[d]; };

    /// Return number of isosurface vertex coordinates.
    int NumCoord() const { return DIM3; };

    /// @brief Return label of isosurface vertex.
    /// - Used for extending vertex types.
    std::string Label() const { return label.Value(); };

    /// Return true if label is set.
    bool IsLabelSet() const { return label.IsSet(); };

    // Set Functions
    void SetType(const ISOSURFACE_VERTEX_TYPE t) { vtype = t; };
    void SetFace(const int index) { face = index; };
    void SetCoord(const int ic, const COORD_TYPE c) 
    { coord[ic] = c; };
    void SetLabel(const std::string & s) 
    { label.Set(s); };
  };


  // ***************************************************************
  // MARCHING CUBES TABLE
  // ***************************************************************

  /*!
   *  @brief Isosurface lookup table.
   *  - Stores isosurface patches for each configuration
   *    of +/- labels at polyhedron vertices.
   */
  class MC_TABLE {

  public:

    /// @brief Type of index of vertex in isosurface patch.
    typedef int ISOPATCH_VERTEX_INDEX_TYPE;

    /*!
     *  @brief Size of bitset used to represent isosurface vertices
     *    in isotable entries.
     *  - Note: Number of isosurface vertices can be 
     *    at most ISO_VERTEX_BITSET_SIZE.
     *  - Note: Isosurface vertex bitsets are used in orienting
     *    isosurface table entries.
     */
    static const int ISO_VERTEX_BITSET_SIZE = 64;

    typedef std::bitset<ISO_VERTEX_BITSET_SIZE> ISO_VERTEX_BITSET;
    
  protected:

    /// Entry in the isosurface lookup table.
    class MC_TABLE_ENTRY {

    public:
      int num_simplices;
      ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list;
      MC_TABLE_ENTRY();                  // constructor
      ~MC_TABLE_ENTRY();                 // destructor

      bool Check(ERROR & error_msg) const;
      void FreeAll();                            // free all memory
    };


  protected:

    /// @brief Marching cubes isosurface table properties.
    MC_TABLE_PROPERTIES table_properties;

    MC_TABLE_POLY polytope;      ///< Mesh polytope.
    int simplex_dimension;       ///< Simplex dimension.
    
    /// Array of isosurface vertex descriptors.
    ISOSURFACE_VERTEX * isosurface_vertex;

    /// Number of vertices in array isosurface_vertex[].
    int num_isosurface_vertices;

    /// Array of isosurface table entries.
    MC_TABLE_ENTRY * entry;

    /// Number of entries in table.
    TABLE_INDEX_TYPE num_table_entries;         

    /// True, if array num_table_entries[] is allocated.
    bool is_table_allocated;  

    /// @brief Check if isosurface vertices are allocated.
    /// - Throw error if not enough isosurface vertices are allocated.
    void CheckIsoVerticesAlloc
    (const char * procname, const int vstart, const int numv);

    /// Initialization routine.
    void Init(const int dimension, const int simplex_dimension);


  public:

    /// @name Constructors and destructors.
    ///@{
    
    MC_TABLE();                 ///< Constructor
    ~MC_TABLE();                ///< Destructor

    ///@}

    
    /// @name Get Functions
    ///@{

    /// @brief Return table encoding.
    ENCODING Encoding() const 
    { return table_properties.encoding; };

    /// @brief Return string for table encoding.
    std::string EncodingName() const 
      { return table_properties.EncodingString(Encoding()); };

    /// @overload
    /// @brief Return string for encoding _encoding.
    std::string EncodingName(const ENCODING _encoding) const
      { return table_properties.EncodingString(_encoding); };

    /// Return isosurface table properties.
    const MC_TABLE_PROPERTIES & Properties() const
    { return table_properties; }

    /// Return polytope dimension.
    int Dimension() const { return polytope.Dimension() ; };

    /// Return isosurface simplex dimension.
    int SimplexDimension() const { return simplex_dimension; };

    /// Return true if lookup table represents an interval volume.
    bool IsIntervalVolume() const
    { return (Dimension() == SimplexDimension()); }
    
    /// Return number of vertices in each isosurface simplex.
    int NumVerticesPerSimplex() const
    { return (SimplexDimension()+1); };

    /// @brief Return number of isosurface vertices
    ///   in array isosurface_vertex[].
    int NumIsosurfaceVertices() const
    { return num_isosurface_vertices; };

    /// Return number of lookup table entries.
    int NumTableEntries() const { return num_table_entries; };

    /// Access isosurface table polytope.
    const MC_TABLE_POLY_BASE & Polytope() const
    { return polytope; };

    /// Return polytope shape.
    POLYTOPE_SHAPE PolyShape() const
    { return Polytope().Shape(); }

    /// Access i'th isosurface vertex.
    const ISOSURFACE_VERTEX & IsosurfaceVertex(const int i) const
    { return isosurface_vertex[i]; }; 

    /// @brief Return number of simplices in isosurface patch
    ///   for table entry \a it.
    int NumSimplices(const TABLE_INDEX_TYPE it) const
    { return entry[it].num_simplices; }; 

    /*!
     *  @brief Return \a k'th vertex of isosurface simplex \a is, 
     *    table entry \a it.
     *  @param it Index of table entry.
     *  @param is Simplex \a is of table entry \a it.
     *  @param k Return \a k'th vertex of simplex \a is.
     */
    int SimplexVertex
    (const TABLE_INDEX_TYPE it, const int is, const int k) const
    { return(entry[it].simplex_vertex_list[is*NumVerticesPerSimplex()+k]); };

    /*!
     *  @brief Return pointer to vertices of all simplices
     *    in table entry table_index.
     */
    const ISOPATCH_VERTEX_INDEX_TYPE *
    SimplexVertices(const TABLE_INDEX_TYPE table_index) const
    { return entry[table_index].simplex_vertex_list; }
      
    /*!
     *  @brief Return pointer to vertices of simplex isimplex
     *    in table entry table_index.
     */
    const ISOPATCH_VERTEX_INDEX_TYPE *
    SimplexVertices(const TABLE_INDEX_TYPE table_index,
                    const int isimplex) const
    { return (entry[table_index].simplex_vertex_list +
              NumVerticesPerSimplex()*isimplex); }

    /// Return true if table memory is allocated.
    bool IsTableAllocated() const
    { return(is_table_allocated); };

    /// @brief Return opposite separation type.
    ISOSURFACE_SEPARATION_TYPE OppositeSeparationType() const
    { return Properties().OppositeSeparationType(); }

    /// @brief Return orientation of isosurface polytopes.
    ISO_POLY_ORIENTATION IsoPolyOrientation() const
    { return Properties().IsoPolyOrientation(); }

    /// @brief Return opposite orientation of isosurface polytopes.
    ISO_POLY_ORIENTATION OppositeIsoPolyOrientation() const
    { return Properties().OppositeIsoPolyOrientation(); }

    /// Return base of isotable encoding.
    int Base() const
    {
      if (Encoding() == BASE3) { return 3; }
      else { return 2; }
    }
      
    /*!
     *  @brief Value representing negative "-" label in isosurface table
     *    with BINARY encoding.
     *  - convert2base converts table index to array of digits,
     *    each one representing label of a vertex.
     *  - BinaryNegative() is the digit indicating a negative label.
     */
    constexpr int BinaryNegative() const { return 0; }

    /// @brief Value representing positive "+" label in isosurface table
    ///   with BINARY encoding.
    constexpr int BinaryPositive() const { return 1; }

    /*!
     *  @brief Value representing negative "-" label in isosurface table
     *    with BASE3 encoding.
     *  - convert2base converts table index to array of digits,
     *    each one representing label of a vertex.
     *  - Base3Negative() is the digit indicating a negative label.
     */
    constexpr int Base3Negative() const { return 0; }

    /// @brief Value representing positive "+" label in isosurface table
    ///   with BASE3 encoding.
    constexpr int Base3Positive() const { return 2; }
    
    /// @brief Value representing positive "=" label in isosurface table
    ///   with BASE3 encoding.
    /// - For interval volumes, "=" represents vertex in volume interior.
    constexpr int Base3Equals() const { return 1; }

    /*!
     *  @brief Return value representing negative label.
     *  - convert2base converts table index to array of digits,
     *    each one representing label of a vertex.
     *  - NegativeLabelValue() returns digit indicate a negative label.
     */
    int NegativeLabelValue() const
    {
      if (Encoding() == BASE3) { return Base3Negative(); }
      else { return BinaryNegative(); }
    }

    /*!
     *  @brief Return value representing positive label.
     *  - convert2base converts table index to array of digits,
     *    each one representing label of a vertex.
     *  - PositiveLabelValue() returns digit indicating a positive label.
     */
    int PositiveLabelValue() const
    {
      if (Encoding() == BASE3) { return Base3Positive(); }
      else { return BinaryPositive(); }
    }

    /*!
     *  @brief Return true if facet vertex labels are identical
     *    in table entries table_indexA and table_indexB.
     */
    bool AreAllFacetVertexLabelsIdentical
    (const TABLE_INDEX_TYPE table_indexA,
     const TABLE_INDEX_TYPE table_indexB,
     const int ifacet) const;

    ///@}

    
    /// @name Set Polytope Functions
    ///@{
    
    void SetDimension(const int d) { polytope.SetDimension(d); };
    void SetPolyShape(const POLYTOPE_SHAPE & shape)
    { polytope.SetShape(shape); }
    void SetNumPolyVertices(const int numv) 
    { polytope.SetNumVertices(numv); };
    void SetNumPolyEdges(const int nume)
    { polytope.SetNumEdges(nume); };
    void SetNumPolyFacets(const int numf)
    { polytope.SetNumFacets(numf); };
    void SetPolySize(const int numv, const int nume, const int numf)
    { SetNumPolyVertices(numv); SetNumPolyEdges(nume); 
      SetNumPolyFacets(numf); };
    void SetPolyVertexCoord(const int iv, const int ic, const int coord)
    { polytope.SetVertexCoord(iv, ic, coord); };
    // Note: SetNumPolyVertices or SetPolySize must be called before 
    //   SetPolyVertexCoord
    void SetPolyEdge(const int ie, const int iv0, const int iv1)
    { polytope.SetEdge(ie, iv0, iv1); };
    // Note: SetNumPolyEdges or SetPolySize must be called
    //   before SetPolyEdge
    void SetPolyNumFacetVertices(const int jf, const int numv)
    { polytope.SetNumFacetVertices(jf, numv); }
    void SetPolyFacetVertex(const int jf, const int k, const int iv)
    { polytope.SetFacetVertex(jf, k, iv); };
    // Note: SetPolyNumFacetVertices must be called
    //   before SetPolyFacetVertex

    /// @brief Set polytope to _poly.
    void Set(const MC_TABLE_POLY_BASE & _poly)
    { this->polytope = _poly; };

    ///@}

    /// @name Set Isosurface Vertices Functions
    ///@{
    void SetNumIsosurfaceVertices(const int num_vertices);
    void SetIsoVertexType(const int i, 
                          const ISOSURFACE_VERTEX::ISOSURFACE_VERTEX_TYPE t) 
    { isosurface_vertex[i].SetType(t); };
    void SetIsoVertexFace(const int i, const int index) 
    { isosurface_vertex[i].SetFace(index); };
    void SetIsoVertexCoord(const int i, 
                           const int ic, const COORD_TYPE c) 
    { isosurface_vertex[i].SetCoord(ic, c); };
    void SetIsoVertexLabel(const int i, const std::string & s) 
    { isosurface_vertex[i].SetLabel(s); };

    /// @brief Set isosurface vertex.
    /// @pre SetNumIsosurfaceVertices() has already been called.
    void SetIsosurfaceVertex
    (const int iv, const ISOSURFACE_VERTEX & isosurface_vertex);
    
    /// @brief Copy isosurface vertices from isotable.
    void CopyIsosurfaceVertices(const MC_TABLE & isotable);
                                
    // store polytope vertices, edges or faces as isosurface vertices
    void StorePolyVerticesAsIsoVertices(const int vstart);
    void StorePolyEdgesAsIsoVertices(const int vstart);
    void StorePolyFacetsAsIsoVertices(const int vstart);
    ///@}

    /// @name Set Isosurface Table Functions
    ///@{
    void SetSimplexDimension(const int d)
    { this->simplex_dimension = d; };
    void SetEncoding(const ENCODING encoding);
    void SetBinaryEncoding() { SetEncoding(BINARY); };
    void SetBase3Encoding() { SetEncoding(BASE3); };
    void SetEncoding(const std::string & encoding_str) 
    { table_properties.SetEncoding(encoding_str); }

    virtual void SetNumTableEntries(const int num_table_entries);
    void SetNumSimplices(const TABLE_INDEX_TYPE it, const int nums);

    /// @brief Set k'th vertex of simplex is in table entry it to isov.
    void SetSimplexVertex
    (const TABLE_INDEX_TYPE it, const int is, const int k,
     const int isov);

    /// @brief Set simplex vertices.
    void SetSimplexVertices
    (const TABLE_INDEX_TYPE it,
     const ISOPATCH_VERTEX_INDEX_TYPE simplex_vertices[],
     const int num_simplices);

    void SetTableType(const LOOKUP_TABLE_TYPE lookup_table_type);
    void SetGridVertexLabelType
      (const GRID_VERTEX_LABEL_TYPE grid_vertex_label_type);
    void SetGridVertexLabelType(const std::string s)
    { table_properties.SetGridVertexLabelType(s); }
    void SetSeparationType
      (const ISOSURFACE_SEPARATION_TYPE separation_type);
    void SetSeparationType(const std::string s)
    { table_properties.SetSeparationType(s); }
    void SetTriangulationType
      (const ISOSURFACE_TRIANGULATION_TYPE triangulation_type);
    void SetTriangulationType(const std::string s)
    { table_properties.SetTriangulationType(s); }
    void SetIsoPolyOrientation
      (const ISO_POLY_ORIENTATION iso_poly_orientation);
    void SetIsoPolyOrientation(const std::string s)
    { table_properties.SetIsoPolyOrientation(s); }
    void SetSeparateOpposite
      (const SEPARATE_OPPOSITE_TYPE separate_opposite);
    void SetSeparateOpposite(const std::string s)
    { table_properties.SetSeparateOpposite(s); }
    void SetSeparateOpposite(const bool flag);

    ///@}

    /// @name Copy Isosurface Table Functions
    ///@{

    /// @brief Copy polytope from isotable.
    void CopyPolytope(const MC_TABLE & isotable)
    { Set(isotable.Polytope()); }

    /// @brief Copy isosurface table properties from isotable.
    void CopyProperties(const MC_TABLE & isotable)
    { table_properties.Copy(isotable.Properties()); }

    ///@}

    /// @name Generate Cube Function
    ///@{
    void GenCube3D() 
    { polytope.GenCube3D(); };
    ///@}

    /// @name Process Simplex Orientations
    ///@{

    /*!
     *  @brief Sort vertices of simplex isimplex in increasing order.
     *  - Used to orient initial simplex for setting table orientation.
     */
    void SortSimplexVertices
    (const TABLE_INDEX_TYPE it, const int isimplex);
    
    /*!
     *  @brief Flip isosurface polytope orientations from +1 to -1 
     *    or from -1 to +1.
     *  - Changes simplx orientation by swapping last two vertices
     *    in simplex.
     *  @param it Table entry index.  In range [0..NumTableEntries()-1].
     *  @param ipoly Polytope index.
     */
    void FlipIsoPolyOrientation
    (const TABLE_INDEX_TYPE it, const int ipoly);

    /*!
     *  @brief Flip all isosurface polytope orientations 
     *    at table entry table_index.
     *  - Changes simplex orientation by swapping last two vertices
     *    in simplex.
     *  @param it Table entry index.  In range [0..NumTableEntries()-1].
     */
    void FlipAllIsoPolyOrientations(const TABLE_INDEX_TYPE table_index);

    /*!
     *  @brief Flip all isosurface polytope orientations 
     *    in isosurface lookup table.
     *  - Changes simplex orientation by swapping last two vertices
     *    in simplex.
     *  - Reverses iso_table_orientation property, if that property
     *    is set.
     */
    void FlipAllIsoPolyOrientations();

    /*!
     *  @brief Orient simplices in table entry.
     *  - Note: Only simplices in same connected component as
     *    simplex istart are oriented.
     *  @param table_index Index of table entry.
     *  @param istart Index of starting simplex.
     *    @pre istart is in range [0..(NumSimplices(it)-1].
     */
    void OrientSimplicesInTableEntry
    (const TABLE_INDEX_TYPE table_index,
     const TABLE_INDEX_TYPE istart);

    /*!
     *  @overload
     *  @brief Orient simplices in table entry. (Argument is_oriented[].)
     *  - Version that uses and sets array is_orientedA[].
     *  @param[out] is_orientedA[isimplexA] 
     *    True if simplex isimplexA is oriented.
     *    - Note: OrientIsoTableWithSimplexFacet() ignores any simplices
     *      where is_orientedA[isimplexA] is true.
     */
    void OrientSimplicesInTableEntry
    (const TABLE_INDEX_TYPE table_index,
     const TABLE_INDEX_TYPE istart,
     std::vector<bool> & is_oriented);

    /*!
     *  @brief Orient all simplices in table entry.
     *  - Orientation of each connected component is arbitrary.
     *  @param[out] num_components Number of connected components
     *    in entry[table_index].simplex_vertex_list[].
     */
    void OrientAllSimplicesInTableEntry
    (const TABLE_INDEX_TYPE table_index, int & num_components);
    
    /*!
     *  @brief Return false if simplices in table entry have 
     *     inconsistent orientations.
     *  @param table_index Index of table entry.
     */
    bool AreSimplicesConsistentlyOriented
    (const TABLE_INDEX_TYPE table_indexA,
     int & isimplexA, int & isimplexB) const;

    ///@}

    
    /// @name Memory Management Functions
    
    ///@{
    virtual void FreeAll();                     /// Free all memory.
    ///@}


    /// @name Check Functions
    
    ///@{
    bool CheckDimension(const int d) const;
    bool CheckDimension() const
    { return(CheckDimension(Dimension())); };
    bool CheckTable(ERROR & error_msg) const;
    bool Check(ERROR & error_msg) const;

    /// @brief Return false and set error message if table_index
    ///   is not in range [0..NumTableEntries()-1].
    bool CheckTableIndex
    (const int table_index, ERROR & error) const;
    
    ///@}

  };

  typedef MC_TABLE * MC_TABLE_PTR;


  // ***************************************************************
  // TABLE MANIPULATION ROUTINES
  // ***************************************************************

  /*!
   *  @brief Invert marching cubes lookup table.
   *  - Swap isosurface patches in table[i] and table[numEntry-1-i]
   *    where numEntry is the number of isosurface table entries.
   *  - Note: Swapping patches swaps SEPARATE_NEG and SEPARATE_POS
   *    and flips isosurface polytope orientations.
   *  @param isotableA Input isosurface table.
   *  @param[out] Inverted table.
   *    - isotableB[i] has isosurface patch isotableA[numEntry-1-i]
   *      where numEntry is the number of isosurface table entries.
   *    - isotableB.SeparationType() is the opposite of
   *      isotableA.SeparationType().
   *    - isotableB.IsoPolyOrientation() is the opposite of
   *      isotableA.IsoPolyOrientation().
   */
  void invert_mcube_isotable
    (const MC_TABLE & isotableA, MC_TABLE & isotableB);


  // *******************************************************************
  // UTILITY FUNCTIONS
  // *******************************************************************

  // calculate num table entries = (num_colors)^(num_vert)
  unsigned long calculate_num_entries
  (const int num_vert, const int num_colors);
    
}

#endif
