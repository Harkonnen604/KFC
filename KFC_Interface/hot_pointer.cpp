#include "kfc_interface_pch.h"
#include "hot_pointer.h"

#include <KFC_KTL\string_conv.h>

// ----------------
// Global routines
// ----------------
bool FromString(const KString&			String,
				THotPointerDirection&	RDirection)
{
	const KString& TempString = String.Trimmed();

	if(String == TEXT("Up"))
	{
		RDirection = HPD_UP;
		return true;
	}

	if(String == TEXT("Right"))
	{
		RDirection = HPD_RIGHT;
		return true;
	}

	if(String == TEXT("Down"))
	{
		RDirection = HPD_DOWN;
		return true;
	}

	if(String == TEXT("Left"))
	{
		RDirection = HPD_LEFT;
		return true;
	}

	return false;
}

void ReadHotPointerDirection(	const KString&			String,
								THotPointerDirection&	RDirection,
								LPCTSTR					pValueName)
{
	if(!FromString(String, RDirection))
		REPORT_INCORRECT_VALUE_FORMAT;
}
