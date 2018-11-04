#ifndef variable_list_h
#define variable_list_h

#include "kstring.h"
#include "list.h"
#include "stream.h"

// ---------
// Variable
// ---------
struct TVariable
{
public:
	KString m_Name;
	KString m_Value;

public:
	TVariable() {}

	TVariable(LPCTSTR pSName, LPCTSTR pSValue) :
		m_Name(pSName), m_Value(pSValue) {}

	TVariable& Set(const KString& SName, const KString& SValue)
	{
		m_Name = SName, m_Value = SValue;

		return *this;
	}
};

DECLARE_STREAMING(TVariable);

// --------------
// Variable list
// --------------
class TVariableList : public TList<TVariable>
{
private:
	bool m_bCaseSensitive;

private:
	TIterator Find(LPCTSTR pName);

	TConstIterator Find(LPCTSTR pName) const
		{ return (const_cast<TVariableList*>(this))->Find(pName); }	

public:
	TVariableList(bool bSCaseSensitive) : m_bCaseSensitive(bSCaseSensitive) {}

	bool IsCaseSensitive() const
		{ return m_bCaseSensitive; }

	bool Has(LPCTSTR pName) const
		{ return Find(pName).IsValid(); }

	LPCTSTR Get(LPCTSTR pName, LPCTSTR pDefaultValue = TEXT("")) const
	{
		TConstIterator Iter = Find(pName);

		return Iter.IsValid() ? (LPCTSTR)Iter->m_Value : pDefaultValue;
	}

	void Set(LPCTSTR pName, LPCTSTR pValue);

	void Add(LPCTSTR pName, LPCTSTR pValue)
		{ AddLast()->Set(pName, pValue); }

	void Del(LPCTSTR pName);

	LPCTSTR operator [] (LPCTSTR pName) const
		{ return Get(pName); }

	TVariableList& operator () (LPCTSTR pName, LPCTSTR pValue)
		{ Add(pName, pValue); return *this; }
};

#endif // variable_list_h
