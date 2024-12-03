/*!
 *  @file iso3D_MCtable_xitIO.h
 *  @brief I/O routines for .xit (XML Isosurface Table) file.
 *  - .xit is an xml format
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


#ifndef _ISO3D_MC_TABLE_XIT_H_
#define _ISO3D_MC_TABLE_XIT_H_

#include "iso3D_const.h"
#include "iso3D_MCtable.h"
#include "iso3D_types.h"
#include "iso3D_error.h"

namespace ISO3D {

  // ***************************************************************
  // TYPES
  // ***************************************************************

  typedef enum { XIT_VERSION_1_0, XIT_VERSION_2_0,
                 XIT_VERSION_2_x,
                 UNKNOWN_XIT_VERSION } XIT_VERSION_TYPE;


  // ***************************************************************
  // READ .xit FILE
  // ***************************************************************

  /// @brief Read .xit file.
  void read_xit
    (std::istream & in, MC_TABLE & table);


  // ***************************************************************
  // WRITE .xit FILE
  // ***************************************************************

  /// @brief Write .xit file.
  void write_xit
    (std::ostream & out, const XIT_VERSION_TYPE xit_version,
     const MC_TABLE & table);

  /// @brief Write version 2.0 .xit file.
  void write_xit_V2
    (std::ostream & out, const MC_TABLE & table);


  // ***************************************************************
  // OLD VERSIONS
  // ***************************************************************

  /// @brief Read (old) version 1.0 .xit file.
  void read_xit_V1
    (std::istream & in, MC_TABLE & table);

  /// @brief Write old version 1.0 .xit file.
  void write_xit_V1
    (std::ostream & out, const MC_TABLE & table);

}

#endif
