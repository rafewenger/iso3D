/*!
 *  @file iso3D_grid_nrrd.h
 *  @brief Class for reading scalar grid from an nrrd file.
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


#ifndef _ISO3D_GRID_NRRD_H_
#define _ISO3D_GRID_NRRD_H_

#include <vector>

#include "iso3D_nrrd.h"
#include "iso3D_const.h"
#include "iso3D_error.h"
#include "iso3D_scalar_grid.h"
#include "iso3D_types.h"

namespace ISO3D {

  // *****************************************************************
  // CLASS NRRD_DATA
  // *****************************************************************

  /// C++ class allocating/freeing Nrrd data.
  class NRRD_DATA {

  protected:
    Nrrd * data;

  public:
    NRRD_DATA();
    ~NRRD_DATA();

    // Get functions

    /// @brief Return dimension.
    int Dimension() const
    { return(data->dim); }

    /// @brief Return numbef of vertices on axis d.
    int AxisSize(const int d) const 
    { return(data->axis[d].size); }

    /// @brief Return Nrrd data structure.
    Nrrd * DataPtr()
    { return(data); };

    /// @brief Return Nrrd data structure.
    Nrrd * DataPtrConst() const
    { return(data); };

    /// Get grid spacing.
    void GetSpacing(std::vector<AXIS_SIZE_TYPE> & grid_spacing) const;

    // Set functions

    /// @brief Set volume dimension and axis size.
    void SetSize(const int dimension,
                 const AXIS_SIZE_TYPE axis_size[]);

    /// @brief Set spacing.
    void SetSpacing(const COORD_TYPE spacing[]);
    void SetSpacing(const std::vector<COORD_TYPE> & spacing);

    /// @brief Copy header.
    void CopyHeader
    (const Nrrd * from_data);

    /// @brief Copy axis information.    
    void CopyAxisInfo
    (const Nrrd * from_data);

    /// @brief Copy key values.    
    void CopyKeyValues
    (const Nrrd * from_data);

    /// @brief Copy comments.
    void CopyComments
    (const Nrrd * from_data);

    /// @brief Add key and value.
    void AddKeyValue(const char * key, const char * value_string);
    void AddKeyValue(const char * key, const std::string & value_string);
    void AddKeyValue
    (const std::string & key, const std::string & value_string);

    /// @brief Return true if dimension and axis sizes match.
    template <typename ATYPE>
    bool CheckSize
    (const int dimension, const ATYPE axis_size[],
     ERROR & error) const;

    /// @brief Return true if dimension and axis sizes match.
    bool CheckAxisSize
    (const Nrrd * nrrd_data2, ERROR & error) const;

    /// @brief Return true if scalar grid dimension and axis sizes match.
    bool CheckScalarGridSize
    (const SCALAR_GRID3D & grid, ERROR & error) const;
  };

  
  // *****************************************************************
  // CLASS GRID3D_NRRD_IN
  // *****************************************************************

  class GRID3D_NRRD_IN:protected NRRD_DATA {

  protected:
    bool read_failed;            ///< True if read failed.

  public:
    GRID3D_NRRD_IN();              ///< Constructor
    ~GRID3D_NRRD_IN();             ///< Destructor
    
    const Nrrd * DataPtrConst() const       ///< Return Nrrd data structure
    { return(this->data); };

    // Get functions
    bool ReadFailed() const      ///< Return true if read failed.
    { return(read_failed); };

    /// Read nrrd file.
    void Read(const char * input_filename, ERROR & error);

    /// @brief Read scalar grid.
    void ReadScalarGrid
      (const char * input_filename, SCALAR_GRID3D & grid, ERROR & error);

    /// @overload
    /// @brief Read scalar grid. (Version using std::string filename)
    void ReadScalarGrid
      (const std::string & input_filename, SCALAR_GRID3D & grid,
       ERROR & error);

    /// @overload
    /// @brief Read scalar grid and return axis info.
    void ReadScalarGrid
    (const char * input_filename, SCALAR_GRID3D & grid,
     NRRD_DATA & header, ERROR & error);

  };


  // *****************************************************************
  //! @name CHECK FUNCTIONS
  // *****************************************************************

  //!@{

  //! Return false and set error message if dimension is negative 
  //! or larger than NRRD_DIM_MAX.
  inline bool check_nrrd_dimension(const int dimension, ERROR & error)
  {
    if (dimension > NRRD_DIM_MAX) {
      error.AddToMessage
        ("Dimension error. Dimension ", dimension, " is too large.");  
      error.AddToMessage("  Max allowable dimension is ", NRRD_DIM_MAX, ".");
      return(false);
    }
    else if (dimension != DIM3) {
      error.AddToMessage
        ("Dimension error.  Dimension must be 3.");
      error.AddToMessage("  Dimension: ", dimension);
    }

    return(true);
  }

  //!@}


  // *****************************************************************
  // FUNCTION add_nrrd_message
  // *****************************************************************

  /// Add nrrd error message.
  inline void add_nrrd_message
  (const char * msg_header, ERROR & error)
  {
    char * s = biffGetDone(NRRD);

    // Make a copy of string s
    std::string nrrd_error_msg = s;

    free(s);      // free error message string

    error.AddToMessage(msg_header, nrrd_error_msg);
  }

  inline void add_nrrd_message(ERROR & error)
  {
    add_nrrd_message("  Nrrd error: ", error);
  }

  
  // *****************************************************************
  /// @name NRRD SET/COPY FUNCTIONS
  // *****************************************************************

  ///@{

  /// Set dimension and axis size of Nrrd data.
  inline void set_nrrd_size
    (const int dimension, const AXIS_SIZE_TYPE axis_size[DIM3],
     Nrrd * nrrd_data)
  {
    size_t nrrd_axis_size[NRRD_DIM_MAX];
    PROCEDURE_ERROR error("set_nrrd_size");

    if (!check_nrrd_dimension(dimension, error)) { throw error; };

    nrrd_data->dim = dimension;

    for (int d = 0; d < dimension; d++) 
      { nrrd_axis_size[d] = axis_size[d]; }

    nrrdAxisInfoSet_nva(nrrd_data, nrrdAxisInfoSize, nrrd_axis_size);
  }


  /// Copy dimension from one Nrrd data structure to another.
  inline void copy_nrrd_dimension(const Nrrd * from_nrrd, Nrrd * to_nrrd)
  {
    to_nrrd->dim = from_nrrd->dim;
  }


  /// Copy axis information from one Nrrd data structure to another.
  inline void copy_nrrd_axis_info(const Nrrd * from_nrrd, Nrrd * to_nrrd)
  {
    PROCEDURE_ERROR error("copy_nrrd_axis_info");

    // Make sure that dimension is copied.
    copy_nrrd_dimension(from_nrrd, to_nrrd);

    bool copy_failed =
      nrrdAxisInfoCopy(to_nrrd, from_nrrd, NULL, 0);

    if (copy_failed) {
      error.AddToMessage("Error copying Nrrd axis information.");
      add_nrrd_message(error);
      throw error;
    }
  }


  /// Set spacing of nrrd data.
  inline void set_nrrd_spacing
  (const int dimension, const COORD_TYPE spacing[], Nrrd * nrrd_data)
  {
    double nrrd_spacing[NRRD_DIM_MAX];
    PROCEDURE_ERROR error("set_nrrd_spacing");

    if (!check_nrrd_dimension(dimension, error)) { throw error; };

    for (int d = 0; d < dimension; d++) 
      { nrrd_spacing[d] = spacing[d]; }

    nrrdAxisInfoSet_nva(nrrd_data, nrrdAxisInfoSpacing, nrrd_spacing);
  }


  /// Copy key values from one Nrrd data structure to another.
  inline void copy_nrrd_key_values(const Nrrd * from_nrrd, Nrrd * to_nrrd)
  {
    PROCEDURE_ERROR error("copy_nrrd_key_values");

    bool copy_failed = nrrdKeyValueCopy(to_nrrd, from_nrrd);

    if (copy_failed) {
      error.AddToMessage("Error copying Nrrd key values.");
      add_nrrd_message(error);
      throw error;
    }
  }


  /// Copy comments from one Nrrd data structure to another.
  inline void copy_nrrd_comments(const Nrrd * from_nrrd, Nrrd * to_nrrd)
  {
    PROCEDURE_ERROR error("copy_nrrd_comments");

    bool copy_failed = nrrdCommentCopy(to_nrrd, from_nrrd);

    if (copy_failed) {
      error.AddToMessage("Error copying Nrrd comments.");
      add_nrrd_message(error);
      throw error;
    }
  }

  
  /// @brief Copy header information from one Nrrd data structure to another.
  /// - Copy dimension, axis size, key values and comments.
  inline void copy_nrrd_header(const Nrrd * from_nrrd, Nrrd * to_nrrd)
  {
    copy_nrrd_dimension(from_nrrd, to_nrrd);
    copy_nrrd_axis_info(from_nrrd, to_nrrd);
    copy_nrrd_key_values(from_nrrd, to_nrrd);
    copy_nrrd_comments(from_nrrd, to_nrrd);
  }

  ///@}


  // *****************************************************************
  /// @name WRITE FUNCTIONS
  // *****************************************************************

  ///@{

  /// @brief Wrap scalar data in nrrd_data. (data type float)
  template <typename ATYPE>
  void wrap_scalar_grid_data
  (Nrrd * nrrd_data, const float * scalar, 
   const int dimension, const ATYPE axis_size[])
  {
    size_t nrrd_axis_size[NRRD_DIM_MAX];
    PROCEDURE_ERROR error("wrap_scalar_grid_data");

    if (!check_nrrd_dimension(dimension, error)) { throw error; };

    for (int d = 0; d < dimension; d++) 
      { nrrd_axis_size[d] = axis_size[d]; }

    nrrdWrap_nva(nrrd_data, (void *)(scalar), nrrdTypeFloat,
                 dimension, nrrd_axis_size);
  }

  
  /// @overload
  /// @brief Wrap scalar data in nrrd_data. (data type int)
  template <typename ATYPE>
  void wrap_scalar_grid_data
  (Nrrd * nrrd_data, const int * scalar, 
   const int dimension, const ATYPE axis_size[])
  {
    size_t nrrd_axis_size[NRRD_DIM_MAX];
    PROCEDURE_ERROR error("wrap_scalar_grid_data");

    if (!check_nrrd_dimension(dimension, error)) { throw error; };

    for (int d = 0; d < dimension; d++) 
      { nrrd_axis_size[d] = axis_size[d]; }

    nrrdWrap_nva(nrrd_data, (void *)(scalar), nrrdTypeInt,
                 dimension, nrrd_axis_size);
  }


  /// @brief Write scalar grid in nrrd file.
  void write_scalar_grid_nrrd
    (const char * output_filename, const SCALAR_GRID3D & grid);

  /// @overload
  /// @brief Write scalar grid in nrrd file. (C++ string version.)
  inline void write_scalar_grid_nrrd
    (const std::string & output_filename, const SCALAR_GRID3D & grid)
  {
    write_scalar_grid_nrrd(output_filename.c_str(), grid);
  }

  /// @oveload
  /// @brief Write scalar grid in nrrd file. Add header information.
  void write_scalar_grid_nrrd
  (const char * output_filename, const SCALAR_GRID3D & grid, 
   const NRRD_DATA & nrrd_header);

  /// @overload
  /// \brief Write scalar grid in nrrd file. Add header information. 
  /// (C++ string version.)
  inline void write_scalar_grid_nrrd
  (const std::string & output_filename, const SCALAR_GRID3D & grid, 
   const NRRD_DATA & nrrd_header)
  {
    write_scalar_grid_nrrd(output_filename.c_str(), grid, nrrd_header);
  }

  /// Write scalar grid in nrrd file. Compress data using gzip.
  void write_scalar_grid_nrrd_gzip
    (const char * output_filename, const SCALAR_GRID3D & grid);

  /// @overload
  /// \brief Write scalar grid in nrrd file. Compress data using gzip. 
  /// (C++ string version.)
  inline void write_scalar_grid_nrrd_gzip
  (const std::string & output_filename, const SCALAR_GRID3D & grid)
  {
    write_scalar_grid_nrrd_gzip(output_filename.c_str(), grid);
  }

  /// \brief Write scalar grid in nrrd file. Add header information.
  /// Compress data using gzip.
  void write_scalar_grid_nrrd_gzip
  (const char * output_filename, const SCALAR_GRID3D & grid, 
   const NRRD_DATA & nrrd_header);

  /*!
   *  \brief Write scalar grid in nrrd file. Compress data using gzip.
   *  Add header information.
   *  (C++ string version.)
   */
  inline void write_scalar_grid_nrrd_gzip
  (const std::string & output_filename, const SCALAR_GRID3D & grid, 
   const NRRD_DATA & nrrd_header)
  {
    write_scalar_grid_nrrd_gzip
      (output_filename.c_str(), grid, nrrd_header);
  }

  //@}

  
  // *****************************************************************
  // Class NRRD_DATA member functions
  // *****************************************************************

  // Return true if dimension and axis sizes match.
  // Otherwise, return false and set error message.
  template <typename ATYPE>
  bool NRRD_DATA::CheckSize
  (const int dimension, const ATYPE axis_size[],
   ERROR & error) const
  {
    if (this->Dimension() != dimension) {
      error.AddToMessage("Incorrect Nrrd dimension.");
      error.AddToMessage
        ("  Nrrd dimension = ", this->Dimension(), ".");
      error.AddToMessage("  Should be = ", dimension, ".");
      return(false);
    }

    for (int d = 0; d < dimension; d++) {
      if (long(AxisSize(d)) != long(axis_size[d])) {
        error.AddToMessage("Incorrect axis_size[", d, "].");
        error.AddToMessage
          ("  axis_size[", d, "] = ", this->AxisSize(d), ".");
        error.AddToMessage("  Should be = ", axis_size[d], ".");
        return(false);
      }
    }

    return(true);
  }

}

#endif
