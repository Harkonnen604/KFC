#include "pch.h"
#include "persistent_fixed_item_heap_test.h"

#include <KFC_KTL/persistent_fixed_item_heap.h>

// ----------------
// Global routines
// ----------------
struct tq_pfih
{
	char* p1;
	char* p2;

	tq_pfih(const char* sp1 = "",
			const char* sp2 = "") :
		p1(strdup(sp1)),
		p2(strdup(sp2)) {}

	~tq_pfih()
		{ free(p2), free(p1); }
};

void TestPersistentItemHeapInst()
{
	typedef TPersistentFixedItemHeap<tq_pfih> theap;

	theap heap;
	theap heap2(1);
	
	if(heap.IsAllocated())
		heap.Release();

	heap.Allocate(1);

	heap.FreeWithoutDestruction(NULL);
	heap.Free(NULL);	

	heap.ReserveWithoutConstruction();
	heap.Reserve();	
	heap.Reserve("qwe");
	heap.Reserve("qwe", "asd");
}

void TestPersistentFixedItemHeap()
{
	puts("Testing persistent fixed item heap...");

	typedef TPersistentFixedItemHeap<tq_pfih> theap;

	theap heap(16);

	tq_pfih* p[1024];
	memset(p, 0, sizeof(p));

	size_t i;

	for(i=0;i<1024;i++)
		p[i] = heap.Reserve((KString)i, (KString)(i*2));

	for(i=1;i<1024;i+=2)
		heap.Free(p[i]), p[i] = NULL;

	for(i=1;i<1024;i+=2)
		p[i] = heap.Reserve((KString)i, (KString)(i*2));

	for(i=0;i<1024;i++)
		tassert(atoi(p[i]->p1) == (int)i && atoi(p[i]->p2) == (int)i*2);

	heap.Free(p[0]), heap.Free(p[1]);
	p[0] = heap.Reserve(), p[1] = heap.Reserve();

	for(i=1023;i!=UINT_MAX;i--)
	{
		if(p[i])
			heap.Free(p[i]), p[i] = NULL;
	}

	puts("Done");
}
