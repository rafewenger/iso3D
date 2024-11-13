/*!
 *  @file iso3D_grid.h
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


#ifndef _ISO3D_SCALAR_GRID_H_
#define _ISO3D_SCALAR_GRID_H_

#include "iso3D_grid.h"

namespace ISO3D {

  /// @brief Class representing a 3D scalar grid.
  /// - Does not allocate or deallocate memory for scalar values.
  class SCALAR_GRID3D_BASE:public GRID3D {

  protected:

    /// @brief scalar[iv] Scalar value of vertex iv.
    SCALAR_TYPE * scalar;

    /// @brief Initialize data structure.
    void Init();
    
  public:

    /// @brief Constructor
    SCALAR_GRID3D_BASE() { Init(); };
    SCALAR_GRID3D_BASE(const AXIS_SIZE_TYPE asize[DIM3]):
      GRID3D(asize)
      { Init(); }

    /// @brief Set scalar value at vertex iv.
    void SetScalar
    (const VERTEX_INDEX_TYPE iv, const SCALAR_TYPE s)
    { scalar[iv] = s; }
      
    /*!
     *  @brief Return scalar value of vertex iv.
     *  @pre 0 <= iv < NumVertices().
     */
    inline SCALAR_TYPE Scalar(const int iv) const
    { return scalar[iv]; }


    // ***************************************************************
    //! @name Output functions - Mainly for debugging 
    // ***************************************************************

    /*!
     *  @brief Output scalar values.
     *  - Output axis_size[2] matrices,
     *      each matrix with dimensions (axis_size[0] x axis_size[1]).
     *  @param line_prefix Print line_prefix at the beginning
     *    of each line.
     */
    template <typename OSTREAM_TYPE>
    void OutScalar(OSTREAM_TYPE & out, const char * line_prefix) const;

  };

  
  /// @brief Class representing a 3D scalar grid.
  /// - Allocates, reallocates and deallocates memory
  ///   for scalar values.
  class SCALAR_GRID3D:public SCALAR_GRID3D_BASE {

  protected:

    /// @brief Initialize data structure.
    void Init();

    /// @brief Free all memory.
    void FreeAll();
    
  public:

    /// @brief Constructor.
    SCALAR_GRID3D() { Init(); };
    SCALAR_GRID3D(const AXIS_SIZE_TYPE asize[DIM3]):
      SCALAR_GRID3D_BASE(asize)
      { Init(); }

    /// @brief Destructor.
    ~SCALAR_GRID3D() { FreeAll(); }

    /// @brief Set axis size.
    /// - Reallocate scalar[] with change in number of vertices.
    template <typename ATYPE>
    void SetAxisSize(const ATYPE asize[]);

    /// @brief Return pointer to array scalar[] storing scalar values.
    SCALAR_TYPE * ScalarPtr() { return(scalar); };

    /// @brief Return const pointer to C array scalar[].
    const SCALAR_TYPE * ScalarPtrConst() const { return(scalar); };

  };  


  // *****************************************************************
  // SCALAR_GRID3D_BASE Output functions - Mainly for debugging
  // *****************************************************************

  // Output scalar values.
  template <typename OSTREAM_TYPE>
  void SCALAR_GRID3D_BASE::OutScalar
  (OSTREAM_TYPE & out, const char * line_prefix) const
  {
    VERTEX_INDEX_TYPE iv = 0;
    for (int z = 0; z < AxisSize(2); z++) {
      for (int y = 0; y < AxisSize(1); y++) {
        iv = z*AxisIncrement(2) + y*AxisIncrement(1);
        out << line_prefix;
        for (int x = 0; x < AxisSize(0); x++) {
          if (x > 0) { out << " "; }
          out << Scalar(iv);
          iv++;
        }
        out << "\n";
      }
      out << "\n";
    }
  }

  
  // *****************************************************************
  // SCALAR_GRID3D member functions
  // *****************************************************************

  // Set axis size.
  template <typename ATYPE>
  void SCALAR_GRID3D::SetAxisSize(const ATYPE asize[])
  {
    const NUMBER_TYPE old_num_vertices = this->NumVertices();
  
    GRID3D::SetAxisSize(asize);

    if (old_num_vertices != this->NumVertices()) {
      FreeAll();

      if (this->NumVertices() > 0) {
        scalar = new SCALAR_TYPE[this->NumVertices()];
      }
    }
  }

}

#endif
