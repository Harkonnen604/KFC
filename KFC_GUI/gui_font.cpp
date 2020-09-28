#include "kfc_gui_pch.h"
#include "gui_font.h"

#include "gui_tokens.h"
#include "gui.h"

// -------------------------
// GUI font creation struct
// -------------------------
T_GUI_FontCreationStruct::T_GUI_FontCreationStruct()
{
    memset(&m_LogFont, 0, sizeof(m_LogFont));

    // Fonts
    m_LogFont.lfHeight          = 0;
    m_LogFont.lfWidth           = 0;
    m_LogFont.lfEscapement      = 0;
    m_LogFont.lfOrientation     = 0;
    m_LogFont.lfWeight          = FW_DONTCARE;
    m_LogFont.lfItalic          = FALSE;
    m_LogFont.lfUnderline       = FALSE;
    m_LogFont.lfStrikeOut       = FALSE;
    m_LogFont.lfCharSet         = ANSI_CHARSET;
    m_LogFont.lfOutPrecision    = OUT_DEFAULT_PRECIS;
    m_LogFont.lfClipPrecision   = CLIP_DEFAULT_PRECIS;
    m_LogFont.lfQuality         = DEFAULT_QUALITY;
    m_LogFont.lfPitchAndFamily  = DEFAULT_PITCH;
    m_LogFont.lfFaceName[0]     = 0;
}

