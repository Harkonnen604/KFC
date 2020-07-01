#ifndef vertex_shader_h
#define vertex_shader_h

// --------------
// Vertex shader
// --------------
class TVertexShader
{
private:
	IDirect3DVertexShader9* m_pVertexShader;
	
public:
	TVertexShader();

	~TVertexShader()
		{ Release(); }
	
	bool IsAllocated() const
		{ return m_pVertexShader; }

	void Release();

	void Allocate(IDirect3DVertexShader9* pSVertexShader);

	IDirect3DVertexShader9* GetVertexShader() const
		{ return m_pVertexShader; }

	operator IDirect3DVertexShader9* () const
		{ return GetVertexShader(); }

	void Install() const;
};

#endif // vertex_shader_h