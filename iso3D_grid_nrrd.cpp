/*!
 *  @file iso3D_grid_nrrd.cpp
 *  @brief Grid class.
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


#include "iso3D_grid_nrrd.h"

using namespace ISO3D;

// *******************************************************************
// Write functions
// *******************************************************************

// Write scalar grid in nrrd file.
void ISO3D::write_scalar_grid_nrrd
(const char * output_filename, const SCALAR_GRID3D & grid)
{
  PROCEDURE_ERROR error("write_scalar_grid_nrrd");

  if (output_filename == NULL) {
    error.AddToMessage("Programming error: Empty output filename.");
    throw error;
  }

  Nrrd * data = nrrdNew();

  wrap_scalar_grid_data(data, grid.ScalarPtrConst(),
                        grid.Dimension(), grid.AxisSize());
  bool save_failed = nrrdSave(output_filename, data, NULL);

  nrrdNix(data);

  if (save_failed) {
    error.AddToMessage("Unable to save nrrd data to ", output_filename, ".");
    add_nrrd_message(error);
    throw error;
  }

}

// *******************************************************************
// Class NRRD_DATA member functions
// *******************************************************************

// Constructor.
NRRD_DATA::NRRD_DATA()
{
  data = NULL;
  data = nrrdNew();
}


// Destructor.
NRRD_DATA::~NRRD_DATA()
{
  if (data != NULL) { nrrdNuke(data); }
  data = NULL;
}


// Get grid spacing.
void NRRD_DATA::GetSpacing
(std::vector<AXIS_SIZE_TYPE> & grid_spacing) const
{
  PROCEDURE_ERROR error("GRID_NNRD_IN::GetSpacing");

  grid_spacing.clear();

  if (this->DataPtrConst() == NULL) {
    error.AddToMessage("Programming error.  Nrrd data structure is empty.");
    throw error;
  }

  int dimension = this->Dimension();
  if (!check_nrrd_dimension(dimension, error)) { throw error; };

  double nrrd_spacing[NRRD_DIM_MAX];
  nrrdAxisInfoGet_nva
    (this->DataPtrConst(), nrrdAxisInfoSpacing, nrrd_spacing);

  grid_spacing.resize(dimension);
  for (int d = 0; d < dimension; d++) {
    if (AIR_EXISTS(nrrd_spacing[d])) 
      { grid_spacing[d] = nrrd_spacing[d]; }
    else {
      // default to 1.
      grid_spacing[d] = 1;
    }
  }
        
}


// Set dimensions and axis size.
void NRRD_DATA::SetSize
(const int dimension, const AXIS_SIZE_TYPE axis_size[])
{
  set_nrrd_size(dimension, axis_size, this->data);
}


// Set spacing.
void NRRD_DATA::SetSpacing
(const COORD_TYPE spacing[])
{
  set_nrrd_spacing(this->Dimension(), spacing, this->data);
}


// Set spacing.
// - Version using C++ STL vector for spacing.
void NRRD_DATA::SetSpacing
(const std::vector<COORD_TYPE> & spacing)
{
  PROCEDURE_ERROR error("NRRD_DATA::SetSpacing");
  
  if (check_array_size(spacing, DIM3, "spacing", error))
    { throw error; }
  
  set_nrrd_spacing
    (this->Dimension(), &(spacing[0]), this->data);
}

// Copy header.
void NRRD_DATA::CopyHeader(const Nrrd * from_data)
{
  copy_nrrd_header(from_data, this->data);
}

// Copy axis information.
void NRRD_DATA::CopyAxisInfo(const Nrrd * from_data)
{
  copy_nrrd_axis_info(from_data, this->data);
}

// Copy key values
void NRRD_DATA::CopyKeyValues(const Nrrd * from_data)
{
  copy_nrrd_key_values(from_data, this->data);
}

// Copy comments
void NRRD_DATA::CopyComments(const Nrrd * from_data)
{
  copy_nrrd_comments(from_data, this->data);
}

// Add key and value.
void NRRD_DATA::AddKeyValue
(const char * key, const char * value_string)
{
  nrrdKeyValueAdd(this->DataPtr(), key, value_string);
}

// Add key and value.
void NRRD_DATA::AddKeyValue
(const char * key, const std::string & value_string)
{
  AddKeyValue(key, value_string.c_str());
}

// Add key and value.
void NRRD_DATA::AddKeyValue
(const std::string & key, const std::string & value_string)
{
  AddKeyValue(key.c_str(), value_string.c_str());
}


// Return true if dimension and axis sizes match.
// Otherwise, return false and set error message.
bool NRRD_DATA::CheckAxisSize
(const Nrrd * data2, ERROR & error) const
{
  const int dimension = Dimension();

  if (data2 == NULL) {
    error.AddToMessage
      ("Programming error. Nrrd data structure data2 has not been allocated.");
    error.AddToMessage
      ("Call \"data = nrrdNew()\" before calling this procedure.");
    throw error;
  }

  if (dimension != int(data2->dim)) {
    error.AddToMessage("Programming error. Dimension ", dimension, 
                     " of this nrrd data structure");
    error.AddToMessage("  does not match dimension ", data2->dim, 
                     " of data2.");
    return (false);
  }

  if (dimension < 1) { return(true); };

  if (!check_nrrd_dimension(dimension, error)) { throw error; };

  size_t axis_size[NRRD_DIM_MAX];
  nrrdAxisInfoGet_nva
    (this->DataPtrConst(), nrrdAxisInfoSize, axis_size);

  return CheckSize(dimension, axis_size, error);
}


// Return true if dimension and axis sizes match.
// Otherwise, return false and set error message.
bool NRRD_DATA::CheckScalarGridSize
(const SCALAR_GRID3D & grid, ERROR & error) const
{
  return CheckSize(grid.Dimension(), grid.AxisSize(), error);
}


// *******************************************************************
// Class GRID3D_NRRD_IN member functions
// *******************************************************************

// Constructor.
GRID3D_NRRD_IN::GRID3D_NRRD_IN()
{
  read_failed = false;
}

  
// Destructor.
GRID3D_NRRD_IN::~GRID3D_NRRD_IN()
{
  read_failed = false;
}

  
// Read nrrd file.
void GRID3D_NRRD_IN::
Read(const char * input_filename, ERROR & read_error)
{
  PROCEDURE_ERROR error("GRID_NRRD_IN::Read");

  if (input_filename == NULL) {
    read_failed = true;
    error.AddToMessage("Programming error: Empty input filename.");
    throw error;
  }

  read_failed = nrrdLoad(this->data, input_filename, NULL);

  if (read_failed) {
    read_error.AddToMessage("Error reading: ", input_filename);
    add_nrrd_message(read_error);
    return;
  }
}

  
// Read scalar grid.
void GRID3D_NRRD_IN::ReadScalarGrid
(const char * input_filename, SCALAR_GRID3D & grid,
 ERROR & read_error)
{
  Read(input_filename, read_error);
  if (ReadFailed()) { return; }

  const int dimension = this->Dimension();

  if (dimension != DIM3) {
    read_error.AddToMessage
      ("Read error. Input file does not have dimension ", DIM3, ".");
    read_error.AddToMessage
      ("  This program only processes dimension ", DIM3, " nrrd files.");
    read_failed = true;
    
    return;
  }

  size_t size[NRRD_DIM_MAX];
  nrrdAxisInfoGet_nva(this->data, nrrdAxisInfoSize, size);
  grid.SetAxisSize(size);
  nrrd2scalar(this->data, grid.ScalarPtr());
};


// Read scalar grid. (Version using std::string filename.)
void GRID3D_NRRD_IN::ReadScalarGrid
(const std::string & input_filename, SCALAR_GRID3D & grid,
 ERROR & error)
{
  ReadScalarGrid(input_filename.c_str(), grid, error);
}

  
// Read scalar grid and return axis info.
void GRID3D_NRRD_IN::
ReadScalarGrid(const char * input_filename, SCALAR_GRID3D & grid,
               NRRD_DATA & header, ERROR & error)
{
  ReadScalarGrid(input_filename, grid, error);

  if (ReadFailed()) { return; };

  header.CopyHeader(this->DataPtrConst());
}
  
