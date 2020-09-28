#include "kfc_gui_pch.h"
#include "gui_tokens.h"

#include "gui_cfg.h"
#include "gui_initials.h"
#include "gui_font_defs.h"

T_GUI_Tokens g_GUI_Tokens;

// -----------
// GUI tokens
// -----------
T_GUI_Tokens::T_GUI_Tokens() : TGlobals(TEXT("GUI tokens"))
{
    AddSubGlobals(g_GUI_Cfg);
    AddSubGlobals(g_GUI_Initials);
}

void T_GUI_Tokens::OnUninitialize()
{
    m_GUIFontIndexTokensRegisterer.         Release();
    m_GUIFontTypeTokensRegisterer.          Release();
    m_GUIFontFamilyTokensRegisterer.        Release();
    m_GUIFontPitchTokensRegisterer.         Release();
    m_GUIFontQualityTokensRegisterer.       Release();
    m_GUIFontClipPrecisionTokensRegisterer. Release();
    m_GUIFontOutPrecisionTokensRegisterer.  Release();
    m_GUIFontCharSetTokensRegisterer.       Release();
    m_GUIFontWeightTokensRegisterer.        Release();

    m_GUIFontIndexTokens.           Clear();
    m_GUIFontTypeTokens.            Clear();
    m_GUIFontFamilyTokens.          Clear();
    m_GUIFontPitchTokens.           Clear();
    m_GUIFontQualityTokens.         Clear();
    m_GUIFontClipPrecisionTokens.   Clear();
    m_GUIFontOutPrecisionTokens.    Clear();
    m_GUIFontCharSetTokens.         Clear();
    m_GUIFontWeightTokens.          Clear();
}

void T_GUI_Tokens::OnInitialize()
{
    // --- GUI font weight tokens ---
    m_GUIFontWeightTokensRegisterer.Allocate(GUI_FONT_WEIGHT_TOKENS);

    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Min]"),          1);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Default]"),      FW_DONTCARE);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Thin]"),         FW_THIN);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[ExtraLight]"),   FW_EXTRALIGHT);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Light]"),        FW_LIGHT);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Normal]"),       FW_NORMAL);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Regular]"),      FW_REGULAR);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Medium]"),       FW_MEDIUM);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[SemiBold]"),     FW_SEMIBOLD);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Bold]"),         FW_BOLD);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[ExtraBold]"),    FW_BOLD);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Heavy]"),        FW_HEAVY);
    m_GUIFontWeightTokensRegisterer.Add(TEXT("[Max]"),          1000);

    // --- GUI font charset tokens ---
    m_GUIFontCharSetTokensRegisterer.Allocate(GUI_FONT_CHARSET_TOKENS);

    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Default]"),     DEFAULT_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Ansi]"),        ANSI_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Baltic]"),      BALTIC_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[ChineseBig5]"), CHINESEBIG5_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[EastEurope]"),  EASTEUROPE_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[GB2312]"),      GB2312_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Greek]"),       GREEK_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Hangul]"),      HANGUL_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Mac]"),         MAC_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[OEM]"),         OEM_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Russian]"),     RUSSIAN_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Shiftjis]"),    SHIFTJIS_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Symbol]"),      SYMBOL_CHARSET);
    m_GUIFontCharSetTokensRegisterer.Add(TEXT("[Turkish]"),     TURKISH_CHARSET);

    // --- GUI font out precision tokens ---
    m_GUIFontOutPrecisionTokensRegisterer.Allocate(GUI_FONT_OUT_PRECISION_TOKENS);

    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("Default"),      OUT_DEFAULT_PRECIS);
    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("Character"),    OUT_CHARACTER_PRECIS);
    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("Device"),       OUT_DEVICE_PRECIS);
    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("Raster"),       OUT_RASTER_PRECIS);
    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("String"),       OUT_STRING_PRECIS);
    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("TrueTypeOnly"), OUT_TT_ONLY_PRECIS);
    m_GUIFontOutPrecisionTokensRegisterer.Add(TEXT("TrueType"),     OUT_TT_PRECIS);

    // --- GUI font clip precision tokens ---
    m_GUIFontClipPrecisionTokensRegisterer.Allocate(GUI_FONT_CLIP_PRECISION_TOKENS);

    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("Default"),             CLIP_DEFAULT_PRECIS);
    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("Character"),           CLIP_CHARACTER_PRECIS);
    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("Stroke"),              CLIP_STROKE_PRECIS);
    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("Mask"),                CLIP_MASK);
    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("Embedded"),            CLIP_EMBEDDED);
    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("LeftHandedAngles"),    CLIP_LH_ANGLES);
    m_GUIFontClipPrecisionTokensRegisterer.Add(TEXT("TrueTypeAlways"),      CLIP_TT_ALWAYS);

    // --- GUI font quality tokens ---
    m_GUIFontQualityTokensRegisterer.Allocate(GUI_FONT_QUALITY_TOKENS);

    m_GUIFontQualityTokensRegisterer.Add(TEXT("Default"),           DEFAULT_QUALITY);
    m_GUIFontQualityTokensRegisterer.Add(TEXT("AntiAliased"),       ANTIALIASED_QUALITY);
    m_GUIFontQualityTokensRegisterer.Add(TEXT("Draft"),             DRAFT_QUALITY);
    m_GUIFontQualityTokensRegisterer.Add(TEXT("NonAntiAliased"),    NONANTIALIASED_QUALITY);
    m_GUIFontQualityTokensRegisterer.Add(TEXT("Proof"),             PROOF_QUALITY);

    // --- GUI font pitch tokens ---
    m_GUIFontPitchTokensRegisterer.Allocate(GUI_FONT_PITCH_TOKENS);

    m_GUIFontPitchTokensRegisterer.Add(TEXT("Default"),     DEFAULT_PITCH);
    m_GUIFontPitchTokensRegisterer.Add(TEXT("Fixed"),       FIXED_PITCH);
    m_GUIFontPitchTokensRegisterer.Add(TEXT("Variable"),    VARIABLE_PITCH);

    // --- GUI font family tokens ---
    m_GUIFontFamilyTokensRegisterer.Allocate(GUI_FONT_FAMILY_TOKENS);

    m_GUIFontFamilyTokensRegisterer.Add(TEXT("Default"),    FF_DONTCARE);
    m_GUIFontFamilyTokensRegisterer.Add(TEXT("Decorative"), FF_DECORATIVE);
    m_GUIFontFamilyTokensRegisterer.Add(TEXT("Modern"),     FF_MODERN);
    m_GUIFontFamilyTokensRegisterer.Add(TEXT("Roman"),      FF_ROMAN);
    m_GUIFontFamilyTokensRegisterer.Add(TEXT("Script"),     FF_SCRIPT);
    m_GUIFontFamilyTokensRegisterer.Add(TEXT("Swiss"),      FF_SWISS);

    // --- GUI font type tokens ---
    m_GUIFontTypeTokensRegisterer.Allocate(GUI_FONT_TYPE_TOKENS);

    m_GUIFontTypeTokensRegisterer.Add(TEXT("[Basic]"), GUI_FONT_TYPE_BASIC);
    m_GUIFontTypeTokensRegisterer.Add(TEXT("[Plain]"), GUI_FONT_TYPE_PLAIN);

    // --- GUI font index tokens ---
    m_GUIFontIndexTokensRegisterer.Allocate(GUI_FONT_INDEX_TOKENS);

    m_GUIFontIndexTokensRegisterer.Add(TEXT("[Default]"), DEFAULT_GUI_FONT_INDEX);
}
