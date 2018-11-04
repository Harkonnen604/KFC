#ifndef gui_tokens_h
#define gui_tokens_h

#include <KFC_KTL\globals.h>
#include <KFC_KTL\tokens.h>

#define GUI_FONT_WEIGHT_TOKENS			(g_GUI_Tokens.m_GUIFontWeightTokens)
#define GUI_FONT_CHARSET_TOKENS			(g_GUI_Tokens.m_GUIFontCharSetTokens)
#define GUI_FONT_OUT_PRECISION_TOKENS	(g_GUI_Tokens.m_GUIFontOutPrecisionTokens)
#define GUI_FONT_CLIP_PRECISION_TOKENS	(g_GUI_Tokens.m_GUIFontClipPrecisionTokens)
#define GUI_FONT_QUALITY_TOKENS			(g_GUI_Tokens.m_GUIFontQualityTokens)
#define GUI_FONT_PITCH_TOKENS			(g_GUI_Tokens.m_GUIFontPitchTokens)
#define GUI_FONT_FAMILY_TOKENS			(g_GUI_Tokens.m_GUIFontFamilyTokens)
#define GUI_FONT_TYPE_TOKENS			(g_GUI_Tokens.m_GUIFontTypeTokens)
#define GUI_FONT_INDEX_TOKENS			(g_GUI_Tokens.m_GUIFontIndexTokens)

// -----------
// GUI tokens
// -----------
class T_GUI_Tokens : public TGlobals
{
private:
	TTokensRegisterer m_GUIFontWeightTokensRegisterer;
	TTokensRegisterer m_GUIFontCharSetTokensRegisterer;
	TTokensRegisterer m_GUIFontOutPrecisionTokensRegisterer;
	TTokensRegisterer m_GUIFontClipPrecisionTokensRegisterer;
	TTokensRegisterer m_GUIFontQualityTokensRegisterer;
	TTokensRegisterer m_GUIFontPitchTokensRegisterer;
	TTokensRegisterer m_GUIFontFamilyTokensRegisterer;
	TTokensRegisterer m_GUIFontTypeTokensRegisterer;
	TTokensRegisterer m_GUIFontIndexTokensRegisterer;

private:
	void OnInitialize	();
	void OnUninitialize	();

public:
	TTokens m_GUIFontWeightTokens;
	TTokens m_GUIFontCharSetTokens;
	TTokens m_GUIFontOutPrecisionTokens;
	TTokens m_GUIFontClipPrecisionTokens;
	TTokens m_GUIFontQualityTokens;
	TTokens m_GUIFontPitchTokens;
	TTokens m_GUIFontFamilyTokens;
	TTokens m_GUIFontTypeTokens;
	TTokens m_GUIFontIndexTokens;

public:
	T_GUI_Tokens();
};

extern T_GUI_Tokens g_GUI_Tokens;

#endif // gui_tokens_h
