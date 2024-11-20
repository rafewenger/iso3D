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
    CUBE_COORD_TYPE VertexCoord(const int iv, const int d) const
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

    /*!
     *  @brief Return edge direction, 0, 1, or 2.
     *  @param iedge Edge index.
     *  @pre iedge is in range [0..11]
     */
    static constexpr int EdgeDirection(const int iedge)
    { return (iedge/NumVerticesPerFacet()); }
      

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

  protected:

    // *** DOES NOT WORK WITH g++10. ***
    // Workaround is to define facet_vertex[] inside
    //   the member function FacetVertex().
    /* NOT CURRENTLY USED.
    static constexpr CUBE_VERTEX_INDEX
    facet_vertex[NumFacets()][NumVerticesPerFacet()] =
      { {0, 2, 4, 6}, {0, 4, 1, 5}, {0, 1, 2, 3},
        {5, 7, 1, 3}, {3, 7, 2,6}, {6, 7, 4, 5} };
    */

    
  public:
    
    // @brief Constructor.
    CUBE3D() {};

    /*!
     *  @brief Return j'th vertex of facet ifacet.
     *  - Facet vertices are NOT listed in cylic order.
     */
    int FacetVertex(const int ifacet, const int j) const
    {
      // Declaring this array in CUBE3D does not work in gc10.
      // This is a workaround.
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


    /*!
     *  @brief Return j'th endpoint of edge iedge.
     *  @param iedge Edge index.
     *  @pre In range [0..11].
     *  @param j Endpoint index, 0 or 1.
     */
    int EdgeEndpoint(const int iedge, const int j) const
    {
      static const int edge_increment[Dimension()] = { 1, 2, 4 };
      
      const int edge_direction = EdgeDirection(iedge);
      const int ifacet_vertex = iedge%NumVerticesPerFacet();
      const int iv = FacetVertex(edge_direction, ifacet_vertex) +
        j*edge_increment[edge_direction];
        
      return iv;
    }

    
    // ***************************************************************
    //! @name Output functions - Mainly for debugging 
    // ***************************************************************    

    /// @brief Output edge endpoints.
    template <typename OSTREAM_TYPE>
    void OutEdgeEndpoints(OSTREAM_TYPE & out, const int iedge) const;

    /*!
     *  @overload
     *  @brief Output edge endpoints with preceding 
     *    and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutEdgeEndpoints
    (OSTREAM_TYPE & out, const char * prefix, const int iedge,
     const char * suffix) const
    { out << prefix; OutEdgeEndpoints(out,iedge); out << suffix; }
      
    /// @brief Output facet vertices.
    template <typename OSTREAM_TYPE>
    void OutFacetVertices(OSTREAM_TYPE & out, const int ifacet) const;

    /*!
     *  @overload
     *  @brief Output facet vertices with preceding 
     *    and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutFacetVertices
    (OSTREAM_TYPE & out, const char * prefix, const int ifacet,
     const char * suffix) const
    { out << prefix; OutFacetVertices(out,ifacet); out << suffix; }

    /// @brief Output facet vertices in counter-clockwise order.
    /// - Facet vertices are NOT listed in cylic order.
    template <typename OSTREAM_TYPE>
    void OutFacetVerticesCCW(OSTREAM_TYPE & out, const int ifacet) const;

    /*!
     *  @overload
     *  @brief Output facet vertices in counter-clockwise order
     *    with preceding and following string.
     */
    template <typename OSTREAM_TYPE>
    void OutFacetVerticesCCW
    (OSTREAM_TYPE & out, const char * prefix, const int ifacet,
     const char * suffix) const
    { out << prefix; OutFacetVerticesCCW(out,ifacet); out << suffix; }      
    
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


  // *****************************************************************
  // CUBE3D Output functions - Mainly for debugging
  // *****************************************************************

  template <typename OSTREAM_TYPE>
  void CUBE3D::OutEdgeEndpoints
  (OSTREAM_TYPE & out, const int iedge) const
  {
    out << "(" << EdgeEndpoint(iedge,0) << ","
        << EdgeEndpoint(iedge,1) << ")";
  }

  
  template <typename OSTREAM_TYPE>
  void CUBE3D::OutFacetVertices
  (OSTREAM_TYPE & out, const int ifacet) const
  {
    out << "(";
    for (int j = 0; j < NumVerticesPerFacet(); j++) {
      out << FacetVertex(ifacet,j);
      if (j+1 < NumVerticesPerFacet())
        { out << ","; }
    }
    out << ")";
  }


  template <typename OSTREAM_TYPE>
  void CUBE3D::OutFacetVerticesCCW
  (OSTREAM_TYPE & out, const int ifacet) const
  {
    out << "(";
    for (int j = 0; j < NumVerticesPerFacet(); j++) {
      out << FacetVertexCCW(ifacet,j);
      if (j+1 < NumVerticesPerFacet())
        { out << ","; }
    }
    out << ")";
  }    
  
}

#endif
