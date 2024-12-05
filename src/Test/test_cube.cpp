/*!
 *  @file test_cube.cpp
 *  @brief Test CUBE3D class.
 */

#include <cstddef>
#include <iostream>
#include <vector>

#include "iso3D_cube.h"
#include "iso3D_error.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;
  

// Forward declarations
void output_cube(const CUBE3D & cube);
void output_edges(const CUBE3D & cube);
void output_facets(const CUBE3D & cube);


int main(int argc, char ** argv)
{
  const CUBE3D cube;
  
  try {
    output_cube(cube);
    cout << endl;
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }
  cout << endl;
  
  return 0;
}


void output_cube(const CUBE3D & cube)
{
  cout << "Dimension: " << cube.Dimension() << endl;
  cout << "Number of cube vertices: " << cube.NumVertices() << endl;

  for (int iv = 0; iv < cube.NumVertices(); iv++) {
    cube.OutVertexIndexAndCoord(cout, "Vertex ", iv, "\n");
  }
  cout << endl;

  output_edges(cube);
  cout << endl;
  
  output_facets(cube);
}


void output_edges(const CUBE3D & cube)
{
  cout << "Number of cube edges:" << cube.NumEdges() << endl;
  for (int iedge = 0; iedge < cube.NumEdges(); iedge++) {
    cout << "Edge " << iedge
         << ", direction " << cube.EdgeDirection(iedge)
         << ", endpoints ";
    cube.OutEdgeEndpoints(cout, "", iedge, "\n");
  }
}

void output_facets(const CUBE3D & cube)
{
  cout << "Number of cube facets:" << cube.NumFacets() << endl;
  for (int ifacet = 0; ifacet < cube.NumFacets(); ifacet++) {
    cout << "Facet " << ifacet
         << ", side " << cube.FacetSide(ifacet)
         << ", orthogonal direction " << cube.FacetOrthDir(ifacet)
         << ", opposite facet " << cube.OppositeFacet(ifacet)
         << "." << endl;

    cube.OutFacetVertices
      (cout, "  Facet vertices: ", ifacet, "\n");
    cube.OutFacetVerticesCCW
      (cout, "  Facet vertices in counter-clockwise order: ", ifacet, "\n");    
  }
  
}

