#ifndef pixel_shader_h
#define pixel_shader_h

// -------------
// Pixel shader
// -------------
class TPixelShader
{
private:
	IDirect3DPixelShader9* m_pPixelShader;
	
public:
	TPixelShader();

	~TPixelShader()
		{ Release(); }
	
	bool IsAllocated() const
		{ return m_pPixelShader; }

	void Release();

	void Allocate(IDirect3DPixelShader9* pSPixelShader);

	IDirect3DPixelShader9* GetPixelShader() const
		{ return m_pPixelShader; }

	operator IDirect3DPixelShader9* () const
		{ return GetPixelShader(); }

	void Install() const;
};

#endif // pixel_shader_h