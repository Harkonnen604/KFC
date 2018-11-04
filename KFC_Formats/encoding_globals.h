#ifndef encoding_globals_h
#define encoding_globals_h

#include <KFC_KTL/globals.h>
#include "encoding.h"

// -----------------
// Encoding globals
// -----------------
class TEncodingGlobals : public TGlobals
{
private:
	void OnUninitialize	();
	void OnInitialize	();

public:
	// Encodings
	typedef TList<TEncoding> TEncodings;

public:
	TEncodings m_Encodings;

public:
	TEncodingGlobals();

	const TEncoding* FindEncoding(LPCTSTR pName) const;

	const TEncoding& GetEncoding(LPCTSTR pName) const;	
};

extern TEncodingGlobals g_EncodingGlobals;

#endif // encoding_globals_h
