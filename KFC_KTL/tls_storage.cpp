#include "kfc_ktl_pch.h"
#include "tls_storage.h"

#include "ktl_tls_item.h"

T_TLS_Storage g_TLS_Storage;

// ------------
// TLS storage
// ------------
T_TLS_Storage::T_TLS_Storage()
{
    m_szTLSIndex = TlsAlloc();

    assert(m_szTLSIndex != TLS_OUT_OF_INDEXES);

    T_KTL_TLS_Item::ReserveItemType();
}

T_TLS_Storage::~T_TLS_Storage()
{
    T_KTL_TLS_Item::FreeItemType();

    if(m_szTLSIndex != TLS_OUT_OF_INDEXES)
        TlsFree((DWORD)m_szTLSIndex), m_szTLSIndex = TLS_OUT_OF_INDEXES;
}

void T_TLS_Storage::CleanThreadChain()
{
    TThreadChains::TIterator Iter;

    if(Iter.FromPVoid(TlsGetValue((DWORD)m_szTLSIndex)).IsValid())
    {
        {
            TCriticalSectionLocker Locker0(m_AccessCS);

            m_ThreadChains.Del(Iter), Iter.Invalidate();
        }

        TlsSetValue((DWORD)m_szTLSIndex, Iter.AsPVoid());
    }
}

void T_TLS_Storage::FreeItemType(size_t& szIndex)
{
    {
        TCriticalSectionLocker Locker0(m_AccessCS);

        assert(szIndex != -1 && szIndex == m_ItemCreators.GetN() - 1);

        m_ItemCreators.DelLast();

        for(TThreadChains::TIterator Iter = m_ThreadChains.GetFirst() ; Iter.IsValid() ; ++Iter)
            Iter->SetN(m_ItemCreators.GetN());
    }

    szIndex = -1;
}

size_t T_TLS_Storage::ReserveItemType(TItem::TCreator* pCreator)
{
    assert(pCreator);

    TCriticalSectionLocker Locker0(m_AccessCS);

    m_ItemCreators.Add() = pCreator;

    return m_ItemCreators.GetLast();
}

T_TLS_Storage::TItem& T_TLS_Storage::GetItem(size_t szIndex) const
{
    #ifdef _DEBUG
        TCriticalSectionLocker Locker0(m_AccessCS);
    #endif // _DEBUG

    assert(szIndex < m_ItemCreators.GetN());

    TThreadChains::TIterator Iter;

    if(!Iter.FromPVoid(TlsGetValue((DWORD)m_szTLSIndex)).IsValid())
    {
        {
            TCriticalSectionLocker Locker0(m_AccessCS);

            Iter = m_ThreadChains.AddLast();
        }

        Iter->SetN(m_ItemCreators.GetN());

        TlsSetValue((DWORD)m_szTLSIndex, Iter.AsPVoid());
    }

    Iter->EnsureN(szIndex + 1);

    TPtrHolder<TItem>& Item = (*Iter)[szIndex];

    if(!Item)
        Item = m_ItemCreators[szIndex]();

    return *Item;
}
