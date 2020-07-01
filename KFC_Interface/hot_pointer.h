#ifndef hot_pointer_h
#define hot_pointer_h

#include <limits.h>
#include <KFC_KTL\kstring.h>

// ----------------------
// Hot pointer direction
// ----------------------
enum THotPointerDirection
{
	HPD_NONE		= -1,
	HPD_UP			= 0,
	HPD_RIGHT		= 1,
	HPD_DOWN		= 2,
	HPD_LEFT		= 3,
	HPD_FORCE_UINT	= UINT_MAX
};

// ----------------
// Global routines
// ----------------
bool FromString(const KString&			String,
				THotPointerDirection&	RDirection);

void ReadHotPointerDirection(	const KString&			String,
								THotPointerDirection&	RDirection,
								LPCTSTR					pValueName);

#endif // hot_pointer_h
