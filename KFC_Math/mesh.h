#ifndef mesh_h
#define mesh_h

#include "spherical_bounds.h"

// -----
// Mesh
// -----
class TMesh
{
public:
	// Vertex
	struct TVertex
	{
		TDVector3 m_Coords;
		TDVector3 m_Normal;
	};

	// Face
	struct TFace
	{
		size_t v1, v2, v3;

		TFace& Set(size_t sv1, size_t sv2, size_t sv3)
			{ v1 = sv1, v2 = sv2, v3 = sv3; return *this; }

		void Shift(int iVDlt)
			{ v1 += iVDlt, v2 += iVDlt, v3 += iVDlt; }

		void Flip()
			{ Swap(v1, v2); }
	};

	// Edge
	struct TEdge
	{
		size_t v1, v2; // base vertices (v1<v2: search index)

		size_t f1, f2; // faces (f1: face(v1<v2), f2: face(v1>v2)

		size_t ov1, ov2; // opposite vertices (ov1: f1, ov2: f2)

		
		TEdge()
			{ f1 = f2 = UINT_MAX; }

		TEdge& Set(size_t sv1, size_t sv2)
			{ v1 = sv1, v2 = sv2; return *this; }

		void SetF1(size_t sf1, size_t sov1)
			{ DEBUG_VERIFY(f1 == UINT_MAX); f1 = sf1; ov1 = sov1; }

		void SetF2(size_t sf2, size_t sov2)
			{ DEBUG_VERIFY(f2 == UINT_MAX); f2 = sf2; ov2 = sov2; }

		void Shift(int iVDlt, int iFDlt)
			{ v1 += iVDlt, v2 += iVDlt; f1 += iFDlt, f2 += iFDlt; ov1 += iVDlt; ov2 += iVDlt; }

		void Flip()
			{ Swap(v1, v2); Swap(f1, f2); Swap(ov1, ov2); }

		static int SortFunction(const void* p1, const void* p2);
	};

private:
	bool m_bAllocated;

	TDVector3 m_CenterOfMasses;

	TSphericalBounds m_Bounds;

	bool m_bEdgesUpdated;

public:
	typedef TArray<TVertex, true> TVertices;
	typedef TArray<TFace,   true> TFaces;
	typedef TArray<TEdge,	true> TEdges;


	TVertices	m_Vertices;
	TFaces		m_Faces;
	TEdges		m_Edges;


	TMesh();

	~TMesh() { Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	TMesh& Allocate(size_t szNVertices, size_t szNFaces);

	void InvalidateCenterOfMasses();

	void InvalidateVertexNormals();

	void InvalidateBounds();

	void InvalidateEdges();

	void UpdateCenterOfMasses();

	void UpdateBounds();

	void UpdateVertexNormals(bool bNormalize = true);

	// Mesh must be capped
	void UpdateEdges();

	TEdge* FindEdge(size_t v1, size_t v2)
		{ return (TEdge*)((const TMesh*)this)->FindEdge(v1, v2); }

	const TEdge* FindEdge(size_t v1, size_t v2) const;

	TEdge& GetEdge(size_t v1, size_t v2)
	{
		TEdge* pEdge = FindEdge(v1, v2);

		DEBUG_VERIFY(pEdge);

		return *pEdge;
	}

	const TEdge& GetEdge(size_t v1, size_t v2) const
	{
		const TEdge* pEdge = FindEdge(v1, v2);

		DEBUG_VERIFY(pEdge);

		return *pEdge;
	}

	TMesh& Merge(const TMesh& SMesh, bool bUpdate = true);

	// Corners 1,2,3 must be in CW order
	TMesh& CreateFace(	const TDVector3& Coord1,
						const TDVector3& Coord2,
						const TDVector3& Coord3);

	// Corners 1,2,3 must be in CW order
	TMesh& CreateFlatGrid(	const TDVector3&	Corner1,
							const TDVector3&	Corner2,
							const TDVector3&	Corner3,
							size_t				szNSegs12 = 1,
							size_t				szNSegs13 = 1);

	// Corners 1,2,3 / 1,3,4 / 1,4,2 must be in CW order
	TMesh& CreateParallelepiped(	const TDVector3&	Corner1,
									const TDVector3&	Corner2,
									const TDVector3&	Corner3,
									const TDVector3&	Corner4,
									size_t				szNSegs12 = 1,
									size_t				szNSegs13 = 1,
									size_t				szNSegs14 = 1);

	TMesh& CreateCube(	const TDVector3&	Center,
						double				dRadius,
						size_t				szNSegs = 1)
	{
		return CreateParallelepiped(Center + TDVector3(-1, -1, -1) * dRadius,
									Center + TDVector3(-1, +1, -1) * dRadius,
									Center + TDVector3(+1, -1, -1) * dRadius,
									Center + TDVector3(-1, -1, +1) * dRadius,
									szNSegs,
									szNSegs,
									szNSegs);
	}

	// 'Radius.y' means half-height
	TMesh& CreateCylinder(	const TDVector3&	Center,
							const TDVector3&	Radius,
							size_t				szNSegsXZ,
							size_t				szNSegsY,
							bool				bCapBottom,
							bool				bCapTop);

	TMesh& CreateEllipsoid(	const TDVector3&	Center,
							const TDVector3&	Radius,
							size_t				szNSegsXZ,
							size_t				szNSegsY);

	// Inverts normals and changes face vertex order
	TMesh& Flip();

	// Merges with flipped mesh
	TMesh& MakeTwoSided();

	TDVector3 GetFaceCenter(size_t i) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(i < GetNFaces());

		const TFace& Face = m_Faces[i];

		return (m_Vertices[Face.v1].m_Coords +
				m_Vertices[Face.v2].m_Coords +
				m_Vertices[Face.v3].m_Coords) * _1_3;
	}

	bool AreEdgesUpdated() const
		{ DEBUG_VERIFY_ALLOCATION; return m_bEdgesUpdated; }

	// Requires edges to be updated
	bool IsConvex() const;

	size_t GetNVertices() const
		{ return m_Vertices.GetN(); }

	size_t GetNFaces() const
		{ return m_Faces.GetN(); }

	size_t GetNEdges() const
		{ return m_Edges.GetN(); }

	const TDVector3& GetCenterOfMasses() const
		{ return m_CenterOfMasses; }

	const TSphericalBounds& GetBounds() const
		{ return m_Bounds; }
};

#endif // mesh_h
