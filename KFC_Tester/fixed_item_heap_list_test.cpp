#include "pch.h"
#include "fixed_item_heap_list_test.h"

#include <KFC_KTL/fixed_item_heap_list.h>
#include <KFC_Common/file_mapping.h>
#include "dancer_growable_allocator.h"

struct tq_fih_list
{
	union
	{
		struct
		{
			size_t a;
			double v;
			int n;
		}q;

		BYTE pad[16];
	};

	tq_fih_list() { q.a=1, q.v=0; }

	~tq_fih_list() { q.a = 3; }
};

template <class ListType>
void TestInst3()
{
	typedef ListType tlist;

	typename tlist::THeap heap;

	typename tlist::THeader header;

	tlist list;
	tlist list2;

	typename tlist::TIterator iter;
	const tlist clist;

	clist.IsAllocated();
	list.Release();

	clist.m_Accessor.IsAllocated();
	list.m_Accessor.Release();

	typename tlist::TIterator it;
	typename tlist::TConstIterator cit;

	it = typename tlist::TIterator(0);
	cit = typename tlist::TConstIterator(0);

	it.IsValid();
	it.Invalidate();

	list[it].q.a = 3;
	if(clist[cit].q.a == 3) {}

	list.GetN();
	clist.GetN();

	cit = it;

	it = list.GetFirst();
	it = list.GetLast();
	it = list.GetPrev(it);
	it = list.GetNext(it);

	it = list.ToFirst(it);
	it = list.ToNext(it);
	it = list.ToPrev(it);
	it = list.ToNext(it);

	cit = clist.GetFirst();
	cit = clist.GetLast();
	cit = clist.GetPrev(it);
	cit = clist.GetNext(it);

	cit = clist.ToFirst(cit);
	cit = clist.ToNext(cit);
	cit = clist.ToPrev(cit);
	cit = clist.ToNext(cit);

	list.Clear();

	list.Add(it, it);
	list.AddBefore(it);
	list.AddAfter(it);
	list.AddFirst();
	list.AddLast();

	list.Del(it);

	list.IsEmpty();
	clist.IsEmpty();

	list.Clear();
}

DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(TArrayGrowableAllocator, list_gap_arr)
DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(list_gap_arr, list_sfiha_arr)

DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(TDancerGrowableAllocator, list_gap_dancer)
DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(list_gap_dancer, list_sfiha_dancer)

DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(TFileMappingGrowableAllocator, list_gap_fm)
DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(list_gap_fm, list_sfiha_fm)

template <class HeapAllocatorType>
void TestInst2()
{
	TestInst3<FIXED_ITEM_HEAP_LIST(tq_fih_list, HeapAllocatorType, TPrivateFixedItemHeapAccessor)>();
	TestInst3<FIXED_ITEM_HEAP_LIST(tq_fih_list, HeapAllocatorType, list_sfiha_arr)>();
	TestInst3<FIXED_ITEM_HEAP_LIST(tq_fih_list, HeapAllocatorType, list_sfiha_fm)>();
}

void TestFixedItemHeapListInst()
{
	TestInst2<TArrayGrowableAllocator>		();
	TestInst2<TDancerGrowableAllocator>		();
	TestInst2<TFileMappingGrowableAllocator>();
}

template <class tlist>
void TestList(tlist& list, bool bFastClean = false)
{
	list.Clear();

	tassert(list.IsEmpty());

	if(bFastClean)
	{
		tassert(list.IsEmpty() &&
				list.m_Accessor.GetHeap().IsTotallyCleanWithAux() &&
				list.m_Accessor.GetHeap().IsTotallyCleanWithAuxDefaultCmp());
	}

	int i;

	typename tlist::TIterator it;
	typename tlist::TConstIterator cit;

	for(i=0;i<1024;i++)
		list[list.AddLast()].q.a = i * 3;

	for(it = list.GetFirst() ; it.IsValid() ; list.ToNext(list.ToNext(it)))
	{
		int v = list[it].q.a;

		list[list.AddBefore(it)].q.a = v - 1;
		list[list.AddAfter (it)].q.a = v + 1;
	}

	tassert(list.GetN() == 1024*3);

	i = -1;
	for(cit = list.GetFirst() ; cit.IsValid() ; list.ToNext(cit), i++)
		tassert((int)list[cit].q.a == i);

	i--;
	for(cit = list.GetLast() ; cit.IsValid() ; list.ToPrev(cit), i--)
		tassert((int)list[cit].q.a == i);

	for(it = list.GetFirst() ; it.IsValid() ; )
	{
		typename tlist::TIterator nit = list.GetNext(it);

		if(nit.IsValid())
			list.ToNext(nit);

		list.Del(it);

		it = nit;
	}

	tassert(list.GetN() == 1024*3/2);

	i = 0;
	for(cit = list.GetFirst() ; cit.IsValid() ; list.ToNext(cit), i += 2)
		tassert((int)list[cit].q.a == i);
}

