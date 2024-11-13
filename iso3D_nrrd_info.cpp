/*!
 *  @file iso3D_nrrd_info
 *  @brief Print information from an nrrd file.
 */

#include <iostream>

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

// Forward declarations
void parse_command_line(int argc, char ** argv);
void output_grid_info(const GRID3D & grid);


int main(int argc, char ** argv)
{
  try {
    ERROR error;
    SCALAR_GRID3D scalar_grid;
    GRID3D_NRRD_IN nrrd_in;

    parse_command_line(argc, argv);
    
    nrrd_in.ReadScalarGrid(input_filename, scalar_grid, error);

    output_grid_info(scalar_grid);

  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
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


// *******************************************************************
// Miscellaneous functions
// *******************************************************************

void usage_msg(std::ostream & out)
{
  out << "Usage: iso3D_nrrd_info {input nrrd file}" << endl;
}


void usage_error()
{
  usage_msg(cerr);
  exit(-1);
}

void parse_command_line(int argc, char ** argv)
{
  int iarg = 1;
  /* NOT USED
  while (iarg < argc) {
    std::string s = argv[iarg];
    if (s == "-help") {
      help_msg();
    }
    else {
      cerr << "Usage error. Illegal parameter: " << s << endl;
      usage_error();
    }

    iarg++;
  }
  */

  if (iarg == argc) {
    cerr << "Usage error. Missing input filename." << endl;
    usage_error();
  }

  if (iarg+1 < argc) {
    usage_error();
  }

  input_filename = argv[iarg];
}

    
