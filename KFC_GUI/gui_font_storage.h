#ifndef gui_font_storage_h
#define gui_font_storage_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\storage.h>

#include "gui_font.h"

#define GUI_FONTS_FACTORY               (g_GUI_FontStorage.m_Factory)
#define GUI_FONTS_REGISTRATION_MANAGER  (g_GUI_FontStorage.m_RegistrationManager)

// -----------------
// GUI font storage
// -----------------
class T_GUI_FontStorage :   public TGlobals,
                            public TStorage<T_GUI_Font>
{
private:
    TFactoryTypesRegisterer<T_GUI_Font> m_SystemGUIFontTypesRegisterer;

    TObjectRegisterer<T_GUI_Font> m_DefaultGUIFontRegisterer;

private:
    void OnInitialize   ();
    void OnUninitialize ();

    void LoadByDirectValue( const KString&              FileName,
                            TObjectPointer<T_GUI_Font>& RObject,
                            bool                        bOmittable);

public:
    T_GUI_FontStorage();
};

extern T_GUI_FontStorage g_GUI_FontStorage;

#endif // gui_font_storage_h
