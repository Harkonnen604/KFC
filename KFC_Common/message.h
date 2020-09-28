#ifndef message_h
#define message_h

#include <KFC_KTL/stacked_storage.h>
#include "common_consts.h"

struct TMessage;

// ----------------
// Message storage
// ----------------
typedef TStackedStorage<TMessage> TMessageStorage;

// ---------------
// Message handle
// ---------------
typedef TStackedStorageIterator<TMessage> TMessageHandle;

// -----------------
// Message iterator
// -----------------
typedef TStackedStorageConstIterator<TMessage> TMessageIterator;

// --------
// Message
// --------
struct TMessage
{
public:
    virtual ~TMessage() {}

    void* operator new(size_t szSize) { return kfc_malloc(szSize); }

    void operator delete(void* pPtr) { if(pPtr) kfc_free(pPtr); }

    void* operator new (size_t              szSize,
                        TMessageStorage&    Storage)

        { return Storage.AllocateData(szSize); }

    void operator delete(void*, TMessageStorage&) {}
};

// ------------------
// Message processor
// ------------------
class TMessageProcessor : public TMessageStorage
{
private:
    bool m_bAllocated;

public:
    TMessageProcessor();

    ~TMessageProcessor() { Release(); }

    bool IsAllocated() const
        { return TMessageStorage::IsAllocated() && m_bAllocated; }

    void Release(bool bFromAllocatorException = false);

    void Allocate(  size_t szMinPortionSize =
                        g_CommonConsts.m_szDefaultMessageProcessorMinPortionSize);
};

#endif // message_h
