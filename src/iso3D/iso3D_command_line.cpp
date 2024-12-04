/*!
 *  @file iso3D_command_line.cpp
 *  @brief Routines for processing command line arguments
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


#include <algorithm>

#include "iso3D_command_line.h"

using namespace ISO3D;

// ***************************************************************
// GET ARGUMENT OPTIONS
// ***************************************************************

// Get boolean (true or false) argument.
// - Accepts true, false, True, False, t, f, T, F, yes, no, y, n.
bool ISO3D::get_boolean_argument
(const int iarg, const int argc, char **argv, ERROR & error)
{
  throw_error_on_missing_argument(iarg, argc, argv, error);

  std::string arg = argv[iarg+1];
  std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);

  if (arg == "true" || arg == "t") { return(true); }
  else if (arg == "false" || arg == "f") { return(false); }
  else if (arg == "yes" || arg == "y") { return(true); }
  else if (arg == "no" || arg == "n") { return(false); }

  // Illegal argument.
  error.AddToMessage
    ("Usage error. Argument for option ", argv[iarg],
     " must be \"true\" or \"false\" or \"yes\" or \"no\".");
  throw error;
}



