#ifndef gui_device_globals_h
#define gui_device_globals_h

#include <KFC_KTL\globals.h>
#include "gui.h"
#include "icon.h"

// -------------------
// GUI device globals
// -------------------
class T_GUI_DeviceGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	TIcon m_CheckIcons[2][2];

	TBrush m_CheckerBrush;

public:
	T_GUI_DeviceGlobals();
};

extern T_GUI_DeviceGlobals g_GUI_DeviceGlobals;

#endif // gui_device_globals_h
