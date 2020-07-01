#ifndef graphics_tokens_h
#define graphics_tokens_h

#include <KFC_KTL\globals.h>
#include <KFC_KTL\token.h>

#define COLOR_TOKENS			(g_GraphicsTokens.m_ColorTokens)
#define SPRITE_TYPE_TOKENS		(g_GraphicsTokens.m_SpriteTypeTokens)
#define SPRITE_INDEX_TOKENS		(g_GraphicsTokens.m_SpriteIndexTokens)
#define FONT_TYPE_TOKENS		(g_GraphicsTokens.m_FontTypeTokens)
#define FONT_INDEX_TOKENS		(g_GraphicsTokens.m_FontIndexTokens)

// ----------------
// Graphics tokens
// ----------------
class TGraphicsTokens : public TGlobals
{
private:
	TTokensRegisterer m_FileNameTokensRegisterer;
	TTokensRegisterer m_ColorTokensRegisterer;
	TTokensRegisterer m_FontTypeTokensRegisterer;
	TTokensRegisterer m_FontIndexTokensRegisterer;
	TTokensRegisterer m_SpriteTypeTokensRegisterer;
	TTokensRegisterer m_SpriteIndexTokensRegisterer;


	void OnUninitialize	();
	void OnInitialize	();

public:
	TTokens m_ColorTokens;
	TTokens m_FontTypeTokens;
	TTokens m_FontIndexTokens;
	TTokens m_SpriteTypeTokens;
	TTokens m_SpriteIndexTokens;
	
		
	TGraphicsTokens();
};

extern TGraphicsTokens g_GraphicsTokens;

#endif // graphics_tokens_h
