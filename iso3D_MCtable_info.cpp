/*!
 *  @file iso3D_MCtable_info.cpp.
 *  @brief Print information about Marching Cubes isosurface lookup table.
 */

#include <fstream>
#include <iomanip>
#include <iostream>

#include "iso3D_bits.h"
#include "iso3D_command_line.h"
#include "iso3D_const.h"
#include "iso3D_error.h"
#include "iso3D_MCtable.h"
#include "iso3D_MCtable_xitIO.h"
#include "iso3D_print.h"
#include "iso3D_types.h"

using namespace ISO3D;
using std::cerr;
using std::cout;
using std::endl;
using std::setw;

// Global variables
char * input_filename;
void out_poly(const MC_TABLE_POLY_BASE & poly);
bool out_stat_flag = false;
bool out_poly_flag = false;
bool out_isovert_flag = false;
std::vector<int> entry;

// Forward declarations
void read_isotable
(const char * isotable_filename, MC_TABLE & isotable);
void parse_command_line(int argc, char ** argv);
void out_isosurface_vertices(const MC_TABLE & isotable);
void out_isovert_stat(const MC_TABLE & isotable);
void out_entry(const int it, const MC_TABLE & isotable);
void out_stat(const MC_TABLE & isotable);


int main(int argc, char ** argv)
{
  try {
    MC_TABLE isotable;
    ERROR error;

    parse_command_line(argc, argv);

    read_isotable(input_filename, isotable);

    if (out_poly_flag) { out_poly(isotable.Polytope()); };
    if (out_isovert_flag) { out_isosurface_vertices(isotable); };
    for (size_t i = 0; i < entry.size(); i++) 
      { out_entry(entry[i], isotable); };
    if (out_stat_flag) { out_stat(isotable); };
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


// **************************************************
// Read isosurface lookup table
// **************************************************

// read isosurface lookup table
void read_isotable
(const char * isotable_filename, MC_TABLE & isotable)
{
  std::ifstream isotable_file(isotable_filename, std::ios::in);

  using std::string;
  
  if (!isotable_file) {
    string error_msg = 
      "Unable to open isosurface table file " + string(isotable_filename) 
      + ".";
    throw(ERROR(error_msg));
  };

  try {
    read_xit(isotable_file, isotable);
  }
  catch(...) {
    cerr << "Error reading file " << isotable_filename << "." << endl;
    throw;
  };

  isotable_file.close();

  ERROR error;
  if (!isotable.Check(error)) {
    cerr << "Warning: Data structure inconsistency in isosurface table."
	 << endl;
    error.Out(cerr);
    cerr << "  Attempting to continue..." << endl << endl;
  };
}


//**************************************************
// Output isosurface table polytope
//**************************************************

// output polytope
void out_poly(const MC_TABLE_POLY_BASE & poly)
{
  if (!poly.IsShapeUndefined()) {
    cout << "Polytope shape: " << poly.ShapeString() << endl;
  }
  int numv = poly.NumVertices();
  cout << "Number of polytope vertices = " << numv << "." << endl;
  cout << "Vertices:" << endl;
  for (int iv = 0; iv < numv; iv++) {
    cout << "  " << setw(3) << iv << ": ";
    poly.PrintVertexCoord(cout, iv);
    cout << endl;
  };
  cout << endl;

  int nume = poly.NumEdges();
  cout << "Number of polytope edges = " << nume << "." << endl;
  cout << "Edges: " << endl;
  for (int ie = 0; ie < nume; ie++) {
    cout << "  " << setw(3) << ie << ": ";
    poly.PrintEdgeEndpoints(cout, ie);
    cout << endl;
  };
  cout << endl;

  int numf = poly.NumFacets();
  cout << "Number of polytope facets = " << numf << "." << endl;
  cout << "Facets: " << endl;
  for (int jf = 0; jf < numf; jf++) {
    cout << "  " << setw(3) << jf << ": ";
    poly.PrintFacetVertices(cout, jf);
    cout << endl;
  }
  cout << endl;
}


//**************************************************
// Output isosurface vertices
//**************************************************

// output isosurface vertices
void out_isosurface_vertices(const MC_TABLE & isotable)
{
  cout << "Number of isosurface vertices = "
       << isotable.NumIsosurfaceVertices() << "." << endl;
  cout << "Vertices:" << endl;
  for (int i = 0; i < isotable.NumIsosurfaceVertices(); i++) {
    cout << "  " << setw(3) << i << ":  ";
    switch(isotable.IsosurfaceVertex(i).Type()) {
    case ISOSURFACE_VERTEX::VERTEX:
      cout << "Poly vertex " << isotable.IsosurfaceVertex(i).Face();
      break;

    case ISOSURFACE_VERTEX::EDGE:
      cout << "Poly edge " << isotable.IsosurfaceVertex(i).Face();
      break;

    case ISOSURFACE_VERTEX::FACET:
      cout << "Poly facet " << isotable.IsosurfaceVertex(i).Face();
      break;

    case ISOSURFACE_VERTEX::POINT:
      cout << "Point";
      if (isotable.IsosurfaceVertex(i).NumCoord() > 0) {
        cout << ".  Coordinates:";
        for (int d = 0; d < isotable.Dimension(); d++) {
          cout << " " << isotable.IsosurfaceVertex(i).Coord(d);
        }
      }

      break;
    };
    cout << ".";

    if (isotable.IsosurfaceVertex(i).IsLabelSet()) {
      cout << "  Label = \"" << isotable.IsosurfaceVertex(i).Label()
           << "\".";
    };
    cout << endl;
  };
  cout << endl;
}


//**************************************************
// Output isosurface table entry
//**************************************************

// return true if "it" is in range
// print error message and return false if "it" is out of range
bool CheckEntryRange(const int it, const MC_TABLE & isotable)
{
  if (it < 0 || it >= isotable.NumTableEntries()) {
    cerr << "Error.  Entry " << it
	 << " is not in range [0.." << isotable.NumTableEntries()-1
	 << "]." << endl;
    cerr << endl;
    return(false);
  }
  else return(true);
}


// output simplices in isosurface edge table entry
void out_simplices(const int it,
                   const MC_TABLE & isotable)
{
  const int num_poly_vertices = isotable.Polytope().NumVertices();
  std::vector<int> digit(num_poly_vertices);
  PROCEDURE_ERROR error("out_entry_simplices");

  if (it < 0 || it >= isotable.NumTableEntries()) {
    error.AddToMessage("Table index ", it, " is out of bounds.");
    throw error;
  };

  convert2base
    (it, isotable.Base(), digit.data(), num_poly_vertices, error);
  
  cout << "  Number of simplices = "
       << isotable.NumSimplices(it) << "." << endl;
  print_list(cout, "  Polytope vertex labels: ",
             digit.data(), num_poly_vertices, "\n");
  cout << "  Simplex vertices:" << endl;
  for (int is = 0; is < isotable.NumSimplices(it); is++) {
    cout << "  ";
    for (int iv = 0; iv < isotable.NumVerticesPerSimplex(); iv++) {
      cout << "  " << int(isotable.SimplexVertex(it, is, iv));
    };
    cout << endl;
  };
}


// output simplices in isosurface table entry
void out_entry(const int it, const MC_TABLE & isotable)
{
  if (!CheckEntryRange(it, isotable)) { return; };

  cout << "Table Entry: " << it << endl;
  out_simplices(it, isotable);
  cout << endl;
}


//**************************************************
// Output isosurface table statistics
//**************************************************

// output polytope statistics
void out_poly_stat(const MC_TABLE & isotable)
{
  cout << "Polytope:" << endl;
  if (!isotable.Polytope().IsShapeUndefined()) {
    cout << "  Shape: " << isotable.Polytope().ShapeString() << endl;
  }
  cout << "  # Vertices = " << isotable.Polytope().NumVertices() << endl;
  cout << "  # Edges = " << isotable.Polytope().NumEdges() << endl;
  cout << "  # Facets = " << isotable.Polytope().NumFacets() << endl;
}

// output isosurface vertices statistics
void out_isovert_stat(const MC_TABLE & isotable)
{
  int num_isov_on_vert = 0;
  int num_isov_on_edges = 0;
  int num_isov_on_facets = 0;
  int num_isov_points = 0;
  for (int i = 0; i < isotable.NumIsosurfaceVertices(); i++) {
    switch(isotable.IsosurfaceVertex(i).Type()) {
    case ISOSURFACE_VERTEX::VERTEX:
      num_isov_on_vert++;
      break;

    case ISOSURFACE_VERTEX::EDGE:
      num_isov_on_edges++;
      break;

    case ISOSURFACE_VERTEX::FACET:
      num_isov_on_facets++;
      break;

    case ISOSURFACE_VERTEX::POINT:
      num_isov_points++;
      break;
    };
  }

  cout << "Isosurface Vertices:" << endl;
  cout << "  Total # vertices = " 
       << isotable.NumIsosurfaceVertices() << endl;
  if (num_isov_on_vert > 0)
    cout << "    # Vertices on poly vertices = " << num_isov_on_vert << endl;
  if (num_isov_on_edges > 0)
    cout << "    # Vertices on poly edges = " << num_isov_on_edges << endl;
  if (num_isov_on_facets > 0)
    cout << "    # Vertices on poly facets = " << num_isov_on_facets << endl;
  if (num_isov_points > 0)
    cout << "    # Free vertices = " << num_isov_points << endl;
}

// output isosurface simplices statistics
void out_simplices_stat(const MC_TABLE & isotable)
{
  long max_s = 0;
  long total_s = 0;
  for (long i = 0; i < isotable.NumTableEntries(); i++) {
    long n = isotable.NumSimplices(i);
    if (max_s < n) max_s = n;
    total_s += n;
  };
  double avg_s = ((double) total_s)/((double) isotable.NumTableEntries());

  cout << "  Max # simplices per entry = " << max_s << endl;
  cout << "  Avg # simplices per entry = " << avg_s << endl;
}


// output isosurface table statistics
void out_stat(const MC_TABLE & isotable)
{
  cout << "Dimension = " << isotable.Dimension() << endl;
  cout << "Simplex Dimension = " << isotable.SimplexDimension() << endl;
  isotable.Properties().Print(cout, "");
  out_poly_stat(isotable);
  out_isovert_stat(isotable);
  cout << "Table:" << endl;
  cout << "  # Entries = " << isotable.NumTableEntries() << endl;
  out_simplices_stat(isotable);
}


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
    else if (s == "-poly") {
      out_poly_flag = true;
    }
    else if (s == "-isovert") {
      out_isovert_flag = true;
    }
    else if (s == "-stat") {
      out_stat_flag = true;
    }
    else if (s == "-entry") {
      const int table_index =
        get_arg_int(iarg, argc, argv, error);
      entry.push_back(table_index);
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

  if (iarg+1 < argc) {
    usage_error();
  }

  if (entry.size() == 0 && !out_poly_flag && !out_isovert_flag) {
    // default: print statistics
    out_stat_flag = true; 
  };
  
  input_filename = argv[iarg];
}

