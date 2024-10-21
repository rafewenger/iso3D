/*!
 *  @file test_string.cpp
 *  @brief Test functions in iso3D_string.h.
 */

#include <iostream>

#include "iso3D_string.h"

using namespace ISO3D;


// Forward declarations
void convert2int(const char * s);


int main(int argc, char ** argv)
{
  try {
    convert2int("31");
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }

  return 0;
}


void convert2int(const char * s)
{
  int x;
  
  if (string2val(s, x)) {
    using namespace std;
    
    cout << "String: " << s << endl;
    cout << "  Integer: " << x << endl;
  }
  else {
    ISO3D::PROCEDURE_ERROR error("convert2int");

    error.AddToMessage("Unable to convert string \"", s, "\" to integer.");
    throw error;
  }
}
