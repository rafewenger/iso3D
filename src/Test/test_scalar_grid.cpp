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
void test_set(const AXIS_SIZE_TYPE asizeA[DIM3],
              const AXIS_SIZE_TYPE asizeB[DIM3]);


int main(int argc, char ** argv)
{
  try {
    const AXIS_SIZE_TYPE asizeA[DIM3] = { 3, 4, 5 };
    const AXIS_SIZE_TYPE asizeB[DIM3] = { 4, 5, 6 };
    SCALAR_GRID3D gridA(asizeA);

    /* NOT YET USED.
    parse_command_line(argc, argv);
    */

    set_scalar_values(gridA);
    
    cout << "Grid A" << endl;
    output_grid_info(gridA);
    cout << endl;

    output_scalar_values(gridA);
    cout << endl;

    test_set(asizeA, asizeB);
    cout << "Passed tests." << endl;
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


bool compare_grids(const SCALAR_GRID3D_BASE & gridA,
                   const SCALAR_GRID3D_BASE & gridB,
                   ERROR & error)
{
  if (gridA.NumVertices() != gridB.NumVertices()) {
    error.AddToMessage("Unequal number of vertices.");
    error.AddToMessage
      ("  Grid A num vertices: ", gridA.NumVertices(), "");
    error.AddToMessage
      ("  Grid B num vertices: ", gridB.NumVertices(), "");
    return false;
  }
  
  for (VERTEX_INDEX_TYPE iv = 0; iv < gridA.NumVertices(); iv++) {
    if (gridA.Scalar(iv) != gridB.Scalar(iv)) {
      error.AddToMessage("Unequal scalar values.");
      error.AddToMessage
        ("  Grid A, scalar[", iv, "]: ", gridA.Scalar(iv), "");
      error.AddToMessage
        ("  Grid B, scalar[", iv, "]: ", gridB.Scalar(iv), "");
      return false;
    }
  }

  return true;
}


void test_set(const AXIS_SIZE_TYPE asizeA[DIM3],
              const AXIS_SIZE_TYPE asizeB[DIM3])
{
  SCALAR_GRID3D gridA(asizeA);
  SCALAR_GRID3D gridB(asizeB);
  SCALAR_GRID3D gridC;
  PROCEDURE_ERROR error("test_set");

  set_scalar_values(gridA);
  set_scalar_values(gridB);
  
  gridC.SetAxisSize(asizeA);
  set_scalar_values(gridC);

  if (!compare_grids(gridA, gridC, error)) {
    throw error;
  }

  gridC.SetAxisSize(asizeB);
  set_scalar_values(gridC);
  if (!compare_grids(gridB, gridC, error)) {
    throw error;
  }

  gridB.SetAxisSize(asizeA);
  set_scalar_values(gridB);
  if (!compare_grids(gridA, gridB, error)) {
    throw error;
  }

}

              
