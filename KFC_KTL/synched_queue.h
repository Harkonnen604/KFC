#ifndef synched_queue_h
#define synched_queue_h

#include "critical_section.h"
#include "event.h"

// --------------
// Synched queue
// --------------
class TSynchedQueue
{
public:
	// Item
	class TItem
	{
	protected:
		TSynchedQueue* const m_pQueue;

	public:
		TItem(TSynchedQueue* pQueue = NULL) : m_pQueue(pQueue)
			{ if(m_pQueue) m_pQueue->RegisterItem(); }

		virtual ~TItem()
			{ if(m_pQueue) m_pQueue->UnregisterItem(); }
	};

private:
	// Items
	typedef TList<TPtrHolder<TItem> > TItems;

private:
	bool m_bAllocated;

	volatile LONG m_lNRegisteredItems;

	mutable TCriticalSection m_AccessCS;

	TEvent m_AvailableEvent;

	TEvent m_DoneEvent;		

	TItems m_Items;

	HANDLE m_hTerminator;

public:
	TSynchedQueue();

	TSynchedQueue(HANDLE hTerminator);

	~TSynchedQueue()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release();

	void Allocate(HANDLE hTerminator);

	void RegisterItem();

	void UnregisterItem();

	void Enqueue(TItem* pItem);

	// Timing out will return 'NULL'
	TItem* DequeueOnceAvailable(size_t szTimeout = INFINITE);

	// Done event will return 'NULL'
	TItem* DequeueUntilDone();

	HANDLE GetDoneEvent() const
		{ DEBUG_VERIFY_ALLOCATION; return m_DoneEvent; }
};

#endif // synched_queue_h
