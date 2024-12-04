/*!
 *  @file iso3D_table_orient.tpp
 *  @brief Classes and functions for orienting Marching Cubes lookup table.
 *  - Version 0.0.2
 */


/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2025 Rephael Wenger

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

#ifndef _ISO3D_MC_TABLE_ORIENT_H_
#define _ISO3D_MC_TABLE_ORIENT_H_

#include <algorithm>
#include <bitset>
#include <stack>
#include <vector>

#include "iso3D_error.h"
#include "iso3D_MCtable.h"
#include "iso3D_simplex.h"
#include "iso3D_types.h"


/// Classes and routines for orienting Marching Cubes lookup table.
namespace ISO3D {

  /*!
   *  @brief Simplex orientation information.
   */
  class SIMPLEX_ORIENT_INFO
  {

  public:

    typedef MC_TABLE::ISO_VERTEX_BITSET ISO_VERTEX_BITSET;
    
    /// @brief Indicates isosurface vertices in simplex;
    /// - in_simplex[iw] = 1, if isosurface vertex iw
    ///   is in simplex js.
    ISO_VERTEX_BITSET in_simplex;

    /// @brief Indicates boundary facets.
    /// - is_boundary_facet[iw] = 1, if facet formed by removing
    ///   vertex iw is a boundary facet.
    /// - Undefined if iw is not a vertex of the simplex.
    ISO_VERTEX_BITSET is_boundary_facet;

    /// @brief Indicates facet swap parity.
    /// - facet_swap_parity[iw] is parity of swaps that move iw
    ///   to last vertex and sort remaining vertices
    ///   in increasing order.
    /// - Undefined if iw is not a vertex of the simplex.
    ISO_VERTEX_BITSET facet_swap_parity;

    /// @brief Index of connected component containing simplex.
    CUBE_VERTEX_INDEX index_of_connected_component;
    
  public:

    /// Constructor.
    SIMPLEX_ORIENT_INFO() {}

    // Set functions.

    /// @brief Set index of connected component containing simplex.
    void SetConnectedComponent(const int icomponent)
    { index_of_connected_component = icomponent; }

    
    // Get functions.

    /// @brief Return true if iw is a simplex vertex.
    bool InSimplex(const int iw) const
    { return bool(in_simplex[iw]); }

    /// @brief Return true if formed by removing
    ///   vertex iw is a boundary facet.
    bool IsBoundaryFacet(const int iw) const
    { return bool(is_boundary_facet[iw]); }
    
    /// @brief Return facet swap parity (0 or 1) of vertex iw.
    /// - facet_swap_parity[iw] is parity of swaps that move iw
    ///   to last vertex and sort remaining vertices
    ///   in increasing order.
    int FacetSwapParity(const int iw) const
    { return facet_swap_parity[iw]; }

    /// @brief Return index of connected component.
    int IndexOfConnectedComponent() const
    { return index_of_connected_component; }

    /// @brief Return size of isosurface vertex bitset.
    /// - Note: Not all bits are used.
    int IsoVertexBitsetSize() const
    { return ISO_VERTEX_BITSET().size(); }

    /// @brief Return bitset in_simplex.
    ISO_VERTEX_BITSET InSimplex() const
    { return in_simplex; }

    /// @brief Return bitset is_boundary_facet.
    ISO_VERTEX_BITSET IsBoundaryFacet() const
    { return is_boundary_facet; }

    /// @brief Return bitset facet_swap_parity.
    ISO_VERTEX_BITSET FacetSwapParity() const
    { return facet_swap_parity; }

    /// @brief Return true if simplex has a boundary facet.
    bool HasBoundaryFacet() const
    { return is_boundary_facet.any(); }

  };

  
  /*!
   *  @brief Flag isosurface vertices in isotable polytope facets.
   */
  class FACET_ISO_VERTEX {

  public:

    typedef MC_TABLE::ISO_VERTEX_BITSET ISO_VERTEX_BITSET;

  protected:

    /// @brief Number of isotable polytope facets.
    int num_facets;

    /// @brief Number of isosurface vertices.
    int num_isosurface_vertices;

    /// @brief in_facet[jf][iw] = true, if isosurface vertex iw
    ///   is in facet jf.
    std::vector<ISO_VERTEX_BITSET> in_facet;

  protected:

    /// @brief Initialize.
    void Init(const MC_TABLE &  isotable);

  public:

    /// @brief Constructor.
    FACET_ISO_VERTEX(const MC_TABLE & isotable)
    { Init(isotable); }

    /// @brief Return bitset indicating isosurface vertices
    ///    in facet ifacet.
    ISO_VERTEX_BITSET InFacet(const int ifacet) const
    { return in_facet[ifacet]; }
                      
    /// @brief Return number of facets.
    int NumFacets() const
    { return num_facets; }

    /// @brief Return number of isosurface vertices.
    int NumIsosurfaceVertices() const
    { return num_isosurface_vertices; }

    /*!
     *  @brief Return true if isosurface vertices indicated by isov_bitset
     *    are contained in facet ifacet.
     */
    bool AreVerticesInFacet
    (const ISO_VERTEX_BITSET & isov_bitset, const int ifacet) const
    {
      const ISO_VERTEX_BITSET shared_vert =
        isov_bitset & InFacet(ifacet);

      const ISO_VERTEX_BITSET not_in_facet =
        isov_bitset ^ shared_vert;

      return not_in_facet.none();
    }
    
    
    // *** Return string representation. Mainly for debugging. ***

    /// @brief Return string representing bitset in_facet[ifacet].
    /// - Only returns num_isosurface_vertices bits,
    ///   since all other bits are zero.
    std::string InFacetStr(const int ifacet) const;
  };


  /// @brief Table containing orientation information.
  class MC_TABLE_ORIENT_INFO {

  public:
    typedef MC_TABLE::ISOPATCH_VERTEX_INDEX_TYPE
    ISOPATCH_VERTEX_INDEX_TYPE;

    typedef MC_TABLE::ISO_VERTEX_BITSET ISO_VERTEX_BITSET;

    typedef ISO_VERTEX_BITSET ISO_CONNECTED_COMPONENT_BITSET;

    
  protected:

    class ORIENT_INFO_ENTRY {

    public:
      
      /// @brief simplex_info[js] Orientation information
      ///   for simplex js.
      std::vector<SIMPLEX_ORIENT_INFO> simplex_info;

      /// @brief Number of connected components.
      CUBE_VERTEX_INDEX_TYPE num_connected_components;

      /// @brief num_oriented_connected_components Number of oriented
      ///   connected components.
      CUBE_VERTEX_INDEX_TYPE num_oriented_connected_components;

      /// @brief is_connected_component_oriented[icomponent]
      ///    True if connected component icomponent is oriented.
      ISO_CONNECTED_COMPONENT_BITSET is_connected_component_oriented;

    public:

