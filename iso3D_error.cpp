/*!
 *  @file iso3D_error.cpp
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


#include "iso3D_error.h"


// *****************************************************************
// Class ERROR member functions.
// *****************************************************************

// Constructor
ISO3D::ERROR::ERROR(const char * error_msg)
{ AddToMessage(error_msg); }


// Constructor
ISO3D::ERROR::ERROR(const std::string & error_msg)
{ AddToMessage(error_msg); }


// Return i'th message line.
std::string ISO3D::ERROR::MessageLine(const int i) const
{
  if (i >= 0 && i < NumMessageLines())
    { return msg[i]; }
  else
    { return ""; }
}


// Add procedure name to message.
void ISO3D::ERROR::AddProcNameToMessage
(const std::string & procedure_name)
{
  const std::string error_msg =
    "Error detected in " + procedure_name + ".";
  AddToMessage(error_msg);
}
