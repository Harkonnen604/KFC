#ifndef color_defs_h
#define color_defs_h

#include "d3d_inc.h"
#include <KFC_KTL\basic_types.h>

D3DCOLOR inline BlackColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 0, 0, 0); }

D3DCOLOR inline BlueColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 0, 0, 255); }

D3DCOLOR inline GreenColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 0, 255, 0); }

D3DCOLOR inline RedColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 255, 0, 0); }

D3DCOLOR inline CyanColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 0, 255, 255); }

D3DCOLOR inline MagentaColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 255, 0, 255); }

D3DCOLOR inline YellowColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 255, 255, 0); }

D3DCOLOR inline WhiteColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 255, 255, 255); }

D3DCOLOR inline GrayColor(BYTE bAlpha = 255)
	{ return D3DCOLOR_ARGB(bAlpha, 128, 128, 128); }

#endif // color_defs_h
