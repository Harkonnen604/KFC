#ifndef font_storage_h
#define font_storage_h

#include <KFC_KTL\globals.h>
#include <KFC_Common\storage.h>
#include "font.h"

// Global speed defs
#define FONTS_FACTORY               (g_FontStorage.m_Factory)
#define FONTS_REGISTRATION_MANAGER  (g_FontStorage.m_RegistrationManager)

// -------------
// Font storage
// -------------
class TFontStorage :    public TGlobals,
                        public TStorage<TFont>
{
private:
    TFactoryTypesRegisterer<TFont>  m_SystemFontTypesRegisterer;
    TObjectRegisterer<TFont>        m_DefaultFontRegisterer;


    void OnInitialize   ();
    void OnUninitialize ();

    void LoadByDirectValue( const KString&          FileName,
                            TObjectPointer<TFont>&  RObject,
                            bool                    bOmittable);

public:
    TFontStorage();
};

extern TFontStorage g_FontStorage;

#endif // font_storage_h
