#ifndef graphics_consts_h
#define graphics_consts_h

#include <KFC_KTL\consts.h>
#include <KFC_KTL\basic_types.h>
#include <KFC_KTL\basic_bitypes.h>
#include <KFC_KTL\kstring.h>
#include <KFC_KTL\array.h>

// ----------------
// Graphics consts
// ----------------
class TGraphicsConsts : public TConsts
{
public:
	// Filenames
	KString m_SpritesFolderName;
	KString m_FontsFolderName;
	KString m_ScreenShotsFolderName;
	KString m_ScreenShotFileNamePrefix;
	KString m_ScreenShotFileNamePostfix;

	// Graphics
	size_t		m_szMaxGraphicsStripRects;
	FSEGMENT	m_DefaultCameraZClipPlanes;
	float		m_fDefaultCameraFOV;

	// Graphics effects
	size_t m_szMaxGraphicsEffectVertices;

	// Storages
	size_t m_szNFontsRegistrationManagerFixedEntries;
	size_t m_szNSpritesRegistrationManagerFixedEntries;


	TGraphicsConsts();
};

extern TGraphicsConsts g_GraphicsConsts;

#endif // graphics_consts_h
