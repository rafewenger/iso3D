/*!
 *  @file test_string.cpp
 *  @brief Test functions in iso3D_string.h.
 */

#include <cstddef>
#include <iostream>
#include <vector>

#include "iso3D_string.h"

using namespace ISO3D;


// Class for testing routines on classes.
class classA {

public:
  int x;
  
public:
  // Constructor
  classA()
  { x = 2; };
};

// Forward declarations
void run_string2val_tests();
void run_val2string_tests();
void run_array2string_tests();


int main(int argc, char ** argv)
{
  using std::cout;
  using std::endl;
  
  try {
    run_string2val_tests();
    cout << endl;
    
    run_val2string_tests();
    cout << endl;

    run_array2string_tests();
    cout << endl;
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
  using std::cout;
  using std::cerr;
  using std::endl;
  
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


void test_bool2string(const bool flag)
{
  using std::cout;
  using std::endl;
  
  const std::string s = bool2string(flag);
  cout << "Type bool, value: ";
  if (flag) { cout << "true" << endl; }
  else { cout << "false" << endl; }
  cout << "  String: \"" << s << "\"" << endl;
}


void run_val2string_tests()
{
  const int x = 5;
  const float y = 3.14;
  const bool flagT = true;
  const bool flagF = false;
  classA A;
  std::string s;

  using std::cout;
  using std::cerr;
  using std::endl;
  
  try {

    test_val2string(555, "int");
    test_val2string(1.23, "float");
    test_val2string(x, "int");
    test_val2string(y, "float");
    test_bool2string(flagT);
    test_bool2string(flagF);
    test_bool2string(true);
    test_bool2string(false);
    cout << endl;

    // Can't convert string. Throws an error.
    cout << "Attempt to call val2string on a C++ string. (Throws an error.)"
         << endl;
    const std::string s = val2string(s);
    cout << "String " << s << endl;
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }

  cout << endl;
  
  try {
    // Can't convert class. Throws an error.
    cout << "Attempt to call val2string on a C++ class. (Throws an error.)"
         << endl;
    const std::string s = val2string(s);
    cout << "String " << s << endl;
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }  
  
}


// *****************************************************************
// Test converting arrays to string.
// *****************************************************************

template <typename T>
void test_array2string
(const T x[], const int length, const char * type_name)
{
  using std::cout;
  using std::endl;

  const std::string s = array2string(x,length,",");
  cout << "Type " << type_name << ", array:";
  for (int i = 0; i < length; i++) {
    cout << "  " << x[i];
  }
  cout << endl;
  cout << "  String: \"" << s << "\"" << endl;  
}


void run_array2string_tests()
{
  const int DIM3(3);
  const int x[DIM3] = { 5, 6, 7};
  const float y[DIM3] = { 5.1, 6.2, 7.3};
  std::string s;

  using std::cout;
  using std::cerr;
  using std::endl;

  try {
    test_array2string(x, DIM3, "int");
    test_array2string(y, DIM3, "float");
  }
  catch (ERROR & error) {
    error.Out(cerr);
  }
  
}
