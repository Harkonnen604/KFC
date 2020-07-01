#ifndef vetex_formats_h
#define vetex_formats_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_Math\mesh.h>
#include "d3d_vector.h"
#include "color_defs.h"

// ---------------------------
// Untransformed unlit vertex
// ---------------------------
template <size_t nt = 1>
struct TUTULVertex
{
private:
	static flags_t GetTextureFVF()
	{
		size_t i;

		flags_t flFormat = nt << D3DFVF_TEXCOUNT_SHIFT;

		for(i = 0 ; i < nt ; i++)
			flFormat |= D3DFVF_TEXCOORDSIZE2(i);

		return flFormat;
	}

public:

	static const size_t		s_szNTextures;
	static const flags_t	s_flFVF;

	TD3DVector	m_Coords;
	TD3DVector	m_Normal;
	D3DCOLOR	m_Color;
	FPOINT		m_TextureCoords[nt];	


	TUTULVertex() {}

	TUTULVertex(const TMesh::TVertex& v)
		{ *this = v; }

	TUTULVertex& operator = (const TMesh::TVertex& v)
	{
		m_Coords = TO_F(v.m_Coords);
		m_Normal = TO_F(v.m_Normal);

		return *this;
	}
};

template <size_t nt>
const size_t TUTULVertex<nt>::s_szNTextures = nt;

template <size_t nt>
const flags_t TUTULVertex<nt>::s_flFVF =	D3DFVF_XYZ		|
											D3DFVF_NORMAL	|
											D3DFVF_DIFFUSE	|
											GetTextureFVF();

// -----------------------
// Transformed lit vertex
// -----------------------
template <size_t nt = 1>
struct TTLVertex
{
private:
	static flags_t GetTextureFVF()
	{
		size_t i;

		flags_t flFormat = nt << D3DFVF_TEXCOUNT_SHIFT;

		for(i = 0 ; i < nt ; i++)
			flFormat |= D3DFVF_TEXCOORDSIZE2(i);

		return flFormat;
	}

public:
	static const size_t		s_szNTextures;
	static const flags_t	s_flFVF;

	TD3DVector	m_Coords;
	float		rhw;
	D3DCOLOR	m_Color;
	FPOINT		m_TextureCoords[nt];	


	void SetScreenCoords(float sx, float sy)
		{ m_Coords.Set(sx, sy, 0.0f), rhw = 1.0f; }
};

template <size_t nt>
const flags_t TTLVertex<nt>::s_szNTextures = nt;

template <size_t nt>
const flags_t TTLVertex<nt>::s_flFVF =	D3DFVF_XYZRHW	|
										D3DFVF_DIFFUSE	|
										GetTextureFVF();

// -----------------
// 2D Effect vertex
// -----------------
struct T2DEffectVertex
{
	static const flags_t s_flFVF;

	TD3DVector	m_Coords;
	float		rhw;
	D3DCOLOR	m_Color;	


	void SetScreenCoords(float sx, float sy)
		{ m_Coords.Set(sx, sy, 0.0f), rhw = 1.0f; }
};

#endif // vetex_formats_h
