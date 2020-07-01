#ifndef _2d_effects_h
#define _2d_effects_h

#include "color_defs.h"

// ------------
// Rect effect
// ------------
class TRectEffect
{	
public:
	static void DrawRect(	FRECT		Rect, // altered inside
							D3DCOLOR	Color = WhiteColor());

	static void DrawRect(	FRECT			Rect, // altered inside
							const D3DCOLOR	Colors[4]);
};

// -----------------
// Wire rect effect
// -----------------
class TWireRectEffect
{
public:
	static void DrawRect(	FRECT			Rect, // altered inside
							const D3DCOLOR	Color = WhiteColor());

	static void DrawRect(	FRECT			Rect, // altered inside
							const D3DCOLOR	Colors[4]);
};

#endif // _2d_effects_h
