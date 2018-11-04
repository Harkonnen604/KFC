#include "pch.h"
#include "fixed_item_heap_test.h"

#include <KFC_KTL/fixed_item_heap.h>
#include <KFC_Common/file_mapping.h>
#include "dancer_growable_allocator.h"

struct tq_fih
{
	BYTE a;
	double v;

	tq_fih() : a(2) {}

	tq_fih(int) : a(2) {}

	tq_fih(int, double) : a(2) {}
	
	tq_fih(int, double, char) : a(2) {}

	~tq_fih() { a = 3; }

	bool IsClean() const
		{ return a==2; }
};

template <class AllocatorType>
void TestHeapInst()
{
/*	typedef TFixedItemHeapWithAux<tq_fih, AllocatorType, thdr> THeap;

	THeap Heap;

	const THeap CHeap;

	Heap.Allocate();
	Heap.Reserve();
	Heap.Reserve(1);
	Heap.Reserve(1, 2.0);
	Heap.Reserve(1, 2.0, 'a');
	CHeap.IsAllocated();

	Heap.Clear();
	Heap.ClearWithAux();
	CHeap.IsTotallyClean();
	CHeap.IsTotallyCleanWithAux();
	CHeap.IsTotallyCleanWithAuxDefaultCmp();

	Heap.Reserve();

	Heap.Free(0);

	Heap.GetOffset(NULL);
	Heap.GetDataPtr(0);

	CHeap.GetOffset(NULL);
	CHeap.GetDataPtr(0);

	CHeap.GetOffset((tq_fih*)1);

	Heap[(size_t)1] = tq_fih();
	const tq_fih& ref = CHeap[(size_t)2];

	*(Heap + 1) = tq_fih();

	const tq_fih* ref2 = CHeap + 1;*/
}

void TestFixedItemHeapInst()
{
/*	TestHeapInst<TArrayGrowableAllocator>		();
	TestHeapInst<TDancerGrowableAllocator>		();
	TestHeapInst<TFileMappingGrowableAllocator>	();*/
}

struct thdr
{
	int a;

	thdr() : a(0) {}

	bool IsClean() const
		{ return !a; }

	bool operator == (const thdr& h) const
		{ return a == h.a; }
};

template <class AllocatorType>
void TestHeap(TFixedItemHeapWithAux<tq_fih, AllocatorType, thdr>& heap, void (*proc)(TFixedItemHeapWithAux<tq_fih, AllocatorType, thdr>&) = NULL)
{
	size_t p[512];
	int nt = 0;

	memset(p, 0, sizeof(p));

 	heap.ClearWithAux();

	tassert(heap.IsTotallyCleanWithAux());
	tassert(heap.IsTotallyCleanWithAuxDefaultCmp());

	heap.GetAuxData().a = 0x604;

	int i;

	for(int q=32*1024;q;q--)
	{
		tassert(heap.GetAuxData().a == 0x604);

		if(rand()%1000 == 0)
		{
			if(proc)
				proc(heap);

			for(i=0;i<(int)ARRAY_SIZE(p);i++)
				tassert(!(p[i] && heap[p[i]].a != (BYTE)p[i]));
		}

		if(nt==ARRAY_SIZE(p) || nt && rand()%2)
		{
			size_t v = rand() % nt;

			for(i=0;;i++)
			{
				tassert(i < (int)ARRAY_SIZE(p));

				if(p[i])
				{
					if(!v)
						break;

					v--;
				}
			}

			heap.Free(p[i]), p[i] = 0, nt--;

			tassert(!p[i]);
		}
		else if(nt < (int)ARRAY_SIZE(p))
		{
			for(i=0;p[i];i++)
				tassert(i < (int)ARRAY_SIZE(p));

			p[i] = heap.Reserve(), nt++;

			tassert(p[i]);

			tassert(heap[p[i]].a == 2);

			heap[p[i]].a = (BYTE)p[i];
		}
	}
}

// ----------------
// Global routines
// ----------------
void TestArrayFixedItemHeap()
{
	puts("Testing array fixed item heap...");

	{
		TFixedItemHeapWithAux<tq_fih, TArrayGrowableAllocator, thdr> heap;

		heap.AllocateWithAux();

		TestHeap(heap);
	}

	puts("Done");
}

void TestDancerFixedItemHeap()
{
	puts("Testing dancer fixed item heap...");

	{
		TFixedItemHeapWithAux<tq_fih, TDancerGrowableAllocator, thdr> heap;

		heap.AllocateWithAux();

		TestHeap(heap);		
	}

	puts("Done");
}

static void reopen_proc(TFixedItemHeapWithAux<tq_fih, TFileMappingGrowableAllocator, thdr>& heap)
{
	heap.m_Allocator.Allocate("test.dat", false);
}

void TestFileMappingFixedItemHeap()
{
	puts("Testing file mapping fixed item heap...");

	{
		TFixedItemHeapWithAux<tq_fih, TFileMappingGrowableAllocator, thdr> heap;

		heap.m_Allocator.Allocate("test.dat", false, 0);

		heap.AllocateWithAux();

		TestHeap(heap, reopen_proc);
	}

	puts("Done");
}
