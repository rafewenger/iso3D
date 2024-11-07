/*!
 *  @file iso3D_cube.h
 *  @brief Cube data structure.
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


#ifndef _ISO3D_CUBE_H_
#define _ISO3D_CUBE_H_

#include <array>

#include "iso3D_const.h"
#include "iso3D_types.h"

namespace ISO3D {

  // ***************************************************************
  // Class CUBE3D_BASE
  // ***************************************************************
  
  /// @brief Basic 3D cube information.  
  class CUBE3D_BASE {

  public:

    /// @brief Constructor.
    CUBE3D_BASE() {};

    /// @brief Return cube dimension (3).
    static constexpr int Dimension()
    { return DIM3; }
    
    /// @brief Return number of cube vertices (8).
    static constexpr int NumVertices()
    { return 8; }

    /// @brief Return number of cube edges (12).
    static constexpr int NumEdges()
    { return 12; }

    /// @brief Return number of cube facets (6).
    static constexpr int NumFacets()
    { return 6; }

    /// @brief Return number of vertices per cube facet (4).
    static constexpr int NumVerticesPerFacet()
    { return 4; }
    
    /// @brief Return d'th coordinate of vertex iv.
    CUBE_COORD VertexCoord(const int iv, const int d) const
    {
      if (d == 0) { return iv%2; }
      else {
        const int x = (iv >> d);
        return x % 2;
      }
    }


    /*!
     *  @brief Return facet side, 0 or 1.
     *  - Facet side 0 is lower/left facet.
     *  - Facet side 1 is top/right facet.
     *  - Facets 0,1,2, are on the lower/left side of cube.
     *  - Facets 3,4,5, are on the top/right of cube.
     *  @param ifacet Facet index.
     *  @pre ifacet is in range [0..5].
     */
    static constexpr int FacetSide(const int ifacet)
    { return (ifacet/Dimension()); }

    /*!
     *  @brief Return direction orthogonal to facet, 0, 1, or 2.
     *  @param ifacet Facet index.
     *  @pre ifacet is in range [0..5].
     */
    static constexpr int FacetOrthDir(const int ifacet)
    { return (ifacet%Dimension()); }

    /*!
     *  @brief Return index of facet parallel and opposite to ifacet.
     *  @param ifacet Facet index.
     *  @pre ifacet is in range [0..5].
     */
    static constexpr int OppositeFacet(const int ifacet)
    { return ((ifacet + Dimension())%NumFacets()); }

      

    // ***************************************************************
    //! @name Output functions - Mainly for debugging 
    // ***************************************************************    

    //@{
    
    /// @brief Output coordinates of vertex iv.
    template <typename OSTREAM_TYPE>
    void OutVertexCoord(OSTREAM_TYPE & out, const int iv) const;

    /*!
     *  @overload
     *  @brief Output vertex coordinates with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutVertexCoord
    (OSTREAM_TYPE & out, const char * prefix, const int iv,
     const char * suffix) const
    { out << prefix; OutVertexAndCoord(out,iv); out << suffix; }
    
    /// @brief Output coordinates of vertex iv.
    template <typename OSTREAM_TYPE>
    void OutVertexIndexAndCoord
    (OSTREAM_TYPE & out, const int iv) const
    { out << iv << " "; OutVertexCoord(out,iv); }

    /*!
     *  @overload
     *  @brief Output vertex index and coordinates 
     *    with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutVertexIndexAndCoord
    (OSTREAM_TYPE & out, const char * prefix, const int iv,
     const char * suffix) const
    { out << prefix; OutVertexIndexAndCoord(out,iv); out << suffix; }

    //@}
    
  };


  // ***************************************************************
  // Class CUBE3D
  // ***************************************************************

  /// @brief 3D cube information.  
  class CUBE3D:public CUBE3D_BASE {

  public:
    
    // @brief Constructor.
    CUBE3D() {};

    /*!
     *  @brief Return j'th vertex of facet ifacet.
     *  - Facet vertices are NOT listed in cylic order.
     */
    int FacetVertex(const int ifacet, const int j) const
    {
      // For some reason, declaring this array in CUBE3D does not work.
      // Not sure why not.
      static constexpr CUBE_VERTEX_INDEX
        facet_vertex[NumFacets()][NumVerticesPerFacet()] =
        { {0, 2, 4, 6}, {0, 4, 1, 5}, {0, 1, 2, 3},
          {5, 7, 1, 3}, {3, 7, 2,6}, {6, 7, 4, 5} };
      
      return facet_vertex[ifacet][j];
    }


    /*!
     *  @brief Return j'th vertex in counter-clockwise order
     *    around facet ifacet.
     *  - Facet vertices are in cyclic order with outward normal
     *    (righ hand orientation).
     */
    int FacetVertexCCW(const int ifacet, const int j) const
    {
      static const int reorder_to_ccw[NumVerticesPerFacet()] =
        { 0, 2, 3, 1 };
      return FacetVertex(ifacet, reorder_to_ccw[j]);
    }
    
  };

  
  // *****************************************************************
  // CUBE3D_BASE Output functions - Mainly for debugging
  // *****************************************************************

  template <typename OSTREAM_TYPE>  
  void CUBE3D_BASE::OutVertexCoord
  (OSTREAM_TYPE & out, const int iv) const
  {
    out << "(";
    for (int d = 0; d < Dimension(); d++) {
      out << VertexCoord(iv,d);
      if (d+1 < Dimension())
        { out << ","; }
    }
    out << ")";
  }

}

#endif
