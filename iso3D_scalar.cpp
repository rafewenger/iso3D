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
const char * input_filename;
const char * output_filename;
const char * default_output_filename = "out.nrrd";
bool flag_add_scalar(false);
SCALAR_TYPE addend(0);

// Set to true, if some operation modifies the scalar grid.
bool flag_modified(false);

// Forward declarations
void parse_command_line(int argc, char ** argv);
void add_scalar(const SCALAR_TYPE addend,
                SCALAR_GRID3D & scalar_grid);
void write_scalar_grid
(const char * output_filename, const SCALAR_GRID3D & scalar_grid);

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

    write_scalar_grid(output_filename, scalar_grid);
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
        iv++;
      }
    }
  }

  flag_modified = true;
}


// *******************************************************************
// Miscellaneous functions
// *******************************************************************

void usage_msg(std::ostream & out)
{
  out << "Usage: iso3D_scalar [-add {s}] {input nrrd file} [{output nrrd file}]"
      << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

void parse_command_line(int argc, char ** argv)
{
  ERROR error;
  
  int iarg = 1;
  while ((iarg < argc) && (argv[iarg][0] == '-')) {
    std::string s = argv[iarg];
    if (s == "-add") {
      flag_add_scalar = true;
      const float x =
        get_arg_float(iarg, argc, argv, error);
      addend = SCALAR_TYPE(x);
      iarg++;
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

  if (iarg+2 < argc) {
    usage_error();
  }

  input_filename = argv[iarg];
  iarg++;

  if (iarg < argc)
    { output_filename = argv[iarg]; }
  else
    { output_filename = default_output_filename; }
}

    
void write_scalar_grid
(const char * output_filename, const SCALAR_GRID3D & scalar_grid)
{
  const char * ofilename = output_filename;
  
  if (ofilename == NULL)
    { ofilename = default_output_filename; }

  if (flag_modified) {
    cout << "Writing scalar grid to nrrd file: "
         << ofilename << endl;
  }
  else {
    cout << "Copying scalar grid to nrrd file: "
         << ofilename << endl;
  }

  write_scalar_grid_nrrd(ofilename, scalar_grid);
}
