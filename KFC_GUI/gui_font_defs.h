#ifndef gui_font_defs_h
#define gui_font_defs_h

#include "gui_consts.h"

// GUI font types
#define GUI_FONT_TYPE_BASIC     (0)
#define GUI_FONT_TYPE_PLAIN     (1)

// Fixed GUI font indices
#define SYSTEM_GUI_FONTS_START_INDEX        (0)
#define INTERFACE_GUI_FONTS_START_INDEX     (g_GUIConsts.m_szNGUIFontsRegistrationManagerFixedEntries >> 3)
#define CUSTOM_GUI_FONTS_START_INDEX        (g_GUIConsts.m_szNGUIFontsRegistrationManagerFixedEntries >> 1)

// System GUI font indices
#define DEFAULT_GUI_FONT_INDEX  (SYSTEM_GUI_FONTS_START_INDEX + 0)

#endif // gui_font_defs_h