void T_GUI_FontCreationStruct::Load(TInfoNodeConstIterator InfoNode)
{
    TInfoParameterConstIterator PIter;

    int     iValue;
    bool    bValue;

    // Getting face name
    if((PIter = InfoNode->FindParameter(TEXT("FaceName"))).IsValid())
    {
        if(!SetFaceName(PIter->m_Value))
        {
            INITIATE_DEFINED_FAILURE((KString)TEXT("Invalid GUI font facename: \"") + PIter->m_Value + TEXT("\"."));
        }
    }

    // Getting height
    if((PIter = InfoNode->FindParameter(TEXT("Height"))).IsValid())
    {
        ReadInt(PIter->m_Value,
                iValue,
                TEXT("GUI font height"));

        m_LogFont.lfHeight = iValue;
    }

    // Getting width
    if((PIter = InfoNode->FindParameter(TEXT("Width"))).IsValid())
    {
        ReadNormalizedInt(  PIter->m_Value,
                            iValue,
                            TEXT("GUI font width"),
                            ISEGMENT(0, INT_MAX));

        m_LogFont.lfWidth = iValue;
    }

    // Getting escapement
    if((PIter = InfoNode->FindParameter(TEXT("Escapement"))).IsValid())
    {
        ReadNormalizedInt(  PIter->m_Value,
                            iValue,
                            TEXT("GUI font escapement"),
                            ISEGMENT(0, 3599));

        m_LogFont.lfEscapement = iValue;
    }

    // Getting orientation
    if((PIter = InfoNode->FindParameter(TEXT("Orientation"))).IsValid())
    {
        ReadNormalizedInt(  PIter->m_Value,
                            iValue,
                            TEXT("GUI font orientation"),
                            ISEGMENT(0, 3599));

        m_LogFont.lfWidth = iValue;
    }

    // Getting weight
    if((PIter = InfoNode->FindParameter(TEXT("Weight"))).IsValid())
    {
        ReadNormalizedInt(  GUI_FONT_WEIGHT_TOKENS.Process(PIter->m_Value),
                            iValue,
                            TEXT("GUI font weight"),
                            ISEGMENT(0, 1000));

        m_LogFont.lfWeight = iValue;
    }

    // Getting italic property
    if((PIter = InfoNode->FindParameter(TEXT("Italic"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    bValue,
                    TEXT("GUI font italic property"));

        m_LogFont.lfItalic = bValue;
    }

    // Getting underline property
    if((PIter = InfoNode->FindParameter(TEXT("Underline"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    bValue,
                    TEXT("GUI font underline property"));

        m_LogFont.lfUnderline = bValue;
    }

    // Getting strike-out property
    if((PIter = InfoNode->FindParameter(TEXT("StrikeOut"))).IsValid())
    {
        ReadBool(   PIter->m_Value,
                    bValue,
                    TEXT("GUI font strike-out property"));

        m_LogFont.lfStrikeOut = bValue;
    }

    // Getting out precision
    if((PIter = InfoNode->FindParameter(TEXT("OutPrecision"))).IsValid())
    {
        DEBUG_EVALUATE_VERIFY(FromString(   GUI_FONT_OUT_PRECISION_TOKENS.
                                                ProcessEnumValue(   PIter->m_Value,
                                                                    TEXT("GUI font out precision")),
                                            iValue));

        m_LogFont.lfOutPrecision = (BYTE)iValue;
    }

    // Getting clip precision
    if((PIter = InfoNode->FindParameter(TEXT("ClipPrecision"))).IsValid())
    {
        DEBUG_EVALUATE_VERIFY(FromString(   GUI_FONT_CLIP_PRECISION_TOKENS.
                                                ProcessEnumValue(   PIter->m_Value,
                                                                    TEXT("GUI font clip precision")),
                                            iValue));

        m_LogFont.lfClipPrecision = (BYTE)iValue;
    }

    // Getting quality
    if((PIter = InfoNode->FindParameter(TEXT("Quality"))).IsValid())
    {
        DEBUG_EVALUATE_VERIFY(FromString(   GUI_FONT_QUALITY_TOKENS.
                                                ProcessEnumValue(   PIter->m_Value,
                                                                    TEXT("GUI font quality")),
                                            iValue));

        m_LogFont.lfQuality = (BYTE)iValue;
    }

    // Getting pitch
    int iPitch = DEFAULT_PITCH;
    if((PIter = InfoNode->FindParameter(TEXT("Pitch"))).IsValid())
    {
        DEBUG_EVALUATE_VERIFY(FromString(   GUI_FONT_PITCH_TOKENS.
                                                ProcessEnumValue(   PIter->m_Value,
                                                                    TEXT("GUI font pitch")),
                                            iPitch));
    }

    // Getting family
    int iFamily = FF_DONTCARE;
    if((PIter = InfoNode->FindParameter(TEXT("Family"))).IsValid())
    {
        DEBUG_EVALUATE_VERIFY(FromString(   GUI_FONT_FAMILY_TOKENS.
                                                ProcessEnumValue(   PIter->m_Value,
                                                                    TEXT("GUI font family")),
                                            iFamily));
    }

    // Setting pitch and family
    m_LogFont.lfPitchAndFamily = iPitch | iFamily;
}

bool T_GUI_FontCreationStruct::SetFaceName(const KString& FaceName)
{
    if(FaceName.GetLength() < 32)
    {
        _tcscpy(m_LogFont.lfFaceName, FaceName);

        return true;
    }

    return false;
}

// ---------
// GUI font
// ---------
T_GUI_Font* T_GUI_Font::Create(ktype_t tpType)
{
    DEBUG_VERIFY(tpType == GUI_FONT_TYPE_PLAIN);

    return new T_GUI_Font;
}

T_GUI_Font::T_GUI_Font()
{
    m_bAllocated = false;

    m_hFont = NULL;
}

T_GUI_Font::T_GUI_Font(const T_GUI_FontCreationStruct& CreationStruct)
{
    m_bAllocated = false;

    m_hFont = NULL;

    Allocate(CreationStruct);
}

void T_GUI_Font::Release(bool bFromAllocatorException)
{
    if(m_bAllocated || bFromAllocatorException)
    {
        m_bAllocated = false;

        if(m_hFont)
            DeleteObject(m_hFont), m_hFont = NULL;
    }
}

void T_GUI_Font::Allocate(const T_GUI_FontCreationStruct& CreationStruct)
{
    Release();

    try
    {
        m_hFont = CreateFontIndirect(&CreationStruct.m_LogFont);

        if(m_hFont == NULL)
            INITIATE_DEFINED_CODE_FAILURE(TEXT("Error creating GUI font"), GetLastError());

        m_bAllocated = true;
    }

    catch(...)
    {
        Release(true);
        throw;
    }
}

void T_GUI_Font::Load(TInfoNodeConstIterator InfoNode)
{
    Release();

    DEBUG_VERIFY(InfoNode.IsValid());

    T_GUI_FontCreationStruct CreationStruct;
    CreationStruct.Load(InfoNode);

    Allocate(CreationStruct);
}

// Sizes
void T_GUI_Font::GetCharSize(TCHAR cChar, SZSIZE& RSize) const
{
    DEBUG_VERIFY_ALLOCATION;

    TDC DC((HWND)NULL);

    TGDIObjectSelector Selector0(DC, m_hFont);

    GetTextExtentPoint32(DC, &cChar, 1, RSize);
}

void T_GUI_Font::GetTextSize(const KString& Text, SZSIZE& RSize) const
{
    DEBUG_VERIFY_ALLOCATION;

    TDC DC((HWND)NULL);

    TGDIObjectSelector Selctor0(DC, m_hFont);

    GetTextExtentPoint32(DC, Text, Text.GetLength(), RSize);
}
