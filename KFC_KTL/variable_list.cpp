#include "kfc_ktl_pch.h"
#include "variable_list.h"

// ---------
// Variable
// ---------
TStream& operator >> (TStream& Stream, TVariable& RVariable)
{
    return Stream >>
        RVariable.m_Name >>
        RVariable.m_Value;
}

TStream& operator << (TStream& Stream, const TVariable& Variable)
{
    return Stream <<
        Variable.m_Name <<
        Variable.m_Value;
}

// --------------
// Variable list
// --------------
TVariableList::TIterator TVariableList::Find(LPCTSTR pName)
{
    TIterator Iter;

    if(m_bCaseSensitive)
    {
        for(Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
        {
            if(!Iter->m_Name.Compare(pName))
                break;
        }
    }
    else
    {
        for(Iter = GetFirst() ; Iter.IsValid() ; ++Iter)
        {
            if(!Iter->m_Name.CompareNoCase(pName))
                break;
        }
    }

    return Iter;
}

void TVariableList::Set(LPCTSTR pName, LPCTSTR pValue)
{
    TIterator Iter = Find(pName);

    if(!Iter.IsValid())
        Iter = AddLast();

    Iter->Set(pName, pValue);
}

void TVariableList::Del(LPCTSTR pName)
{
    TIterator Iter = Find(pName);

    if(Iter.IsValid())
        TList<TVariable>::Del(Iter);
}
