#ifndef sprite_defs_h
#define sprite_defs_h

#include <KFC_KTL\object_defs.h>
#include "graphics_consts.h"

// Sprite types
#define SPRITE_TYPE_FLAT			(1)
#define SPRITE_TYPE_TRANSITION		(2)
#define SPRITE_TYPE_COMPOSITE		(3)
#define SPRITE_TYPE_BORDER			(4)
#define SPRITE_TYPE_RECT_EFFECT		(5)
#define SPRITE_TYPE_TEXT			(6)
#define SPRITE_TYPE_DUMMY			(7)
#define SPRITE_TYPE_STATE_SKIPPER	(8)

// Fixed sprite indices
#define SYSTEM_SPRITES_START_INDEX		(0)
#define INTERFACE_SPRITES_START_INDEX	(g_GraphicsConsts.m_szNSpritesRegistrationManagerFixedEntries >> 3)
#define CUSTOM_SPRITES_START_INDEX		(g_GraphicsConsts.m_szNSpritesRegistrationManagerFixedEntries >> 1)

#endif // sprite_defs_h
