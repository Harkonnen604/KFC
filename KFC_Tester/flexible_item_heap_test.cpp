#include "pch.h"
#include "flexible_item_heap_test.h"

#include <KFC_KTL/flexible_item_heap.h>
#include <KFC_Common/file_mapping.h>
#include "dancer_growable_allocator.h"

template <class AllocatorType>
void TestHeapInst()
{
	typedef TFlexibleItemHeap<AllocatorType, AllocatorType> THeap;

	THeap Heap;
	const THeap CHeap;

	typename THeap::TIterator it;

	Heap.Allocate();
	Heap.Release();
	CHeap.IsAllocated();

	Heap.Clear();
	it = Heap.Reserve(1);
	Heap.Free(it);
	Heap.GetDataPtr(it);
	CHeap.GetDataPtr(it);
	Heap[it];
	CHeap[it];
}

void TestFlexibleItemHeap()
{
	TestHeapInst<TArrayGrowableAllocator>		();
	TestHeapInst<TDancerGrowableAllocator>		();
	TestHeapInst<TFileMappingGrowableAllocator>	();
}

template <class THeap>
static void TestHeap(THeap& Heap)
{
	Heap.Clear();

	for(size_t q = 16 ; q ; q--)
	{
		typename THeap::TIterator o1, o2, o3, o4, o5, o6;

		o1 = Heap.Reserve(1);
		o2 = Heap.Reserve(2);
		o3 = Heap.Reserve(3);
		o4 = Heap.Reserve(6);

		Heap.Free(o3);
		Heap.Free(o1);
		Heap.Free(o2);
		o5 = Heap.Reserve(3);
		o6 = Heap.Reserve(2);
		Heap.Free(o6);
		Heap.Free(o5);
		Heap.Free(o4);

		tassert(Heap.GetDataAllocator().GetN() == 0);

		typename THeap::TIterator it[256];
		size_t sz[ARRAY_SIZE(it)];
		int nt = 0;

		size_t i, j;

		for(int w=1024 ; w ; w--)
		{
			if(!(rand() % 100))
			{
				for(i=0;i<ARRAY_SIZE(it);i++)
				{
					if(!it[i].IsValid())
						continue;

					const BYTE* p = (BYTE*)Heap[it[i]];

					for(j=0;j<sz[i];j++)
						tassert(p[j] == (BYTE)(sz[i]+j));
				}
			}

			if(nt==ARRAY_SIZE(it) || nt && rand()%2)
			{
				size_t v = rand() % nt;

				for(i=0;;i++)
				{
					tassert(i < ARRAY_SIZE(it));

					if(it[i].IsValid())
					{
						if(!v)
							break;

						v--;
					}
				}

				Heap.Free(it[i]), it[i].Invalidate(), nt--;
			}
			else if(nt < (int)ARRAY_SIZE(it))
			{
				for(i=0 ; it[i].IsValid() ; i++)
					tassert(i < ARRAY_SIZE(it));

				it[i] = Heap.Reserve(sz[i] = (rand() & 1023)), nt++;

				tassert(it[i].IsValid());

				BYTE* p = (BYTE*)Heap[it[i]];

				for(j = 0 ; j < sz[i] ; j++)
					p[j] = (BYTE)(sz[i] + j);
			}
		}

		for(i=0;i<ARRAY_SIZE(it);i++)
		{
			if(it[i].IsValid())
				Heap.Free(it[i]), it[i].Invalidate();
		}
	}
}

// ----------------
// Global routines
// ----------------
void TestArrayFlexibleItemHeap()
{
	puts("Testing array flexible item heap...");

	{
		TFlexibleItemHeap<TArrayGrowableAllocator, TArrayGrowableAllocator> Heap;

		Heap.Allocate();

		TestHeap(Heap);
	}

	puts("Done");
}

void TestDancerFlexibleItemHeap()
{
	puts("Testing dancer flexible item heap...");

	{
		TFlexibleItemHeap<TDancerGrowableAllocator, TDancerGrowableAllocator> Heap;
		
		Heap.Allocate();

		TestHeap(Heap);
	}

	puts("Done");
}

void TestFileMappingFlexibleItemHeap()
{
	puts("Testing file mapping flexible item heap...");

	{
		TFlexibleItemHeap<TFileMappingGrowableAllocator, TFileMappingGrowableAllocator> Heap;

		Heap.GetHeadersAllocator().Allocate("test.hdr", false, 0);

		Heap.GetDataAllocator().Allocate("test.dat", false, 0);

		Heap.Allocate();

		TestHeap(Heap);
	}

	puts("Done");
}
