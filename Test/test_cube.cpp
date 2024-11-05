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

// Forward declarations
void output_cube(const CUBE3D & cube);


int main(int argc, char ** argv)
{
  using std::cout;
  using std::endl;

  const CUBE3D cube;
  
  try {
    output_cube(cube);
    cout << endl;
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


void output_cube(const CUBE3D & cube)
{
  using std::cout;
  using std::endl;
  
  cout << "Dimension: " << cube.Dimension() << endl;
  cout << "Number of cube vertices: " << cube.NumVertices() << endl;
  cout << "Number of cube edges: " << cube.NumEdges() << endl;
  cout << "Number of cube facets:" << cube.NumFacets() << endl;

  for (int iv = 0; iv < cube.NumVertices(); iv++) {
    cube.OutVertexIndexAndCoord(cout, "Vertex ", iv, "\n");
  }
}

