#ifndef sound_defs_h
#define sound_defs_h

#include <KFC_KTL\object_defs.h>
#include "sound_consts.h"

// Sound types
#define SOUND_TYPE_PLAIN    (1)

// Fixed sound indices
#define SYSTEM_SOUNDS_START_INDEX       (0)
#define INTERFACE_SOUNDS_START_INDEX    (g_SoundConsts.m_szNSoundsRegistrationManagerFixedEntries >> 3)
#define CUSTOM_SOUNDS_START_INDEX       (g_SoundConsts.m_szNSoundsRegistrationManagerFixedEntries >> 1)

#endif // sound_defs_h