      /// Constructor.      
      ORIENT_INFO_ENTRY()
      {
        num_connected_components = 0;
        num_oriented_connected_components = 0;
      }
        
      /// @brief Set index of connected component containing isimplex.
      void SetConnectedComponent
      (const int isimplex, const int icomponent)
      { simplex_info[isimplex].SetConnectedComponent(icomponent); }

      /// @brief Return number of simplices in entry.
      int NumSimplices() const
      { return simplex_info.size(); }

      /// @brief Return number of oriented connected components.
      int NumOrientedConnectedComponents() const
      { return num_oriented_connected_components; }
      
      /// @brief Return index of connected component containing 
      ///   simplex isimplex.
      int IndexOfConnectedComponent(const int isimplex) const
      { return simplex_info[isimplex].IndexOfConnectedComponent(); }
      
      /// @brief Return bitset is_connected_component_oriented.
      ISO_CONNECTED_COMPONENT_BITSET
      IsConnectedComponentOriented() const
      { return is_connected_component_oriented; }

      /// @brief Return is_connected_component_oriented[icomponent]
      bool IsConnectedComponentOriented(const int icomponent) const
      { return bool(is_connected_component_oriented[icomponent]); }

      /// @brief Return true if simplex isimplex is oriented.
      bool IsSimplexOriented(const int isimplex) const
      { return IsConnectedComponentOriented
          (IndexOfConnectedComponent(isimplex)); }

      /// @brief Return true if all simplices are oriented.
      /// - Equivalently, return true if all connected components are oriented.
      bool AreAllSimplicesOriented() const
      { return (num_connected_components ==
                num_oriented_connected_components); }

    };
    
    
  protected:

    /// @brief Number of vertices per simplex.
    int num_vertices_per_simplex;

    /// @brief Number of isosurface vertices.
    int num_isosurface_vertices;

    /// @brief Table entries.
    std::vector<ORIENT_INFO_ENTRY> entry;

    /// @brief Facet information.
    FACET_ISO_VERTEX facet_info;
      
    /// @brief Initialize.
    void Init(const MC_TABLE & isotable);

    /// @brief Set connected component of simplex isimplex
    ///   in entry[table_index].
    void _SetConnectedComponent
    (const TABLE_INDEX_TYPE table_index, const int isimplex,
     const int icomponent)
    {
      entry[table_index].SetConnectedComponent(isimplex,icomponent);
    }

    /*!
     *  @brief Flag vertices in each simplex.
     */
    void _FlagVerticesInEachSimplex
    (const TABLE_INDEX_TYPE table_index, 
     const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list,
     const int num_simplices);

    /*!
     *  @brief Set facet swap parity in all facets of simplex isimplex
     *    in table entry table_index.
     *  @param table_index Table index.
     *  @param isimplex Simplex index.
     *  @param simplex_vertex[j] j'th vertex of simplex isimplex.
     *  @param temp_simplex_vert[] Temporary array to store simplex vertices.
     *    @pre temp_simplex_vert[] has size at least NumVertPerSimplex().
     */
    void _SetSimplexFacetSwapParity
    (const TABLE_INDEX_TYPE table_index,
     const int isimplex,
     const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex,
     ISOPATCH_VERTEX_INDEX_TYPE temp_simplex_vert[]);

    /*!
     *  @overload
     *  @brief Set facet swap parity in all facets of simplex isimplex
     *    in table entry table_index. (No temporary array.)
     *  - Version whose argument list does not include 
     *    temporary array temp_simplex_vert[].
     */
    void _SetSimplexFacetSwapParity
    (const TABLE_INDEX_TYPE table_index,
     const int isimplex,
     const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex);

    /*!
     *  @brief Set parity of swaps that move iw to last vertex
     *    and sort remaining vertices in increasing order.
     *  - Set boundary facets, i.e. facets thar are
     *    in only one simplex in simplex_vertex_list[].
     *  @pre Call _FlagVerticesInEachSimplex before
     *    calling this function.
     *  @param simplex_vertex_list[] List of simplices.
     *    - Each simplex has NumVerticesPerSimplex() vertices.
     */
    void _SetFacetSwapParity
    (const TABLE_INDEX_TYPE table_index, 
     const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list,
     const int num_simplices);

    /*!
     *  @brief Flag boundary facets, i.e. facets thar are
     *    in only one simplex in simplex_vertex_list[].
     *  @pre Call _FlagVerticesInEachSimplex before
     *    calling this function.
     *  @param simplex_vertex_list[] List of simplices.
     *    - Each simplex has NumVerticesPerSimplex() vertices.
     */
    void _FlagBoundaryFacets(const TABLE_INDEX_TYPE table_index);

    /// @brief Set connected component containing each simplex
    ///    for entry[table_index].
    void _SetConnectedComponentIndices
    (const TABLE_INDEX_TYPE table_index,
     const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list,
     const int num_simplices);

    /// @brief Convert ISO_VERTEX_BITSET to string.
    /// - Only returns num_isosurface_vertices bits,
    ///   since all other bits are zero.
    std::string _ConvertBitsetToString
    (const ISO_VERTEX_BITSET & bitset) const;
    
    
  public:

    /// @brief Constructor.
    MC_TABLE_ORIENT_INFO(const MC_TABLE & isotable):
      facet_info(isotable)
    { Init(isotable); }

    
    // *** Get routines ***

    /// @brief Return number of table entries.
    TABLE_INDEX_TYPE NumTableEntries() const
      { return entry.size(); }
    
    /// @brief Return number of vertices per simplex.
    int NumVerticesPerSimplex() const
    { return num_vertices_per_simplex; }

    /// @brief Return number of isosurface vertices.
    int NumIsosurfaceVertices() const
    { return num_isosurface_vertices; }

    /// Return number of simplices for entry table_index.
    int NumSimplices(const TABLE_INDEX_TYPE table_index) const
    { return entry[table_index].NumSimplices(); }

    /// @brief Return orientation information for simplex isimplex
    ///   in table entry table_index.
    const SIMPLEX_ORIENT_INFO & SimplexInfo
    (const TABLE_INDEX_TYPE table_index, const int isimplex) const
    { return entry[table_index].simplex_info[isimplex]; }

    /// @brief Return facet information.
    const FACET_ISO_VERTEX & FacetInfo() const
    { return facet_info; }

    /// @brief Return index of connected component containing simplex isimplex
    ///   in table entry table_index.
    int IndexOfConnectedComponent
    (const TABLE_INDEX_TYPE table_index, const int isimplex) const
    { return SimplexInfo(table_index, isimplex).IndexOfConnectedComponent(); }
    
    /// @brief Return true if connected component icomponent is oriented.
    bool IsConnectedComponentOriented
    (const TABLE_INDEX_TYPE table_index,
     const int icomponent) const
    { return entry[table_index].is_connected_component_oriented[icomponent]; };

