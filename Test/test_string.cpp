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
void run_string2val_tests();
void run_val2string_tests();


int main(int argc, char ** argv)
{
  using std::cout, std::endl;
  
  try {
    run_string2val_tests();
    cout << endl;
    
    run_val2string_tests();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }
  cout << endl;
  
  return 0;
}


// *****************************************************************
// Test converting string to values/vectors.
// *****************************************************************


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


// Test string2vector_append().
void test_int_vector(const char * s1, const char * s2)
{
  std::vector<int> v;
  ISO3D::PROCEDURE_ERROR error("test_int_vector");
  
  if (string2vector(s1, v)) {
    if (string2vector(s2, v)) {
      using namespace std;

      cout << "String1: \"" << s1 << "\"" << endl;
      cout << "String2: \"" << s2 << "\"" << endl;
      cout << "  Int vector:";
      for (size_t i = 0; i < v.size(); i++) {
        cout << "  " << v[i];
      }
      cout << endl;
    }
    else {
      error.AddToMessage
        ("Unable to convert and append string \"", s2, "\" to integer vector.");
      throw error;
    }
  }
  else {
    error.AddToMessage
      ("Unable to convert string \"", s1, "\" to integer vector.");
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


void test_string_vector(const char * s)
{
  std::vector<std::string> v;
  
  if (string2vector(s, v)) {
    using namespace std;

    cout << "String: \"" << s << "\"" << endl;
    cout << "  String vector:";
    for (size_t i = 0; i < v.size(); i++) {
      cout << "  \"" << v[i] << "\"";
    }
    cout << endl;
  }
  else {
    ISO3D::PROCEDURE_ERROR error("test_string_vector");

    error.AddToMessage
      ("Unable to convert string \"", s, "\" to string vector.");
    throw error;
  }
}


void run_string2val_tests()
{
  using std::cout, std::cerr, std::endl;
  
  try {
    test_int("31");
    test_int("31  ");
    test_int("31.5");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }
  cout << endl;
  
  try {
    test_float("31");
    test_float("31.5");
    test_float("31.5  ");
    test_float("31.5.7");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }  
  cout << endl;

  try {
    test_int_vector("3 33 333");
    test_int_vector("3 33 333  ");
    test_int_vector("3 33.5 333");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }
  cout << endl;

  try {
    test_int_vector("4 44 444", "5 55 555");
    test_int_vector("4 44.5 444", "5 55 555");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }
  
  try {
    test_int_vector("4 44 444", "5 55.5 555");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }  
  cout << endl;  

  try {
    test_float_vector("3 33 333");
    test_float_vector("3 3.3 3.33  ");
    test_float_vector("3 3.3 3.33 3.33 xyz");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }
  cout << endl;

  try {
    test_string_vector("3 33 333");
    test_string_vector("3 3.3 3.33 3.33 xyz");
    test_string_vector("The rain in Spain");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }      
}


// *****************************************************************
// Test converting values to string.
// *****************************************************************

template <typename T>
void test_val2string(const T x, const char * type_name)
{
  using std::cout;
  using std::endl;
  
  const std::string s = val2string(x);
  cout << "Type " << type_name << ", value: " << x << endl;
  cout << "  String: \"" << s << "\"" << endl;
}


void run_val2string_tests()
{
  const int x = 5;
  const float y = 3.14;
  std::string s;

  using std::cout, std::cerr, std::endl;
  
  try {
    class classA {

    public:
      classA() {};
      
      int x = 2;
      float y = 8.5;
    } sA;
    
    test_val2string(x, "int");
    test_val2string(y, "float");

    // Can't convert sA. Throws an error.
    const std::string s = val2string(s);
    cout << "String " << s << endl;
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }  
}
