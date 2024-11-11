/*!
 *  @file test_scalar_grid.cpp
 *  @brief Test SCALAR_GRID3D class.
 */

#include <cstddef>
#include <iostream>

#include "iso3D_const.h"
#include "iso3D_types.h"
#include "iso3D_scalar_grid.h"
#include "iso3D_error.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;

// Global variables

// Forward declarations
void parse_command_line(int argc, char ** argv);
void set_scalar_values(SCALAR_GRID3D_BASE & grid);
void output_grid_info(const GRID3D & grid);
void output_scalar_values(const SCALAR_GRID3D_BASE & grid);


int main(int argc, char ** argv)
{
  try {
    const AXIS_SIZE_TYPE asizeA[DIM3] = { 3, 4, 5 };
    SCALAR_GRID3D gridA(asizeA);
    GRID_COORD_TYPE grid_coordA123[DIM3] = { 1, 2, 3 };

    /* NOT YET USED.
    parse_command_line(argc, argv);
    */

    set_scalar_values(gridA);
    
    cout << "Grid A" << endl;
    output_grid_info(gridA);
    cout << endl;

    output_scalar_values(gridA);
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
  out << "Usage: test_scalar_grid" << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

void set_scalar_values(SCALAR_GRID3D_BASE & grid)
{
  VERTEX_INDEX_TYPE iv = 0;
  for (int z = 0; z < grid.AxisSize(2); z++) {
    for (int y = 0; y < grid.AxisSize(1); y++) {
      iv = z*grid.AxisIncrement(2) + y*grid.AxisIncrement(1);
      for (int x = 0; x < grid.AxisSize(0); x++) {
        const SCALAR_TYPE s = z*100 + y*10 + x;
        grid.SetScalar(iv, s);
        iv++;
      }
    }
  }
}

void output_grid_info(const GRID3D & grid)
{
  cout << "Dimension: " << grid.Dimension() << endl;
  grid.OutAxisSize(cout, "Axis size: ", "\n");
  grid.OutSpacing(cout, "Spacing: ", "\n");
  cout << "Number of grid vertices: " << grid.NumVertices() << endl;
  cout << "Number of grid cubes: " << grid.NumCubes() << endl;
}


void output_scalar_values(const SCALAR_GRID3D_BASE & grid)
{
  cout << "Scalar values:" << endl;
  grid.OutScalar(cout, "  ");
}

