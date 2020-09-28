#ifndef vertex_buffer_h
#define vertex_buffer_h

// #include "graphics_device_globals.h" // cross-inclusion
// #include "graphics_state_manager.h" // cross-inclusion

// -------------------
// Vertex buffer base
// -------------------
class TVertexBufferBase
{
private:
    IDirect3DVertexBuffer9* m_pVertexBuffer;

public:
    TVertexBufferBase();

    ~TVertexBufferBase()
        { Release(); }

    bool IsAllocated() const
        { return m_pVertexBuffer; }

    void Release();

    void Allocate(IDirect3DVertexBuffer9* pSVertexBuffer);

    IDirect3DVertexBuffer9* GetVertexBuffer() const
        { DEBUG_VERIFY_ALLOCATION; return m_pVertexBuffer; }

    operator IDirect3DVertexBuffer9* () const
        { return GetVertexBuffer(); }
};

// ------------------
// FVF vertex buffer
// ------------------
template <class t>
class TFVFVertexBuffer : public TVertexBufferBase
{
private:
    size_t m_szNVertices;

public:
    typedef t TVertex;


    void Allocate(  size_t  szSNVertices,
                    flags_t flUsage = D3DUSAGE_WRITEONLY,
                    D3DPOOL Pool    = D3DPOOL_MANAGED);

    void Lock(  t*&                 pRData,
                flags_t             flFlags     = 0,
                const SZSEGMENT*    pSegment    = NULL);

    void Unlock();

    void Install() const
    {
        DEBUG_VERIFY_ALLOCATION;

        g_GraphicsStateManager.SetStreamSource(0, GetVertexBuffer(), 0, sizeof(t));

        g_GraphicsStateManager.SetVertexShader(NULL);

        g_GraphicsStateManager.SetFVF(t::s_flFVF);
    }

    // ---------------- TRIVIALS ----------------
    size_t GetNVertices() const { return m_szNVertices; }
};

template <class t>
void TFVFVertexBuffer<t>::Allocate( size_t  szSNVertices,
                                    flags_t flUsage,
                                    D3DPOOL Pool)
{
    Release();

    try
    {
        HRESULT r;

        DEBUG_VERIFY(szSNVertices);

        m_szNVertices = szSNVertices;

        IDirect3DVertexBuffer9* pVertexBuffer = NULL;

        if(r = g_GraphicsDeviceGlobals.m_pD3DDevice->CreateVertexBuffer(m_szNVertices * sizeof(t),
                                                                        flUsage,
                                                                        t::s_flFVF,
                                                                        Pool,
                                                                        &pVertexBuffer,
                                                                        NULL))
        {
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating Direct3D FVF vertex buffer"), r);
        }

        TVertexBufferBase::Allocate(pVertexBuffer);
    }

    catch(...)
    {
        Release();
        throw;
    }
}

template <class t>
void TFVFVertexBuffer<t>::Lock( t*&                 pRData,
                                flags_t             flFlags,
                                const SZSEGMENT*    pSegment)
{
    DEBUG_VERIFY_ALLOCATION;

    HRESULT r;

    if(pSegment)
    {
        DEBUG_VERIFY(pSegment->IsValid() && pSegment->m_Last <= m_szNVertices);

        if(r = GetVertexBuffer()->Lock( pSegment->m_First       * sizeof(t),
                                        pSegment->GetLength()   * sizeof(t),
                                        (void**)&pRData,
                                        flFlags))
        {
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error locking Direct3D FVF vertex buffer segment"), r);
        }
    }
    else
    {
        if(r = GetVertexBuffer()->Lock(0, 0, (void**)&pRData, flFlags))
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error locking Direct3D FVF vertex buffer"), r);
    }
}

template <class t>
void TFVFVertexBuffer<t>::Unlock()
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_EVALUATE_VERIFY(!GetVertexBuffer()->Unlock());
}

// -------------------------
// FVF vertex buffer locker
// -------------------------
template <class t>
class TFVFVertexBufferLocker
{
private:
    TFVFVertexBuffer<t>& m_VertexBuffer;

public:
    TFVFVertexBufferLocker( TFVFVertexBuffer<t>&    SVertexBuffer,
                            t*&                     pRData,
                            flags_t                 flFlags     = 0,
                            const SZSEGMENT*        pSegment    = NULL);

    ~TFVFVertexBufferLocker();

    // ---------------- TRIVIALS ----------------
    TFVFVertexBuffer& GetVertexBuffer() { return m_VertexBuffer; }
};

template <class t>
TFVFVertexBufferLocker<t>::TFVFVertexBufferLocker(  TFVFVertexBuffer<t>&    SVertexBuffer,
                                                    t*&                     pRData,
                                                    flags_t                 flFlags,
                                                    const SZSEGMENT*        pSegment) : m_VertexBuffer(SVertexBuffer)
{
    m_VertexBuffer.Lock(pRData, flFlags, pSegment);
}

template <class t>
TFVFVertexBufferLocker<t>::~TFVFVertexBufferLocker()
{
    m_VertexBuffer.Unlock();
}

#endif // vertex_buffer_h
