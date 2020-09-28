#include "kfc_math_pch.h"
#include "mesh.h"

#include <KFC_KTL\bounding.h>

// -----
// Mesh
// -----

// Edge
int TMesh::TEdge::SortFunction(const void* p1, const void* p2)
{
    const TEdge& e1 = *(const TEdge*)p1;
    const TEdge& e2 = *(const TEdge*)p2;

    int d;

    if(d = e1.v1 - e2.v1)
        return d;

    if(d = e1.v2 - e2.v2)
        return d;

    return 0;
}

// Mesh
TMesh::TMesh()
{
    m_bAllocated = false;
}

void TMesh::Release()
{
    m_bAllocated = false;

    m_Edges.    Clear();
    m_Vertices. Clear();
    m_Faces.    Clear();
}

TMesh& TMesh::Allocate(size_t szNVertices, size_t szNFaces)
{
    Release();

    try
    {
        DEBUG_VERIFY(szNVertices > 0 && szNFaces > 0);

        m_Vertices.SetN(szNVertices);
        m_Faces.   SetN(szNFaces);

        m_bEdgesUpdated = false;

        m_bAllocated = true;
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

void TMesh::InvalidateCenterOfMasses()
{
    m_CenterOfMasses.SetZero();
}

void TMesh::InvalidateVertexNormals()
{
    size_t i;

    TVertex* pVertex;

    for(pVertex = m_Vertices.GetDataPtr(), i = GetNVertices() ; i ; i--, pVertex++)
        pVertex->m_Normal.SetZero();
}

void TMesh::InvalidateBounds()
{
    m_Bounds.Reset(m_CenterOfMasses);
}

void TMesh::InvalidateEdges()
{
    m_Edges.Clear();

    m_bEdgesUpdated = false;
}

void TMesh::UpdateCenterOfMasses()
{
    InvalidateCenterOfMasses();

    double tv = 0.0;

    size_t i;

    for(i = 0 ; i < GetNFaces() ; i++)
    {
        const TDVector3& c1 = m_Vertices[m_Faces[i].v1].m_Coords;
        const TDVector3& c2 = m_Vertices[m_Faces[i].v2].m_Coords;
        const TDVector3& c3 = m_Vertices[m_Faces[i].v3].m_Coords;

        const double v = MixedProduct(c1, c2, c3);

        m_CenterOfMasses += (c1 + c2 + c3) * v, tv += v;
    }

    if(Compare(tv, 0.0))
        m_CenterOfMasses *= 0.25 / tv;
}

void TMesh::UpdateBounds()
{
    DEBUG_VERIFY_ALLOCATION;

    InvalidateBounds();

    const TVertex* pVertex;

    size_t i;

    for(pVertex = m_Vertices.GetDataPtr(), i = GetNVertices() ; i ; pVertex++, i--)
        m_Bounds.Update(pVertex->m_Coords);
}

void TMesh::UpdateVertexNormals(bool bNormalize)
{
    DEBUG_VERIFY_ALLOCATION;

    size_t i;

    const TFace* pFace;

    // Summing
    for(pFace = m_Faces.GetDataPtr(), i = GetNFaces() ; i ; i--, pFace++)
    {
        TVertex& Vertex1 = m_Vertices[pFace->v1];
        TVertex& Vertex2 = m_Vertices[pFace->v2];
        TVertex& Vertex3 = m_Vertices[pFace->v3];

        const TDVector3& Coords1 = Vertex1.m_Coords;
        const TDVector3& Coords2 = Vertex2.m_Coords;
        const TDVector3& Coords3 = Vertex3.m_Coords;

        const TDVector3 FaceNormal = (Coords2 - Coords1) * (Coords3 - Coords1);

        Vertex1.m_Normal += FaceNormal;
        Vertex2.m_Normal += FaceNormal;
        Vertex3.m_Normal += FaceNormal;
    }

    // Normalizing
    if(bNormalize)
    {
        TVertex* pVertex;

        for(pVertex = m_Vertices.GetDataPtr(), i = GetNVertices() ; i ; i--, pVertex++)
            pVertex->m_Normal.Normalize();
    }
}

void TMesh::UpdateEdges()
{
    DEBUG_VERIFY_ALLOCATION;

    InvalidateEdges();

    size_t i;

    const TFace* pFace;

    // Adding
    for(pFace = m_Faces.GetDataPtr(), i = 0 ; i < GetNFaces() ; pFace++, i++)
    {
        if(pFace->v1 < pFace->v2)
            m_Edges.Add().Set(pFace->v1, pFace->v2).SetF1(i, pFace->v3);

        if(pFace->v2 < pFace->v3)
            m_Edges.Add().Set(pFace->v2, pFace->v3).SetF1(i, pFace->v1);

        if(pFace->v3 < pFace->v1)
            m_Edges.Add().Set(pFace->v3, pFace->v1).SetF1(i, pFace->v2);
    }

    DEBUG_VERIFY(GetNFaces() % 2 == 0 && m_Edges.GetN() == GetNFaces() * 3 / 2);

    // Sorting
    m_Edges.Sort(TEdge::SortFunction);

    // Setting 2nd face indices
    for(pFace = m_Faces.GetDataPtr(), i = 0 ; i < GetNFaces() ; pFace++, i++)
    {
        if(pFace->v1 > pFace->v2)
            GetEdge(pFace->v2, pFace->v1).SetF2(i, pFace->v3);

        if(pFace->v2 > pFace->v3)
            GetEdge(pFace->v3, pFace->v2).SetF2(i, pFace->v1);

        if(pFace->v3 > pFace->v1)
            GetEdge(pFace->v1, pFace->v3).SetF2(i, pFace->v2);
    }

    #ifdef _DEBUG
    {
        TEdge* pEdge;

        for(pEdge = m_Edges.GetDataPtr(), i = GetNEdges() ; i ; pEdge++, i--)
            DEBUG_VERIFY(pEdge->f1 != UINT_MAX && pEdge->f2 != UINT_MAX);
    }
    #endif // _DEBUG

    m_bEdgesUpdated = true;
}

const TMesh::TEdge* TMesh::FindEdge(size_t v1, size_t v2) const
{
    size_t l = 0, r = GetNEdges() - 1;

    for(;;)
    {
        const size_t m = (l + r) >> 1;

        const TEdge& Edge = m_Edges[m];

        int d;

        if(Edge.v1 < v1)
            d = -1;
        else if(Edge.v1 > v1)
            d = +1;
        else if(Edge.v2 < v2)
            d = -1;
        else if(Edge.v2 > v2)
            d = +1;
        else
            return &Edge;

        if(l == r)
            break;

        if(d < 0)
            l = l == m ? r : m;
        else
            r = m;
    }

    return NULL;
}

TMesh& TMesh::Merge(const TMesh& SMesh, bool bUpdate)
{
    if(!IsAllocated())
        return *this = SMesh;

    DEBUG_VERIFY(&SMesh != this);

    size_t i;

    const int iVDlt = GetNVertices();
    const int iFDlt = GetNFaces   ();

    // Vertices
    for(i = 0 ; i < SMesh.GetNVertices() ; i++)
        m_Vertices.Add() = SMesh.m_Vertices[i];

    // Faces
    for(i = 0 ; i < SMesh.GetNFaces() ; i++)
        (m_Faces.Add() = SMesh.m_Faces[i]).Shift(iVDlt);

    if(bUpdate)
    {
        UpdateCenterOfMasses();

        UpdateBounds();
    }

    if(AreEdgesUpdated())
    {
        if(SMesh.AreEdgesUpdated())
        {
            // Edges
            for(i = 0 ; i < SMesh.GetNEdges() ; i++)
                (m_Edges.Add() = SMesh.m_Edges[i]).Shift(iVDlt, iFDlt);
        }
        else
        {
            InvalidateEdges();
        }
    }

    return *this;
}

TMesh& TMesh::CreateFace(   const TDVector3& Coord1,
                            const TDVector3& Coord2,
                            const TDVector3& Coord3)
{
    Release();

    try
    {
        Allocate(3, 1);

        // Vertices
        m_Vertices[0].m_Coords = Coord1;
        m_Vertices[1].m_Coords = Coord2;
        m_Vertices[2].m_Coords = Coord3;

        // Faces
        m_Faces[0].Set(0, 1, 2);

        // Processing
        UpdateCenterOfMasses();

        UpdateBounds();

        UpdateVertexNormals();
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

TMesh& TMesh::CreateFlatGrid(   const TDVector3&    Corner1,
                                const TDVector3&    Corner2,
                                const TDVector3&    Corner3,
                                size_t              szNSegs12,
                                size_t              szNSegs13)
{
    Release();

    try
    {
        DEBUG_VERIFY(szNSegs12 > 0 && szNSegs13 > 0);

        size_t i, j;

        const TDVector3 Corner4 = Corner2 + Corner3 - Corner1;

        Allocate((szNSegs12 + 1) * (szNSegs13 + 1), (szNSegs12 * szNSegs13) << 1);

        // Vertices
        {
            TVertex* pVertex = m_Vertices.GetDataPtr();

            const TDVector3 d2 = Corner2 - Corner1;
            const TDVector3 d3 = Corner3 - Corner1;

            const double c2 = 1.0 / szNSegs12;
            const double c3 = 1.0 / szNSegs13;

            const TDVector3 Normal = (d2 * d3).Normalize();

            for(i = 0 ; i <= szNSegs12 ; i++)
            {
                const TDVector3 v2 = d2 * (i * c2);

                for(j = 0 ; j <= szNSegs13 ; j++)
                {
                    const TDVector3 v3 = d3 * (j * c3);

                    pVertex->m_Coords = Corner1 + v2 + v3;
                    pVertex->m_Normal = Normal;

                    pVertex++;
                }
            }

            DEBUG_VERIFY((size_t)(pVertex - m_Vertices.GetDataPtr()) == m_Vertices.GetN());
        }

        // Faces
        {
            TFace* pFace = m_Faces.GetDataPtr();

            size_t szIndex;

            for(i = 0 ; i < szNSegs12 ; i++)
            {
                szIndex = i * (szNSegs13 + 1);

                for(j = 0 ; j < szNSegs13 ; j++, szIndex++)
                {
                    pFace++ -> Set(szIndex, szIndex + (szNSegs13 + 1), szIndex + 1);
                    pFace++ -> Set(szIndex + (szNSegs13 + 1), szIndex + (szNSegs13 + 2), szIndex + 1);
                }
            }

            DEBUG_VERIFY((size_t)(pFace - m_Faces.GetDataPtr()) == GetNFaces());
        }

        // Processing
        UpdateCenterOfMasses();

        UpdateBounds();
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

TMesh& TMesh::CreateParallelepiped( const TDVector3&    Corner1,
                                    const TDVector3&    Corner2,
                                    const TDVector3&    Corner3,
                                    const TDVector3&    Corner4,
                                    size_t              szNSegs12,
                                    size_t              szNSegs13,
                                    size_t              szNSegs14)
{
    Release();

    try
    {
        const TDVector3 d2 = Corner2 - Corner1;
        const TDVector3 d3 = Corner3 - Corner1;
        const TDVector3 d4 = Corner4 - Corner1;

        const TDVector3 OCorner1 = Corner1 + d2 + d3 + d4;
        const TDVector3 OCorner2 = OCorner1 - d2;
        const TDVector3 OCorner3 = OCorner1 - d3;
        const TDVector3 OCorner4 = OCorner1 - d4;

        Merge(TMesh().CreateFlatGrid(Corner1, Corner2, Corner3, szNSegs12, szNSegs13), false);
        Merge(TMesh().CreateFlatGrid(Corner1, Corner3, Corner4, szNSegs13, szNSegs14), false);
        Merge(TMesh().CreateFlatGrid(Corner1, Corner4, Corner2, szNSegs14, szNSegs12), false);

        Merge(TMesh().CreateFlatGrid(OCorner1, OCorner3, OCorner2, szNSegs13, szNSegs12), false);
        Merge(TMesh().CreateFlatGrid(OCorner1, OCorner4, OCorner3, szNSegs14, szNSegs13), false);
        Merge(TMesh().CreateFlatGrid(OCorner1, OCorner2, OCorner4, szNSegs12, szNSegs14), false);

        // Processing
        UpdateCenterOfMasses();

        UpdateBounds();
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

TMesh& TMesh::CreateCylinder(   const TDVector3&    Center,
                                const TDVector3&    Radius,
                                size_t              szNSegsXZ,
                                size_t              szNSegsY,
                                bool                bCapBottom,
                                bool                bCapTop)
{
    Release();

    try
    {
        DEBUG_VERIFY(Radius.IsPositive());

        DEBUG_VERIFY(szNSegsXZ >= 2);

        Allocate(   szNSegsXZ * (szNSegsY + 1) +
                        (bCapBottom ? szNSegsXZ + 1 : 0) +
                        (bCapTop    ? szNSegsXZ + 1 : 0),

                    szNSegsXZ * szNSegsY * 2 +
                        (bCapBottom ? szNSegsXZ : 0) +
                        (bCapTop    ? szNSegsXZ : 0));

        size_t i, j;

        size_t szBottomStart, szBottomCenter;
        size_t szTopStart,    szTopCenter;

        // Vertices
        {
            TVertex* pVertex = m_Vertices.GetDataPtr();

            const TDVector3 NormalCoefs(1.0 / Sqr(Radius.x), 0, 1.0 / Sqr(Radius.z));

            const double by = -Radius.y;

            const double yc = Radius.y * 2 / szNSegsY;

            const double ac = g_PI * 2 / szNSegsXZ;

            // Side
            for(i = 0 ; i <= szNSegsY ; i++)
            {
                const double cy = by + i * yc;

                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const double ang = j * ac;

                    pVertex->m_Coords.Set(cos(ang) * Radius.x, cy, sin(ang) * Radius.z);

                    pVertex->m_Normal.Set(  pVertex->m_Coords.x * NormalCoefs.x,
                                            0,
                                            pVertex->m_Coords.z * NormalCoefs.z).Normalize();

                    pVertex->m_Coords += Center;

                    pVertex++;
                }
            }

            // Bottom
            if(bCapBottom)
            {
                szBottomStart = pVertex - m_Vertices.GetDataPtr();

                // Perimeter
                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const double ang = j * ac;

                    pVertex->m_Coords =
                        Center + TDVector3( cos(ang) * Radius.x,
                                            -Radius.y,
                                            sin(ang) * Radius.z);

                    pVertex->m_Normal.Set(0, -1, 0);

                    pVertex++;
                }

                // Center
                {
                    pVertex->m_Coords.Set(Center.x, Center.y - Radius.y, Center.z);

                    pVertex->m_Normal.Set(0, -1, 0);

                    szBottomCenter = pVertex++ - m_Vertices.GetDataPtr();
                }
            }

            // Top
            if(bCapTop)
            {
                szTopStart = pVertex - m_Vertices.GetDataPtr();

                // Perimeter
                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const double ang = j * ac;

                    pVertex->m_Coords =
                        Center + TDVector3( cos(ang) * Radius.x,
                                            +Radius.y,
                                            sin(ang) * Radius.z);

                    pVertex->m_Normal.Set(0, +1, 0);

                    pVertex++;
                }

                // Center
                {
                    pVertex->m_Coords.Set(Center.x, Center.y + Radius.y, Center.z);

                    pVertex->m_Normal.Set(0, +1, 0);

                    szTopCenter = pVertex++ - m_Vertices.GetDataPtr();
                }
            }

            DEBUG_VERIFY((size_t)(pVertex - m_Vertices.GetDataPtr()) == m_Vertices.GetN());
        }

        // Faces
        {
            TFace* pFace = m_Faces.GetDataPtr();

            size_t szIndex1, szIndex2;

            // Side
            for(i = 0 ; i < szNSegsY ; i++)
            {
                szIndex1 = i * szNSegsXZ;
                szIndex2 = (i + 1) * szNSegsXZ;

                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const size_t j1 = j;
                    const size_t j2 = j == szNSegsXZ - 1 ? 0 : j + 1;

                    pFace++ -> Set(szIndex1 + j1, szIndex2 + j1, szIndex1 + j2);
                    pFace++ -> Set(szIndex1 + j2, szIndex2 + j1, szIndex2 + j2);
                }
            }

            // Bottom
            if(bCapBottom)
            {
                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const size_t j1 = j;
                    const size_t j2 = j == szNSegsXZ - 1 ? 0 : j + 1;

                    pFace++ -> Set(szBottomCenter, szBottomStart + j1, szBottomStart + j2);
                }
            }

            // Top
            if(bCapTop)
            {
                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const size_t j1 = j;
                    const size_t j2 = j == szNSegsXZ - 1 ? 0 : j + 1;

                    pFace++ -> Set(szTopCenter, szTopStart + j2, szTopStart + j1);
                }
            }

            DEBUG_VERIFY((size_t)(pFace - m_Faces.GetDataPtr()) == GetNFaces());
        }

        // Processing
        m_CenterOfMasses = Center;

        m_Bounds.Set(Center, Hypot(Max(Radius.x, Radius.z), Radius.y));

        UpdateBounds();

        if(bCapTop && bCapBottom)
            UpdateEdges();
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

TMesh& TMesh::CreateEllipsoid(  const TDVector3&    Center,
                                const TDVector3&    Radius,
                                size_t              szNSegsXZ,
                                size_t              szNSegsY)
{
    Release();

    try
    {
        DEBUG_VERIFY(Radius.IsPositive());

        DEBUG_VERIFY(szNSegsXZ >= 2);

        Allocate(szNSegsXZ * (szNSegsY + 1) + 2, szNSegsXZ * (szNSegsY + 1) * 2);

        size_t i, j;

        size_t szBottom;
        size_t szTop;

        // Vertices
        {
            TVertex* pVertex = m_Vertices.GetDataPtr();

            // Base
            {
                const TDVector3 NormalCoefs(1.0 / Sqr(Radius.x),
                                            1.0 / Sqr(Radius.y),
                                            1.0 / Sqr(Radius.z));

                const double b1 = -g_PI * 0.5;

                const double c1 = g_PI / (szNSegsY + 2);
                const double c2 = g_PI * 2 / szNSegsXZ;

                for(i = 1 ; i <= szNSegsY + 1 ; i++)
                {
                    const double a1 = b1 + i * c1;

                    const double cs1 = cos(a1);
                    const double sn1 = sin(a1);

                    for(j = 0 ; j < szNSegsXZ ; j++)
                    {
                        const double a2 = j * c2;

                        const double cs2 = cos(a2);
                        const double sn2 = sin(a2);

                        pVertex->m_Coords.Set(  cs2 * cs1 * Radius.x,
                                                sn1       * Radius.y,
                                                sn2 * cs1 * Radius.z);

                        pVertex->m_Normal.Set(  pVertex->m_Coords.x * NormalCoefs.x,
                                                pVertex->m_Coords.y * NormalCoefs.y,
                                                pVertex->m_Coords.z * NormalCoefs.z).Normalize();

                        pVertex->m_Coords += Center;

                        pVertex++;
                    }
                }
            }

            // Bottom
            {
                pVertex->m_Coords.Set(Center.x, Center.y - Radius.y, Center.z);
                pVertex->m_Normal.SetYAxis(-1);

                szBottom = pVertex++ - m_Vertices.GetDataPtr();
            }

            // Top
            {
                pVertex->m_Coords.Set(Center.x, Center.y + Radius.y, Center.z);
                pVertex->m_Normal.SetYAxis(+1);

                szTop = pVertex++ - m_Vertices.GetDataPtr();
            }

            DEBUG_VERIFY((size_t)(pVertex - m_Vertices.GetDataPtr()) == GetNVertices());
        }

        // Faces
        {
            TFace* pFace = m_Faces.GetDataPtr();

            size_t szIndex1, szIndex2;

            // Base
            for(i = 0 ; i < szNSegsY ; i++)
            {
                szIndex1 = szNSegsXZ * i;
                szIndex2 = szNSegsXZ * (i + 1);

                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const size_t j1 = j;
                    const size_t j2 = j == szNSegsXZ - 1 ? 0 : j + 1;

                    pFace++ -> Set(szIndex1 + j1, szIndex2 + j1, szIndex1 + j2);
                    pFace++ -> Set(szIndex1 + j2, szIndex2 + j1, szIndex2 + j2);
                }
            }

            // Bottom
            {
                szIndex1 = 0;

                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const size_t j1 = j;
                    const size_t j2 = j == szNSegsXZ - 1 ? 0 : j + 1;

                    pFace++ -> Set(szBottom, szIndex1 + j1, szIndex1 + j2);
                }
            }

            // Top
            {
                szIndex1 = szNSegsXZ * szNSegsY;

                for(j = 0 ; j < szNSegsXZ ; j++)
                {
                    const size_t j1 = j;
                    const size_t j2 = j == szNSegsXZ - 1 ? 0 : j + 1;

                    pFace++ -> Set(szTop, szIndex1 + j2, szIndex1 + j1);
                }
            }

            DEBUG_VERIFY((size_t)(pFace - m_Faces.GetDataPtr()) == GetNFaces());
        }

        // Processing
        m_CenterOfMasses = Center;

        m_Bounds.Set(Center, Max(Radius.x, Radius.y, Radius.z));

        UpdateEdges();
    }

    catch(...)
    {
        Release();
        throw;
    }

    return *this;
}

TMesh& TMesh::Flip()
{
    DEBUG_VERIFY_ALLOCATION;

    size_t i;

    // Vertex normals
    for(i = 0 ; i < GetNVertices() ; i++)
        m_Vertices[i].m_Normal.Flip();

    // Faces
    for(i = 0 ; i < GetNFaces() ; i++)
        m_Faces[i].Flip();

    if(AreEdgesUpdated())
    {
        // Edges
        for(i = 0 ; i < GetNEdges() ; i++)
            m_Edges[i].Flip();
    }

    InvalidateEdges();

    return *this;
}

TMesh& TMesh::MakeTwoSided()
{
    DEBUG_VERIFY_ALLOCATION;

    return Merge(TMesh(*this).Flip(), false);
}

bool TMesh::IsConvex() const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(AreEdgesUpdated());

    size_t i;

    const TEdge* pEdge;

    // Checking that 3rd vertex of one face is within non-positive sub-space of adjacent face
    for(pEdge = m_Edges.GetDataPtr(), i = GetNEdges() ; i ; pEdge++, i--)
    {
        const TDVector3& Origin =m_Vertices[pEdge->ov1].m_Coords;

        if(Sign((m_Vertices[pEdge->ov2].m_Coords - Origin) &
                (   (m_Vertices[pEdge->v1].m_Coords - Origin) *
                    (m_Vertices[pEdge->v2].m_Coords - Origin))) > 0)
        {
            return false;
        }
    }

    return true;
}
