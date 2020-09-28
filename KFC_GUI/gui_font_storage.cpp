#include "kfc_gui_pch.h"
#include "gui_font_storage.h"
#include "gui_consts.h"
#include "gui_cfg.h"
#include "gui_initials.h"
#include "gui_tokens.h"
#include "gui_device_globals.h"

T_GUI_FontStorage g_GUI_FontStorage;

// -----------------
// GUI font storage
// -----------------
T_GUI_FontStorage::T_GUI_FontStorage() : TGlobals(TEXT("GUI font storage"))
{
    AddSubGlobals(g_GUI_Cfg);
    AddSubGlobals(g_GUI_Initials);
    AddSubGlobals(g_GUI_Tokens);
    AddSubGlobals(g_GUI_DeviceGlobals);
}

void T_GUI_FontStorage::OnUninitialize()
{
    m_DefaultGUIFontRegisterer.Release();

    m_SystemGUIFontTypesRegisterer.Release();

    TStorage<T_GUI_Font>::Release();
}

void T_GUI_FontStorage::OnInitialize()
{
    if(!g_GUI_Consts.m_bInitializeGUIFontStorage)
        return;

    // Storage
    try
    {
        TStorage<T_GUI_Font>::Allocate( GUI_FONT_TYPE_TOKENS,
                                        g_GUI_Consts.m_szNGUIFontsRegistrationManagerFixedEntries,
                                        GUI_FONT_INDEX_TOKENS);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error allocating GUI fonts storage."));
    }

    // System GUI font types
    try
    {
        m_SystemGUIFontTypesRegisterer.Allocate(GUI_FONTS_FACTORY);

        m_SystemGUIFontTypesRegisterer.Add(T_GUI_Font::Create, GUI_FONT_TYPE_PLAIN);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error registering system GUI font types."));
    }

    // System GUI fonts
    try
    {
        TStructuredInfo Info(FILENAME_TOKENS.
            Process(TEXT("[StartFolder][DefinitionsFolder]System.Definition")));

        TInfoNodeConstIterator InfoNode =
            Info.GetNode(Info.GetRootNode(), TEXT("GUIFonts"));

        TObjectPointer<T_GUI_Font> GUIFont;

        // Default
        LoadObject( InfoNode,
                    TEXT("Default"),
                    GUIFont,
                    false);

        m_DefaultGUIFontRegisterer.Allocate(GUI_FONTS_REGISTRATION_MANAGER,
                                            GUIFont,
                                            DEFAULT_GUI_FONT_INDEX);
    }

    catch(...)
    {
        INITIATE_DEFINED_FAILURE(TEXT("Error loading system GUIFonts."));
    }
}

void T_GUI_FontStorage::LoadByDirectValue(  const KString&              FileName,
                                            TObjectPointer<T_GUI_Font>& RObject,
                                            bool                        bOmittable)
{
    T_GUI_FontCreationStruct CreationStruct;

    if(!CreationStruct.SetFaceName(FileName))
    {
        if(!bOmittable)
        {
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Invalid GUI font face name: \"") +
                                            FileName +
                                            TEXT("\"."));
        }
    }
    else
    {
        ((T_GUI_Font*)RObject.
            Allocate(new T_GUI_Font, false))->
                Allocate(CreationStruct);
    }
}