    /// @brief Return true if simplex isimplex in table entry table_index
    ///   is oriented.
    bool IsSimplexOriented
    (const TABLE_INDEX_TYPE table_index,
     const int isimplex) const
    { return entry[table_index].IsSimplexOriented(isimplex); }

    /// @brief Return flag are_all_simplices_oriented
    ///   for table entry table_index.
    bool AreAllSimplicesOriented(const TABLE_INDEX_TYPE table_index) const
    { return entry[table_index].AreAllSimplicesOriented(); }

    /// @brief Return true if all table entries are marked as oriented.
    /// @param[out] table_index Index of table entry that has not
    ///    been completely oriented.
    bool AreAllTableEntriesOriented(TABLE_INDEX_TYPE & table_index) const;

    /// @brief Return number of connected components
    ///   in table entry table_index.
    int NumConnectedComponents
    (const TABLE_INDEX_TYPE table_index) const
    { return entry[table_index].num_connected_components; }

    /// @brief Return number of oriented connected components
    ///   in table entry table_index.
    int NumOrientedConnectedComponents
    (const TABLE_INDEX_TYPE table_index) const
    { return entry[table_index].NumOrientedConnectedComponents(); }
    
    /// @brief Return number of facets per simplex.
    int NumFacetsPerSimplex() const
    { return NumVerticesPerSimplex(); }

    /// @brief Return number of vertices per simplex facet.
    int NumVerticesPerSimplexFacet() const
    { return NumVerticesPerSimplex()-1; }

    /// @brief Return true if some simplex in connected component
    ///    icomponent has a boundary facet.
    bool ComponentHasBoundaryFacet
    (const TABLE_INDEX_TYPE table_index, const int icomponent) const;

    /// @brief Return true if some simplex in table entry has 
    ///    a boundary facet.
    bool SomeSimplexHasBoundaryFacet(const TABLE_INDEX_TYPE table_index) const;

    /*!
     *  @brief Find index of table entry with a single
     *    connected component that has at least one
     *    boundary simplex facet.
     *  @param[out] table_index Index of table entry found.
     *    - Undefined if table entry not found.
     *  @param[out] flag_found True if table entry found.
     */
    void FindEntryWithSingleConnectedComponentWithBoundary
    (TABLE_INDEX_TYPE table_index, bool & flag_found) const;


    // *** Orient routines ***

    /*!
     *  @brief Set flag is_connected_component_oriented 
     *    for connected component icomponent.
     *  - Also modifies entry[table_index].num_oriented_connected_components.
     */
    void SetIsConnectedComponentOriented
    (const TABLE_INDEX_TYPE table_index, const int icomponent,
     const bool flag);

    /*!
     *  @brief Update flag entry[table_index].is_connected_component_oriented. 
     *  - Update by "or" with is connected_component_oriented.
     *  - Also modifies entry[table_index].num_oriented_connected_components.
     */
    void UpdateIsConnectedComponentOriented
    (const TABLE_INDEX_TYPE table_index, 
     const ISO_CONNECTED_COMPONENT_BITSET & is_component_oriented);

    /*!
     *  @brief Return true if two simplices share a facet.
     *  - Also returns bitset of shared vertices, number of shared vertices,
     *    comparison of facet swap parities, and bitsets indicating 
     *    unshared vertices.
     *  @param[out] shared_vert Bits indicating vertices shared 
     *    by two simplices.
     *  @param[out] num_shared_vertices Number of vertices shared 
     *    by two simplices.
     *  @param[out] are_shared_facet_swap_parities_equal
     *    - True if simplices share a facet and
     *      shared facet has the same swap parity in each simplex.
     *    - False if simplices share a facet and
     *      shared facet has different swap parities in each simplex.
     *    - Undefined if simplices do not share a facet.
     *  @param[out] not_in_facetA Bit indicating vertex of simplexA
     *    not shared with simplexB.
     *    - Undefined if simplices A and B do not share a facet
     *      or are identical.
     *  @param[out] not_in_facetB Bit indicating vertex of simplexB
     *    not shared with simplexA.
     *    - Undefined if simplices A and B do not share a facet
     *      or are identical.
     */
    bool DoSimplicesShareFacet
    (const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     const int isimplexA, const int isimplexB,
     ISO_VERTEX_BITSET & shared_vert,
     int & num_shared_vertices,
     bool & are_shared_facet_swap_parities_equal,
     ISO_VERTEX_BITSET & not_in_facetA,
     ISO_VERTEX_BITSET & not_in_facetB) const;

    /*!
     *  @overload
     *  @brief Return true if two simplices share a facet.
     *    (Return only comparison of swap parities.)
     *  - Version that returns only comparison of swap parities.
     */
    bool DoSimplicesShareFacet
    (const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     const int isimplexA, const int isimplexB,
     bool & are_shared_facet_swap_parities_equal) const;
    
    /*!
     *  @brief Return true if two simplices share a boundary facet.
     *  - Also returns bitset of shared vertices, number of shared vertices,
     *    comparison of facet swap parities, and bitsets indicating 
     *    unshared vertices.
     *  @param[out] shared_vert Bits indicating vertices shared 
     *    by two simplices.
     *  @param[out] num_shared_vertices Number of vertices shared 
     *    by two simplices.
     *  @param[out] are_shared_facet_swap_parities_equal
     *    - True if simplices share a facet and
     *      shared facet has the same swap parity in each simplex.
     *    - False if simplices share a facet and
     *      shared facet has different swap parities in each simplex.
     *    - Undefined if simplices do not share a facet.
     *  @param[out] not_in_facetA Bit indicating vertex of simplexA
     *    not shared with simplexB.
     *    - Undefined if simplices A and B do not share a facet
     *      or are identical.
     *  @param[out] not_in_facetB Bit indicating vertex of simplexB
     *    not shared with simplexA.
     *    - Undefined if simplices A and B do not share a facet
     *      or are identical.
     */
    bool DoSimplicesShareBoundaryFacet
    (const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     const int isimplexA, const int isimplexB,
     ISO_VERTEX_BITSET & shared_vert,
     int & num_shared_vertices,
     bool & are_shared_facet_swap_parities_equal,
     ISO_VERTEX_BITSET & not_in_facetA,
     ISO_VERTEX_BITSET & not_in_facetB) const;


    /*!
     *  @overload
     *  @brief Return true if two simplices share a boundary facet.
     *  - Version that does not return bitsets indicating 
     *    unshared vertices.
     */
    bool DoSimplicesShareBoundaryFacet
    (const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     const int isimplexA, const int isimplexB,
     ISO_VERTEX_BITSET & shared_vert,
     int & num_shared_vertices,
     bool & are_shared_facet_swap_parities_equal) const;

    /*!
     *  @brief Orient all simplices in connected component containing isimplexA
     *    consistently with isimplexA.
     *  - Modifies both isotable and entry[table_index].simplex_info[].
     */
    void OrientConnectedComponent
    (const TABLE_INDEX_TYPE table_index, const int isimplexA,
     MC_TABLE & isotable);
    
