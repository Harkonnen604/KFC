#ifndef sprite_helpers_h
#define sprite_helpers_h

class TSprite;

// ----------------
// Global routines
// ----------------
KString GetSpriteText(	const TSprite*	pSprite,
						bool*			pRSuccess	= NULL,
						bool			bRecursive	= true);

bool SetSpriteText(	TSprite*		pSprite,
					const KString&	Text,
					bool			bRecursive = true);

#endif // sprite_helpers_h