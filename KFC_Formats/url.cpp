#include "kfc_formats_pch.h"
#include "url.h"

#include <KFC_KTL/file_names.h>

// ----------------
// Global routines
// ----------------

// Resource
T_URL::TResource& T_URL::TResource::Empty()
{
	m_File.Empty();

	m_Arguments.Empty();

	return *this;
}

T_URL::TResource& T_URL::TResource::operator = (KString s)
{
	FixSlashes(s, '/');

	size_t i;
	
	if((i = s.Find(TEXT('?'))) != UINT_MAX)
		m_File = s.Left(i), m_Arguments = s.Mid(i+1);
	else
		m_File = s, m_Arguments.Empty();

	if(m_File.IsEmpty())
		m_File = TEXT('/');

	// Trimming
	m_File.Trim(), m_Arguments.Trim();

	return *this;
}

T_URL::TResource::operator KString () const
{
	KString Text;

	if(m_File[0] != TEXT('/'))
		Text += TEXT('/');

	Text += m_File;

	if(!m_Arguments.IsEmpty())
		Text += TEXT('?'), Text += m_Arguments;

	return Text;
}

// URL
T_URL& T_URL::Empty()
{
	m_Protocol.Empty();

	m_Address.Empty();

	m_wPort = 0;

	m_Resource.Empty();

	m_Section.Empty();

	return *this;
}

T_URL& T_URL::operator = (KString s)
{
	FixSlashes(s, '/');

	size_t i;

	if((i = s.Find(TEXT('#'))) != UINT_MAX)
		m_Section = s.Mid(i+1), s.SetLeft(i);
	else
		m_Section.Empty();	

	if((i = s.Find(TEXT("://"))) != UINT_MAX)
		m_Protocol = s.Left(i), s.SetMid(i + 3);
	else
		m_Protocol = TEXT("http");

	i = Min(s.Find(TEXT('/')), s.Find(TEXT('?')));

	if(i == UINT_MAX)
		m_Address = s, m_Resource = TEXT('/');
	else
		m_Address = s.Left(i), m_Resource = s.Mid(i);

	// Splitting address to address/port
	if((i = m_Address.Find(TEXT(':'))) != UINT_MAX)
		m_wPort = (WORD)atoi(m_Address.Mid(i+1)), m_Address = m_Address.Left(i).Trimmed();
	else
		m_wPort = GetDefaultPort(m_Protocol);	

	// Trimming
	m_Protocol.Trim(), m_Address.Trim(), m_Section.Trim();

	// Canonizing
	m_Protocol.ToLower(), m_Address.ToLower();

	return *this;
}

T_URL::operator KString () const
{
	KString Text = GetStringNoSection();

	if(!m_Section.IsEmpty())
		Text += TEXT('#'), Text += m_Section;

	return Text;
}

KString T_URL::GetStringNoSection() const
{
	KString Text;

	Text += m_Protocol;
	Text += TEXT("://");
	Text += m_Address;

	if(m_wPort != GetDefaultPort(m_Protocol))
		Text += TEXT(':'), Text += (int)m_wPort;

	Text += (KString)m_Resource;

	return Text;
}

WORD T_URL::GetDefaultPort(LPCTSTR pProtocol)
{
	if(!_tcsicmp(pProtocol, "http"))
		return 80;

	if(!_tcsicmp(pProtocol, "https"))
		return 443;

	if(!_tcsicmp(pProtocol, "ftp"))
		return 21;

	if(!_tcsicmp(pProtocol, "pop3"))
		return 110;

	if(!_tcsicmp(pProtocol, "smtp"))
		return 25;

	return 0;
}

bool T_URL::GetLinkTarget(KString Link, T_URL& R_URL) const
{
	size_t i;

	Link.Trim();

	LPCTSTR s = Link;

	// Checking for '[protocol]://' specification
	if(_tcsstr(s, TEXT("://")))
		return R_URL = s, true;	

	// Checking for 'news:', 'mailto:', 'javascript:', etc...
	{
		size_t i;

		for(i=0 ; _istalpha(s[i]) ; i++);

		if(s[i] == TEXT(':'))
			return false;
	}

	// Resolving relative URL
	R_URL = *this, R_URL.m_Section.Empty();

	if((i = Link.Find(TEXT('#'), 1)) != UINT_MAX)
		R_URL.m_Section = Link.Mid(i+1).Trim(), s = Link.SetLeft(i);

	if(IsSlash(s[0]))
		R_URL.m_Resource = s;
	else if(s[0] == TEXT('?'))
		R_URL.m_Resource.m_Arguments = s + 1;
	else if(s[0] == TEXT('#'))
		R_URL.m_Section = s + 1;
	else
		R_URL.m_Resource = FixSlashes(temp<KString>(FollowPath(GetFilePath(m_Resource.m_File), GetFilePath(s)) + GetFileName(s))(), TEXT('/'));

	return true;
}

T_URL T_URL::GetLinkTarget(const KString& Link) const
{
	T_URL URL;

	if(!GetLinkTarget(Link, URL))
		INITIATE_DEFINED_FAILURE((KString)"Attempt to follow invalid link: \"" + Link + "\".");

	return URL;
}