    /*!
     *  @brief Consistently orient all simplices in each
     *    connected component in table entry table_index.
     *  - Orientation of each connected component is arbitrary.
     *  - Modifies both isotable and entry[table_index].simplex_info[].
     */
    void OrientAllSimplicesInTableEntry
    (const TABLE_INDEX_TYPE table_index, MC_TABLE & isotable);


    /*! 
     *  @brief Flip all simplices in connected component icomponent
     *    in table entry table_index.
     *  - Also recomputes facet_swap_parity for each modified simplex.
     */
    void FlipSimplicesInConnectedComponent
    (const TABLE_INDEX_TYPE table_index, const int icomponent,
     MC_TABLE & isotable);
    
    /*!
     *  @brief Orient simplices in table entry table_indexB
     *    from orientation of some boundary facet of table_indexA.
     *    - Does nothing if some simplex in table_indexB has
     *      orientation matching orientation of boundary facet 
     *      from table_indexA.
     *  @pre OrientAllSimplicesInTableEntry() has been called
     *    on table_indexA and table_indexB.
     *  @param table_indexA Index of first table entry.
     *  @param table_indexB Index of second table entry.
     *    - Orient table_indexB from table_indexA.
     *    @pre table_indexB != table_indexA.
     */
    void OrientTwoTableEntries
    (const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     MC_TABLE & isotable);

    
    // *** Check routines ***

    /// @brief Check against isosurface lookup table.
    /// - Return true if passes check.
    template <typename _ISOTABLE_TYPE>
    bool Check
    (const _ISOTABLE_TYPE & isotable, ERROR & error) const;
    
    /// @brief Check that bitset entry[table_index].simplex_info[*].in_simplex 
    ///   is set.
    /// - Return true if all bitsets are set.
    bool CheckInSimplexIsSet
    (const TABLE_INDEX_TYPE table_index, ERROR & error) const;

    /*!
     *  @brief Check that orientations of all pairs of simplices 
     *    in entry[table_index] are consistent.
     *  - Return true if all orientations in entry[table_index] are consistent.
     *  @param[out] isimplexA Simplex in entry[table_index]
     *    whose orientation does not match isimplexB.
     *    - Defined only if check returns false.
     *  @param[out] isimplexB Simplex in entry[table_index]
     *    whose orientation does not match isimplexA.
     *    - Defined only if check returns false.
     *  - Note: isimplexA and isimplexB share a facet.
     */
    bool CheckOrientationsInTableEntry
    (const TABLE_INDEX_TYPE table_index, int & isimplexA, int & isimplexB,
    ERROR & error) const;

    /*!
     *  @brief Check that orientations of all pairs of simplices 
     *    in entry[table_index] are consistent. (Doesn't return simplices.)
     *  - Version that does not return mismatched simplices.
     */
    bool CheckOrientationsInTableEntry
    (const TABLE_INDEX_TYPE table_index, ERROR & error) const;    
    
    /*!
     *  @brief Check that orientations in every table entry
     *     are consistent.
     *  - Does NOT compare orientations of simplices in
     *    DIFFERENT table entries.
     *  @param[out] table_index Index of table entry
     *    with simplices with mismatched orientations.
     *    - Defined only if check returns false.
     *  @param[out] isimplexA Simplex in entry[table_index]
     *    whose orientation does not match isimplexB.
     *    - Defined only if check returns false.
     *  @param[out] isimplexB Simplex in entry[table_index]
     *    whose orientation does not match isimplexA.
     *    - Defined only if check returns false.
     *  - Note: isimplexA and isimplexB share a facet.

     */
    bool CheckOrientationsInEveryTableEntry
    (TABLE_INDEX_TYPE & table_index, int & isimplexA, int & isimplexB,
    ERROR & error) const;

    /*!
     *  @brief Check that orientations in every table entry
     *     are consistent. (Doesn't return table index or simplices.)
     *  - Does NOT compare orientations of simplices in
     *    DIFFERENT table entries.
     *  - Version that does not return index of table entry or 
     *    mismatched simplices.
     */
    bool CheckOrientationsInEveryTableEntry(ERROR & error) const;


    /*!
     *  @brief  Return true if orientations of simplices 
     *    in entry[table_indexB] are consistent with orientations 
     *    of simplices in entry[table_indexA].
     *  @pre CheckOrientationsInTableEntry(table_indexA) returns true
     *    and CheckOrientationsInTableEntry(table_indexB) returns true.
     *  @param[out] isimplexA Simplex in entry[table_indexA]
     *    whose orientation does not match isimplexB.
     *    - Defined only if check returns false.
     *  @param[out] isimplexB Simplex in entry[table_indexB]
     *    whose orientation does not match isimplexB.
     *    - Defined only if check returns false.
     *    - Note: isimplexA and isimplexB share a facet.
     *  @param[out] componentB_checked Bitset indicating
     *    which components were checked.
     *    - componentB_checked[i] = true if some boundary facet
     *      of some simplex in component i of table_indexB
     *      matched a boundary facet of some simplex of table_indexA.
     */
    template <typename _ISOTABLE_TYPE>
    bool CheckOrientationsOfTwoTableEntries
    (const _ISOTABLE_TYPE & isotable,
     const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     int & isimplexA, int & isimplexB,
     ISO_CONNECTED_COMPONENT_BITSET & componentB_checked,
     ERROR & error) const;

    /// @brief Return true if orientations of simplices in entry[table_indexB] 
    ///   are consistent with orientations of simplices in entry[table_indexA].
    /// - Version that does not return bitset componentB_checked.
    template <typename _ISOTABLE_TYPE>
    bool CheckOrientationsOfTwoTableEntries
    (const _ISOTABLE_TYPE & isotable,
     const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
     int & isimplexA, int & isimplexB,
     ERROR & error) const;

    /*!
     *  @brief Check orientation of table entry table_indexA against
     *    all other table entries.
     *  @param[out] isimplexA Simplex in entry[table_indexA]
     *    whose orientation does not match isimplexB.
     *    - Defined only if check returns false.
     *  @param[out] table_indexB Table index whose orientation
     *      does not match orientation in table_indexA.
     *    - Defined only if check returns false.
     *  @param[out] isimplexB Simplex in entry[table_indexB]
     *    whose orientation does not match isimplexB.
     *    - Defined only if check returns false.
     *    - Note: isimplexA and isimplexB share a facet.
     */
    template <typename _ISOTABLE_TYPE>
    bool CheckOrientationOfTableEntryAgainstAllOthers
    (const _ISOTABLE_TYPE & isotable,
     const TABLE_INDEX_TYPE table_indexA, 
     int & isimplexA, TABLE_INDEX_TYPE & table_indexB, int & isimplexB,
     ERROR & error) const;

