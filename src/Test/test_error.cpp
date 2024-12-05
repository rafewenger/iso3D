/*!
 *  @file test_error.cpp
 *  @brief Test ERROR and PROCEDURE_ERROR classes in iso3D_error.h.
 */

#include <iostream>

#include "iso3D_error.h"

// Forward declarations.
void funcA();
void funcB();
void funcC();
void funcD();
void funcE();
void funcF();

using namespace ISO3D;


int main(int argc, char ** argv)
{
  try {
    funcA();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }

  std::cout << std::endl;
  
  try {
    funcB();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }  

  std::cout << std::endl;


  try {
    funcC();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }  

  std::cout << std::endl;

  try {
    funcD();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }

  std::cout << std::endl;  
  
  try {
    funcE();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }

  std::cout << std::endl;  
  
  try {
    funcF();
  }
  catch (ERROR & error) {
    error.Out(std::cerr);
  }      

  std::cout << std::endl;

  return 0;
}


void funcA()
{
  ERROR error;

  error.AddToMessage("In funcA.");
  error.AddToMessage("First error line.");
  error.AddToMessage("Second error line.");
  throw error;
}


void funcB()
{
  PROCEDURE_ERROR error("funcB");

  error.AddToMessage("Second error line.");
  error.AddToMessage("Second error line.");
  throw error;
}


void funcC()
{
  PROCEDURE_ERROR error("funcC");
  const int x = 25;

  error.AddToMessage("Error object");
  error.AddToMessage(x);
  throw error;
}


void funcD()
{
  PROCEDURE_ERROR error("funcD");
  const float x = 25.5;

  error.AddToMessage("Error object");
  error.AddToMessage(x);
  throw error;
}


void funcE()
{
  PROCEDURE_ERROR error("funcE");
  const int x = 7;

  error.AddToMessage("Error. x = ", x);
  throw error;
}


void funcF()
{
  PROCEDURE_ERROR error("funcF");
  const int x = 7;
  const float y = 8.5;
  const bool b0 = false;
  const bool b1 = true;

  error.AddToMessage
    ("Error. x = ", x, " y = ", y, " b0 = ", b0, " b1 = ", b1);
  throw error;
}





