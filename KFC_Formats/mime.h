#ifndef mime_h
#define mime_h

/*
// ---------------
// E-mail address
// ---------------
struct TEMailAddress
{
	KString m_Address;
	KString m_Name;


	TEMailAddress() {}

	TEMailAddress(LPCTSTR s)
		{ Parse(s); }

	void Parse(LPCTSTR s);
};

// -----------------
// E-mail addresses
// -----------------
class TEMailAddresses : public TList<TEMailAddress>
{
public:
	TEMailAddresses() {}

	TEMailAddresses(LPCTSTR s)
		{ Parse(s); }

	TIterator Parse(LPCTSTR s, bool bClearFirst = true);
};

// ------------
// MIME header
// ------------
struct TMIMEHeader
{
	KString m_Name;
	KString m_Value;


	TMIMEHeader& Set(LPCTSTR pSName, LPCTSTR pSValue)
		{ m_Name = pSName, m_Value = pSValue; return *this; }
};

// -------------
// MIME headers
// ------------
class TMIMEHeaders : public TList<TMIMEHeader>
{
public:
	TMIMEHeaders() {}

	TMIMEHeaders(bool bDotted, LPCTSTR pBody, size_t szLength = UINT_MAX)
		{ Parse(bDotted, pBody, szLength); }

	void Parse(bool bDotted, LPCTSTR pBody, size_t szLength = UINT_MAX);

	KString GetValue(LPCTSTR pName, LPCTSTR pDefaultValue = TEXT("")) const;

	TEMailAddresses::TIterator
		ExtractFromAddresses(	TEMailAddresses&	RAddresses,
								bool				bClearFirst = true) const;

	TEMailAddresses::TIterator
		ExtractToAddresses(	TEMailAddresses&	RAddresses,
							bool				bClearFirst = true) const;
};

// ----------------
// Global routines
// ----------------

// Converts to \r\n, folds headers, adds leading dots, adds trailing dot
KString PrepareMailBody(KString RawBody);
*/

#endif // mime_h
