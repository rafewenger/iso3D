/*!
 *  @file test_grid.cpp
 *  @brief Test GRID3D class.
 */

#include <cstddef>
#include <iostream>
#include <vector>

#include "iso3D_const.h"
#include "iso3D_grid.h"
#include "iso3D_error.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;

// Global variables
bool flag_output_coord(false);

// Forward declarations
void parse_command_line(int argc, char ** argv);
void output_grid(const GRID3D & grid);
void output_grid_vertices(const GRID3D & grid);
void output_prev_next_grid_vertices(const GRID3D & grid);
void output_prev_next_grid_vertex(const GRID3D & grid, const int iv);


int main(int argc, char ** argv)
{
  try {
    const AXIS_SIZE_TYPE asizeA[DIM3] = { 3, 4, 5 };
    GRID3D gridA(asizeA);

    parse_command_line(argc, argv);
    
    cout << "Grid A" << endl;
    output_grid(gridA);
    cout << endl;

    if (flag_output_coord) {
      output_grid_vertices(gridA);
      cout << endl;
    }

    output_prev_next_grid_vertices(gridA);
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
  out << "Usage: test_grid -coord" << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

  
void parse_command_line(int argc, char ** argv)
{
  int iarg = 1;
  while (iarg < argc) {
    std::string s = argv[iarg];
    if (s == "-coord") {
      flag_output_coord = true;
    }
    else {
      cerr << "Usage error. Illegal parameter: " << s << endl;
      usage_error();
    }

    iarg++;
  }
}


void output_grid(const GRID3D & grid)
{
  cout << "Dimension: " << grid.Dimension() << endl;
  grid.OutAxisSize(cout, "Axis size: ", "\n");
  cout << "Number of grid vertices: " << grid.NumVertices() << endl;
  cout << "Number of grid cubes: " << grid.NumCubes() << endl;
  grid.OutAxisIncrement(cout, "Axis increment: ", "\n");
  grid.OutCubeVertexIncrement(cout, "Cube vertex increment: ", "\n");
}


void output_grid_vertices(const GRID3D & grid)
{
  for (int iv = 0; iv < grid.NumVertices(); iv++) {
    grid.OutVertexIndexAndCoord(cout, "Grid vertex ", iv, "\n");
  }
}


void output_prev_next_grid_vertex(const GRID3D & grid, const int iv)
{
  GRID_COORD vertex_coord[DIM3];
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
      const VERTEX_INDEX iv_prev = grid.PrevVertex(iv, d);
      grid.OutVertexIndexAndCoord(cout, "  Prev vertex ", iv_prev, ".");
    }

    if (vertex_coord[d]+1 == grid.AxisSize(d)) {
      cout << "  On upper facet orthogonal to " << d << ".";
    }
    else {
      const VERTEX_INDEX iv_next = grid.NextVertex(iv, d);
      grid.OutVertexIndexAndCoord(cout, "  Next vertex ", iv_next, ".");
    }
    cout << endl;
  }
}


void output_prev_next_grid_vertices(const GRID3D & grid)
{
  output_prev_next_grid_vertex(grid, 0);
  for (int d = 0; d < grid.Dimension(); d++) {
    output_prev_next_grid_vertex(grid, grid.AxisSize(d)-1);
    output_prev_next_grid_vertex(grid, grid.AxisSize(d));
  }
  output_prev_next_grid_vertex(grid, grid.NumVertices()-1);

  if ((grid.AxisSize(0) > 1) && (grid.AxisSize(1) > 2) &&
      (grid.AxisSize(2) > 3)) {
    const VERTEX_INDEX iv =
      1 + 2*grid.AxisIncrement(1) + 3*grid.AxisIncrement(2);
    output_prev_next_grid_vertex(grid, iv);
  }
}
