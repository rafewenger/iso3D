/*!
 *  @file iso3D_nrrd_info.cpp.
 *  @brief Print information about an nrrd file.
 */

#include <iostream>

#include "iso3D_command_line.h"
#include "iso3D_const.h"
#include "iso3D_error.h"
#include "iso3D_grid_nrrd.h"
#include "iso3D_isopatch.h"
#include "iso3D_scalar_grid.h"
#include "iso3D_templates.h"
#include "iso3D_types.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;

// Global variables
char * input_filename;
bool flag_out_scalar(false);
bool flag_out_active_cubes(false);
SET_VALUE<SCALAR_TYPE> isovalue(0.0);

// Maximum number of scalar values allowed for output.
const int MAX_NUM_SCALAR_OUTPUT(1000);

// Forward declarations
void parse_command_line(int argc, char ** argv);
void output_grid_info(const GRID3D & grid);
void output_scalar_grid_info(const SCALAR_GRID3D & scalar_grid);
void output_scalar_values(const SCALAR_GRID3D_BASE & grid);


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
      
    output_grid_info(scalar_grid);
    output_scalar_grid_info(scalar_grid);
    
    if (flag_out_scalar) {
      if (scalar_grid.NumVertices() <= MAX_NUM_SCALAR_OUTPUT)
        { output_scalar_values(scalar_grid); }
      else {
        cerr << endl;
        cerr << "*** Too many (" << "> " << MAX_NUM_SCALAR_OUTPUT
             << ") scalar values to output." << endl;
      }
    }
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


// *******************************************************************
// Count functions
// *******************************************************************

NUMBER_TYPE count_num_active_cubes
(const SCALAR_GRID3D & scalar_grid, const SCALAR_TYPE isovalue)
{
  // Initialize
  NUMBER_TYPE num_active_cubes = 0;
  
  CUBE_INDEX_TYPE icube = 0;
  for (int z = 0; z+1 < scalar_grid.AxisSize(2); z++) {
    for (int y = 0; y+1 < scalar_grid.AxisSize(1); y++) {
      icube = z*scalar_grid.AxisIncrement(2) + y*scalar_grid.AxisIncrement(1);
      for (int x = 0; x+1 < scalar_grid.AxisSize(0); x++) {
        if (intersects_cube(scalar_grid, isovalue, icube))
          { num_active_cubes++; }
        icube++;
      }
    }
  }


  return num_active_cubes;
}


// *******************************************************************
// Output functions
// *******************************************************************

void output_grid_info(const GRID3D & grid)
{
  cout << "Dimension: " << grid.Dimension() << endl;
  grid.OutAxisSize(cout, "Axis size: ", "\n");
  grid.OutSpacing(cout, "Spacing: ", "\n");
  cout << "Number of grid vertices: " << grid.NumVertices() << endl;
  cout << "Number of grid cubes: " << grid.NumCubes() << endl;
}

void output_scalar_grid_info(const SCALAR_GRID3D & scalar_grid)
{
  if (isovalue.IsSet()) {
    const NUMBER_TYPE num_active_cubes =
      count_num_active_cubes(scalar_grid, isovalue.Value());
    cout << "Number of grid cubes intersecting the isosurface: "
         << num_active_cubes << endl;
  }
}

void output_scalar_values(const SCALAR_GRID3D_BASE & grid)
{
  cout << "Scalar values:" << endl;
  grid.OutScalar(cout, "  ");
}


// *******************************************************************
// Miscellaneous functions
// *******************************************************************

void usage_msg(std::ostream & out)
{
  out << "Usage: iso3D_nrrd_info [-isovalue {s}] [-scalar] [-help] {input nrrd file}" << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

void help_msg()
{
  usage_msg(cout);
  cout << endl;
  cout << "iso3D_nrrd_info - Output information about the scalar grid in an nrrd file." << endl;
  cout << endl;
  cout << "Options:" << endl;
  cout << "  -isovalue {s}: Set isovalue to {s}." << endl;
  cout << "           When isovalue is set, output number of grid cubes"
       << endl;
  cout << "           intersecting the isosurface." << endl;
  cout << "  -scalar: Print the grid of scalar values, formatted in rows and columns." << endl;
  cout << "           Does not print if number of grid vertices exceeds "
       << MAX_NUM_SCALAR_OUTPUT << "." << endl;
  cout << "  -help:   Print this help message." << endl;
}

void help()
{
  help_msg();
  exit(0);
}

void parse_command_line(int argc, char ** argv)
{
  ERROR error;
  
  int iarg = 1;
  while ((iarg < argc) && (argv[iarg][0] == '-')) {
    std::string s = argv[iarg];
    if (s == "-scalar") {
      flag_out_scalar = true;
    }
    else if (s == "-isovalue") {
      const float x =
        get_arg_float(iarg, argc, argv, error);
      isovalue.Set(SCALAR_TYPE(x));
      iarg++;
    }
    else if (s == "-help") {
      help();
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

    
