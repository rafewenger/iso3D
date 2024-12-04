/*!
 *  @file iso3D_templates.h
 *  @brief Some simple template definitions for iso3D.
 *  @authors "Rephael Wenger"
 *  @version 0.0.2
 */

/*
  ISO3D: 3D isosurface generation code.
  Copyright (C) 2024 Rephael Wenger

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#ifndef _ISO3D_TEMPLATES_H_
#define _ISO3D_TEMPLATES_H_

#include <limits>

namespace ISO3D {
  
  // *****************************************************************
  //! @name Template class SET_VALUE
  // *****************************************************************

  //@{
  
  /// @brief Template class SET_VALUE with flag is_set
  ///    to indicate if value has been set.
  template <typename T>
  class SET_VALUE {

  protected:
    T v;
    bool is_set;

    void Init() 
    { is_set = false; }

    void Init(const T value)
    {
      v = value;
      is_set = false;
    }

  public:
    SET_VALUE(){ Init(); };
    SET_VALUE(const T value) { Init(value); };

    /// @brief Set v to value and is_set to true.
    void Set(const T value)
    {
      v = value;
      is_set = true;
    }

    /// @brief Set is_set to false.
    void Unset()
    { is_set = false; }

    /// @brief Return true if value has been set.
    bool IsSet() const { return is_set; };

    /// @brief Return value.
    /// - Undefined if value has not been set.
    T Value() const { return v; };
  };


  /// @brief Template specification SET_VALUE<bool> with additional 
  ///   member function IsSetAndTrue().
  class BOOLEAN_SET_VALUE:public SET_VALUE<bool> {

  public:
    BOOLEAN_SET_VALUE() {};
    BOOLEAN_SET_VALUE(const bool value):SET_VALUE<bool>(value) {};

    bool IsSetAndTrue() const
    { return((this->IsSet() && Value())); }
  };

  //@}


  // *****************************************************************
  //! @name CONVERT/INITIALIZE C++ STL VECTORS.
  // *****************************************************************

  //@{

  /// @brief Returns C++ pointer to C array storing in vector data.
  /// - If vector is empty, returns NULL pointer.
  /// - If vector is not empty, returns pointer to a C array.
  template <typename T>
  const T * vector2pointer(const std::vector<T> & v)
  {
    if (v.empty()) { return(NULL); }
    else { return v.data(); }
  }


  /*!
   *  @overload
   *  @brief Returns C++ pointer to C array storing in vector data. (Non const.)
   *  - Version that returns non const pointer.
   */
  template <typename T>
  T * vector2pointer(std::vector<T> & v)
  {
    if (v.empty()) { return(NULL); }
    else { return v.data(); }
  }    

  
  // *****************************************************************
  //! @name INTEGER POWER FUNCTIONS
  // *****************************************************************

  //@{


  /// @brief Integer power function.
  /// @param[out] result Equals (base)^p.
  template <typename ITYPE0, typename ITYPE1, typename ITYPE2>
  void int_power(const ITYPE0 base, const ITYPE1 p, 
                 ITYPE2 & result)
  {
    result = 1;
    for (ITYPE1 k = 0; k < p; k++) {
      result *= base;
    }
  }


  /// @brief Integer power function with error checking.
  /// @param[out] result Equals (base)^p.
  template <typename ITYPE0, typename ITYPE1, typename ITYPE2,
            typename ETYPE>
  void int_power(const ITYPE0 base, const ITYPE1 p, 
                 ITYPE2 & result, ETYPE & error)
  {
    const ITYPE2 result_bound = (std::numeric_limits<ITYPE2>::max()/base);
    
    result = 1;
    for (ITYPE1 k = 0; k < p; k++) {

      if (result > result_bound) {
        error.AddToMessage
          ("Result out of bounds. ", base, "^", p,
           " is larger than ", std::numeric_limits<ITYPE2>::max(), ".");
        throw error;
      }
      result *= base;
    }
  }
  
  //@}

    
  // *****************************************************************
  //! @name CHECK FUNCTIONS
  // *****************************************************************

  //@{

  /// @brief Return true if array memory is allocated.
  /// - Return false and set error message if array is NULL.
  template <typename T>
  bool check_array_allocated
  (const T * array, const char * array_name, ERROR & error)
  {
    if (array == NULL) {
      error.AddToMessage("Programming error. Memory for array ",
                       array_name, "[] not allocated.");
      return(false);
    }

    return(true);
  }


  /// @brief Return true if array is not empty (not NULL).
  /// - Return false and set error message if array is NULL.
  template <typename T>
  bool check_array_non_empty
  (const T * array, const char * array_name, ERROR & error)
  {
    if (array == NULL) {
      error.AddToMessage("Programming error. Array ",
                       array_name, "[] is empty.");
      return(false);
    }

    return(true);
  }


  /// @brief Return true if vA.size() = vB.size().
  /// - Return false and set error message
  ///   if vA.size() != vB.size().
  template <typename TA, typename TB>
  bool check_equal_vector_sizes
  (const std::vector<TA> & vA,
   const std::vector<TB> & vB,
   const char * vA_name,
   const char * vB_name,
   ERROR & error)
  {
    if (vA.size() != vB.size()) {
      error.AddToMessage
        ("Programming error. Unequal sizes of C++ STL vectors ",
         vA_name, " and ", vB_name, ".");
      error.AddToMessage("  ", vA_name, ".size() = ", vA.size(), ".");
      error.AddToMessage("  ", vB_name, ".size() = ", vB.size(), ".");
      return(false);
    }

    return(true);
  }


  /// Return true if ptr is NULL
  template <typename T>
  bool check_is_NULL
  (const T * ptr, const char * variable_name, ERROR & error)
  {
    if (ptr != NULL) {
      error.AddToMessage
        ("Programming error.  Previously allocated memory for variable ",
         variable_name, " not released.");
      return(false);
    }

    return(true);
  }

  //@}
    
}

#endif