    /*!
     *  @brief Check orientation of table entry table_indexA against
     *    all other table entries. (Doesn't return table index or simplices.)
     *  - Version that does not return table index or simplices
     *    that have inconsistent orientations.
     */
    template <typename _ISOTABLE_TYPE>
    bool CheckOrientationOfTableEntryAgainstAllOthers
    (const _ISOTABLE_TYPE & isotable,
     const TABLE_INDEX_TYPE table_indexA, 
     ERROR & error) const;


    // *** Return string representation. Mainly for debugging. ***

    /// @brief Return string representing bitset SimplexInfo().in_simplex.
    /// - Only returns num_isosurface_vertices bits,
    ///   since all other bits are zero.
    std::string InSimplexStr
    (const TABLE_INDEX_TYPE table_index, const int isimplex) const
    {
      return _ConvertBitsetToString
        (SimplexInfo(table_index,isimplex).in_simplex);
    }

    /// @brief Return string representing bitset SimplexInfo().is_boudary_facet.
    /// - Only returns num_isosurface_vertices bits,
    ///   since all other bits are zero.
    std::string IsBoundaryFacetStr
    (const TABLE_INDEX_TYPE table_index, const int isimplex) const
    {
      return _ConvertBitsetToString
        (SimplexInfo(table_index,isimplex).is_boundary_facet);
    }

    /// @brief Return string representing bitset SimplexInfo().facet_swap_parity.
    /// - Only returns num_isosurface_vertices bits,
    ///   since all other bits are zero.
    std::string FacetSwapParityStr
    (const TABLE_INDEX_TYPE table_index, const int isimplex) const
    {
      return _ConvertBitsetToString
        (SimplexInfo(table_index,isimplex).facet_swap_parity);
    }
    
  };


  namespace {

    /*!
     *  @brief Return true passes check.
     *  - Table entry istart must have at least one simplex and
     *    exactly one connected component.
     */
    template <typename _ISOTABLE_TYPE, typename _ORIENT_INFO_TABLE>
    bool check_orient_simplices_starting_table_entry
    (const _ISOTABLE_TYPE & isotable,
     const _ORIENT_INFO_TABLE & orient_info,
     const TABLE_INDEX_TYPE istart, ERROR & error)
    {
      if (isotable.NumSimplices(istart) < 1) {
        error.AddToMessage
        ("Programming error. Marching Cubes lookup table entry ",
         istart, " has no simplices.");
        error.AddToMessage
          ("  Table index must have at least one simplex to orient MC table.");
        return false;
        
      }

      const int numc = orient_info.NumConnectedComponents(istart);

      if (numc < 1) {
        error.AddToMessage
          ("Programming error. Marching Cubes table entry ", istart,
           " has zero connected component.");
        error.AddToMessage
          ("  Routine must start from table entry with exactly one connected component.");
        return false;
      }
      else if (numc != 1) {
        error.AddToMessage
          ("Programming error. Marching Cubes table entry ", istart,
           " has ", numc, " connected components.");
        error.AddToMessage
          ("  Routine must start from table entry with exactly one connected component.");
        return false;
      }

      return true;
    }

    
    /// @brief Swap a[i] with last element in a[].
    /// - Does nothing if a[i] is last element.
    /// @pre i < a.size().
    template <typename ETYPE>
    void _swap_with_last(const int i, std::vector<ETYPE> & a)
    {
      if (i+1 < a.size()) {
        const int ilast = a.size() - 1;
        std::swap(a[i], a[ilast]);
      }
    }


    /*!
     *  @brief Orient simplex lists in Marching Cubes lookup table. (Local.)
     *  @param isotable Marching Cubes lookup table.
     *  @param istart Starting table index.
     *    - Orient consistent with first simplex in isotable.entry[istart].
     *    @pre isotable.NumSimplices(table_index) > 0.
     *    @pre Simplices in isotable.entry[table_index] are
     *    in one connected component.
     *  @param flag_verbose Print messages if flag_verbose is true.
     *  @param output_trigger Print messages whenever more than output_trigger
     *    table entries are completed.
     */
    template <typename _OSTREAM_TYPE, typename _ISOTABLE_TYPE>
    void orient_mcube_table_local
    (_OSTREAM_TYPE & out,
     _ISOTABLE_TYPE & isotable, const TABLE_INDEX_TYPE istart,
     const bool flag_verbose, const TABLE_INDEX_TYPE output_trigger)
    {
      MC_TABLE_ORIENT_INFO orient_info(isotable);
      PROCEDURE_ERROR error("orient_mcube_table_local");

      if (orient_info.NumVerticesPerSimplex() < 2) {
        // Nothing to orient.
        return;
      }

      if (!isotable.CheckTableIndex(istart, error))
        { throw error; }

      for (TABLE_INDEX_TYPE table_index = 0;
           table_index < isotable.NumTableEntries(); table_index++) {
        orient_info.OrientAllSimplicesInTableEntry(table_index, isotable);
      }

      if (!check_orient_simplices_starting_table_entry
          (isotable, orient_info, istart, error))
        { throw error; }


      int num_completed = 1;

      // Mark bounded components as oriented.
      for (TABLE_INDEX_TYPE table_index = 0; 
           table_index < orient_info.NumTableEntries(); table_index++) {
        for (int icomponent = 0; 
             icomponent < orient_info.NumConnectedComponents(table_index);
             icomponent++) {

          if (!orient_info.ComponentHasBoundaryFacet
              (table_index, icomponent)) {
            // Isosurface patch is closed?!? with no boundary.
            // Component was already oriented.
            // Nothing to check against other table entries.
            orient_info.SetIsConnectedComponentOriented
              (table_index, icomponent, true);

            if (orient_info.AreAllSimplicesOriented(table_index))
              { num_completed++; }
          }
        }
      }

      // Array of unoriented table entries.
      std::vector<TABLE_INDEX_TYPE> unoriented_entry;
      
      for (TABLE_INDEX_TYPE table_index = 0;
           table_index < orient_info.NumTableEntries();
           table_index++) {

        if (table_index == istart) {
          // Orient all other table entries from istart.
          continue;
        }
        
        if (orient_info.AreAllSimplicesOriented(table_index)) {
          // Either no simplices or no boundary simplex facets.
          continue;
        }

        unoriented_entry.push_back(table_index);
      }


      // Stack containing indices of table entries with 1 component that have been oriented.
      std::stack<TABLE_INDEX_TYPE> stackI;

      // Stack containing indices of table entries with multiple components
      //   that have been oriented.
      std::stack<TABLE_INDEX_TYPE> stack_multi;
      
      const int icomponent =
        orient_info.IndexOfConnectedComponent(istart, 0);
      orient_info.SetIsConnectedComponentOriented
        (istart, icomponent, true);

      stackI.push(istart);
      while (stackI.size() > 0) {
        const int table_indexA = stackI.top();
        stackI.pop();

        int j = 0;
        while (j < unoriented_entry.size()) {
          const TABLE_INDEX_TYPE table_indexB = unoriented_entry[j];

          if ((orient_info.AreAllSimplicesOriented(table_indexB)) ||
              (table_indexA == table_indexB) ||
              (isotable.NumSimplices(table_indexB) == 0)) {
            // None of these should happen, but just in case.

            // Remove table_indexB from unoriented_entries.
            _swap_with_last(j, unoriented_entry);
            unoriented_entry.pop_back();

            // Process new element in unoriented_entry[j].
            continue;
          }

          orient_info.OrientTwoTableEntries
            (table_indexA, table_indexB, isotable);


          if (orient_info.AreAllSimplicesOriented(table_indexB)) {
            if (orient_info.NumConnectedComponents(table_indexB) == 1)
              { stackI.push(table_indexB); }
            else
              { stack_multi.push(table_indexB); }

            // Remove table_indexB from unoriented_entries.
            _swap_with_last(j, unoriented_entry);
            unoriented_entry.pop_back();

            num_completed++;
            if (flag_verbose && num_completed%output_trigger == 0) {
              out << "  Completed orientation of " << num_completed
                << " isosurface table entries.\n";
              out.flush();
            }

            // Process new element in unoriented_entry[j].
            continue;
          }

          // Go to next element in unoriented_entry[j].
          j = j+1;
        }
      }

      if (unoriented_entry.size() != 0) {
        // Try using stack of multi components to orient.
        while (stack_multi.size() > 0) {
          const int table_indexA = stack_multi.top();
          stack_multi.pop();

          int j = 0;
          while (j < unoriented_entry.size()) {
            const TABLE_INDEX_TYPE table_indexB = unoriented_entry[j];

            if ((orient_info.AreAllSimplicesOriented(table_indexB)) ||
                (table_indexA == table_indexB) ||
                (isotable.NumSimplices(table_indexB) == 0)) {
              // None of these should happen, but just in case.

              // Remove table_indexB from unoriented_entries.
              _swap_with_last(j, unoriented_entry);
              unoriented_entry.pop_back();

              // Process new element in unoriented_entry[j].
              continue;
            };

            orient_info.OrientTwoTableEntries
              (table_indexA, table_indexB, isotable);

            if (orient_info.AreAllSimplicesOriented(table_indexB)) {
              { stack_multi.push(table_indexB); }

              // Remove table_indexB from unoriented_entries.
              _swap_with_last(j, unoriented_entry);
              unoriented_entry.pop_back();

              num_completed++;
              if (flag_verbose && num_completed%output_trigger == 0) {
                out << "  Completed orientation of " << num_completed
                    << " isosurface table entries.\n";
                out.flush();
              }

              // Process new element in unoriented_entry[j].
              continue;
            }

            // Go to next element in unoriented_entry[j].
            j = j+1;
          }
        }
      }


      if (flag_verbose) {
        TABLE_INDEX_TYPE table_indexQ;
        if (orient_info.AreAllTableEntriesOriented(table_indexQ)) {
          if (num_completed > output_trigger) {
            out << "  Completed orientation of all isosurface table entries.\n";
            out.flush();
          }
        }
        else {
          out << "*** Warning: Unable to determine orientation for table index: "
              << table_indexQ << ".\n";
          out.flush();
        }
      }
    }


    
    // *****************************************************************
    // NO_OUTPUT_MESSAGE class
    // *****************************************************************

