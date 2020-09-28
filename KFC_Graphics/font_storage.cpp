#include "kfc_graphics_pch.h"
#include "font_storage.h"

#include "graphics_consts.h"
#include "graphics_cfg.h"
#include "graphics_initials.h"
#include "graphics_tokens.h"
#include "graphics_device_globals.h"
#include "2d_fonts.h"
#include "font_defs.h"

TFontStorage g_FontStorage;

// -------------
// Font globals
// -------------
TFontStorage::TFontStorage() : TGlobals(TEXT("Font storage"))
{
    AddSubGlobals(g_GraphicsCfg);
    AddSubGlobals(g_GraphicsInitials);
    AddSubGlobals(g_GraphicsTokens);
    AddSubGlobals(g_GraphicsDeviceGlobals);
}

void TFontStorage::OnUninitialize()
{
    m_DefaultFontRegisterer.Release();

    m_SystemFontTypesRegisterer.Release();

    TStorage<TFont>::Release();
}

void TFontStorage::OnInitialize()
{
    // Storage
    try
    {
        TStorage<TFont>::Allocate(  FONT_TYPE_TOKENS,
                                    g_GraphicsConsts.m_szNFontsRegistrationManagerFixedEntries,
                                    FONT_INDEX_TOKENS);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error allocating fonts storage."));
    }

    // System font types
    try
    {
        m_SystemFontTypesRegisterer.Allocate(FONTS_FACTORY);

        m_SystemFontTypesRegisterer.Add(TTextureFont::          Create, FONT_TYPE_FLAT);
        m_SystemFontTypesRegisterer.Add(TTextureShadowFont::    Create, FONT_TYPE_FLAT_SHADOW);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error registering system font types."));
    }

    // System fonts
    try
    {
        TStructuredInfo Info(FILENAME_TOKENS.
            Process(TEXT("[StartFolder][DefinitionsFolder]System.Definition")));

        TInfoNodeConstIterator InfoNode =
            Info.GetNode(Info.GetRootNode(), TEXT("Fonts"));

        TObjectPointer<TFont> Font;

        // Default
        LoadObject(InfoNode, TEXT("Default"), Font, false);

        m_DefaultFontRegisterer.Allocate(   FONTS_REGISTRATION_MANAGER,
                                            Font,
                                            DEFAULT_FONT_INDEX);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error loading system fonts."));
    }
}

void TFontStorage::LoadByDirectValue(   const KString&          FileName,
                                        TObjectPointer<TFont>&  RObject,
                                        bool                    bOmittable)
{
    TTextureFontCreationStruct CreationStruct;

    if(!CreationStruct.m_Source.SetFileName(FileName))
    {
        if(!bOmittable)
        {
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Invalid direct value font filename \"") +
                                            FileName +
                                            TEXT("\"."));
        }
    }
    else
    {
        ((TTextureFont*)RObject.
            Allocate(new TTextureFont, false))->
                Allocate(CreationStruct);
    }
}
