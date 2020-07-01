#ifndef visual_model_h
#define visual_model_h

#include <KFC_Math\mesh.h>
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "d3d_matrix.h"
#include "graphics_device_globals.h"
#include "graphics_state_manager.h"
#include "texture_mapper.h"
#include "d3d_state_block.h"

// ------------------
// Visual model base
// ------------------
class TVisualModelBase
{
private:
	bool m_bAllocated;

	bool m_bDynamic;
	
protected:
	void Allocate(bool bSDynamic);

public:
	TVisualModelBase();

	virtual ~TVisualModelBase()
		{ Release(); }

	virtual bool IsAllocated() const
		{ return m_bAllocated; }

	virtual void Release();

	virtual void SetVertices(const TMesh& SMesh) = 0;

	virtual void Render() const = 0;	

	bool IsDynamic() const
		{ DEBUG_VERIFY_LOCAL_ALLOCATION(TVisualModelBase); return m_bDynamic; }
};

// -----------------
// FVF Visual model
// -----------------
template <class vt, class it>
class TFVFVisualModel : public TVisualModelBase
{
private:
	bool m_bAllocated;

	TD3DStateBlockNode m_StateBlockNode;

	size_t m_szNVertices;
	size_t m_szNFaces;

public:
	typedef vt TVertex;

	typedef it TIndex;


	TFVFVertexBuffer<vt> m_VertexBuffer;

	TIndexBuffer<it> m_IndexBuffer;


	TFVFVisualModel();

	~TFVFVisualModel()
		{ Release(); }

	bool IsAllocated() const
		{ return TVisualModelBase::IsAllocated() && m_bAllocated; }

	void Release();

	void Allocate(	const TMesh&				Mesh,
					bool						bSDynamic,
					const TD3DStateBlockNode&	ParentStateBlockNode,
					const TTextureMapper*		pTextureMappers		= NULL,
					size_t						szNTextureMappers	= 0);

	void SetVertices(const TMesh& SMesh);

	void Render() const;

	size_t GetNVertices() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szNVertices; }

	size_t GetNFaces() const
		{ DEBUG_VERIFY_ALLOCATION; return m_szNFaces; }	
};

template <class vt, class it>
TFVFVisualModel<vt, it>::TFVFVisualModel()
{
	m_bAllocated = false;
}

template <class vt, class it>
void TFVFVisualModel<vt, it>::Release()
{
	m_bAllocated = false;

	m_StateBlockNode.Release();

	m_VertexBuffer.Release();

	m_IndexBuffer.Release();
}

template <class vt, class it>
void TFVFVisualModel<vt, it>::Allocate(	const TMesh&				Mesh,
										bool						bSDynamic,
										const TD3DStateBlockNode&	ParentStateBlockNode,
										const TTextureMapper*		pTextureMappers,
										size_t						szNTextureMappers)
{
	Release();

	try
	{
		DEBUG_VERIFY(Mesh.IsAllocated());

		DEBUG_VERIFY(szNTextureMappers == 0 || pTextureMappers);

		DEBUG_VERIFY(szNTextureMappers <= vt::s_szNTextures);
		
		TVisualModelBase::Allocate(bSDynamic);

		size_t i, j;

		m_VertexBuffer.Allocate(m_szNVertices = Mesh.GetNVertices(),
								(IsDynamic() ? D3DUSAGE_DYNAMIC : 0) | D3DUSAGE_WRITEONLY,
								(IsDynamic() ? D3DPOOL_SYSTEMMEM : D3DPOOL_MANAGED));

		m_IndexBuffer.Allocate((m_szNFaces = Mesh.GetNFaces()) * 3);
		
		// State block
		{
			m_StateBlockNode.Allocate(&ParentStateBlockNode);

			TD3DStateBlockRecorder Recoder0(m_StateBlockNode.m_StateBlock);

			g_GraphicsDeviceGlobals.m_pD3DDevice->
				SetRenderState(D3DRS_NORMALIZENORMALS, IsDynamic() ? TRUE : FALSE);

			for(i = 0 ; i < szNTextureMappers ; i++)
			{
				const TPoint<bool> WrapTextureCoords =
					pTextureMappers[i].GetWrapTextureCoords();

				g_GraphicsDeviceGlobals.m_pD3DDevice->
					SetRenderState(	(D3DRENDERSTATETYPE)(D3DRS_WRAP0 + i),
									(	(WrapTextureCoords.x ? D3DWRAP_U : 0) |
										(WrapTextureCoords.y ? D3DWRAP_V : 0)));
			}

			for( ; i < vt::s_szNTextures ; i++)
			{
				g_GraphicsDeviceGlobals.m_pD3DDevice->
					SetRenderState((D3DRENDERSTATETYPE)(D3DRS_WRAP0 + i), 0);
			}
		}

		// Vertices
		{
			const TMesh::TVertex* pSrc = Mesh.m_Vertices.GetDataPtr();

			vt* pDst;
			TFVFVertexBufferLocker<vt> Locker1(m_VertexBuffer, pDst);

			for(i = m_szNVertices ; i ; i--)
			{
				vt Vertex = *pSrc++;

				// Color
				Vertex.m_Color = WhiteColor();

				// Textures
				{
					for(j = 0 ; j < szNTextureMappers ; j++)
						Vertex.m_TextureCoords[j] = pTextureMappers[j].GetTextureCoords(Vertex.m_Coords);

					for( ; j < vt::s_szNTextures ; j++)
						Vertex.m_TextureCoords[j].SetZero();
				}

				*pDst++ = Vertex;
			}
		}
		
		// Faces
		{
			const TMesh::TFace* pSrc = Mesh.m_Faces.GetDataPtr();

			it* pDst;
			TIndexBufferLocker<it> Locker1(m_IndexBuffer, pDst);

			for(i = m_szNFaces ; i ; i--)
				*pDst++ = (it)pSrc->v1, *pDst++ = (it)pSrc->v2, *pDst++ = (it)pSrc->v3, pSrc++;
		}

		m_bAllocated = true;
	}

	catch(...)
	{
		Release();
		throw;
	}
}

template <class vt, class it>
void TFVFVisualModel<vt, it>::SetVertices(const TMesh& SMesh)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(m_szNVertices == SMesh.GetNVertices());

	size_t i;

	{
		const TMesh::TVertex* pSrc = SMesh.m_Vertices.GetDataPtr();

		vt* pDst;
		TFVFVertexBufferLocker<vt> Locker1(m_VertexBuffer, pDst, 0/*D3DLOCK_DISCARD*/); // (otherwise texture coords can break)

		for(i = m_szNVertices ; i ; i--)
			*pDst++ = *pSrc++;
	}	
}

template <class vt, class it>
void TFVFVisualModel<vt, it>::Render() const
{
	DEBUG_VERIFY_ALLOCATION;

	m_VertexBuffer.Install();

	m_IndexBuffer.Install();

	g_GraphicsStateManager.SetStateBlockNode(&m_StateBlockNode);

	DEBUG_EVALUATE_VERIFY
		(!g_GraphicsDeviceGlobals.m_pD3DDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST,
																		0,
																		0,
																		m_szNVertices,
																		0,
																		m_szNFaces));
}

#endif // visual_model_h