    /// @brief Class that does not output any message.
    class NO_OUTPUT_MESSAGE {
      
    public:
      /* Empty class */

      void flush()
      { /* Do nothing. */ }

    };


    /// @brief Overload << on NO_OUTPUT_MESSAGE.
    template <typename _T>
    NO_OUTPUT_MESSAGE & operator <<
    (NO_OUTPUT_MESSAGE & no_output_message, const _T x)
    {
      // Do nothing.
      return no_output_message;
    }

  }


  // *****************************************************************
  // Orient mcube table routines
  // *****************************************************************


  /*!
   *  @brief Orient simplex lists in Marching Cubes lookup table.
   *  @param isotable Marching Cubes lookup table.
   *  @param istart Starting table index.
   *    - Orient consistent with first simplex in isotable.entry[istart].
   *    @pre isotable.NumSimplices(table_index) > 0.
   *    @pre Simplices in isotable.entry[table_index] are
   *    in one connected component.
   *  @param flag_verbose Print messages if flag_verbose is true.
   *  @param output_trigger Print messages whenever more than output_trigger
   *    table entries are completed.
   */
  template <typename OSTREAM_TYPE, typename _ISOTABLE_TYPE>
  void orient_mcube_table
  (OSTREAM_TYPE & out,
   _ISOTABLE_TYPE & isotable, const TABLE_INDEX_TYPE istart,
   const bool flag_verbose, const TABLE_INDEX_TYPE output_trigger)
  {
    orient_mcube_table_local
      (out, isotable, istart, flag_verbose, output_trigger);
  }


  /*!
   *  @overload
   *  @brief Orient simplex lists in Marching Cubes lookup table. 
   *    (No output messages.)
   *  - Version that does not print output messages.
   *  @param isotable Marching Cubes lookup table.
   *  @param istart Starting table index.
   *    - Orient consistent with first simplex in isotable.entry[istart].
   *    @pre isotable.NumSimplices(table_index) > 0.
   *    @pre Simplices in isotable.entry[table_index] are
   *    in one connected component.
   */
  template <typename _ISOTABLE_TYPE>
  void orient_mcube_table
  (_ISOTABLE_TYPE & isotable, const TABLE_INDEX_TYPE istart)
  {
    NO_OUTPUT_MESSAGE no_output_message;

    // Dummy parameters.
    bool flag_verbose = false;
    int output_trigger(1);

    orient_mcube_table_local
      (no_output_message, isotable, istart, flag_verbose, output_trigger);
  }


  // *****************************************************************
  // Check mcube table orientation
  // *****************************************************************

