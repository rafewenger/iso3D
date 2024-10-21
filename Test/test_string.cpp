/*!
 *  @file test_string.cpp
 *  @brief Test functions in iso3D_string.h.
 */

#include <cstddef>
#include <iostream>
#include <vector>

#include "iso3D_string.h"

using namespace ISO3D;


// Forward declarations
void test_int(const char * s);
void test_float(const char * s);
void test_int_vector(const char * s);
void test_float_vector(const char * s);


int main(int argc, char ** argv)
{
  try {
    test_int("31");
    test_int("31  ");
    test_int("31.5");
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  std::cout << std::endl;
  
  try {
    test_float("31");
    test_float("31.5");
    test_float("31.5  ");
    test_float("31.5.7");
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }  
  std::cout << std::endl;

  try {
    test_int_vector("3 33 333");
    test_int_vector("3 33 333  ");
    test_int_vector("3 33.5 333");
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  std::cout << std::endl;

  try {
    test_float_vector("3 33 333");
    test_float_vector("3 3.3 3.33  ");
    test_float_vector("3 3.3 3.33 3.33 xyz");
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }      
  
  return 0;
}


void test_int(const char * s)
{
  int x;
  
  if (string2val(s, x)) {
    using namespace std;
    
    cout << "String: \"" << s << "\"" << endl;
    cout << "  Integer: " << x << endl;
  }
  else {
    ISO3D::PROCEDURE_ERROR error("test_int");

    error.AddToMessage("Unable to convert string \"", s, "\" to integer.");
    throw error;
  }
}


void test_float(const char * s)
{
  float x;
  
  if (string2val(s, x)) {
    using namespace std;
    
    cout << "String: \"" << s << "\"" << endl;
    cout << "  Float: " << x << endl;
  }
  else {
    ISO3D::PROCEDURE_ERROR error("test_float");

    error.AddToMessage("Unable to convert string \"", s, "\" to float.");
    throw error;
  }
}


void test_int_vector(const char * s)
{
  std::vector<int> v;
  
  if (string2vector(s, v)) {
    using namespace std;

    cout << "String: \"" << s << "\"" << endl;
    cout << "  Int vector:";
    for (size_t i = 0; i < v.size(); i++) {
      cout << "  " << v[i];
    }
    cout << endl;
  }
  else {
    ISO3D::PROCEDURE_ERROR error("test_int_vector");

    error.AddToMessage
      ("Unable to convert string \"", s, "\" to integer vector.");
    throw error;
  }
}


void test_float_vector(const char * s)
{
  std::vector<float> v;
  
  if (string2vector(s, v)) {
    using namespace std;

    cout << "String: \"" << s << "\"" << endl;
    cout << "  Float vector:";
    for (size_t i = 0; i < v.size(); i++) {
      cout << "  " << v[i];
    }
    cout << endl;
  }
  else {
    ISO3D::PROCEDURE_ERROR error("test_float_vector");

    error.AddToMessage
      ("Unable to convert string \"", s, "\" to float vector.");
    throw error;
  }
}
