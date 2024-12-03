/*!
 *  @file iso3D_MCtable_info.cpp.
 *  @brief Print information about Marching Cubes isosurface lookup table.
 */

#include <iostream>

#include "iso3D_command_line.h"
#include "iso3D_const.h"
#include "iso3D_error.h"
#include "iso3D_MCtable.h"
#include "iso3D_types.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;

// Global variables
char * input_filename;

// Forward declarations
void parse_command_line(int argc, char ** argv);


int main(int argc, char ** argv)
{
  try {
    ERROR error;

    parse_command_line(argc, argv);
    
    // TO BE CONTINUED...
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



// *******************************************************************
// Miscellaneous functions
// *******************************************************************

void usage_msg(std::ostream & out)
{
  out << "Usage: iso3D_MCtable_info {MCtable .xit file}" << endl;
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
  cout << "iso3D_MCtable_info - Output information about the Marching Cubes isosurface lookup table." << endl;
  cout << endl;
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
    const std::string s = argv[iarg];
    if (s == "-help") {
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

