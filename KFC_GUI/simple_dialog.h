#ifndef simple_dialog_h
#define simple_dialog_h

#include <KFC_Common/module.h>
#include "dialog.h"

// --------------
// Simple dialog
// --------------
class TSimpleDialog : public TDialog
{
private:
    const HINSTANCE m_hInstance;
    const int       m_iTemplateID;


    HINSTANCE GetInstance() const;

    int GetTemplateID() const;

public:
    TSimpleDialog(HINSTANCE hSInstance, int iSTemplateID, bool bInputDialog) :
        TDialog(bInputDialog),
        m_hInstance(hSInstance), m_iTemplateID(iSTemplateID) {}

    TSimpleDialog(int iSTemplateID, bool bInputDialog) :
        TDialog(bInputDialog),
        m_hInstance(GetKModuleHandle()), m_iTemplateID(iSTemplateID) {}
};

#endif // simple_dialog_h
