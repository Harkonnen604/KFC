#ifndef graphics_effect_globals_h
#define graphics_effect_globals_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\globals.h>
#include "vertex_formats.h"
#include "vertex_buffer.h"

// ------------------------------
// Effect vertex buffer typedefs
// ------------------------------
typedef TFVFVertexBuffer		<T2DEffectVertex> TEffectVertexBuffer;
typedef TFVFVertexBufferLocker	<T2DEffectVertex> TEffectVertexBufferLocker;

// ------------------------
// Graphics effect globals
// ------------------------
class TGraphicsEffectGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	// Effects vertex buffer
	mutable TEffectVertexBuffer m_EffectVertexBuffer;


	TGraphicsEffectGlobals();
};

extern TGraphicsEffectGlobals g_GraphicsEffectGlobals;

#endif // graphics_effect_globals_h
