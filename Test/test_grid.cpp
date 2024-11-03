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

// Forward declarations
void output_grid(const GRID3D & grid);


int main(int argc, char ** argv)
{
  using std::cout;
  using std::endl;
  
  try {
    const AXIS_SIZE_TYPE asizeA[DIM3] = { 3, 4, 5 };
    GRID3D gridA(asizeA);

    cout << "Grid A" << endl;
    output_grid(gridA);
    
    cout << endl;
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


void output_grid(const GRID3D & grid)
{
  using std::cout;
  using std::endl;
  
  cout << "Dimension: " << grid.Dimension() << endl;

  // TO BE CONTINUED...
}

