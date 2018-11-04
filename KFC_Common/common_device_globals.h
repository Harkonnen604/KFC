#ifndef common_device_globals_h
#define common_device_globals_h

#include <KFC_KTL/globals.h>
#include <KFC_KTL/event.h>
#include "msg_box.h"
#include "com.h"

// ----------------------
// Common device globals
// ----------------------
class TCommonDeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	#ifdef _MSC_VER
		T_OLE_Initializer m_OLE_Initializer;
		T_COM_Initializer m_COM_Initializer;
	#endif // _MSC_VER

	TEvent m_MsgBoxesEvent;

public:
	TCommonDeviceGlobals();
};

extern TCommonDeviceGlobals g_CommonDeviceGlobals;

#endif // common_device_globals_h
