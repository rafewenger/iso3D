/*!
 *  @file iso3D_templates.h
 *  @brief Some simple template definitions for iso3D.
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


#ifndef _ISO3D_TEMPLATES_H_
#define _ISO3D_TEMPLATES_H_

namespace ISO3D {
  
  // *****************************************************************
  // TEMPLATE CLASS SET_VALUE
  // *****************************************************************

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


#endif
