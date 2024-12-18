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
bool flag_set_scalar(false);
SCALAR_TYPE addend(0);
SCALAR_TYPE scalar_value(0);

// Apply operation to interior vertices.
bool flag_interior_vertices(true);

// Apply operation to boundary vertices.
bool flag_boundary_vertices(true);

// Set to true, if some operation modifies the scalar grid.
bool flag_modified(false);

// Forward declarations, add/set functions.
void add_scalar
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid);
void add_scalar_to_interior_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid);
void add_scalar_to_boundary_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid);
void set_scalar
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid);
void set_scalar_of_interior_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid);
void set_scalar_of_boundary_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid);

// Forward declarations, misc functions.
void parse_command_line(int argc, char ** argv);
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
      if (flag_interior_vertices && flag_boundary_vertices)
        { add_scalar(addend, scalar_grid); }
      else if (flag_interior_vertices) {
        add_scalar_to_interior_vertices(addend, scalar_grid);
      }
      else if (flag_boundary_vertices) {
        add_scalar_to_boundary_vertices(addend, scalar_grid);
      }
    }
    else if (flag_set_scalar) {
      if (flag_interior_vertices && flag_boundary_vertices)
        { set_scalar(addend, scalar_grid); }
      else if (flag_interior_vertices) {
        set_scalar_of_interior_vertices(addend, scalar_grid);
      }
      else if (flag_boundary_vertices) {
        set_scalar_of_boundary_vertices(addend, scalar_grid);
      }
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
// Add scalar functions
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


void add_scalar_to_interior_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid)
{
  VERTEX_INDEX_TYPE iv = 0;
  for (int z = 1; z+1 < scalar_grid.AxisSize(2); z++) {
    for (int y = 1; y+1 < scalar_grid.AxisSize(1); y++) {
      iv = z*scalar_grid.AxisIncrement(2) +
        y*scalar_grid.AxisIncrement(1) + 1;
      for (int x = 1; x+1 < scalar_grid.AxisSize(0); x++) {
        const SCALAR_TYPE s = scalar_grid.Scalar(iv);
        scalar_grid.SetScalar(iv, s+addend);
        iv++;
      }
    }
  }

  flag_modified = true;
}


void add_scalar_to_boundary_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid)
{
  BOUNDARY_BITS_TYPE boundary_bits;

  // This algorithm iterates through all the grid vertices
  //   and checks if they are on the boundary.
  // A faster, but much more complicated way, to process
  //   boundary vertices would be to directly compute the
  //   vertices on each facet. The complication is in not
  //   processing the same boundary vertex more than once.
  
  for (VERTEX_INDEX_TYPE iv = 0; iv < scalar_grid.NumVertices(); iv++) {
    // This is an expensive operation, and should be avoided
    //   unless necessary.
    scalar_grid.ComputeVertexBoundaryBits(iv, boundary_bits);
    
    if (boundary_bits != 0) {
      // Some boundary bit is 1.
      // Vertex is on the boundary.
      const SCALAR_TYPE s = scalar_grid.Scalar(iv);
      scalar_grid.SetScalar(iv, s+addend);
    }
  }
  
  flag_modified = true;
}


// *******************************************************************
// Set scalar functions
// *******************************************************************

void set_scalar(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid)
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
        scalar_grid.SetScalar(iv, scalar_value);
        iv++;
      }
    }
  }

  flag_modified = true;
}


void set_scalar_of_interior_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid)
{
  VERTEX_INDEX_TYPE iv = 0;
  for (int z = 1; z+1 < scalar_grid.AxisSize(2); z++) {
    for (int y = 1; y+1 < scalar_grid.AxisSize(1); y++) {
      iv = z*scalar_grid.AxisIncrement(2) +
        y*scalar_grid.AxisIncrement(1) + 1;
      for (int x = 1; x+1 < scalar_grid.AxisSize(0); x++) {
        scalar_grid.SetScalar(iv, scalar_value);
        iv++;
      }
    }
  }

  flag_modified = true;
}


void set_scalar_of_boundary_vertices
(const SCALAR_TYPE addend, SCALAR_GRID3D & scalar_grid)
{
  BOUNDARY_BITS_TYPE boundary_bits;

  // This algorithm iterates through all the grid vertices
  //   and checks if they are on the boundary.
  // A faster, but much more complicated way, to process
  //   boundary vertices would be to directly compute the
  //   vertices on each facet. The complication is in not
  //   processing the same boundary vertex more than once.
  
  for (VERTEX_INDEX_TYPE iv = 0; iv < scalar_grid.NumVertices(); iv++) {
    // This is an expensive operation, and should be avoided
    //   unless necessary.
    scalar_grid.ComputeVertexBoundaryBits(iv, boundary_bits);
    
    if (boundary_bits != 0) {
      // Some boundary bit is 1.
      // Vertex is on the boundary.
      scalar_grid.SetScalar(iv, scalar_value);
    }
  }
  
  flag_modified = true;
}


// *******************************************************************
// Miscellaneous functions
// *******************************************************************

void usage_msg(std::ostream & out)
{
  out << "Usage: iso3D_scalar [OPTIONS] {input nrrd file} [{output nrrd file}]"
      << endl;
}

void options_msg(std::ostream & out)
{
  out << "Options:" << endl;
  out << "  [-add {s} | -set {s}] [-interior_vertices] [-boundary_vertices]" << endl;
  out << "  [-help]" << endl;
}

void usage_error()
{
  usage_msg(cerr);
  options_msg(cerr);
  
  exit(-1);
}

void help_msg()
{
  usage_msg(cout);
  cout << endl;
  cout << "iso3D_scalar - Process a scalar grid." << endl;
  cout << "  Mainly an example of reading/writing nrrd files," << endl;
  cout << "  and processing vertices/cubes in a scalar grid." << endl;
  cout << endl;
  cout << "Options:" << endl;
  cout << "  -add {s}: Add scalar value {s} to each grid vertex." << endl;
  cout << "  -set {s}: Set all scalar values to {s}." << endl;
  cout << "  -interior_vertices: Apply operations only to grid interior vertices." << endl;
  cout << "  -boundary_vertices: Apply operations only to grid boundary vertices." << endl;
  cout << "  -all_vertices:      Apply operations to all vertices." << endl;
  cout << "  -help:    Print this help message." << endl;
}


void help()
{
  help_msg();
  exit(0);
}

// Check command line options for inconsistencies.
void check_options()
{
  if (flag_add_scalar && flag_set_scalar) {
    cerr << "Usage error. Options -add and -set are incompatible."
         << endl;
    cerr << "  Use one or the other, not both." << endl;
    usage_error();
  }
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
    else if (s == "-set") {
      flag_set_scalar = true;
      const float x =
        get_arg_float(iarg, argc, argv, error);
      scalar_value = SCALAR_TYPE(x);
      iarg++;
    }
    else if (s == "-interior_vertices") {
      flag_interior_vertices = true;
      flag_boundary_vertices = false;
    }
    else if (s == "-boundary_vertices") {
      flag_interior_vertices = false;
      flag_boundary_vertices = true;
    }
    else if (s == "-all_vertices") {
      flag_interior_vertices = true;
      flag_boundary_vertices = true;
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

  if (iarg+2 < argc) {
    usage_error();
  }

  input_filename = argv[iarg];
  iarg++;

  if (iarg < argc)
    { output_filename = argv[iarg]; }
  else
    { output_filename = default_output_filename; }

  check_options();
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
