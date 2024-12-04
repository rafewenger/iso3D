/*!
 *  @file iso3D_MCtable_orient.cpp
 *  @brief Orient Marching Cubes lookup table.
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


#include "iso3D_MCtable_orient.h"

using namespace ISO3D;


// *****************************************************************
// MC_TABLE_ORIENT_INFO member functions
// *****************************************************************

void ISO3D:MC_TABLE_ORIENT_INFO::Init(const MC_TABLE & isotable)
{
  const int num_table_entries = isotable.NumTableEntries();
  num_vertices_per_simplex = isotable.NumVerticesPerSimplex();
  num_isosurface_vertices = isotable.NumIsosurfaceVertices();

  entry.resize(num_table_entries);
  for (TABLE_INDEX_TYPE table_index = 0;
       table_index < NumTableEntries(); table_index++) {
    const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertices =
      isotable.SimplexVertices(table_index);
    const int num_simplices = isotable.NumSimplices(table_index);
    int numc;
    std::vector<_ITYPE> simplex_component;

    entry[table_index].simplex_info.resize(num_simplices);
      
    _FlagVerticesInEachSimplex
      (table_index, simplex_vertices, num_simplices);
    _SetConnectedComponentIndices
      (table_index, simplex_vertices, num_simplices);
    _SetFacetSwapParity(table_index, simplex_vertices, num_simplices);
    _FlagBoundaryFacets(table_index);
  }
};


void ISO3D::MC_TABLE_ORIENT_INFO::_FlagVerticesInEachSimplex
(const TABLE_INDEX_TYPE table_index,
 const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list,
 const int num_simplices)
{
  for (int isimplex = 0; isimplex < num_simplices; isimplex++) {

    entry[table_index].simplex_info[isimplex].in_simplex.reset();

    const ISOPATCH_VERTEX_INDEX_TYPE * first_simplex_vertex =
      simplex_vertex_list + isimplex*NumVerticesPerSimplex();

    for (int j = 0; j < NumVerticesPerSimplex(); j++) {
      const ISOPATCH_VERTEX_INDEX_TYPE jw = first_simplex_vertex[j];
      entry[table_index].simplex_info[isimplex].in_simplex.set(jw);
    }
  }
};
  

void ISO3D::MC_TABLE_ORIENT_INFO::_SetConnectedComponentIndices
(const TABLE_INDEX_TYPE table_index,
 const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list,
 const int num_simplices)
{
  int num_components;
  std::vector<_ITYPE> simplex_component;

  get_facet_connected_components_in_simplicial_complex
    (simplex_vertex_list, NumVerticesPerSimplex(), num_simplices,
     simplex_component, num_components);

  entry[table_index].num_connected_components = num_components;

  for (int isimplex = 0; isimplex < simplex_component.size();
       isimplex++) {
    _SetConnectedComponent
      (table_index, isimplex, simplex_component[isimplex]);
  }
};


// Set facet swap parity in all facets of simplex isimplex
//   in table entry table_index.
void ISO3D::MC_TABLE_ORIENT_INFO::_SetSimplexFacetSwapParity
(const TABLE_INDEX_TYPE table_index,
 const int isimplex,
 const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex,
 ISOPATCH_VERTEX_INDEX_TYPE temp_simplex_vert[])
{
  int swap_parity;
    
  entry[table_index].simplex_info[isimplex].facet_swap_parity.reset();

  for (int jloc = 0; jloc < NumFacetsPerSimplex(); jloc++) {
    const ISOPATCH_VERTEX_INDEX_TYPE jw = simplex_vertex[jloc];

    sort_simplex_facet_vertices
      (simplex_vertex, NumVerticesPerSimplex(), jloc,
       temp_simplex_vert, swap_parity);

    if (swap_parity) {
      entry[table_index].simplex_info[isimplex].facet_swap_parity.set(jw);
    }
  }
};


// Set facet swap parity in all facets of simplex isimplex
//   in table entry table_index. (No temporary array.)
void ISO3D::MC_TABLE_ORIENT_INFO::_SetSimplexFacetSwapParity
(const TABLE_INDEX_TYPE table_index,
 const int isimplex,
 const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex)
{
  std::vector<ISOPATCH_VERTEX_INDEX_TYPE>
    temp_simplex_vert(NumVerticesPerSimplex());

  _SetSimplexFacetSwapParity
    (table_index, isimplex, simplex_vertex, temp_simplex_vert.data());
};


// Set facet swap parity of all facets of all simplices
//   in table entry table_index.
void ISO3D::MC_TABLE_ORIENT_INFO::_SetFacetSwapParity
(const TABLE_INDEX_TYPE table_index, 
 const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertex_list,
 const int num_simplices)
{
  std::vector<ISOPATCH_VERTEX_INDEX_TYPE>
    temp_simplex_vert(NumVerticesPerSimplex());
    
  for (int isimplex = 0; isimplex < num_simplices; isimplex++) {

    const ISOPATCH_VERTEX_INDEX_TYPE * first_simplex_vertex =
      simplex_vertex_list + isimplex*NumVerticesPerSimplex();

    _SetSimplexFacetSwapParity
      (table_index, isimplex, first_simplex_vertex,
       temp_simplex_vert.data());
  }

};


void ISO3D::MC_TABLE_ORIENT_INFO::_FlagBoundaryFacets
(const TABLE_INDEX_TYPE table_index)
{
  PROCEDURE_ERROR
    error("MCUBE_ISOTABLE_ORIENT_INFO::_FlagBoundaryFacets");

  if (!CheckInSimplexIsSet(table_index, error)) {
    error.AddMessage
      ("  Call _FlagVerticesInEachSimplex() before _FlagBoundaryFacets().");
    throw error;
  }

  // Compute boundary simplex facets.
  for (int isimplexA = 0; isimplexA < NumSimplices(table_index);
       isimplexA++) {

    entry[table_index].simplex_info[isimplexA].is_boundary_facet.reset();
    const ISO_VERTEX_BITSET_TYPE in_simplexA =
      entry[table_index].simplex_info[isimplexA].in_simplex;

    // Initialize is_boundary_facet to true for all facets.
    entry[table_index].simplex_info[isimplexA].is_boundary_facet =
      in_simplexA;
      
    for (int isimplexB = 0; isimplexB < NumSimplices(table_index);
         isimplexB++) {
        
      if (isimplexA == isimplexB)
        { continue; }

      const ISO_VERTEX_BITSET_TYPE in_simplexB =
        entry[table_index].simplex_info[isimplexB].in_simplex;

      const ISO_VERTEX_BITSET_TYPE shared_vert =
        in_simplexA & in_simplexB;

      const int num_ones = shared_vert.count();
        
      if (num_ones == NumVerticesPerSimplex()) {
        // Duplicate simplex in list of simplices?!?
        // Ignore simplexB.
        continue;
      }

      if (num_ones == NumVerticesPerSimplexFacet()) {
        // Simplices isimplexA and isimplexB share a facet.
        // Mark facet as not on boundary.

        // Compute bit indicating vertex not in facet.
        ISO_VERTEX_BITSET_TYPE not_in_facet_bitset =
          in_simplexA ^ shared_vert;


        // Flip bit to off.
        not_in_facet_bitset.flip();
        entry[table_index].simplex_info[isimplexA].is_boundary_facet &=
          not_in_facet_bitset;
      }
    }
  }
};


// Return true if all table entries are marked as oriented.
bool ISO3D::MC_TABLE_ORIENT_INFO::AreAllTableEntriesOriented
(TABLE_INDEX_TYPE & table_index) const
{
  table_index = 0;
  for (TABLE_INDEX_TYPE table_indexB = 0; table_indexB < NumTableEntries();
       table_indexB++) {

    if (!AreAllSimplicesOriented(table_indexB)) {
      table_index = table_indexB;
      return false;
    }
  }

  return true;
}
  

// Return true if some simplex in connected component
//   icomponent has a boundary facet.
bool ISO3D::MC_TABLE_ORIENT_INFO::ComponentHasBoundaryFacet
(const TABLE_INDEX_TYPE table_index, const int icomponent) const
{
  for (int isimplex = 0; isimplex < NumSimplices(table_index); isimplex++) {
    if (IndexOfConnectedComponent(table_index, isimplex) == icomponent) {
      if (SimplexInfo(table_index, isimplex).HasBoundaryFacet())
        { return true; }
    }
  }

  return false;
}


// Return true if some simplex in table entry has a boundary facet.
bool ISO3D::MC_TABLE_ORIENT_INFO::SomeSimplexHasBoundaryFacet
(const TABLE_INDEX_TYPE table_index) const
{
  for (int isimplex = 0; isimplex < NumSimplices(table_index); isimplex++) {
    if (SimplexInfo(table_index, isimplex).HasBoundaryFacet())
      { return true; }
  }

  return false;
}


// Find index of table entry with a single connected component 
//   that has at least one boundary simplex facet.
void ISO3D::MC_TABLE_ORIENT_INFO::
FindEntryWithSingleConnectedComponentWithBoundary
(TABLE_INDEX_TYPE table_index, bool & flag_found) const
{
  const int ONE(1);

  // Initialize.
  table_index = 0;
  flag_found = false;

  for (TABLE_INDEX_TYPE table_indexA = 0; table_indexA < NumTableEntries();
       table_indexA++) {

    if (NumConnectedComponents(table_indexA) == ONE) {

      // Single connected component has index 0.
      if (ComponentHasBoundaryFacet(table_indexA, 0)) {
        table_index = table_indexA;
        flag_found = true;
        return;
      }
    }
  }
}


// Convert ISO_VERTEX_BITSET_TYPE to string.
std::stringvoid ISO3D::MC_TABLE_ORIENT_INFO::
_ConvertBitsetToString(const ISO_VERTEX_BITSET_TYPE & bitset) const
{
  const int num_isov = NumIsosurfaceVertices();
    
  std::string bitset_str = bitset.to_string();
  bitset_str = bitset_str.substr(bitset.size()-num_isov);
  return bitset_str;
};
  

// Set flag is_connected_component_oriented 
//   for connected component icomponent.
void ISO3D::MC_TABLE_ORIENT_INFO::SetIsConnectedComponentOriented
(const TABLE_INDEX_TYPE table_index, const int icomponent,
 const bool flag)
{
  entry[table_index].is_connected_component_oriented[icomponent] = flag;
  entry[table_index].num_oriented_connected_components =
    entry[table_index].is_connected_component_oriented.count();
};


// Update flag entry[table_index].is_connected_component_oriented. 
//   - Update by "or" with is connected_component_oriented.
// - Also modifies entry[table_index].num_oriented_connected_components.
void ISO3D::MC_TABLE_ORIENT_INFO::UpdateIsConnectedComponentOriented
(const TABLE_INDEX_TYPE table_index, 
 const ISO_CONNECTED_COMPONENT_BITSET & is_component_oriented)
{
  entry[table_index].is_connected_component_oriented |=
    is_component_oriented;
  entry[table_index].num_oriented_connected_components =
    entry[table_index].is_connected_component_oriented.count();
}

// Return true if two simplices share a facet.
// - Also returns bitset of shared vertices, number of shared vertices,
//   comparison of facet swap parities, and bitsets indicating 
//   unshared vertices.
bool ISO3D::MC_TABLE_ORIENT_INFO::DoSimplicesShareFacet
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 const int isimplexA, const int isimplexB,
 ISO_VERTEX_BITSET_TYPE & shared_vert,
 int & num_shared_vertices,
 bool & are_shared_facet_swap_parities_equal,
 ISO_VERTEX_BITSET_TYPE & not_in_facetA,
 ISO_VERTEX_BITSET_TYPE & not_in_facetB) const
{
  const ISO_VERTEX_BITSET_TYPE in_simplexA =
    SimplexInfo(table_indexA, isimplexA).in_simplex;
  const ISO_VERTEX_BITSET_TYPE in_simplexB =
    SimplexInfo(table_indexB, isimplexB).in_simplex;

  shared_vert = in_simplexA & in_simplexB;
  num_shared_vertices = shared_vert.count();

  if (num_shared_vertices == NumVerticesPerSimplex()) {

    // Duplicate simplex.
    const ISO_VERTEX_BITSET_TYPE facet_swap_parityA =
      SimplexInfo(table_indexA, isimplexA).FacetSwapParity();
    const ISO_VERTEX_BITSET_TYPE facet_swap_parityB =
      SimplexInfo(table_indexB, isimplexB).FacetSwapParity();

    are_shared_facet_swap_parities_equal =
      (facet_swap_parityA == facet_swap_parityB);

    return true;
  }
  else if (num_shared_vertices == NumVerticesPerSimplexFacet()) {
      
    // Simplices share one facet.
    const ISO_VERTEX_BITSET_TYPE facet_swap_parityA =
      SimplexInfo(table_indexA, isimplexA).facet_swap_parity;
    const ISO_VERTEX_BITSET_TYPE facet_swap_parityB =
      SimplexInfo(table_indexB, isimplexB).facet_swap_parity;
      
    not_in_facetA = in_simplexA ^ shared_vert;
    not_in_facetB = in_simplexB ^ shared_vert;

    const bool shared_facet_swap_parityA =
      bool((not_in_facetA & facet_swap_parityA) == 0);
    const bool shared_facet_swap_parityB =
      bool((not_in_facetB & facet_swap_parityB) == 0);

    are_shared_facet_swap_parities_equal =
      (shared_facet_swap_parityA == shared_facet_swap_parityB);

    return true;
  }
  else {
    // Clear arguments. (For code safety.)
    are_shared_facet_swap_parities_equal = false;
    not_in_facetA.reset();
    not_in_facetB.reset();

    // Simplices do not share a facet.
    return false;
  }

}


// Return true if two simplices share a facet.
//  - Version that returns only comparison of swap parities.
bool ISO3D::MC_TABLE_ORIENT_INFO::DoSimplicesShareFacet
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 const int isimplexA, const int isimplexB,
 bool & are_shared_facet_swap_parities_equal) const
{
  ISO_VERTEX_BITSET_TYPE  shared_vert;
  int num_shared_vertices;
  ISO_VERTEX_BITSET_TYPE not_in_facetA;
  ISO_VERTEX_BITSET_TYPE not_in_facetB;

  return DoSimplicesShareFacet
    (table_indexA, table_indexB, isimplexA, isimplexB,
     shared_vert, num_shared_vertices,
     are_shared_facet_swap_parities_equal,
     not_in_facetA, not_in_facetB);
}


// Return true if two simplices share a boundary facet.
// - Also returns bitset of shared vertices, number of shared vertices,
//   comparison of facet swap parities, and bitsets indicating 
//   unshared vertices.
bool ISO3D::MC_TABLE_ORIENT_INFO::DoSimplicesShareBoundaryFacet
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 const int isimplexA, const int isimplexB,
 ISO_VERTEX_BITSET_TYPE & shared_vert,
 int & num_shared_vertices,
 bool & are_shared_facet_swap_parities_equal,
 ISO_VERTEX_BITSET_TYPE & not_in_facetA,
 ISO_VERTEX_BITSET_TYPE & not_in_facetB) const
{
  if (DoSimplicesShareFacet
      (table_indexA, table_indexB, isimplexA, isimplexB,
       shared_vert, num_shared_vertices,
       are_shared_facet_swap_parities_equal,
       not_in_facetA, not_in_facetB)) {

    // Simplices A and B have a common facet.

    const ISO_VERTEX_BITSET_TYPE is_boundary_facetA =
      SimplexInfo(table_indexA, isimplexA).IsBoundaryFacet();
    const ISO_VERTEX_BITSET_TYPE is_boundary_facetB =
      SimplexInfo(table_indexB, isimplexB).IsBoundaryFacet();

    if (num_shared_vertices == NumVerticesPerSimplex()) {

      // Duplicate simplex.

      if ((is_boundary_facetA & is_boundary_facetB).none()) {
        // No facet is a boundary facet in both table entries.
        return false;
      }

      return true;
    }
    else {
      // Simplices share exactly one facet.
	
      if ((not_in_facetA & is_boundary_facetA).none()) {
        // Shared facet is not a boundary facet in table_indexA.
        return false;
      }

      if ((not_in_facetB & is_boundary_facetB).none()) {
        // Shared facet is not a boundary facet in table_indexB.
        return false;
      }

      // Shared facet is a boundary facet in both table entries.
      return true;
    }
  }

  return false;
}


// Return true if two simplices share a boundary facet.
// - Version that does not return bitsets indicating 
//   unshared vertices.
bool ISO3D::MC_TABLE_ORIENT_INFO::DoSimplicesShareBoundaryFacet
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 const int isimplexA, const int isimplexB,
 ISO_VERTEX_BITSET_TYPE & shared_vert,
 int & num_shared_vertices,
 bool & are_shared_facet_swap_parities_equal) const
{
  ISO_VERTEX_BITSET_TYPE not_in_facetA;
  ISO_VERTEX_BITSET_TYPE not_in_facetB;

  return DoSimplicesShareBoundaryFacet
    (table_indexA, table_indexB, isimplexA, isimplexB,
     shared_vert, num_shared_vertices,
     are_shared_facet_swap_parities_equal,
     not_in_facetA, not_in_facetB);
}

  
// Orient all simplices in connected component containing isimplexA
//   consistently with isimplexA.
void ISO3D::MC_TABLE_ORIENT_INFO::OrientConnectedComponent
(const TABLE_INDEX_TYPE table_index, const int isimplexA,
 MC_TABLE & isotable)
{
  const int num_simplices = NumSimplices(table_index);    
  std::vector<bool> is_oriented(num_simplices, false);
  std::stack<int> simplex_stack;

  is_oriented[isimplexA] = true;
  simplex_stack.push(isimplexA);
  while (simplex_stack.size() != 0) {
    const int isimplexB = simplex_stack.top();
    simplex_stack.pop();      

    const int icomponentB = 
      IndexOfConnectedComponent(table_index, isimplexB);

    for (int isimplexC = 0; isimplexC < num_simplices; isimplexC++) {

      if (isimplexB == isimplexC)
        { continue; }
        
      if (is_oriented[isimplexC]) {
        // Simplex C is already oriented.
        continue;
      }

      const int icomponentC =
        IndexOfConnectedComponent(table_index, isimplexC);

      if (icomponentB != icomponentC) {
        // isimplexB and isimplexC are in different connected components.
        continue;
      }

      bool are_shared_facet_swap_parities_equal;

      if (DoSimplicesShareFacet
          (table_index, table_index, isimplexB, isimplexC,
           are_shared_facet_swap_parities_equal)) {

        if (are_shared_facet_swap_parities_equal) {
          // Simplex C has opposite orientation to simplexB.
          isotable.FlipIsoPolyOrientation(table_index, isimplexC);

          // Set facet swap parity.
          const ISOPATCH_VERTEX_INDEX_TYPE * simplexC_vertices =
            isotable.SimplexVertices(table_index, isimplexC);

          _SetSimplexFacetSwapParity
            (table_index, isimplexC, simplexC_vertices);
        }

        is_oriented[isimplexC] = true;
        simplex_stack.push(isimplexC);
      }
    }
  }
}

  
// Consistently orient all simplices in each
//   connected component in table entry table_index.
void ISO3D::MC_TABLE_ORIENT_INFO::OrientAllSimplicesInTableEntry
(const TABLE_INDEX_TYPE table_index, MC_TABLE & isotable)
{
  const int num_simplices = NumSimplices(table_index);    
  const int num_connected_components =
    NumConnectedComponents(table_index);
  std::vector<bool> is_oriented(num_connected_components, false);

  for (int isimplex = 0; isimplex < num_simplices; isimplex++) {
    const int icomponent =
      SimplexInfo(table_index, isimplex).IndexOfConnectedComponent();

    if (!is_oriented[icomponent]) {
      OrientConnectedComponent(table_index, isimplex, isotable);
      is_oriented[icomponent] = true;
    }
  }      
};


// Flip all simplices in connected component icomponent
//   in table entry table_index.
void ISO3D::MC_TABLE_ORIENT_INFO::FlipSimplicesInConnectedComponent
(const TABLE_INDEX_TYPE table_index, const int icomponent,
 MC_TABLE & isotable)
{
  for (int isimplex = 0; isimplex < NumSimplices(table_index);
       isimplex++) {
    if (SimplexInfo(table_index,isimplex).IndexOfConnectedComponent() ==
        icomponent) {
      const ISOPATCH_VERTEX_INDEX_TYPE * simplex_vertices =
        isotable.SimplexVertices(table_index, isimplex);
        
      isotable.FlipIsoPolyOrientation(table_index,isimplex);
      _SetSimplexFacetSwapParity
        (table_index, isimplex, simplex_vertices);
    }
  }
};
  

// Orient simplices in table entry table_indexB
//   from orientation of some boundary facet of table_indexA.
void ISO3D::MC_TABLE_ORIENT_INFO::OrientTwoTableEntries
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 MC_TABLE & isotable)
{
  ISO_VERTEX_BITSET_TYPE shared_vert;
  int num_shared_vertices;

  if (AreAllSimplicesOriented(table_indexB)) {
    // Nothing left to orient.
    return;
  }

  for (int isimplexB = 0; isimplexB < NumSimplices(table_indexB);
       isimplexB++) {

    if (IsSimplexOriented(table_indexB, isimplexB))
      { continue; }

    bool flag_matched = false;
    for (int isimplexA = 0; 
         isimplexA < NumSimplices(table_indexA) && !flag_matched;
         isimplexA++) {

      bool are_shared_facet_swap_parities_equal;
      if (DoSimplicesShareBoundaryFacet
          (table_indexA, table_indexB, isimplexA, isimplexB,
           shared_vert, num_shared_vertices, 
           are_shared_facet_swap_parities_equal)) {

        for (int ifacet = 0; ifacet < facet_info.NumFacets(); ifacet++) {
          if (facet_info.AreVerticesInFacet(shared_vert, ifacet)) {

            if (isotable.AreAllFacetVertexLabelsIdentical
                (table_indexA, table_indexB, ifacet)) {

              const int icomponentB = 
                IndexOfConnectedComponent(table_indexB, isimplexB);

              if (!are_shared_facet_swap_parities_equal) {

                // Simplices have different orientations.

                // Reverse orientaton of all simplices in icomponentB.
                FlipSimplicesInConnectedComponent
                  (table_indexB, icomponentB, isotable);

                // Shared facet swap parities are now equal.
                are_shared_facet_swap_parities_equal = true;
              }

              // Flag connected component icomponentB as oriented.
              SetIsConnectedComponentOriented
                (table_indexB, icomponentB, true);

              if (AreAllSimplicesOriented(table_indexB)) {
                // All simplices are oriented.
                return;
              }

              flag_matched = true;
              break;
            }
          }
        }
      }
    }
  }
}
  

// *****************************************************************
// MCUBE_ISOTABLE_ORIENT_INFO check routines.
// *****************************************************************

// Check against isosurface lookup table.
template <typename _ISO_VERTEX_BITSET_TYPE,
          typename _ISO_CONNECTED_COMPONENT_BITSET_TYPE,
          typename _ITYPE>
template <typename _ISOTABLE_TYPE>
bool MCUBE_ISOTABLE_ORIENT_INFO
<_ISO_VERTEX_BITSET_TYPE,_ISO_CONNECTED_COMPONENT_BITSET_TYPE,_ITYPE>::
Check(const _ISOTABLE_TYPE & isotable, ERROR & error) const
{
  if (NumIsosurfaceVertices() != isotable.NumIsosurfaceVertices()) {
    error.AddMessage
      ("Programming error. Incorrect value for NumIsosurfaceVertices().");
    return false;
  }
    
  if (NumVerticesPerSimplex() != isotable.NumVerticesPerSimplex()) {
    error.AddMessage
      ("Programming error. Incorrect value for NumVerticesPerSimplex().");
    return false;
  }

  if (NumTableEntries() != isotable.NumTableEntries()) {
    error.AddMessage
      ("Programming error. Incorrect value for NumTableEntries().");
    return false;
  }

  for (TABLE_INDEX_TYPE table_index = 0; table_index < isotable.NumTableEntries();
       table_index++) {

    if (NumSimplices(table_index) != isotable.NumSimplices(table_index)) {
      error.AddMessage
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
          error.AddMessage
            ("Programming error. Missing isosurface vertex ", int(iw),
             " in table entry ", table_index, ", simplex ", isimplex, ".");
          return false;
        }
      }
    }
  }

  return true;
}


// Check that bitset entry[table_index].simplex_info[*].in_simplex is set.
bool MC_TABLE_ORIENT_INFO::CheckInSimplexIsSet
(const TABLE_INDEX_TYPE table_index, ERROR & error) const
{
  for (int isimplex = 0; isimplex < NumSimplices(table_index);
       isimplex++) {
           
    const int num_ones =
      SimplexInfo(table_index,isimplex).in_simplex.count();
    
    if (num_ones != NumVerticesPerSimplex()) {
      if (num_ones == 0) {
        error.AddMessage
          ("Programming error. Bitset in_simplex not set.");
        error.AddMessage
          ("  Table index: ", table_index, "  Simplex: ", isimplex);
      }
      else {
        error.AddMessage
          ("Programming error. Incorrect number of ones bitset in_simplex not set.");
        error.AddMessage
          ("  Table index: ", table_index, "  Simplex: ", isimplex);
        error.AddMessage
          ("  Bitset in_simplex has ", num_ones, " ones.");
        error.AddMessage
          ("  Number of ones should match number of simplex vertices, ",
           NumVerticesPerSimplex(), ".");
      }
      
      return false;
    }
  }

  return true;
}

  

// Check that orientations of all pairs of simplices in entry[table_index]
//   are consistent.
// - Return true if all orientations in entry[table_index] are consistent.
bool MC_TABLE_ORIENT_INFO::CheckOrientationsInTableEntry
(const TABLE_INDEX_TYPE table_index, int & isimplexA, int & isimplexB,
 ERROR & error) const
{
  isimplexA = 0;
  isimplexB = 0;
    
  for (int jsimplexA = 0; jsimplexA+1 < entry[table_index].NumSimplices();
       jsimplexA++) {
    for (int jsimplexB = jsimplexA+1; 
         jsimplexB < entry[table_index].NumSimplices();
         jsimplexB++) {

      bool are_shared_facet_swap_parities_equal;
      if (DoSimplicesShareFacet
          (table_index, table_index, jsimplexA, jsimplexB,
           are_shared_facet_swap_parities_equal)) {

        if (are_shared_facet_swap_parities_equal) {

          isimplexA = jsimplexA;
          isimplexB = jsimplexB;

          // Simplex B has opposite orientation to simplex A.
          error.AddMessage
            ("Simplices ", isimplexA, " and ", isimplexB,
             " in table entry ", table_index,
             " are not consistently oriented.");

          return false;
        }
      }
    }
  }
    
  return true;
}


// Check that orientations of all pairs of simplices 
//   in entry[table_index] are consistent. (Doesn't return simplices.)
// - Version that does not return mismatched simplices.
bool MC_TABLE_ORIENT_INFO::CheckOrientationsInTableEntry
(const TABLE_INDEX_TYPE table_index, ERROR & error) const
{
  int isimplexA, isimplexB;
    
  return CheckOrientationsInTableEntry
    (table_index, isimplexA, isimplexB, error);
}


// Check that orientations in every table entry are consistent.
//  - Does NOT compare orientations of simplices in DIFFERENT table entries.
bool MC_TABLE_ORIENT_INFO::CheckOrientationsInEveryTableEntry
(TABLE_INDEX_TYPE & table_index, int & isimplexA, int & isimplexB,
 ERROR & error) const
{
  const int num_vert_per_simplex = NumVerticesPerSimplex();
  int jsimplexA, jsimplexB;

  // Initialize.
  table_index = 0;
  isimplexA = isimplexB = 0;

  if (num_vert_per_simplex < 2) {
    // Nothing to check
    return true;
  }

  for (TABLE_INDEX_TYPE table_indexA = 0;
       table_indexA < NumTableEntries(); table_indexA++) {

    if (!CheckOrientationsInTableEntry
        (table_indexA, jsimplexA, jsimplexB, error)) {

      table_index = table_indexA;
      isimplexA = jsimplexA;
      isimplexB = jsimplexB;

      return false;
    }
  }

  return true;
}


// Check that orientations in every table entry are consistent. 
//    (Doesn't return table index or simplices.)
//  - Does NOT compare orientations of simplices in DIFFERENT table entries.
//  - Version that does not return index of table entry or 
//    mismatched simplices.
template <typename _ISO_VERTEX_BITSET_TYPE,
          typename _ISO_CONNECTED_COMPONENT_BITSET_TYPE,
          typename _ITYPE>
bool MCUBE_ISOTABLE_ORIENT_INFO
<_ISO_VERTEX_BITSET_TYPE,_ISO_CONNECTED_COMPONENT_BITSET_TYPE,_ITYPE>::
CheckOrientationsInEveryTableEntry(ERROR & error) const
{
  TABLE_INDEX_TYPE table_index;
  int isimplexA, isimplexB;

  return CheckOrientationsInEveryTableEntry
    (table_index, isimplexA, isimplexB, error);
}


// Return true if orientations of simplices in entry[table_indexB] 
//   are consistent with orientations of simplices in entry[table_indexA].
bool MC_TABLE_ORIENT_INFO::CheckOrientationsOfTwoTableEntries
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 int & isimplexA, int & isimplexB,
 _ISO_CONNECTED_COMPONENT_BITSET_TYPE & componentB_checked,
 ERROR & error) const
{
  ISO_VERTEX_BITSET_TYPE shared_vert;
  int num_shared_vertices;

  // Initialize.
  componentB_checked.reset();

  for (int jsimplexB = 0; jsimplexB < NumSimplices(table_indexB);
       jsimplexB++) {

    const int icomponentB =
      IndexOfConnectedComponent(table_indexB, jsimplexB);

    if (bool(componentB_checked[icomponentB])) {
      // Orientation of some simplex in connected component icomponentB
      //   has already been checked.
      continue;
    }

    bool flag_checked = false;
    for (int jsimplexA = 0; 
         jsimplexA < NumSimplices(table_indexA) && !flag_checked;
         jsimplexA++) {

      bool are_shared_facet_swap_parities_equal;
      if (DoSimplicesShareBoundaryFacet
          (table_indexA, table_indexB, jsimplexA, jsimplexB,
           shared_vert, num_shared_vertices,
           are_shared_facet_swap_parities_equal)) {

        for (int ifacet = 0; ifacet < facet_info.NumFacets(); ifacet++) {
          if (facet_info.AreVerticesInFacet(shared_vert, ifacet)) {

            if (AreAllFacetVertexLabelsIdentical
                (table_indexA, table_indexB, ifacet)) {

              if (!are_shared_facet_swap_parities_equal) {

                // Simplices have different orientations.
                isimplexA = jsimplexA;
                isimplexB = jsimplexB;

                error.AddMessage
                  ("  Simplex ", isimplexA, " in table entry ", table_indexA,
                   " has inconsistent orientation");
                error.AddMessage
                  ("  with simplex ", isimplexB, " in table entry ",
                   table_indexB, ".");

                return false;
              }

              // Flag connected component icomponentB as checked.
              componentB_checked[icomponentB] = 1;
              flag_checked = true;

              continue;
            }
          }
        }
      }
    }
  }

  return true;
}

  
// Return true if orientations of simplices in entry[table_indexB] 
//   are consistent with orientations of simplices in entry[table_indexA].
// - Version that does not return bitset componentB_checked.
bool MC_TABLE_ORIENT_INFO::CheckOrientationsOfTwoTableEntries
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 int & isimplexA, int & isimplexB,
 ERROR & error) const
{
  ISO_CONNECTED_COMPONENT_BITSET_TYPE  componentB_checked;

  return CheckOrientationsOfTwoTableEntries
    (table_indexA, table_indexB, isimplexA, isimplexB,
     componentB_checked, error);
}


// Check orientation of table entry table_indexA against
//    all other table entries.
bool MC_TABLE_ORIENT_INFO::CheckOrientationOfTableEntryAgainstAllOthers
(const TABLE_INDEX_TYPE table_indexA, 
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
        (table_indexB, table_index, 
         isimplexA, isimplexB, error)) {

	    table_indexB = table_index;
	    return false;
    }
  }

  return true;
}


// Check orientation of table entry table_indexA against
//   all other table entries. (Doesn't return table index or simplices.)
bool MC_TABLE_ORIENT_INFO::CheckOrientationOfTableEntryAgainstAllOthers
(const TABLE_INDEX_TYPE table_indexA, 
 ERROR & error) const
{
  TABLE_INDEX_TYPE table_indexB;
  int isimplexA, isimplexB;

  return CheckOrientationOfTableEntryAgainstAllOthers
    (table_indexA, isimplexA, table_indexB,
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

