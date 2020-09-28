#ifndef kfc_gui_pch_h
#define kfc_gui_pch_h

#include <KFC_KTL\kfc_ktl_pch.h>
#include <KFC_Common\kfc_common_pch.h>

#include <KFC_Common\module.h>

#include <commctrl.h>
#include <prsht.h>

#define GET_LPARAM_COORDS(lParam) \
    (IPOINT((int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam)))

#endif // kfc_gui_pch_h
