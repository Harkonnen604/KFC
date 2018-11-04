#ifndef url_h
#define url_h

// ----
// URL
// ----
struct T_URL
{
	// Resource
	struct TResource
	{
		KString m_File;
		KString m_Arguments;


		TResource() {}

		TResource(KString s)
			{ *this = s; }

		TResource& Empty();

		TResource& operator = (KString s);

		operator KString () const;
	};

	KString		m_Protocol;
	KString		m_Address;
	WORD		m_wPort;
	TResource	m_Resource;
	KString		m_Section;


	T_URL()
		{ m_wPort = 0; }

	T_URL(KString s)
		{ *this = s; }

	T_URL(LPCTSTR s)
		{ *this = s; }

	T_URL& Empty();

	T_URL& operator = (KString s);

	T_URL& operator = (LPCTSTR s)
		{ return *this = KString(s); }

	operator KString () const;

	KString GetStringNoSection() const;

	static WORD GetDefaultPort(LPCTSTR pProtocol);

	bool GetLinkTarget(KString Link, T_URL& R_URL) const;

	T_URL GetLinkTarget(const KString& Link) const; // might raise exception

	KString GetLinkTargetSafe(const KString& Link) const
		{ T_URL URL; return GetLinkTarget(Link, URL) ? URL : TEXT(""); }

	T_URL& SetLinkTarget(const KString& Link) // might raise exception
		{ return *this = GetLinkTarget(Link); }
};

#endif // url_h
