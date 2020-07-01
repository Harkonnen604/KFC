#ifndef index_buffer_h
#define index_buffer_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
// #include "graphics_device_globals.h" // cross-inclusion
// #include "graphics_state_manager.h" // cross-inclusion

// ------------------
// Index buffer base
// ------------------
class TIndexBufferBase
{
private:
	IDirect3DIndexBuffer9* m_pIndexBuffer;

public:
	TIndexBufferBase();

	~TIndexBufferBase()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pIndexBuffer; }

	void Release();

	void Allocate(IDirect3DIndexBuffer9* pSIndexBuffer);

	IDirect3DIndexBuffer9* GetIndexBuffer() const
		{ DEBUG_VERIFY_ALLOCATION; return m_pIndexBuffer; }

	operator IDirect3DIndexBuffer9* () const
		{ return GetIndexBuffer(); }
};

// -------------
// Index buffer
// -------------
template <class t>
class TIndexBuffer : public TIndexBufferBase
{
private:
	size_t		m_szNIndices;
	D3DFORMAT	m_Format;
	
public:
	typedef t TIndex;


	void Allocate(	size_t	szSNIndices,
					flags_t	flUsage	= D3DUSAGE_WRITEONLY,
					D3DPOOL	Pool	= D3DPOOL_MANAGED);

	void Lock(	t*&					pRData,
				flags_t				flFlags		= D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE,
				const SZSEGMENT*	pSegment	= NULL);

	void Unlock();

	void Install() const
	{
		DEBUG_VERIFY_ALLOCATION;

		g_GraphicsStateManager.SetIndices(GetIndexBuffer());
	}

	// ---------------- TRIVIALS ----------------
	size_t		GetNIndices	() const { return m_szNIndices;	}
	D3DFORMAT	GetFormat	() const { return m_Format;		}
};

template <class t>
void TIndexBuffer<t>::Allocate(	size_t	szSNIndices,
								flags_t	flUsage,
								D3DPOOL	Pool)
{
	Release();

	HRESULT r;

	try
	{		
		DEBUG_VERIFY(szSNIndices);

		m_szNIndices = szSNIndices;

		switch(sizeof(t))
		{
		case sizeof(WORD):
			m_Format = D3DFMT_INDEX16;
			break;

		case sizeof(DWORD):
			m_Format = D3DFMT_INDEX32;
			break;

		default:
			INITIATE_FAILURE;
		}

		IDirect3DIndexBuffer9* pIndexBuffer = NULL;

		if(r = g_GraphicsDeviceGlobals.m_pD3DDevice->CreateIndexBuffer(	m_szNIndices * sizeof(t),
																		flUsage,
																		m_Format,
																		Pool,
																		&pIndexBuffer,
																		NULL))
		{		
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating Direct3D index buffer"), r);
		}

		TIndexBufferBase::Allocate(pIndexBuffer);
	}

	catch(...)
	{
		Release();
		throw;
	}
}

template <class t>
void TIndexBuffer<t>::Lock(	t*&					pRData,
							flags_t				flFlags,
							const SZSEGMENT*	pSegment)
{
	DEBUG_VERIFY_ALLOCATION;

	HRESULT r;

	if(pSegment)
	{
		DEBUG_VERIFY(pSegment->IsValid() && pSegment->m_Last <= m_szNIndices);

		if(r = GetIndexBuffer()->Lock(	pSegment->m_First		* sizeof(t),
										pSegment->GetLength()	* sizeof(t),
										(void**)&pRData,
										flFlags))
		{
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error locking Direct3D index buffer segment"), r);
		}
	}
	else
	{
		if(r = GetIndexBuffer()->Lock(0, 0, (void**)&pRData, flFlags))
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error locking Direct3D index buffer"), r);
	}
}

template <class t>
void TIndexBuffer<t>::Unlock()
{
	DEBUG_VERIFY_ALLOCATION;

	GetIndexBuffer()->Unlock();
}

// --------------------
// Index buffer locker
// --------------------
template <class t>
class TIndexBufferLocker
{
private:
	TIndexBuffer<t>& m_IndexBuffer;

public:
	TIndexBufferLocker(	TIndexBuffer<t>&	SIndexBuffer,
						t*&					pRData,
						flags_t				flFlags		= D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE,
						const SZSEGMENT*	pSegment	= NULL);

	~TIndexBufferLocker();

	// ---------------- TRIVIALS ----------------
	TIndexBuffer<t>& GetIndexBuffer() { return m_IndexBuffer; }
};

template <class t>
TIndexBufferLocker<t>::TIndexBufferLocker(	TIndexBuffer<t>&	SIndexBuffer,
											t*&					pRData,
											flags_t				flFlags,
											const SZSEGMENT*	pSegment) : m_IndexBuffer(SIndexBuffer)
{
	m_IndexBuffer.Lock(pRData, flFlags, pSegment);
}

template <class t>
TIndexBufferLocker<t>::~TIndexBufferLocker()
{
	m_IndexBuffer.Unlock();
}

#endif // index_buffer_h