// ----------------
// Global routines
// ----------------
void TestArrayFixedItemHeapList()
{
	{
		puts("Testing array fixed item heap list with private heap...");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list, TArrayGrowableAllocator, TPrivateFixedItemHeapAccessor) tlist;

		tlist list;

		list.m_Accessor.AllocateHeap();

		list.Clear();

		TestList(list, true);

		puts("Done");
	}

	{
		puts("Testing array fixed item heap list with shared heap, direct header ptr");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list, TArrayGrowableAllocator, TSharedFixedItemHeapAccessor<TDirectPtr>::_) tlist;

		tlist list;

		tlist::THeap heap;

		heap.Allocate();

		heap.Clear();

		tlist::THeader header;

		list.m_Accessor.Allocate(heap, header);

		TestList(list);

		puts("Done");
	}

	{
		puts("Testing array fixed item heap list with shared heap, heap header ptr");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list,
			TArrayGrowableAllocator,
			list_sfiha_arr) tlist;

		tlist list;

		tlist::THeap heap;

		heap.Allocate();

		heap.Clear();

		tlist::THeader& Header =
			reinterpret_cast<tlist::THeader&>
				(heap[heap.Reserve(tlist::TItem())]);

		new(&Header) tlist::THeader();

		list.m_Accessor.Allocate(heap, list_gap_arr<tlist::THeader>(heap.m_Allocator, &Header));

		TestList(list);

		puts("Done");
	}
}

void TestDancerFixedItemHeapList()
{
	{
		puts("Testing dancer fixed item heap list with private heap...");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list, TDancerGrowableAllocator, TPrivateFixedItemHeapAccessor) tlist;

		tlist list;
		
		list.m_Accessor.AllocateHeap();

		list.Clear();

		TestList(list, true);

		puts("Done");
	}

	{
		puts("Testing dancer fixed item heap list with shared heap, direct header ptr");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list,
			TDancerGrowableAllocator,
			TSharedFixedItemHeapAccessor<TDirectPtr>::_) tlist;

		tlist list;

		tlist::THeap heap;

		heap.Allocate();

		heap.Clear();

		tlist::THeader header;

		list.m_Accessor.Allocate(heap, header);

		TestList(list);

		puts("Done");
	}

	{
		puts("Testing dancer fixed item heap list with shared heap, heap header ptr");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list,
			TDancerGrowableAllocator,
			list_sfiha_dancer) tlist;

		tlist list;

		tlist::THeap heap;

		heap.Allocate();

		heap.Clear();

		tlist::THeader& Header =
			reinterpret_cast<tlist::THeader&>
				(heap[heap.Reserve(tlist::TItem())]);

		new(&Header) tlist::THeader();

		list.m_Accessor.Allocate(heap, list_gap_dancer<tlist::THeader>(heap.m_Allocator, &Header));

		TestList(list);

		puts("Done");
	}
}

void TestFileMappingFixedItemHeapList()
{
	{
		puts("Testing file mapping fixed item heap list with private heap...");

		typedef FIXED_ITEM_HEAP_LIST(tq_fih_list, TFileMappingGrowableAllocator, TPrivateFixedItemHeapAccessor) tlist;

		tlist list;

		list.m_Accessor.GetHeapAllocator().Allocate("test.dat", false);

		list.m_Accessor.AllocateHeap();

		list.Clear();

		TestList(list, true);

		puts("Done");
	}

	{
		puts("Testing file mapping fixed item heap list with shared heap, direct header ptr");

		typedef FIXED_ITEM_HEAP_LIST(	tq_fih_list,
			TFileMappingGrowableAllocator,
			TSharedFixedItemHeapAccessor<TDirectPtr>::_) tlist;

		tlist list;

		tlist::THeap heap;

		heap.m_Allocator.Allocate("test.dat", false);

		heap.Allocate();

		heap.Clear();

		tlist::THeader header;

		list.m_Accessor.Allocate(heap, header);

		TestList(list);

		puts("Done");
	}

	{
		puts("Testing file mapping fixed item heap list with shared heap, heap header ptr");

		typedef FIXED_ITEM_HEAP_LIST(	tq_fih_list,
			TFileMappingGrowableAllocator,
			list_sfiha_fm) tlist;

		tlist list;

		tlist::THeap heap;

		heap.m_Allocator.Allocate("test.dat", false);

		heap.Allocate();

		heap.Clear();

		tlist::THeader& Header =
			reinterpret_cast<tlist::THeader&>
				(heap[heap.Reserve(tlist::TItem())]);

		new(&Header) tlist::THeader();

		list.m_Accessor.Allocate(heap, list_gap_fm<tlist::THeader>(heap.m_Allocator, &Header));

		TestList(list);

		puts("Done");
	}
}
