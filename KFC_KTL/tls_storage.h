#ifndef tls_storage_h
#define tls_storage_h

#include "critical_section.h"

// ------------
// TLS storage
// ------------
class T_TLS_Storage
{
public:
    // Item
    struct TItem
    {
        // Creator
        typedef TItem* TCreator();


        virtual ~TItem() {}
    };

private:
    // Thread chain
    typedef TArray< TPtrHolder<TItem> > TThreadChain;

    // Thread chains
    typedef TList<TThreadChain> TThreadChains;

private:
    mutable TCriticalSection m_AccessCS;

    size_t m_szTLSIndex;

    TArray<TItem::TCreator*, true> m_ItemCreators;

    mutable TThreadChains m_ThreadChains;

    size_t m_sz_KTL_TLS_Index;

public:
    T_TLS_Storage();

    ~T_TLS_Storage();

    void CleanThreadChain();

    void FreeItemType(size_t& szIndex);

    size_t ReserveItemType(TItem::TCreator* pCreator);

    TItem& GetItem(size_t szIndex) const;

    TItem& operator [] (size_t szIndex) const
        { return GetItem(szIndex); }
};

extern T_TLS_Storage g_TLS_Storage;

#endif // tls_storage_h