  /*!
   *  @brief Return true if all simplex lists in Marching Cubes lookup table
   *    are consistently oriented. 
   *  - Fast algorithm: Skips entries that have already matched
   *    some table entry.
   *  @param isotable Marching Cubes lookup table.
   */
  template <typename OSTREAM_TYPE, typename _ISOTABLE_TYPE>
  bool check_mcube_table_orientation
  (OSTREAM_TYPE & out, const _ISOTABLE_TYPE & isotable,
   const bool flag_verbose, const TABLE_INDEX_TYPE output_trigger,
   ERROR & error)
  {
    typedef typename _ISOTABLE_TYPE::ISO_VERTEX_BITSET ISO_VERTEX_BITSET;
    
    const int ONE(1);
    const int num_table_entries = isotable.NumTableEntries();
    const int num_vert_per_simplex = isotable.NumVerticesPerSimplex();
    int isimplexA, isimplexB;

    int num_table_entries_checked = 0;

    if (num_vert_per_simplex < 2) {
      // Nothing to check
      num_table_entries_checked = num_table_entries;
      return true;
    }

    MC_TABLE_ORIENT_INFO orient_info(isotable);

    if (!orient_info.CheckOrientationsInEveryTableEntry(error))
      { return false; }

    if (flag_verbose && 
        orient_info.NumTableEntries() > output_trigger) {
      out << "  All table entries have internal orientation consistency.\n";
    }

    bool flag_found;
    TABLE_INDEX_TYPE istart;
    orient_info.FindEntryWithSingleConnectedComponentWithBoundary
      (istart, flag_found);

    if (!flag_found) {
      // No table entries with one connected component
      //   where connected component has a boundary.
      if (flag_verbose) {
        out << "***  No table entries with single connected component\n"
            << "     where connected component has a boundary.\n";
      }
      return false;
    }

    // Stack containing indices of table entries that have been checked.
    std::stack<TABLE_INDEX_TYPE> stack;

    num_table_entries_checked = 1;

    const int icomponent =
      orient_info.IndexOfConnectedComponent(istart, 0);
    orient_info.SetIsConnectedComponentOriented
      (istart, icomponent, true);

    stack.push(istart);
    while (stack.size() > 0) {
      const int table_indexA = stack.top();
      stack.pop();

      for (int table_indexB = 0;
	   table_indexB < isotable.NumTableEntries(); table_indexB++) {

	if (orient_info.AreAllSimplicesOriented(table_indexB)) {
	  // table_indexB has already been checked.
	  continue;
	}

	ISO_VERTEX_BITSET componentB_checked;
	
	if (!orient_info.CheckOrientationsOfTwoTableEntries
	    (isotable, table_indexA, table_indexB,
	     isimplexA, isimplexB, componentB_checked, error))
	  { return false; }

	orient_info.UpdateIsConnectedComponentOriented
	  (table_indexB, componentB_checked);

	if (orient_info.AreAllSimplicesOriented(table_indexB)) {

	  if (orient_info.NumConnectedComponents(table_indexB) == ONE) 
	    { stack.push(table_indexB); }

	  num_table_entries_checked++;


	  if (flag_verbose && (num_table_entries_checked > 0) &&
	      num_table_entries_checked%output_trigger == 0) {
	    out << "  Checked " << num_table_entries_checked
		<< " out of " << isotable.NumTableEntries()
		<< " isosurface table entry orientations.\n";
	    out.flush();
	  }
	}
      }
    }

    // Verify that all table entries have been checked.
    for (TABLE_INDEX_TYPE table_index = 0;
	 table_index < orient_info.NumTableEntries();
	 table_index++) {

      if (orient_info.AreAllSimplicesOriented(table_index))
        { continue; }

      if (!orient_info.SomeSimplexHasBoundaryFacet(table_index)) {
        // Isosurface patch is closed?!? with no boundary.
        // Consistency of table entry was already checked.
        // Nothing to check against other table entries.
        continue;
      }

      if (!orient_info.CheckOrientationOfTableEntryAgainstAllOthers
          (isotable, table_index, error)) {
        return false;
      }

      num_table_entries_checked++;

      if (flag_verbose && (num_table_entries_checked > 0) &&
          num_table_entries_checked%output_trigger == 0) {
        out << "  Checked " << num_table_entries_checked
            << " out of " << isotable.NumTableEntries()
            << " isosurface table entry orientations.\n";
        out.flush();
      }
    }

    if (flag_verbose &&
        isotable.NumTableEntries() > output_trigger) {
      out << "  Checked orientations on all "
          << isotable.NumTableEntries()
          << " isosurface table entries.\n";
      out.flush();
    }

    return true;
  }


  /*!
   *  @brief Return true if all simplex lists in Marching Cubes lookup table
   *    are consistently oriented.
   *  - Check every table entry against every other table entry. (Very slow).
   *  @param isotable Marching Cubes lookup table.
   */
  template <typename OSTREAM_TYPE, typename _ISOTABLE_TYPE>
  bool check_mcube_table_orientation_all_pairs
  (OSTREAM_TYPE & out, const _ISOTABLE_TYPE & isotable, 
   const bool flag_verbose, const TABLE_INDEX_TYPE output_trigger,
   ERROR & error)
  {
    const int num_table_entries = isotable.NumTableEntries();
    const int num_vert_per_simplex = isotable.NumVerticesPerSimplex();
    int isimplexA, isimplexB;

    if (num_vert_per_simplex < 2) {
      // Nothing to check
      return true;
    }

    MC_TABLE_ORIENT_INFO orient_info(isotable);

    if (!orient_info.CheckOrientationsInEveryTableEntry(error))
      { return false; }

    if (flag_verbose && 
        orient_info.NumTableEntries() > output_trigger) {
      out << "  All table entries have internal orientation consistency.\n";
    }

    for (TABLE_INDEX_TYPE table_indexA = 0; 
         table_indexA < isotable.NumTableEntries(); table_indexA++) {

      for (TABLE_INDEX_TYPE table_indexB = table_indexA+1;
           table_indexB < isotable.NumTableEntries(); table_indexB++) {

        if (!orient_info.CheckOrientationsOfTwoTableEntries
            (isotable, table_indexA, table_indexB,
             isimplexA, isimplexB, error))
          { return false; }
      }
      
      if (flag_verbose && (table_indexA > 0) &&
          table_indexA%output_trigger == 0) {
        out << "  Checked " << table_indexA
            << " out of " << isotable.NumTableEntries()
            << " isosurface table entry orientations.\n";
        out.flush();
      }
    }

    if (flag_verbose &&
        isotable.NumTableEntries() > output_trigger) {
      out << "  Checked orientations on all "
          << isotable.NumTableEntries()
          << " isosurface table entries.\n";
      out.flush();
    }

    return true;
  }

      
  /*!
   *  @brief Return true if all simplex lists in Marching Cubes lookup table
   *    are consistently oriented. (Local)
   *  @param isotable Marching Cubes lookup table.
   */
  template <typename OSTREAM_TYPE, typename _ISOTABLE_TYPE>
  bool check_mcube_table_orientation
  (OSTREAM_TYPE & out, const _ISOTABLE_TYPE & isotable, 
   const bool flag_verbose,
   const TABLE_INDEX_TYPE output_trigger,
   const bool flag_check_all_pairs,
   ERROR & error)
  {
    if (flag_check_all_pairs) {
      return check_mcube_table_orientation_all_pairs
        (out, isotable, flag_verbose, output_trigger, error);
    }
    else {
      return check_mcube_table_orientation
        (out, isotable, flag_verbose, output_trigger, error);
    }
  }


  /*!
   *  @overload
   *  @brief Return true if all simplex lists in Marching Cubes lookup table
   *    are consistently oriented. (No output message.)
   *  - Version that does not print output messages.
   *  @param isotable Marching Cubes lookup table.
   */
  template <typename _ISOTABLE_TYPE>
  bool check_mcube_table_orientation
  (const _ISOTABLE_TYPE & isotable, const bool flag_check_all_pairs,
   ERROR & error)
  {
    NO_OUTPUT_MESSAGE no_output_message;

    // Dummy parameters.
    bool flag_verbose = false;
    int output_trigger(1);

    return check_mcube_table_orientation
      (no_output_message, isotable, flag_verbose, output_trigger,
       flag_check_all_pairs, error);
  }


