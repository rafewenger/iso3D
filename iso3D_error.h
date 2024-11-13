/*!
 *  @file iso3D_error.h
 *  @brief Class for handling errors.
 *  @authors "Rephael Wenger"
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

#ifndef _ISO3D_ERROR_H_
#define _ISO3D_ERROR_H_

#include <cstddef>
#include <string>
#include <vector>

namespace ISO3D {

  // *****************************************************************
  // Classes ERROR and PROCEDURE_ERROR.
  // *****************************************************************
  
  /// @brief Error class.
  class ERROR {

  protected:

    /// @brief Error message.
    std::vector<std::string> msg;

    /// @brief C-string representation of true.
    static constexpr const char * cstr_true = "true";

    /// @brief C-sString representation of false.
    static constexpr const char * cstr_false = "false";

    
  protected:
    
    /// @brief Internal template function for converting value
    ///   of arbitray type to string.
    template <typename T>
    std::string _convert2string(const T v)
    { return std::to_string(v); }

    /*!
     *  @overload
     *  @brief Variadic template version of _convert2string.
     */
    template <typename T0, typename... T>
    std::string _convert2string(const T0 & v0, const T&... v)
    {
      const std::string s0 = _convert2string(v0);
      const std::string s = _convert2string(v...);
      return (s0 + s);
    }

    /*!
     *  @overload
     *  @brief Specialization of template function _convert2string.
     *  - Specialization for value of type string.
     */
    std::string _convert2string(const std::string & s)
    { return s; }

    /*!
     *  @overload
     *  @brief Specialization of template function _convert2string.
     *  - Specialization for value of type const char *.
     */
    std::string _convert2string(const char * s)
    { return std::string(s); }

    /*!
     *  @overload
     *  @brief Specialization of template function _convert2string.
     *  - Specialization for value of type bool.
     *  - Function returns str_true or str_false.
     */
    std::string _convert2string(const bool b)
    {
      if (b) { return std::string(cstr_true); }
      else { return std::string(cstr_false); }
    }
    
    
  public:
    
    /// @brief Constructor.
    ERROR(){};
    
    /*!
     *  @overload
     *  @brief Constructor and (const char *) error message.
     */
    ERROR(const char * error_msg);

    /*!
     *  @overload
     *  @brief Constructor and (std::string) error message.
     */
    ERROR(const std::string & error_msg);
    

    // *** Get functions ***

    /// @brief Return the number of message lines.
    int NumMessageLines() const
    { return msg.size(); }

    /// @brief Return i'th message line.
    std::string MessageLine(const int i) const;


    // *** Set Functions ***

    /// @brief Add (std::string) line to error message.
    void AddToMessage(const std::string & error_msg)
    { msg.push_back(error_msg); };

    /*!
     *  @overload
     *  @brief Add (const char) line to error message.
     */
    void AddToMessage(const char * error_msg)
    { AddToMessage(std::string(error_msg)); }

    /// @brief Add procedure name (std::string) to message.
    void AddProcNameToMessage(const std::string & procedure_name);

    /*!
     *  @overload
     *  @brief Add procedure name (const char *) to message.
     */
    void AddProcNameToMessage(const char * procedure_name)
    { AddProcNameToMessage(std::string(procedure_name)); }

    /*!
     *  @overload
     *  @brief Template to create message from arbitrary type.
     */
    template <typename T>
    void AddToMessage(const T m)
    { AddToMessage(_convert2string(m)); }

    /*!
     *  @overload
     *  @brief Variadic template to create message from arbitrary type.
     */
    template <typename... T>
    void AddToMessage(const T &... m)
    { AddToMessage(_convert2string(m...)); }      
    

    /// @brief Send error message to output stream out.
    template <typename OSTREAM_TYPE>
      void Out(OSTREAM_TYPE & out) const
      {
        for (int i = 0; i < NumMessageLines(); i++) {
          out << MessageLine(i) << "\n";
        }
      }
    
  };


  /*!
   *  @brief Procedure error class.
   *  - Set first line of error message
   *    to be "Error in procedure..."
   */
  class PROCEDURE_ERROR:public ERROR {

  public:

    /// @brief Constructor.
    PROCEDURE_ERROR(const char * proc_name)
    { AddProcNameToMessage(proc_name); }
  };


  /*!
   *  @brief Return false and set error message if C++ STL vector size
   *    is not equal to expected size.
   */
  template <typename T>
  bool check_array_size
  (const std::vector<T> & a, const std::size_t expected_size,
   const char * array_name, ERROR & error)
  {
    if (a.size() != expected_size) {
      error.AddToMessage
        ("Error. Incorrect sizeof aarry ", array_name, ".");
      error.AddToMessage("  Array size: ", a.size());
      error.AddToMessage("  Expected array size: ", expected_size);
      return false;
    }

    return true;
  }
  
}

#endif


