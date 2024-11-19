/*!
 *  @file iso3D_scalar.cpp
 *  @brief Process a scalar grid.
 *  - This program is mainly meant to provide examples
 *    on processing vertices and cubes in a scalar grid.
 */

#include <iostream>

#include "iso3D_command_line.h"
#include "iso3D_const.h"
#include "iso3D_error.h"
#include "iso3D_grid_nrrd.h"
#include "iso3D_scalar_grid.h"
#include "iso3D_types.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;

// Global variables
char * input_filename;
bool flag_add_scalar(false);
SCALAR_TYPE addend(0);

// Forward declarations
void parse_command_line(int argc, char ** argv);
void add_scalar(const SCALAR_TYPE addend,
                SCALAR_GRID3D & scalar_grid);


int main(int argc, char ** argv)
{
  try {
    ERROR error;
    SCALAR_GRID3D scalar_grid;
    GRID3D_NRRD_IN nrrd_in;

    parse_command_line(argc, argv);
    
    nrrd_in.ReadScalarGrid(input_filename, scalar_grid, error);

    if (nrrd_in.ReadFailed())
      { throw error; }

    if (flag_add_scalar) {
      add_scalar(addend, scalar_grid);
    }

  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


// *******************************************************************
// Processing functions
// *******************************************************************

void add_scalar(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid)
{
  // A simpler way would be to use:
  //   for (VERTEX_INDEX_TYPE iv = 0; iv < scalar_grid.NumVertices(); iv++).
  // However, this code shows how one can process grid vertices
  //   along the x, then y, then z axes.
  VERTEX_INDEX_TYPE iv = 0;
  for (int z = 0; z < scalar_grid.AxisSize(2); z++) {
    for (int y = 0; y < scalar_grid.AxisSize(1); y++) {
      iv = z*scalar_grid.AxisIncrement(2) + y*scalar_grid.AxisIncrement(1);
      for (int x = 0; x < scalar_grid.AxisSize(0); x++) {
        const SCALAR_TYPE s = scalar_grid.Scalar(iv);
        scalar_grid.SetScalar(iv, s+addend);
      }
    }
  }
    
}


// *******************************************************************
// Miscellaneous functions
// *******************************************************************

void usage_msg(std::ostream & out)
{
  out << "Usage: iso3D_scalar [-add {s}] {input nrrd file} {output nrrd file}"
      << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

void parse_command_line(int argc, char ** argv)
{
  int iarg = 1;
  while ((iarg < argc) && (argv[iarg][0] == '-')) {
    std::string s = argv[iarg];
    if (s == "-add") {
      flag_add_scalar = true;
      // TO BE CONTINUED...
    }
    else {
      cerr << "Usage error. Illegal parameter: " << s << endl;
      usage_error();
    }

    iarg++;
  }

  if (iarg == argc) {
    cerr << "Usage error. Missing input filename." << endl;
    usage_error();
  }

  if (iarg+1 < argc) {
    usage_error();
  }

  input_filename = argv[iarg];
}

    
