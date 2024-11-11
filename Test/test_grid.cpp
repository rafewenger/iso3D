/*!
 *  @file test_grid.cpp
 *  @brief Test GRID3D class.
 */

#include <cstddef>
#include <iostream>

#include "iso3D_const.h"
#include "iso3D_cube.h"
#include "iso3D_grid.h"
#include "iso3D_error.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;

// Global variables
bool flag_list_vertices(false);
bool flag_list_cubes(false);
bool flag_boundary(false);

// Forward declarations
void parse_command_line(int argc, char ** argv);
void output_grid(const GRID3D & grid);
void output_grid_vertices(const GRID3D & grid);
void output_grid_cubes(const GRID3D & grid);
void output_some_prev_next_boundary_grid_vertices(const GRID3D & grid);
void output_prev_next_grid_vertex(const GRID3D & grid, const int iv);
void output_prev_next_grid_vertex
(const GRID3D & grid, const GRID_COORD_TYPE vertex_coord[DIM3]);
void output_cube_vertices
(const GRID3D & grid, const GRID_COORD_TYPE cube_coord[DIM3]);


int main(int argc, char ** argv)
{
  try {
    const AXIS_SIZE_TYPE asizeA[DIM3] = { 3, 4, 5 };
    GRID3D gridA(asizeA);
    GRID_COORD_TYPE grid_coordA123[DIM3] = { 1, 2, 3 };

    parse_command_line(argc, argv);
    
    cout << "Grid A" << endl;
    output_grid(gridA);
    cout << endl;

    if (flag_list_vertices) {
      output_grid_vertices(gridA);
      cout << endl;
    }

    if (flag_list_cubes) {
      output_grid_cubes(gridA);
      cout << endl;
    }

    output_prev_next_grid_vertex(gridA, grid_coordA123);
    
    if (flag_boundary) {
      output_some_prev_next_boundary_grid_vertices(gridA);
    }
    cout << endl;

    output_cube_vertices(gridA, grid_coordA123);
    cout << endl;
                         
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


void usage_msg(std::ostream & out)
{
  out << "Usage: test_grid -listv -listc -boundary" << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

void help_msg()
{
  usage_msg(cout);
  cout << "  -listv: Print coordinates of each vertex." << endl;
  cout << "  -listc: Print coordinates of each cube." << endl;
  cout << "  -boundary: Print prev/next of some vertices on the grid boundary."
       << endl;
  exit(0);
}

void parse_command_line(int argc, char ** argv)
{
  int iarg = 1;
  while (iarg < argc) {
    std::string s = argv[iarg];
    if (s == "-listv") {
      flag_list_vertices = true;
    }
    else if (s == "-listc") {
      flag_list_cubes = true;
    }
    else if (s == "-boundary") {
      flag_boundary = true;
    }
    else if (s == "-help") {
      help_msg();
    }
    else {
      cerr << "Usage error. Illegal parameter: " << s << endl;
      usage_error();
    }

    iarg++;
  }
}

void output_grid_facet_info
(const GRID3D & grid, const int ifacet)
{
  const int num_vertices_in_grid_facet =
    grid.ComputeNumVerticesInGridFacet(ifacet);
  const int num_cubes_supported_by_grid_facet =
    grid.ComputeNumCubesSupportedByGridFacet(ifacet);

  cout << "Facet " << ifacet
       << ". Number of vertices: " << num_vertices_in_grid_facet
       << ". Number of supported cubes: " << num_cubes_supported_by_grid_facet
       << endl;
}


void output_grid(const GRID3D & grid)
{
  cout << "Dimension: " << grid.Dimension() << endl;
  grid.OutAxisSize(cout, "Axis size: ", "\n");
  grid.OutSpacing(cout, "Spacing: ", "\n");
  cout << "Number of grid vertices: " << grid.NumVertices() << endl;
  cout << "Number of grid cubes: " << grid.NumCubes() << endl;
  output_grid_facet_info(grid, 0);
  output_grid_facet_info(grid, 1);
  output_grid_facet_info(grid, 2);
  grid.OutAxisIncrement(cout, "Axis increment: ", "\n");
  grid.OutCubeVertexIncrement(cout, "Cube vertex increment: ", "\n");
}


void output_grid_vertices(const GRID3D & grid)
{
  for (int iv = 0; iv < grid.NumVertices(); iv++) {
    grid.OutVertexIndexAndCoord(cout, "Grid vertex ", iv, ".");
    grid.OutVertexBoundaryBits(cout, "  Boundary bits: ", iv, ".\n");
  }
}


void output_grid_cubes(const GRID3D & grid)
{
  CUBE_INDEX_TYPE icube = 0;
  for (int z = 0; z+1 < grid.AxisSize(2); z++) {
    for (int y = 0; y+1 < grid.AxisSize(1); y++) {
      icube = z*grid.AxisIncrement(2) + y*grid.AxisIncrement(1);
      for (int x = 0; x+1 < grid.AxisSize(0); x++) {
        cout << "Cube " << icube
             << " (" << x << "," << y << "," << z << ")";
        grid.OutCubeCenterCoord
          (cout, ".  Cube center: ", icube, ".");
        grid.OutCubeBoundaryBits(cout, "  Boundary bits: ", icube, ".\n");
        icube++;
      }
    }
  }
}


void output_prev_next_grid_vertex(const GRID3D & grid, const int iv)
{
  GRID_COORD_TYPE vertex_coord[DIM3];
  PROCEDURE_ERROR error("output_prev_next_grid_vertex");

  if ((iv < 0) || (iv >= grid.NumVertices())) {
    error.AddToMessage("Error. Illegal vertex value ", iv, ".");
    throw error;
  }

  grid.ComputeCoord(iv, vertex_coord);
  grid.OutVertexIndexAndCoord(cout, "Grid vertex ", iv, ".\n");

  for (int d = 0; d < grid.Dimension(); d++) {
    if (vertex_coord[d] == 0) {
      cout << "  On lower facet orthogonal to " << d << ".";
    }
    else {
      const VERTEX_INDEX_TYPE iv_prev = grid.PrevVertex(iv, d);
      grid.OutVertexIndexAndCoord(cout, "  Prev vertex ", iv_prev, ".");
    }

    if (vertex_coord[d]+1 == grid.AxisSize(d)) {
      cout << "  On upper facet orthogonal to " << d << ".";
    }
    else {
      const VERTEX_INDEX_TYPE iv_next = grid.NextVertex(iv, d);
      grid.OutVertexIndexAndCoord(cout, "  Next vertex ", iv_next, ".");
    }
    cout << endl;
  }
}


void output_prev_next_grid_vertex
(const GRID3D & grid, const GRID_COORD_TYPE vertex_coord[DIM3])
{
  const VERTEX_INDEX_TYPE iv = grid.ComputeVertexIndex(vertex_coord);
  output_prev_next_grid_vertex(grid, iv);
}
  
void output_some_prev_next_boundary_grid_vertices(const GRID3D & grid)
{
  output_prev_next_grid_vertex(grid, 0);
  for (int d = 0; d < grid.Dimension(); d++) {
    output_prev_next_grid_vertex(grid, grid.AxisSize(d)-1);
    output_prev_next_grid_vertex(grid, grid.AxisSize(d));
  }
  output_prev_next_grid_vertex(grid, grid.NumVertices()-1);
}


void output_cube_vertices(const GRID3D & grid, const int icube)
{
  PROCEDURE_ERROR error("output_cube_vertices");

  if (!grid.CheckCubeIndex(icube, error))
    { throw error; }
  
  grid.OutVertexIndexAndCoord(cout, "Cube ", icube, ". Vertices: \n");
  for (int j = 0; j < CUBE3D::NumVertices(); j++) {
    const VERTEX_INDEX_TYPE jv = grid.CubeVertex(icube, j);
    grid.OutVertexIndexAndCoord(cout, "  Vertex ", jv, "\n");
  }
}


void output_cube_vertices
  (const GRID3D & grid, const GRID_COORD_TYPE cube_coord[DIM3])
{
  const CUBE_INDEX_TYPE icube = grid.ComputeVertexIndex(cube_coord);

  output_cube_vertices(grid, icube);
}
