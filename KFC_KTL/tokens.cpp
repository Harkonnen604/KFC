#include "kfc_ktl_pch.h"
#include "tokens.h"

// -------
// Tokens
// -------
KString TTokens::Process(LPCTSTR pString) const
{
	DEBUG_VERIFY(pString);

	if(IsEmpty())
		return pString;

	KString DstString;

	while(*pString)
	{
		TStorage::TConstIterator Iter;
		
		LPCTSTR pSubString;
		
		for(Iter = m_Storage.GetRoot(), pSubString = pString ;
			Iter.IsValid() && !Iter.IsTerminal() && *pSubString ;
			Iter = Iter.FindChild(*pSubString++));

		if(Iter.IsValid() && Iter.IsTerminal())
			DstString += *Iter, pString = pSubString;
		else
			DstString += *pString++;
	}	

	return DstString;
}

const KString& TTokens::ProcessEnumValue(	LPCTSTR pString,
											LPCTSTR pValueName) const
{
	TStorage::TConstIterator Iter = m_Storage.Find(pString);
	
	if(!Iter.IsValid())
		REPORT_INVALID_VALUE(pString, pValueName);
		
	return *Iter;
}

// ------------------
// Tokens registerer
// ------------------
TTokensRegisterer::TTokensRegisterer()
{
	m_pTokens = NULL;
}

TTokensRegisterer::TTokensRegisterer(TTokens& STokens)
{
	m_pTokens = NULL;

	Allocate(STokens);
}

void TTokensRegisterer::Release()
{
	if(m_pTokens)
	{
		FOR_EACH_LIST_REV(m_SrcStrings, KStrings::TConstIterator, Iter)
			m_pTokens->Del(*Iter);

		m_SrcStrings.Clear();

		m_pTokens = NULL;
	}
}

void TTokensRegisterer::Allocate(TTokens& STokens)
{
	Release();

	m_pTokens = &STokens;
}

void TTokensRegisterer::Add(LPCTSTR pSrcString, const KString& DstString)
{
	DEBUG_VERIFY_ALLOCATION;

	m_pTokens->Add(pSrcString, DstString);
	
	m_SrcStrings << pSrcString;	
}
