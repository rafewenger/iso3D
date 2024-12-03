/*!
 *  @file iso3D_MCtable_poly.cpp
 *  @brief Marching Cubes isosurface table polytope.
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

#include <limits>

#include "iso3D_MCtable.h"

using namespace ISO3D;


// *******************************************************************
// MC_TABLE_POLY_BASE MEMBER FUNCTIONS
// *******************************************************************

// Constructor.
MC_TABLE_POLY_BASE::MC_TABLE_POLY_BASE(const int d):
  shape_list(UNDEFINED_SHAPE, _poly_shape_list)
{
  Init();
  dimension = d;
}

// Destructor.
MC_TABLE_POLY_BASE::~MC_TABLE_POLY_BASE()
{
  FreeAll();
}


// Copy.
MC_TABLE_POLY_BASE::MC_TABLE_POLY_BASE
(const MC_TABLE_POLY_BASE & init):
  shape_list(UNDEFINED_SHAPE, _poly_shape_list)
{
  Init();

  *this = init;
}


// Assign.
const MC_TABLE_POLY_BASE & MC_TABLE_POLY_BASE::operator = 
(const MC_TABLE_POLY_BASE & right)  
{
  if (&right != this) {         // avoid self-assignment
    FreeAll();
    shape = right.Shape();
    dimension = right.Dimension();
    SetSize(right.NumVertices(), right.NumEdges(), right.NumFacets());

    // copy vertices
    for (int iv = 0; iv < NumVertices(); iv++)
      for (int ic = 0; ic < Dimension(); ic++)
        SetVertexCoord(iv, ic, right.VertexCoord(iv, ic));

    // copy edges
    for (int ie = 0; ie < NumEdges(); ie++)
      SetEdge(ie, right.EdgeEndpoint(ie, 0), right.EdgeEndpoint(ie, 1));

    // copy facets
    for (int jf = 0; jf < NumFacets(); jf++) {
      facet_vertex_bitset[jf] = right.FacetVertexBitset(jf);
      int num_fv = right.NumFacetVertices(jf);
      SetNumFacetVertices(jf, num_fv);

      for (int k = 0; k < num_fv; k++) {
        int iv = right.FacetVertex(jf, k);
        SetFacetVertex(jf, k, iv);
      }
    }

  };

  return *this;
}

// Free facet arrays.
void MC_TABLE_POLY_BASE::FreeFacets()
{
  if (facet_vertex_list != NULL) {
    for (int jf = 0; jf < num_facets; jf++) {
      delete[] facet_vertex_list[jf];
      facet_vertex_list[jf] = NULL;
    }
  }
  delete [] facet_vertex_list;
  facet_vertex_list = NULL;

  delete [] num_facet_vertices;
  num_facet_vertices = NULL;
  delete [] facet_vertex_bitset;
  facet_vertex_bitset = NULL;
  num_facets = 0;
}


// Free incident edge arrays.
void MC_TABLE_POLY_BASE::FreeIncidentEdges()
{
  if (incident_edge_list != NULL) {
    for (int iv = 0; iv < num_vertices; iv++) {
      delete [] incident_edge_list[iv];
      incident_edge_list[iv] = NULL;
      num_incident_edges[iv] = 0;
    }
  }
  delete [] incident_edge_list;
  incident_edge_list = NULL;

  delete [] num_incident_edges;
  num_incident_edges = NULL;
}


// Free all memory.
void MC_TABLE_POLY_BASE::FreeAll()
{
  FreeFacets();
  FreeIncidentEdges();

  num_vertices = 0;
  num_edges = 0;
  delete [] vertex_coord;
  vertex_coord = NULL;
  delete [] edge_endpoint;
  edge_endpoint = NULL;
}


// Initialize.
void MC_TABLE_POLY_BASE::Init()
{
  dimension = 0;
  num_vertices = 0;
  num_edges = 0;
  num_facets = 0;
  vertex_coord = NULL;
  edge_endpoint = NULL;
  facet_vertex_bitset = NULL;
  num_facet_vertices = NULL;
  facet_vertex_list = NULL;
  num_incident_edges = NULL;
  incident_edge_list = NULL;
  shape = UNDEFINED_SHAPE;
}


// Set polytope shape.
void MC_TABLE_POLY_BASE::SetShape(const POLYTOPE_SHAPE & shape)
{
  this->shape = shape;
}

// Set polyhedron dimension.
void MC_TABLE_POLY_BASE::SetDimension(const int d)
{
  FreeAll();
  num_vertices = num_edges = 0;

  dimension = d;
}


// Set number of vertices.
// - Must be called before setting polyhedron vertices.
void MC_TABLE_POLY_BASE::SetNumVertices(const int numv)
{
  const char * procname = "MC_TABLE_POLY_BASE::SetNumVertices";

  if (!CheckDimension())
    throw PROCEDURE_ERROR(procname, "Illegal polyhedron dimension.");

  FreeAll();
  num_vertices = num_edges = 0;

  if (numv == 0)
    throw PROCEDURE_ERROR(procname, "Number of vertices must be non-zero.");

  // Note that even if numv <= MAX_NUM_POLYTOPE_VERTICES,
  //   there may not be enough memory to store the isosurface table.
  if (numv > MAX_NUM_POLYTOPE_VERTICES)
    throw PROCEDURE_ERROR
      (procname, "Number of polyhedron vertices is too large.");

  num_vertices = numv;
  vertex_coord = new CUBE_COORD_TYPE[num_vertices*Dimension()];

  incident_edge_list = new CUBE_EDGE_INDEX_TYPE *[num_vertices];
  num_incident_edges = new CUBE_EDGE_INDEX_TYPE[num_vertices];
  for (int iv = 0; iv < num_vertices; iv++) {
    incident_edge_list[iv] = NULL;
    num_incident_edges[iv] = 0;
  }
}


// Set number of edges.
// - Must be called before setting polyhedron edges.
void MC_TABLE_POLY_BASE::SetNumEdges(const int nume)
{
  const char * procname = "MC_TABLE_POLY_BASE::SetNumEdges";

  delete [] edge_endpoint;
  edge_endpoint = NULL;
  num_edges = 0;

  if (!CheckDimension()) 
    throw PROCEDURE_ERROR(procname, "Illegal dimension.");

  if (NumVertices() == 0)
    throw PROCEDURE_ERROR
      (procname, "Number of vertices must be set before number of edges.");

  if (nume < 1)
    throw PROCEDURE_ERROR(procname, "Number of edges must be non-zero.");

  if (nume > std::numeric_limits<CUBE_EDGE_INDEX_TYPE>::max())
    throw PROCEDURE_ERROR
      (procname, "Number of polyhedron edges is too large.");

  num_edges = nume;
  edge_endpoint = new CUBE_VERTEX_INDEX_TYPE[num_edges*2];
}


// Set number of facets.
// - Must be called before setting polyhedron facets.
void MC_TABLE_POLY_BASE::SetNumFacets(const int numf)
{
  const char * procname = "MC_TABLE_POLY_BASE::SetNumFacets";

  FreeFacets();

  if (!CheckDimension())
    throw PROCEDURE_ERROR(procname, "Illegal dimension.");

  if (NumVertices() == 0)
    throw PROCEDURE_ERROR
      (procname, "Number of vertices must be set before number of facets.");

  if (numf < 1)
    throw PROCEDURE_ERROR(procname, "Number of facets must be non-zero.");

  if (numf > std::numeric_limits<CUBE_FACET_INDEX_TYPE>::max())
    throw PROCEDURE_ERROR
      (procname, "Number of polyhedron facets is too large.");

  num_facets = numf;
  facet_vertex_bitset = new CUBE_VERTEX_BITSET[numf];
  num_facet_vertices = new CUBE_VERTEX_INDEX_TYPE[numf];
  facet_vertex_list = new CUBE_VERTEX_INDEX_TYPE *[numf];

  if (facet_vertex_bitset == NULL || num_facet_vertices == NULL || 
      facet_vertex_list == NULL)
    throw PROCEDURE_ERROR
      (procname, "Unable to allocate memory for list of facets.");

  // initialize each facet to 0
  for (int jf = 0; jf < numf; jf++) {
    facet_vertex_bitset[jf].reset();
    num_facet_vertices[jf] = 0;
    facet_vertex_list[jf] = NULL;
  };

}

/// Set number of vertices in facet \a jf.
void MC_TABLE_POLY_BASE::SetNumFacetVertices
(const int jf, const int numv)
{
  const char * procname = 
    "MC_TABLE_POLY_BASE::SetNumFacetVertices";

  int jf_int = int(jf);
  if (jf_int < 0 || jf_int >= NumFacets())
    throw PROCEDURE_ERROR(procname, "Illegal facet index.");


  if (facet_vertex_list[jf] != NULL) {
    delete [] facet_vertex_list[jf];
    facet_vertex_list[jf] = NULL;
  }

  num_facet_vertices[jf] = numv;
  facet_vertex_list[jf] = new CUBE_VERTEX_INDEX[numv];
}

void MC_TABLE_POLY_BASE::SetVertexCoord
(const int iv, const int ic, const int coord)
  // set polyhedron vertex coordinate
  // iv = vertex index.  In range [0..NumVertices()-1].
  // ic = coordinate index. In range [0..Dimension()-1].
  // coord = coordinate.  Must be even.
{
  const char * procname = "MC_TABLE_POLY_BASE::SetVertexCoord";

  if (iv < 0 || iv >= NumVertices())
    throw PROCEDURE_ERROR(procname, "Illegal vertex index.");

  if (ic < 0 || ic >= Dimension())
    throw PROCEDURE_ERROR(procname, "Illegal vertex coordinate index.");

  if (coord%2 != 0)
    throw PROCEDURE_ERROR
      (procname, "Illegal vertex coordinate.  Vertex coordinate must be even.");

  if (vertex_coord == NULL)
    throw PROCEDURE_ERROR
      (procname, "Vertex coordinate memory not allocated.");

  vertex_coord[iv*Dimension() + ic] = coord;
}


// Set polyhedron edge endpoints.
void MC_TABLE_POLY_BASE::SetEdge
(const int ie, const int iv0, const int iv1)
{
  const char * procname = "MC_TABLE_POLY_BASE::SetEdge";

  int ie_int = int(ie);
  if (ie_int < 0 || ie_int >= NumEdges())
    throw PROCEDURE_ERROR(procname, "Illegal edge index.");

  if (iv0 < 0 || iv0 > NumVertices() ||
      iv1 < 0 || iv1 > NumVertices())
    throw PROCEDURE_ERROR(procname, "Illegal vertex index.");

  if (edge_endpoint == NULL)
    throw PROCEDURE_ERROR(procname, "Edge endpoint memory not allocated.");

  edge_endpoint[2*int(ie)] = iv0;
  edge_endpoint[2*int(ie)+1] = iv1;
}

void MC_TABLE_POLY_BASE::SetFacetVertex
(const int jf, const int k, const int iv)
{
  const char * procname = "MC_TABLE_POLY_BASE::SetFacetVertex";

  if (k < 0 || k >= NumFacetVertices(jf)) 
    throw PROCEDURE_ERROR
      (procname, "Illegal index to list of facet vertices.");

  if (iv < 0 || iv >= NumVertices())
    throw PROCEDURE_ERROR(procname, "Illegal vertex index.");

  facet_vertex_list[jf][k] = iv;
  facet_vertex_bitset[jf].set(iv);
}


// Compute edges incident on each vertex.
void MC_TABLE_POLY_BASE::ComputeIncidentEdges()
{
  PROCEDURE_ERROR error
    ("MC_TABLE_POLY_BASE::ComputeIncidentEdges");

  if ((NumVertices() == 0) || (NumEdges() == 0)) {
    // Nothing to compute.
    return;
  }
  
  if (!check_array_allocated
      (incident_edge_list, "incident_edge_list", error))
    throw error;

  if (!check_array_allocated
      (num_incident_edges, "num_incident_edges", error))
    throw error;

  if (!check_array_allocated
      (edge_endpoint, "edge_endpoint", error)) {
    error.AddToMessage
      ("Call SetEdge() for all edges before calling ComputeVertexNeighbors.");
    throw error;
  }

  // Initialize num_incident_edges[] and incident_edge_list[][].
  for (int iv = 0; iv < NumVertices(); iv++) {
    if (incident_edge_list[iv] != NULL) {
      delete [] incident_edge_list[iv];
      incident_edge_list[iv] = NULL;
    }
    num_incident_edges[iv] = 0;    
  }

  for (int ie = 0; ie < NumEdges(); ie++) {
    for (int j = 0; j < 2; j++) {
      const int iend = EdgeEndpoint(ie, j);
      num_incident_edges[iend]++;
    }
  }

  // Allocate incident_edge_list[iv] for each vertex iv.
  for (int iv = 0; iv < NumVertices(); iv++) {
    if (num_incident_edges[iv] > 0) {
      incident_edge_list[iv] =
        new CUBE_EDGE_INDEX_TYPE[num_incident_edges[iv]];
    }
  }

  std::vector<int> num_found(NumVertices(), 0);

  for (int ie = 0; ie < NumEdges(); ie++) {
    for (int j = 0; j < 2; j++) {
      const int iend = EdgeEndpoint(ie, j);
      const int k = num_found[iend];
      incident_edge_list[iend][k] = ie;
      num_found[iend]++;
    }
  }

  for (int iv = 0; iv < NumVertices(); iv++) {
    if (num_found[iv] != num_incident_edges[iv]) {
      error.AddToMessage
        ("Programming error. Error in computing edges incident on vertices.");
      error.AddToMessage
        ("  Vertex ", iv, " has ", num_incident_edges[iv],
         " incident edges but");
      error.AddToMessage
        ("    ", num_found[iv],
         " neighbors were added to incident_edge_list.");
      throw error;
    }
  }
}


// Return ic'th coordinate of midpoint of edge ie
// Note: vertice coordinates must all be even so midpoint coordinate
//   is an integer.
int MC_TABLE_POLY_BASE::MidpointCoord
(const int ie, const int ic) const
{
  int iv0 = EdgeEndpoint(ie, 0);
  int iv1 = EdgeEndpoint(ie, 1);
  int coord0 = VertexCoord(iv0, ic);
  int coord1 = VertexCoord(iv1, ic);
  
  return((coord0+coord1)/2);
}


// Generate a cube.
void MC_TABLE_POLY_BASE::GenCube3D()
{
  const char * procname = "MC_TABLE_POLY_BASE::GenCube3D";

  shape = CUBE;
  dimension = DIM3;

  // TO BE CONTINUED...

  ComputeIncidentEdges();
}


// Return true if dimension >= 1.
bool MC_TABLE_POLY_BASE::CheckDimension() const
{
  if (dimension < 1)
    return(false);
  else
    return(true);
}


// Check polyhedron.
bool MC_TABLE_POLY_BASE::Check(ERROR & error_msg) const
{
  if (!CheckDimension()) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Illegal polyhedron dimension ",
                         Dimension(), ".");
    return(false);
  }

  if (NumVertices() < 1) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Illegal number of vertices.");
    return(false);
  };

  if (NumEdges() < 1) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Illegal number of edges.");
    return(false);
  };

  if (vertex_coord == NULL) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Memory for vertex coordinate list not allocated.");
    return(false);
  };

  if (edge_endpoint == NULL) {
    error_msg.ClearAll();
    error_msg.AddToMessage("Memory for edge endpoint list not allocated.");
    return(false);
  }

  for (int iv = 0; iv < NumVertices(); iv++) {
    for (int ic = 0; ic < Dimension(); ic++) {
      if ((VertexCoord(iv, ic) % 2) != 0) {
        error_msg.ClearAll();
        error_msg.AddToMessage("Vertex coordinates must be even integers.");
        return(false);
      };
    };
  };

  for (int ie = 0; ie < NumEdges(); ie++) {
    for (int ip = 0; ip < 2; ip++) {
      int iv = EdgeEndpoint(ie, ip);
      if (iv < 0 || iv >= NumVertices()) {
        error_msg.ClearAll();
        error_msg.AddToMessage("Illegal edge endpoint ", iv,
                             " for edge ", ie, ".");
        return(false);
      };
    };
  };

  if (NumFacets() > 0) {
    if (facet_vertex_bitset == NULL) {
      error_msg.ClearAll();
      error_msg.AddToMessage("Memory for facet list not allocated.");
      return false;
    };
  };

  return(true);
}


// *******************************************************************
// MC_TABLE_HALF_EDGE_POLY_3D MEMBER FUNCTIONS
// *******************************************************************

// Constructor.
MC_TABLE_HALF_EDGE_POLY_3D::MC_TABLE_HALF_EDGE_POLY_3D():
  MC_TABLE_POLY_BASE(3)
{
  Init();
}


void MC_TABLE_HALF_EDGE_POLY_3D::Init()
{
  next_half_edge_in_facet = NULL;
  prev_half_edge_in_facet = NULL;
}


/// Destructor.
MC_TABLE_HALF_EDGE_POLY_3D::
~MC_TABLE_HALF_EDGE_POLY_3D()
{
  FreeAll();
}


void MC_TABLE_HALF_EDGE_POLY_3D::Allocate()
{
  FreeLocal();

  if (NumEdges() < 1) {
    PROCEDURE_ERROR error
      ("MC_TABLE_HALF_EDGE_POLY_3D::Allocate");
    error.AddToMessage
      ("Programming error. Number of polyhedron edges not set.");
    throw error;
  }
      
  next_half_edge_in_facet = new HALF_EDGE[2*NumEdges()];
  prev_half_edge_in_facet = new HALF_EDGE[2*NumEdges()];
}

void MC_TABLE_HALF_EDGE_POLY_3D::FreeLocal()
{
  if (next_half_edge_in_facet != NULL) {
    delete [] next_half_edge_in_facet;
    next_half_edge_in_facet = NULL;
  }

  if (prev_half_edge_in_facet != NULL) {
    delete [] prev_half_edge_in_facet;
    prev_half_edge_in_facet = NULL;
  }
}


void MC_TABLE_HALF_EDGE_POLY_3D::FreeAll()
{
  FreeLocal();
  MC_TABLE_POLY_BASE::FreeAll();
}


// Check half edge data structures.
bool MC_TABLE_HALF_EDGE_POLY_3D::
CheckHalfEdge(ERROR & error) const
{
  if (NumEdges() < 1) {
    error.AddToMessage("Polytope has no edges.");
    return false;
  }

  if (!check_array_allocated
      (next_half_edge_in_facet, "next_half_edge_in_facet",
       error))
    { return false; }

  if (!check_array_allocated
      (next_half_edge_in_facet, "next_half_edge_in_facet",
       error))
    { return false; }

  // TO BE CONTINUED...

  return true;
}


/// Check MC_TABLE_HALF_EDGE_POLY_3D.
bool MC_TABLE_HALF_EDGE_POLY_3D::
Check(ERROR & error) const
{
  if (!MC_TABLE_POLY_BASE::Check(error))
    { return false; }

  if (!CheckHalfEdge(error))
    { return false; }

  return true;
}


// *******************************************************************
// GENERATE POLYTOPE
// *******************************************************************

// Generate a prism with base base_polyhedron
// - First numv vertices have last coordinate = 0
// - Last numv vertices have last coordinate = 2
// - First nume edges connect first numv vertices
// - Second nume edges connect second numv vertices
// - Third numv edges connect first to second set of vertices
// - (numv = # vertices in base_polyhedron; nume = # edges in base_polyhedron)
void ISO3D::generate_prism
(const MC_TABLE_POLY_BASE & base_polyhedron,
 MC_TABLE_POLY & prism)
{
  int dim = base_polyhedron.Dimension();
  int numv = base_polyhedron.NumVertices();
  int nume = base_polyhedron.NumEdges();
  int numf = base_polyhedron.NumFacets();
  int prism_dim = dim + 1;
  int prism_numc = prism_dim;
  int prism_lastc = prism_numc - 1;
  int prism_numv = numv * 2;
  int prism_nume = nume * 2 + numv;
  int prism_numf = 2 + numf;
  prism.SetShape(SIMPLEX_PRISM);
  prism.SetDimension(prism_dim);
  prism.SetSize(prism_numv, prism_nume, prism_numf);

  // set prism vertex coord
  for (int iv = 0; iv < numv; iv++) {
    for (int ic = 0; ic < prism_lastc; ic++) {
      int coord = base_polyhedron.VertexCoord(iv, ic);
      prism.SetVertexCoord(iv, ic, coord);
      prism.SetVertexCoord(iv+numv, ic, coord);
    };
    prism.SetVertexCoord(iv, prism_lastc, 0);
    prism.SetVertexCoord(iv+numv, prism_lastc, 2);
  };

  // set edges
  for (int ie = 0; ie < base_polyhedron.NumEdges(); ie++) {
    int iv0 = base_polyhedron.EdgeEndpoint(ie, 0);
    int iv1 = base_polyhedron.EdgeEndpoint(ie, 1);
    prism.SetEdge(ie, iv0, iv1);
    prism.SetEdge(ie+nume, iv0+numv, iv1+numv);
  };

  for (int iv = 0; iv < base_polyhedron.NumVertices(); iv++) {
    int ie = 2*nume + iv;
    prism.SetEdge(ie, iv, iv+numv);
  };

  // set facets
  prism.SetNumFacetVertices(0, numv);
  prism.SetNumFacetVertices(1, numv);

  for (int iv = 0; iv < numv; iv++) {
    // set two base facets
    prism.SetFacetVertex(0, iv, iv);
    prism.SetFacetVertex(1, iv, iv+numv);
  };

  for (int jf = 0; jf < numf; jf++) {
    // set prism facet corresponding to original facet jf
    int prism_jf = 2 + jf;

    int base_num_fv = base_polyhedron.NumFacetVertices(jf);
    prism.SetNumFacetVertices(prism_jf, 2*base_num_fv);

    for (int k = 0; k < base_num_fv; k++) {
      int iv = base_polyhedron.FacetVertex(jf, k);
      prism.SetFacetVertex(prism_jf, k, iv);
      prism.SetFacetVertex(prism_jf, k+base_num_fv, iv+numv);
    }
  };


}
