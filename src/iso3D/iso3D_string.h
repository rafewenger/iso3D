/*!
 *  @file iso3D_string.h
 *  @brief Class for handling strings.
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


#ifndef _ISO3D_STRING_H_
#define _ISO3D_STRING_H_

#include <sstream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <vector>

#include "iso3D_error.h"


namespace ISO3D {

  // *****************************************************************
  //! @name CONVERT STRING TO NUMERIC VALUES/ARRAYS
  // *****************************************************************


  // Local namespace
  namespace {

    /// Remove trailing blanks from string s
    inline void remove_trailing_blanks(std::string & s)
    {
      size_t pos = 0;
      for (size_t i = 0; i < s.length(); i++) {
        if (!isspace(s[i])) { pos = i+1; }
      }
      if (pos < s.length()) { s.erase(pos); };
        
    }
  };

  
  //@{

  /// @brief Convert string to value
  template <typename VTYPE>
  bool string2val(const char * s, VTYPE & val)
  {
    std::istringstream v_string;

    std::string s2 = s;
    remove_trailing_blanks(s2);

    v_string.str(s2);

    v_string >> val;

    if (!v_string.eof()) 
      { return false; }
    else
      { return true; }
  }


  /// @brief Convert string to array of elements and append to vector.
  template <typename ETYPE>
  bool string2vector_append(const char * s, std::vector<ETYPE> & v)
  {
    std::istringstream v_string;

    std::string s2 = s;
    remove_trailing_blanks(s2);
    if (s2.size() == 0) {
      // Empty string. Vector v has length 0.
      return(true);
    }

    v_string.str(s2);
    while (v_string.good()) {
      ETYPE x;
      v_string >> x;
      v.push_back(x);
    }

    if (!v_string.eof()) 
      { return(false); }
    else
      { return(true); }
  }

    
  /// @brief Convert string to array of elements.
  template <typename ETYPE>
  bool string2vector(const char * s, std::vector<ETYPE> & v)
  {
    v.clear();
    return(string2vector_append(s, v));
  }

  //@}

    
  // *****************************************************************
  //! @name CONVERT NUMERIC VALUE OR BOOL TO STRING.
  // *****************************************************************

  //@{


  /*!
   *  @brief Format value as type string using output string stream.
   *  - Throw error if unable to convert value to string.
   *  @pre Template type T is a scalar type.
   */
  template <typename T>
  std::string val2string(T & x)
  {
    std::ostringstream s_stream;

    if (std::is_scalar<T>::value == false) {
      ISO3D::PROCEDURE_ERROR error("val2string");
      error.AddToMessage("Error converting value to string.");
      error.AddToMessage("Value is not a scalar type.");
      throw error;
    }
    
    s_stream << x;

    if (s_stream.bad()) {
      ISO3D::PROCEDURE_ERROR error("val2string");
      error.AddToMessage("Error converting value ", x, "to string.");
      throw error;
    }

    return s_stream.str();
  }

  
  /// @brief Convert array to string.
  /// - Throw error if unable to convert value to string.
  template <typename T>
  std::string array2string
  (const T x[], const int length, const char * separator)
  {
    std::ostringstream s_stream;

    if (length <= 0) { return std::string(""); };

    s_stream << x[0];
    if (s_stream.bad()) { 
      ISO3D::PROCEDURE_ERROR error("array2string");
      error.AddToMessage("Error converting value ", x[0], "to string.");
      throw error;
    };
    
    for (int i = 1; i < length; i++) {
      s_stream << separator << x[i];
      if (s_stream.bad()) {
        ISO3D::PROCEDURE_ERROR error("array2string");
        error.AddToMessage("Error converting value ", x[i], "to string.");
        throw error;
      }
    }

    return s_stream.str();
  }

  
  /// @brief Convert C++ vector to string.
  /// - Throw error if unable to convert value to string.
  template <typename T>
  std::string vector2string
  (const std::vector<T> & x, const char * separator)
  {
    if (x.size() <= 0) { return std::string(""); };

    return array2string(&(x.front()), x.size(), separator);
  }


  /// @brief Convert bool to string true or false.
  /// - Return string.
  /// - Mainly useful for printing bool value.
  inline std::string bool2string(const bool flag)
  {
    if (flag) { return std::string("true"); }
    else { return std::string("false"); }
  }
  
  //@}


  // *****************************************************************
  //! @name SPLIT STRING INTO PREFIX AND SUFFIX.
  // *****************************************************************

  //@{

  /*!
   *  @brief Split string at last occurrence of character c 
   *    into prefix and suffix.
   *  @param s Input string.
   *  @param c Split at character c.
   *  @param[out] prefix Prefix. All characters before last occurrence of c.
   *    - Does not include last occurrence of character c.
   *  @param[out] suffix Suffix. All characters after last occurrence of c.
   *    - Does not include last occurrence of character c.
   */
  void split_string(const std::string & s, const char c,
                    std::string & prefix, std::string & suffix);

  /*!
   *  @overload
   *  @brief Split string into prefix and suffix. (Input string type char *.) 
   *  - Version with input string of type char *.
   *  @param s Input string. (Type char *.)
   */
  inline void split_string(const char * s, const char c,
                           std::string & prefix, std::string & suffix)
  {
    split_string(std::string(s), c, prefix, suffix);
  }

  ///@}

}

#endif
