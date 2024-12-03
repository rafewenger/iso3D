/*!
 *  @file iso3D_command_line.h
 *  @brief Routines for processing command line arguments
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


#ifndef _ISO3D_COMMAND_LINE_H_
#define _ISO3D_COMMAND_LINE_H_

#include "iso3D_error.h"
#include "iso3D_string.h"

namespace ISO3D {

  // **************************************************
  // CHECK NUMBER OF ARGUMENTS
  // **************************************************

  /// Throw an error on a missing argument.
  inline void throw_error_on_missing_argument
  (const int iarg, const int argc, char **argv, ERROR & error)
  {
    if (iarg+1 >= argc) {
      error.AddToMessage
        ("Usage error. Missing argument for option ", argv[iarg], ".");
      throw error;
    }
  }

  /// @brief Throw an error on a missing argument.
  /// - Version for more than one argument.
  inline void throw_error_on_missing_argument
  (const int iarg, const int argc, char **argv, const int num_arg,
   ERROR & error)
  {
    if (iarg+num_arg >= argc) {
      error.AddToMessage
        ("Usage error. Missing arguments for option ", argv[iarg], ".");
      error.AddToMessage
        ("  Option ", argv[iarg], " requires ", num_arg, " arguments.");
      throw error;
    }
  }
  

  // ***************************************************************
  // GET ARGUMENT OPTIONS
  // ***************************************************************

  /// Get float argument argv[iarg+1].
  inline float get_arg_float
    (const int iarg, const int argc, char **argv, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    float x;
    if (!string2val(argv[iarg+1], x)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-numeric character in string: ", argv[iarg+1], "");
      throw error;
    }

    return(x);
  }


  /// @brief Get float argument argv[iarg+1].
  /// - Version without parameter IJK::ERROR.
  inline float get_arg_float(const int iarg, const int argc, char **argv)
  {
    ERROR error;
    return(get_arg_float(iarg, argc, argv, error));
  }

  /// Get two float arguments argv[iarg+1] and argv[iarg+2].
  inline void get_arg2_float
  (const int iarg, const int argc, char **argv,
   float & x1, float & x2, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, 2, error);

    if (!string2val(argv[iarg+1], x1)) {
      error.AddToMessage
        ("Usage error.  Error in first argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-numeric character in string: ", argv[iarg+1], "");
      throw error;
    }

    if (!string2val(argv[iarg+2], x2)) {
      error.AddToMessage
        ("Usage error.  Error in second argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-numeric character in string: ", argv[iarg+2], "");
      throw error;
    }
  }


  /// @brief Get two float arguments argv[iarg+1] and argv[iarg+2].
  /// - Version without ERROR.
  inline void get_arg2_float
  (const int iarg, const int argc, char **argv,
   float & x1, float & x2)
  {
    ERROR error;
    get_arg2_float(iarg, argc, argv, x1, x2, error);
  }


  /// Get integer argument argv[iarg+1].
  inline int get_arg_int(const int iarg, const int argc, char **argv,
                         ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    int x;
    if (!string2val(argv[iarg+1], x)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-integer character in string: ", argv[iarg+1], "");
      throw error;
    }

    return(x);
  }


  /// @brief Get integer argument argv[iarg+1].
  /// - Version without parameter ERROR.
  inline int get_arg_int(const int iarg, const int argc, char **argv)
  {
    ERROR error;
    return(get_arg_int(iarg, argc, argv, error));
  }


  /// Get two integer arguments argv[iarg+1] and argv[iarg+2].
  inline void get_arg2_int
  (const int iarg, const int argc, char **argv,
   int & x1, int & x2, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, 2, error);

    if (!string2val(argv[iarg+1], x1)) {
      error.AddToMessage
        ("Usage error.  Error in first argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-integer character in string: ", argv[iarg+1], "");
      throw error;
    }

    if (!string2val(argv[iarg+2], x2)) {
      error.AddToMessage
        ("Usage error.  Error in second argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-integer character in string: ", argv[iarg+2], "");
      throw error;
    }
  }


  /// @brief Get two integer arguments argv[iarg+1] and argv[iarg+2].
  /// - Version without ERROR.
  inline void get_arg2_int
  (const int iarg, const int argc, char **argv,
   int & x1, int & x2)
  {
    ERROR error;
    get_arg2_int(iarg, argc, argv, x1, x2, error);
  }


  /// Get integer argument argv[iarg+1] and float arguments argv[iarg+2] and argv[iarg+3].
  inline void get_arg3_int_float_float
  (const int iarg, const int argc, char **argv,
   int & i1, float & x1, float & x2, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, 3, error);

    if (!string2val(argv[iarg+1], i1)) {
      error.AddToMessage
        ("Usage error.  Error in first argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-integer character in string: ", argv[iarg+1], "");
      throw error;
    }

    if (!string2val(argv[iarg+2], x1)) {
      error.AddToMessage
        ("Usage error.  Error in second argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-numeric character in string: ", argv[iarg+2], "");
      throw error;
    }

    if (!string2val(argv[iarg+3], x2)) {
      error.AddToMessage
        ("Usage error.  Error in third argument for option: ", argv[iarg], "");
      error.AddToMessage
        ("Non-numeric character in string: ", argv[iarg+3], "");
      throw error;
    }
  }


  /// @brief Get integer argument argv[iarg+1] and
  ///   float arguments argv[iarg+2] and argv[iarg+3].
  /// - Version without ERROR.
  inline void get_arg3_int_float_float
  (const int iarg, const int argc, char **argv,
   int & i1, float & x1, float & x2)
  {
    ERROR error;
    get_arg3_int_float_float(iarg, argc, argv, i1, x1, x2, error);
  }


  /// @brief Get boolean (true or false) argument.
  /// - Accepts true, false, True, False, t, f, T, F, yes, no, y, n.
  bool get_boolean_argument
  (const int iarg, const int argc, char **argv, ERROR & error);
  
  /* OBSOLETE
  inline bool get_boolean_argument
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
  */


  /// Get string argument argv[iarg+1] and convert to list of arguments.
  template <typename ETYPE>
  inline void get_arg_multiple_arguments
  (const int iarg, const int argc, char **argv,
   std::vector<ETYPE> & v, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    if (!string2vector(argv[iarg+1], v)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], ".");
      error.AddToMessage("Illegal character in string: \"", argv[iarg+1], "\"");
      throw error;
    }
  }


  /// @brief Get string argument argv[iarg+1] and convert
  ///   to list of arguments.
  /// - Version without ERROR.
  template <typename ETYPE>
  inline void get_arg_multiple_arguments
  (const int iarg, const int argc, char **argv,
   std::vector<ETYPE> & v)
  {
    ERROR error;
    get_arg_multiple_arguments(iarg, argc, argv, v, error);
  }


  /// @brief Get string argument argv[iarg+1]
  ///   and convert to list of (int) arguments.
  /// - Specialization for type int.
  template <>
  inline void get_arg_multiple_arguments
  (const int iarg, const int argc, char **argv,
   std::vector<int> & v, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    if (!string2vector(argv[iarg+1], v)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], ".");
      error.AddToMessage
        ("Non-integer character in string: \"", argv[iarg+1], "\"");
      throw error;
    }
  }


  /// @brief Get string argument argv[iarg+1]
  ///   and convert to list of arguments.
  /// - Specialization for type float.
  template <>
  inline void get_arg_multiple_arguments
  (const int iarg, const int argc, char **argv,
   std::vector<float> & v, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    if (!string2vector(argv[iarg+1], v)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], ".");
      error.AddToMessage
        ("Non-numeric character in string: \"", argv[iarg+1], "\"");
      throw error;
    }
  }


  /// @brief Get string argument argv[iarg+1]
  ///   and convert to list of (float) arguments.
  /// - Specialization for type float.
  /// - Version without ERROR.
  template <>
  inline void get_arg_multiple_arguments
  (const int iarg, const int argc, char **argv,
   std::vector<float> & v)
  {
    ERROR error;
    get_arg_multiple_arguments(iarg, argc, argv, v, error);
  }


  /// @brief Get string argument argv[iarg+1]
  ///   and convert to list of (double) arguments.
  /// - Specialization for type double.
  template <>
  inline void get_arg_multiple_arguments
  (const int iarg, const int argc, char **argv,
   std::vector<double> & v, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    if (!string2vector(argv[iarg+1], v)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], ".");
      error.AddToMessage
        ("Non-numeric character in string: \"", argv[iarg+1], "\"");
      throw error;
    }
  }


  /// Get string argument argv[iarg+1], append to list of arguments.
  template <typename ETYPE>
  inline void get_arg_multiple_append
  (const int iarg, const int argc, char **argv,
   std::vector<ETYPE> & v, ERROR & error)
  {
    throw_error_on_missing_argument(iarg, argc, argv, error);

    if (!string2vector_append(argv[iarg+1], v)) {
      error.AddToMessage
        ("Usage error.  Error in argument for option: ", argv[iarg], ".");
      error.AddToMessage("Illegal character in string: \"", argv[iarg+1], "\"");
      throw error;
    }
  }


}

#endif
