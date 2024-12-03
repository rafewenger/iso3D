/*!
 *  @file iso3D_MCtable.cpp
 *  @brief Marching cubes lookup table.
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

#include <algorithm>

#include "iso3D_bits.h"
#include "iso3D_MCtable.h"
#include "iso3D_MCtable_orient.h"

using namespace ISO3D;


// ***************************************************************
// TABLE MANIPULATION ROUTINES
// ***************************************************************

void ISO3D::invert_mcube_isotable
(const MC_TABLE & isotableA, MC_TABLE & isotableB)
{
  const TABLE_INDEX_TYPE num_table_entries =
    isotableA.NumTableEntries();
  isotableB.CopyPolytope(isotableA);
  isotableB.CopyIsosurfaceVertices(isotableA);
  isotableB.SetNumTableEntries(num_table_entries);
  isotableB.SetSimplexDimension(isotableA.SimplexDimension());
  isotableB.CopyProperties(isotableA);

  // Inverted table has opposite separation type and orientation.
  isotableB.SetSeparationType(isotableA.OppositeSeparationType());
  isotableB.SetIsoPolyOrientation(isotableA.OppositeIsoPolyOrientation());

  for (TABLE_INDEX_TYPE itB = 0; itB < num_table_entries; itB++) {
    const TABLE_INDEX_TYPE itA = num_table_entries - itB - 1;
    const int num_simplices = isotableA.NumSimplices(itA);
    isotableB.SetSimplexVertices
      (itB, isotableA.SimplexVertices(itA), num_simplices);
  }
}


// *******************************************************************
// MC_TABLE_PROPERTIES member functions
// *******************************************************************

void MC_TABLE_PROPERTIES::Init()
{
  lookup_table_type = table_type_list.UndefinedValue();
  encoding = encoding_list.UndefinedValue();
  grid_vertex_label_type = grid_vertex_label_type_list.UndefinedValue();
  isosurface_separation_type = 
    isosurface_separation_type_list.UndefinedValue();
  isosurface_triangulation_type =
    isosurface_triangulation_type_list.UndefinedValue();
  iso_poly_orientation =
    iso_poly_orientation_list.UndefinedValue();
  separate_opposite =
    separate_opposite_type_list.UndefinedValue();
}


std::string MC_TABLE_PROPERTIES::SeparationTypeLabel() const
{
  if (isosurface_separation_type == SEPARATE_NEG) 
    { return std::string("sepNeg"); }
  else if (isosurface_separation_type == SEPARATE_POS) 
    { return std::string("sepPos"); }
  else 
    { return std::string(""); }
}


std::string MC_TABLE_PROPERTIES::IsoPolyOrientationLabel() const
{
  if (iso_poly_orientation == POSITIVE_ORIENT) 
    { return std::string("posO"); }
  else if (iso_poly_orientation == NEGATIVE_ORIENT) 
    { return std::string("negO"); }
  else 
    { return std::string(""); }
}

std::string MC_TABLE_PROPERTIES::TriangulationTypeLabel() const
{
  if (isosurface_triangulation_type == CONVEX_HULL)
    { return std::string("cHull"); }
  else if (isosurface_triangulation_type == EDGE_GROUPS)
    { return std::string("edgeGroups"); }
  else 
    { return std::string(""); }
}


// Return opposite separation type.
ISOSURFACE_SEPARATION_TYPE MC_TABLE_PROPERTIES::
OppositeSeparationType() const
{
  if (isosurface_separation_type == SEPARATE_NEG)
    { return SEPARATE_POS; }
  else if (isosurface_separation_type == SEPARATE_POS)
    { return SEPARATE_NEG; }
  else {
    // All other values are unchanged.
    return isosurface_separation_type;
  }
}


// Return opposite orientation type.
ISO_POLY_ORIENTATION MC_TABLE_PROPERTIES::
OppositeIsoPolyOrientation() const
{
  if (iso_poly_orientation == NEGATIVE_ORIENT)
    { return POSITIVE_ORIENT; }
  else if (iso_poly_orientation == POSITIVE_ORIENT)
    { return NEGATIVE_ORIENT; }
  {
    // All other values are unchanged.
    return iso_poly_orientation;
  }
}


// Copy properties from isotable_properties.
void MC_TABLE_PROPERTIES::Copy
(const MC_TABLE_PROPERTIES & isotable_properties)
{
  lookup_table_type = isotable_properties.TableType();
  encoding = isotable_properties.Encoding();
  grid_vertex_label_type = isotable_properties.GridVertexLabelType();
  isosurface_triangulation_type = 
    isotable_properties.TriangulationType();
  isosurface_separation_type = isotable_properties.SeparationType();
  separate_opposite = isotable_properties.SeparateOpposite();
  iso_poly_orientation = isotable_properties.IsoPolyOrientation();
}


// Return true if isotable has properties specified in properties.
bool MC_TABLE_PROPERTIES::Check
(const MC_TABLE_PROPERTIES & properties,
 ERROR & error) const
{
  const bool flag0 =
    CheckTableType(properties.TableType(), error);
  const bool flag1 =
    CheckEncoding(properties.Encoding(), error);
  const bool flag2 = 
    CheckTriangulationType(properties.TriangulationType(), error);
  const bool flag3 =
    CheckSeparationType(properties.SeparationType(), error);
  const bool flag4 =
    CheckOrientation(properties.IsoPolyOrientation(), error);
  const bool flag5 =
    CheckSeparateOpposite(properties.SeparateOpposite(), error);

  return (flag0 && flag1 && flag2 && flag3 && flag4 && flag5);
}


// Return true if lookup_table_type matches table_type.
bool MC_TABLE_PROPERTIES::CheckTableType
(const LOOKUP_TABLE_TYPE table_type, ERROR & error) const
{
  if (table_type_list.IsUndefined(table_type)) {
    // Nothing to check.
    return true;
  }

  if (table_type != TableType()) {
    error.AddToMessage("Incorrect isosurface lookup table type.");
    error.AddToMessage
      ("  Isotable type: ", TableTypeString());
    error.AddToMessage
      ("  Expected table type: ", TableTypeString(table_type));

    return false;
  }

  return true;
}


// Return true if this->encoding matches encoding.
bool MC_TABLE_PROPERTIES::CheckEncoding
(const ENCODING encoding, ERROR & error) const
{
  if (encoding_list.IsUndefined(encoding)) {
    // Nothing to check.
    return true;
  }

  if (encoding != Encoding()) {
    error.AddToMessage("Incorrect isosurface lookup table encoding.");
    error.AddToMessage
      ("  Isotable encoding: ", EncodingString());
    error.AddToMessage("  Expected encoding: ", EncodingString(encoding));

    return false;
  }

  return true;
}


// Return true if isosurface_triangulation_type matches tri_type.
bool MC_TABLE_PROPERTIES::CheckTriangulationType
(const ISOSURFACE_TRIANGULATION_TYPE tri_type,
 ERROR & error) const
{
  if (isosurface_triangulation_type_list.IsUndefined(tri_type)) {
    // Nothing to check.
    return true;
  }

  if (tri_type == UNKNOWN_ISOSURFACE_TRIANGULATION_TYPE) {
    // Nothing to check.
    return true;
  }

  if (tri_type != TriangulationType()) {
    error.AddToMessage("Incorrect isosurface lookup table triangulation type.");
    error.AddToMessage
      ("  Isotable triangulation type: ", TriangulationTypeString());
    error.AddToMessage
      ("  Expected triangulation type: ", 
       TriangulationTypeString(tri_type));

    return false;
  }

  return true;
}


// Return true if iso poly orientation matches _orientation.
bool MC_TABLE_PROPERTIES::CheckOrientation
(const ISO_POLY_ORIENTATION _orientation,
 ERROR & error) const
{
  if (iso_poly_orientation_list.IsUndefined(_orientation)) {
    // Nothing to check.
    return true;
  }

  if (_orientation == NO_ORIENT) {
    // Nothing to check.
    return true;
  }

  if (_orientation != IsoPolyOrientation()) {
    error.AddToMessage("Incorrect isosurface lookup table polytope orientation.");
    error.AddToMessage
      ("  Isotable polytope orientation: ", IsoPolyOrientationString());
    error.AddToMessage
      ("  Expected polytope orientation: ", 
       IsoPolyOrientationString(_orientation));

    return false;
  }

  return true;
}


// Return true if isosurface_separation_type matches _sep_type.
bool MC_TABLE_PROPERTIES::CheckSeparationType
(const ISOSURFACE_SEPARATION_TYPE _sep_type, ERROR & error) const
{
  if (isosurface_separation_type_list.IsUndefined(_sep_type)) {
    // Nothing to check.
    return true;
  }

  if (_sep_type == UNKNOWN_SEPARATION_TYPE) {
    // Nothing to check.
    return true;
  }

  if (_sep_type != SeparationType()) {
    error.AddToMessage("Incorrect isosurface lookup table separation type.");
    error.AddToMessage
      ("  Isotable separation type: ", SeparationTypeString());
    error.AddToMessage
      ("  Expected separation type: ", SeparationTypeString(_sep_type));

    return false;
  }

  return true;
}


// Return true if this->separate_opposite matches _sep_opposite.
bool MC_TABLE_PROPERTIES::CheckSeparateOpposite
(const SEPARATE_OPPOSITE_TYPE _sep_opposite, ERROR & error) const
{
  if (separate_opposite_type_list.IsUndefined(_sep_opposite)) {
    // Nothing to check.
    return true;
  }

  if (_sep_opposite != SeparateOpposite()) {
    error.AddToMessage("Incorrect isosurface lookup table separate opposite flag.");
    error.AddToMessage
      ("  Isotable separate opposite: ", SeparateOppositeString());
    error.AddToMessage
      ("  Expected separate opposit: ", SeparateOppositeString(_sep_opposite));

    return false;
  }

  return true;
}


// *******************************************************************
// MC_TABLE member functions
// *******************************************************************

// Constructor.
MC_TABLE::MC_TABLE_ENTRY::MC_TABLE_ENTRY()
{
  num_simplices = 0;
  simplex_vertex_list = NULL;
}

MC_TABLE::MC_TABLE_ENTRY::~MC_TABLE_ENTRY()
  // destructor
{
  FreeAll();
}

bool MC_TABLE::MC_TABLE_ENTRY::Check
(ERROR & error_msg) const
{
  if (num_simplices < 0) {
    error_msg.ClearAll();
    error_msg.AddToMessage
      ("Number of simplices in isosurface table entry must be non-negative.");
    return(false);
  }

  if (num_simplices > 0 && simplex_vertex_list == NULL) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Memory for simplex vertex list not allocated.");
    return(false);
  }

  return(true);
}

void MC_TABLE::MC_TABLE_ENTRY::FreeAll()
  // free all memory
{
  delete [] simplex_vertex_list;
  simplex_vertex_list = NULL;
  num_simplices = 0;
}


// Default constructor. dimension = 3.
MC_TABLE::MC_TABLE(): polytope(3)
{
  Init(3, 2);
}


// Constructor.
// @param d Dimension of space containing isosurface.  Should be 2, 3 or 4.
void MC_TABLE::Init(const int dimension, const int simplex_dimension)
{
  const char * procname = "MC_TABLE::Init";

  this->simplex_dimension = simplex_dimension;

  num_isosurface_vertices = 0;
  isosurface_vertex = NULL;

  num_table_entries = 0;
  entry = NULL;
  is_table_allocated = false;
  if (!CheckDimension())
    throw PROCEDURE_ERROR(procname, "Illegal polytope dimension.");
}


// Destructor.
MC_TABLE::~MC_TABLE()
{
  FreeAll();
}


void MC_TABLE::SetEncoding(const ENCODING encoding)
{
  this->table_properties.encoding = encoding;
}


void MC_TABLE::SetTableType
(const LOOKUP_TABLE_TYPE lookup_table_type)
{
  this->table_properties.lookup_table_type = lookup_table_type;
}


void MC_TABLE::SetGridVertexLabelType
(const GRID_VERTEX_LABEL_TYPE grid_vertex_label_type)
{
  this->table_properties.grid_vertex_label_type = grid_vertex_label_type;
}


void MC_TABLE::SetSeparationType
(const ISOSURFACE_SEPARATION_TYPE separation_type)
{
  this->table_properties.isosurface_separation_type = separation_type;
}

void MC_TABLE::SetTriangulationType
(const ISOSURFACE_TRIANGULATION_TYPE triangulation_type)
{
  this->table_properties.isosurface_triangulation_type = 
    triangulation_type;
}

void MC_TABLE::SetIsoPolyOrientation
(const ISO_POLY_ORIENTATION iso_poly_orientation)
{
  this->table_properties.iso_poly_orientation = iso_poly_orientation;
}

void MC_TABLE::SetSeparateOpposite
(const SEPARATE_OPPOSITE_TYPE separate_opposite)
{
  this->table_properties.separate_opposite = separate_opposite;
}

void MC_TABLE::SetSeparateOpposite(const bool flag)
{
  this->table_properties.SetSeparateOpposite(flag);
}

void MC_TABLE::SetNumIsosurfaceVertices(const int num_vertices)
{
  if (isosurface_vertex != NULL)
    delete [] isosurface_vertex;
  isosurface_vertex = NULL;

  num_isosurface_vertices = num_vertices;
  isosurface_vertex = new ISOSURFACE_VERTEX[num_vertices];
}


// Check allocation of array isosurface_vertices
// procname = calling procedure name, for error messages
// vstart = first vertex
// numv = number of vertices required
void MC_TABLE::CheckIsoVerticesAlloc
(const char * procname, const int vstart, const int numv)
{
  if (numv == 0) return;

  if (isosurface_vertex == NULL) {
    throw PROCEDURE_ERROR
      (procname, "Set number of isosurface vertices before storing vertices.");
  }

  if (numv+vstart > NumIsosurfaceVertices()) {
    throw PROCEDURE_ERROR
      (procname, "Illegal isosurface vertex index.");
  }
}

void MC_TABLE::StorePolyVerticesAsIsoVertices(const int vstart)
  // store polytope vertices as isosurface vertices
  // store polytope vertices starting at isosurface vertex vstart
{
  const int num_polyv = Polytope().NumVertices();
  const char * procname = "MC_TABLE::StorePolyVerticesAsIsoVertices";

  CheckIsoVerticesAlloc(procname, vstart, num_polyv);

  for (int iv = 0; iv < num_polyv; iv++) {
    SetIsoVertexType(iv+vstart, ISOSURFACE_VERTEX::VERTEX);
    SetIsoVertexFace(iv+vstart, iv);
  }
}


// @brief Set isosurface vertex.
void MC_TABLE::SetIsosurfaceVertex
(const int iv, const ISOSURFACE_VERTEX & isosurface_vertex)
{
  PROCEDURE_ERROR error
    ("MC_TABLE::SetIsosurfaceVertex");
  
  if (NumIsosurfaceVertices() == 0) {
    error.AddToMessage
      ("Programming error. Call MC_TABLE::SetNumIsosurfaceVertices()");
    error.AddToMessage
      ("  before calling MC_TABLE::SetIsosurfaceVertex().");
    throw error;
  }

  if (iv >= NumIsosurfaceVertices()) {
    error.AddToMessage
      ("Programming error. Illegal isosurface vertex index ", iv, ".");
    error.AddToMessage
      ("  Isosurface vertices should be in range [0..",
       NumIsosurfaceVertices()-1, "].");
    error.AddToMessage
      ("  Check call to MC_TABLE:SetNumIsosurfaceVertices().");
    throw error;
  }
  
  SetIsoVertexType(iv, isosurface_vertex.Type());
  SetIsoVertexFace(iv, isosurface_vertex.Face());
  for (int ic = 0; ic < isosurface_vertex.NumCoord(); ic++)
    { SetIsoVertexCoord(iv, ic, isosurface_vertex.Coord(ic)); }
  if (isosurface_vertex.IsLabelSet())
    { SetIsoVertexLabel(iv, isosurface_vertex.Label()); }
}


// Copy isosurface vertices from isotable.
void MC_TABLE::CopyIsosurfaceVertices
(const MC_TABLE & isotable)
{
  const int num_isosurface_vertices =
    isotable.NumIsosurfaceVertices();
  
  SetNumIsosurfaceVertices(num_isosurface_vertices);
  for (int iv = 0; iv < num_isosurface_vertices; iv++) {
    SetIsosurfaceVertex(iv, isotable.IsosurfaceVertex(iv));
  }
}


void MC_TABLE::StorePolyEdgesAsIsoVertices(const int vstart)
  // store polytope edges as isosurface vertices
  // store polytope edges starting at isosurface vertex vstart
{
  const int num_polye = Polytope().NumEdges();
  const char * procname = "MC_TABLE::StorePolyEdgesAsIsoVertices";

  CheckIsoVerticesAlloc(procname, vstart, num_polye);

  for (int ie = 0; ie < num_polye; ie++) {
    SetIsoVertexType(ie+vstart, ISOSURFACE_VERTEX::EDGE);
    SetIsoVertexFace(ie+vstart, ie);
  }
}


// Store polytope facets as isosurface vertices.
// store polytope facets starting at isosurface vertex vstart.
void MC_TABLE::StorePolyFacetsAsIsoVertices(const int vstart)
{
  const int num_polyf = Polytope().NumFacets();
  const char * procname = "MC_TABLE::StorePolyFacetsAsIsoVertices";

  CheckIsoVerticesAlloc(procname, vstart, num_polyf);

  for (int jf = 0; jf < num_polyf; jf++) {
    SetIsoVertexType(jf+vstart, ISOSURFACE_VERTEX::FACET);
    SetIsoVertexFace(jf+vstart, jf);
  }
}


// Allocate table
void MC_TABLE::SetNumTableEntries(const int num_table_entries)
{
  const char * procname = "MC_TABLE::SetNumTableEntries";

  if (entry != NULL) delete [] entry;
  entry = NULL;
  this->num_table_entries = 0;
  is_table_allocated = false;

  entry = new MC_TABLE_ENTRY[num_table_entries];
  if (entry == NULL && num_table_entries > 0)
    throw PROCEDURE_ERROR
      (procname, "Unable to allocate memory for isosurface table.");

  this->num_table_entries = num_table_entries;
  is_table_allocated = true;
}


void MC_TABLE::SetNumSimplices(const TABLE_INDEX_TYPE it, const int nums)
  // set number of simplices in table entry it
  // it = table entry
  // nums = number of simplices
{
  const char * procname = "MC_TABLE::SetNumSimplices";

  if (!IsTableAllocated() || entry == NULL) {
    throw PROCEDURE_ERROR
      (procname, "Table must be allocated before entering table entries.");
  };

  if (it < 0 || it >= NumTableEntries())
    throw PROCEDURE_ERROR(procname, "Illegal table index.");
  if (nums < 0)
    throw PROCEDURE_ERROR
      (procname, "Number of simplices must be non-negative.");

  entry[it].num_simplices = 0;
  delete entry[it].simplex_vertex_list;
  entry[it].simplex_vertex_list = NULL;

  if (nums > 0)
    entry[it].simplex_vertex_list = 
      new ISOPATCH_VERTEX_INDEX_TYPE[nums*NumVerticesPerSimplex()];

  entry[it].num_simplices = nums;
}


// Set simplex vertex.
// it = index table entry.  In range [0..NumTableEntries()-1].
// is = index simplex.  
// k = k'th simplex vertex.  In range [0..NumVerticesPerSimplex()-1].
// isov = index of isosurface vertex
void MC_TABLE::SetSimplexVertex
(const TABLE_INDEX_TYPE it, const int is, const int k, 
 const int isov)
{
  entry[it].simplex_vertex_list[is*NumVerticesPerSimplex()+k] = isov;
}


// Set simplex vertices.
void MC_TABLE::SetSimplexVertices
(const TABLE_INDEX_TYPE it, const ISOPATCH_VERTEX_INDEX_TYPE simplex_vertices[],
 const int num_simplices)
{
  SetNumSimplices(it, num_simplices);

  std::copy(simplex_vertices,
            simplex_vertices+num_simplices*NumVerticesPerSimplex(),
            entry[it].simplex_vertex_list);
}


// Return true if facet vertex labels are identical
//   in table entries table_indexA and table_indexB.
bool MC_TABLE::AreAllFacetVertexLabelsIdentical
(const TABLE_INDEX_TYPE table_indexA, const TABLE_INDEX_TYPE table_indexB,
 const int ifacet) const
{
  const int num_poly_vertices = Polytope().NumVertices();
  std::vector<int> digitA(num_poly_vertices);
  std::vector<int> digitB(num_poly_vertices);
  PROCEDURE_ERROR error
    ("MC_TABLE::AreAllFacetVertexLabelsIdentical");

  convert2base
    (table_indexA, Base(), digitA.data(), num_poly_vertices, error);
  convert2base
    (table_indexB, Base(), digitB.data(), num_poly_vertices, error);
  
  for (int j = 0; j < Polytope().NumFacetVertices(ifacet); j++) {
    const int jv = Polytope().FacetVertex(ifacet, j);

    if (digitA[jv] != digitB[jv])
      { return false; }
  }

  return true;
}


// Sort simplex vertices in increasing order.
void MC_TABLE::SortSimplexVertices
(const TABLE_INDEX_TYPE it, const int isimplex)
{
  const int numv_per_simplex = NumVerticesPerSimplex();
  ISOPATCH_VERTEX_INDEX_TYPE * simplex_vert =
    entry[it].simplex_vertex_list + isimplex*numv_per_simplex;
  
  std::sort(simplex_vert, simplex_vert+numv_per_simplex);
}


// Flip all isosurface polytope orientations from +1 to -1 or from -1 to +1.
void MC_TABLE::FlipIsoPolyOrientation
(const TABLE_INDEX_TYPE it, const int ipoly)
{
  const int numv_per_simplex = NumVerticesPerSimplex();
  ISOPATCH_VERTEX_INDEX_TYPE * simplex_vert =
    entry[it].simplex_vertex_list + ipoly*numv_per_simplex;

  if (numv_per_simplex < 2) {
    // Nothing to switch.
    return;
  }
  
  const int ilast = numv_per_simplex-1;
  std::swap(simplex_vert[ilast], simplex_vert[ilast-1]);
}


// Flip all isosurface polytope orientations at table entry table_index.
void MC_TABLE::FlipAllIsoPolyOrientations
(const TABLE_INDEX_TYPE table_index)
{
  for (int isimplex = 0; isimplex < NumSimplices(table_index);
       isimplex++)
    { FlipIsoPolyOrientation(table_index, isimplex); }
}


// Flip all isosurface polytope orientations at in isosurface lookup table.
void MC_TABLE::FlipAllIsoPolyOrientations()
{
  for (TABLE_INDEX_TYPE table_index = 0;
       table_index < NumTableEntries();
       table_index++)
    { FlipAllIsoPolyOrientations(table_index); }

  // Flip property iso_poly_orientation.
  SetIsoPolyOrientation(OppositeIsoPolyOrientation());
}


// Orient simplices in table entry.
void MC_TABLE::OrientSimplicesInTableEntry
(const TABLE_INDEX_TYPE table_index, const TABLE_INDEX_TYPE istart)
{
  orient_simplices
    (entry[table_index].simplex_vertex_list,
     NumVerticesPerSimplex(), NumSimplices(table_index), istart);
}


// Orient simplices in table entry.
// - Version that uses and sets array is_orientedA[].
void MC_TABLE::OrientSimplicesInTableEntry
(const TABLE_INDEX_TYPE table_index, const TABLE_INDEX_TYPE istart,
 std::vector<bool> & is_oriented)
{
  orient_simplices
    (entry[table_index].simplex_vertex_list,
     NumVerticesPerSimplex(), NumSimplices(table_index), istart,
     is_oriented);
}


// Orient all simplices in table entry.
void MC_TABLE::OrientAllSimplicesInTableEntry
(const TABLE_INDEX_TYPE table_index, int & num_components)
{
  const int num_vert_per_simplex = SimplexDimension()+1;
  
  orient_all_simplices
    (entry[table_index].simplex_vertex_list, num_vert_per_simplex,
     NumSimplices(table_index), num_components);
}


// Return false if simplices in table entry have
//   inconsistent orientations.
bool MC_TABLE::AreSimplicesConsistentlyOriented
(const TABLE_INDEX_TYPE table_index,
 int & isimplexA, int & isimplexB) const
{
  const int num_vert_per_simplex = SimplexDimension()+1;
  
  return are_simplices_consistently_oriented
    (entry[table_index].simplex_vertex_list, num_vert_per_simplex,
     NumSimplices(table_index), isimplexA, isimplexB);
}


bool MC_TABLE::CheckDimension(const int d) const
  // check dimension
{
  if (d < 1)
    return(false);
  else
    return(true);
}


bool MC_TABLE::CheckTable(ERROR & error_msg) const
  // check table
{
  if (polytope.NumVertices() >= MAX_NUM_POLYTOPE_VERTICES) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Too many polytope vertices");
    return(false);
  }

  if (polytope.NumVertices() < 1) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Polytope must have at least one vertex.");
    return(false);
  }

  if (entry == NULL) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Memory for isosurface table not allocated.");
    return(false);
  }

  for (int it = 0; it < NumTableEntries(); it++)
    if (!entry[it].Check(error_msg)) {
      error_msg.AddToMessage("Error detected at isosurface table entry ", 
                           it, ".");
      return(false);
    }

  for (int jt = 0; jt < NumTableEntries(); jt++)
    for (int is = 0; is < NumSimplices(jt); is++)
      for (int iv = 0; iv < NumVerticesPerSimplex(); iv++) {
        int iso_v = int(SimplexVertex(jt, is, iv));
        if (iso_v < 0 || iso_v >= NumIsosurfaceVertices()) {
          error_msg.ClearAll();
          error_msg.AddToMessage
            ("Illegal isosurface vertex ", iso_v, " in isosurface table entry ", jt, "."); 
          return(false);
        }
      };

  return(true);
}

bool MC_TABLE::Check(ERROR & error_msg) const
{
  if (!Polytope().Check(error_msg)) return(false);
  if (!CheckTable(error_msg)) return(false);
  return(true);
}


// Return false and set error message if table_index
//   is not in range [0..NumTableEntries()-1].
bool MC_TABLE::CheckTableIndex
(const int table_index, ERROR & error) const
{
  if (table_index < 0) {
    error.AddToMessage
      ("Programming error. Illegal negative table index ",
       table_index, ".");
    error.AddToMessage
      ("  Table index should be non-negative.");
    return false;
  }

  if (NumTableEntries() == 0) {
    error.AddToMessage("Programming error. No table entries.");
    error.AddToMessage
      ("  Call SetNumTableEntries() to create table entries.");
    return false;
  }

  if (table_index >= NumTableEntries()) {
    error.AddToMessage("Programming error. Table index ",
                     table_index, " out of bounds.");
    error.AddToMessage
      ("  Number of table entries: ", num_table_entries);
    error.AddToMessage
      ("  Table index must be less than number of table entries.");
    return false;
  }

  return true;
}


// Free all memory
void MC_TABLE::FreeAll()
{
  if (entry != NULL) {
    for (int i = 0; i < num_table_entries; i++)
      entry[i].FreeAll();
    delete [] entry;
    entry = NULL;
  };
  num_table_entries = 0;
  is_table_allocated = false;

  polytope.FreeAll();

  delete [] isosurface_vertex;
  isosurface_vertex = NULL;
  num_isosurface_vertices = 0;
}


// *******************************************************************
// ISOSURFACE_VERTEX member functions
// *******************************************************************

ISOSURFACE_VERTEX::~ISOSURFACE_VERTEX()
{
  label.Unset();
}


// *******************************************************************
// UTILITY FUNCTIONS
// *******************************************************************

// calculate num table entries = (num_colors)^(num_vert)
unsigned long ISO3D::calculate_num_entries
(const int num_vert, const int num_colors)
{
  PROCEDURE_ERROR error("calculate_num_entries");

  unsigned long num_table_entries = 1;
  int_power(num_colors, num_vert, num_table_entries, error);

  return(num_table_entries);
}