  // *****************************************************************
  // MCUBE_ISOTABLE_ORIENT_INFO check routines.
  // *****************************************************************

  // Check against isosurface lookup table.
  template <typename _ISOTABLE_TYPE>
  bool MC_TABLE_ORIENT_INFO::Check
  (const _ISOTABLE_TYPE & isotable, ERROR & error) const
  {
    if (NumIsosurfaceVertices() != isotable.NumIsosurfaceVertices()) {
      error.AddToMessage
        ("Programming error. Incorrect value for NumIsosurfaceVertices().");
      return false;
    }
    
    if (NumVerticesPerSimplex() != isotable.NumVerticesPerSimplex()) {
      error.AddToMessage
        ("Programming error. Incorrect value for NumVerticesPerSimplex().");
      return false;
    }

    if (NumTableEntries() != isotable.NumTableEntries()) {
      error.AddToMessage
        ("Programming error. Incorrect value for NumTableEntries().");
      return false;
    }

    for (TABLE_INDEX_TYPE table_index = 0; table_index < isotable.NumTableEntries();
       table_index++) {

      if (NumSimplices(table_index) != isotable.NumSimplices(table_index)) {
        error.AddToMessage
          ("Programming error. Incorrect number of simplices for table entry ",
           table_index, ".");
        return false;
      }

      if (!CheckInSimplexIsSet(table_index, error))
        { return false; }

      for (int isimplex = 0; isimplex < isotable.NumSimplices(table_index);
           isimplex++) {
        for (int j = 0; j < isotable.NumVerticesPerSimplex(); j++) {
          const ISOPATCH_VERTEX_INDEX_TYPE iw =
            isotable.SimplexVertex(table_index, isimplex, j);

          if (!SimplexInfo(table_index, isimplex).InSimplex(iw)) {
            error.AddToMessage
              ("Programming error. Missing isosurface vertex ", int(iw),
               " in table entry ", table_index, ", simplex ", isimplex, ".");
            return false;
          }
        }
      }
    }

    return true;
  }


  // Return true if orientations of simplices in entry[table_indexB] 
  //   are consistent with orientations of simplices in entry[table_indexA].
  // - Version that does not return bitset componentB_checked.
  template <typename _ISOTABLE_TYPE>
  bool MC_TABLE_ORIENT_INFO::CheckOrientationsOfTwoTableEntries
  (const _ISOTABLE_TYPE & isotable,
   const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
   int & isimplexA, int & isimplexB,
   ERROR & error) const
  {
    ISO_CONNECTED_COMPONENT_BITSET componentB_checked;

    return CheckOrientationsOfTwoTableEntries
      (isotable, table_indexA, table_indexB, isimplexA, isimplexB,
       componentB_checked, error);
  }


  // Check orientation of table entry table_indexA against
  //    all other table entries.
  template <typename _ISOTABLE_TYPE>
  bool MC_TABLE_ORIENT_INFO::
  CheckOrientationOfTableEntryAgainstAllOthers
  (const _ISOTABLE_TYPE & isotable,
   const TABLE_INDEX_TYPE table_indexA, 
   int & isimplexA, TABLE_INDEX_TYPE & table_indexB, int & isimplexB,
   ERROR & error) const
  {
    // Initialize.
    isimplexA = 0;
    table_indexB = 0;
    isimplexB = 0;


    for (int table_index = 0; table_index < NumTableEntries();
	 table_index++) {

      if (table_index == table_indexA)
	{ continue; }

      if (!CheckOrientationsOfTwoTableEntries
	  (isotable, table_indexB, table_index, 
	   isimplexA, isimplexB, error)) {

	    table_indexB = table_index;
	    return false;
      }
    }

    return true;
  }


  // Check orientation of table entry table_indexA against
  //   all other table entries. (Doesn't return table index or simplices.)
  template <typename _ISOTABLE_TYPE>
  bool MC_TABLE_ORIENT_INFO::
  CheckOrientationOfTableEntryAgainstAllOthers
  (const _ISOTABLE_TYPE & isotable,
   const TABLE_INDEX_TYPE table_indexA, 
   ERROR & error) const
  {
    TABLE_INDEX_TYPE table_indexB;
    int isimplexA, isimplexB;

    return CheckOrientationOfTableEntryAgainstAllOthers
      (isotable, table_indexA, isimplexA, table_indexB,
       isimplexB, error);
  }


  // *****************************************************************
  // FACET_ISO_VERTEX member functions
  // *****************************************************************

  void FACET_ISO_VERTEX::Init(const MC_TABLE & isotable) {

    num_facets = isotable.Polytope().NumFacets();
    num_isosurface_vertices =
      isotable.NumIsosurfaceVertices();

    in_facet.resize(num_facets);

    for (int ifacet = 0; ifacet < NumFacets(); ifacet++) {
      in_facet[ifacet].reset();

      for (int iw = 0; iw < isotable.NumIsosurfaceVertices(); iw++) {
        if (isotable.IsosurfaceVertex(iw).Type() == ISOSURFACE_VERTEX::VERTEX) {
          const int iv = isotable.IsosurfaceVertex(iw).Face();
          if (isotable.Polytope().IsVertexInFacet(ifacet, iv))
            { in_facet[ifacet][iw] = 1; }
        }
        else if (isotable.IsosurfaceVertex(iw).Type() == ISOSURFACE_VERTEX::EDGE) {
          const int ie = isotable.IsosurfaceVertex(iw).Face();
          const int iend0 = isotable.Polytope().EdgeEndpoint(ie, 0);
          const int iend1 = isotable.Polytope().EdgeEndpoint(ie, 1);
          if (isotable.Polytope().IsVertexInFacet(ifacet, iend0) &&
              isotable.Polytope().IsVertexInFacet(ifacet, iend1))
            { in_facet[ifacet][iw] = 1; }
        }
        else if (isotable.IsosurfaceVertex(iw).Type() == ISOSURFACE_VERTEX::FACET) {
          const int jfacet = isotable.IsosurfaceVertex(iw).Face();
          if (ifacet == jfacet)
            { in_facet[ifacet][iw] = 1; }
        }
        else {
          // Vertex is not on any facet.
          continue;
        }
      }
    }
  };


  // Return string representing bitset in_facet[ifacet].
  std::string FACET_ISO_VERTEX::InFacetStr(const int ifacet) const
  {
    const int num_isov = NumIsosurfaceVertices();
    
    std::string in_facet_str = in_facet[ifacet].to_string();
    in_facet_str = in_facet_str.substr(in_facet[ifacet].size()-num_isov);
    return in_facet_str;
  }

}

#endif

