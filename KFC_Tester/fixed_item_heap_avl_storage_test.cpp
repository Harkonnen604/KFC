#include "pch.h"
#include "fixed_item_heap_avl_storage_test.h"

#include <KFC_KTL/fixed_item_heap_avl_storage.h>
#include <KFC_Common/file_mapping.h>
#include "dancer_growable_allocator.h"

struct tq_fih_avl
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

	tq_fih_avl() {}

	tq_fih_avl(size_t sa) { q.a=sa; }

	tq_fih_avl(size_t sa, size_t sv) { q.a=sa, q.v=sv; }

	~tq_fih_avl() { q.a = 3; }
};

inline int Compare(const tq_fih_avl& v1, size_t v2)
	{ return Compare(v1.q.a, v2); }

template <class StorageType>
void update_proc(StorageType& stg, typename StorageType::TIterator it)
{
	stg[it].q.n =	(stg.GetLeft (it).IsValid() ? stg[stg.GetLeft (it)].q.n : 0) +
					(stg.GetRight(it).IsValid() ? stg[stg.GetRight(it)].q.n : 0) + 1;
}

template <class StorageType>
void TestInst3()
{
	typedef StorageType tstg;

	typename tstg::THeap heap;

	typename tstg::THeader header;
	
	tstg stg(update_proc<tstg>);
	tstg stg2;

	typename tstg::TIterator iter;
	const tstg cstg;

	cstg.IsAllocated();
	stg.Release();

	cstg.m_Accessor.IsAllocated();
	stg.m_Accessor.Release();

	typename tstg::TIterator it;
	typename tstg::TConstIterator cit;

	it = typename tstg::TIterator(0);

	it.IsValid();
	it.Invalidate();

	stg[it].q.a = 3;

	stg.GetLeft(it);
	stg.GetRight(it);
	stg.GetParent(it);
	stg.GetPrev(it);
	stg.GetNext(it);

	stg.ToLeft(it);
	stg.ToRight(it);
	stg.ToParent(it);
	stg.ToPrev(it);
	stg.ToNext(it);

	stg.GetH(it);

	if(stg[cit].q.a==3) {}

	stg.GetLeft(cit);
	stg.GetRight(cit);
	stg.GetParent(cit);
	stg.GetPrev(cit);
	stg.GetNext(cit);

	stg.ToLeft(cit);
	stg.ToRight(cit);
	stg.ToParent(cit);
	stg.ToPrev(cit);
	stg.ToNext(cit);

	stg.GetH(cit);

	stg.Clear();

	stg.Find(2);
	cstg.Find(2);

	it  = stg. FindFirst(2);
	cit = cstg.FindFirst(2);
	it  = stg. FindFirstGreater(2);
	cit = cstg.FindFirstGreater(2);
	it  = stg. FindFirstGreaterEq(2);
	cit = cstg.FindFirstGreaterEq(2);

	it  = stg. FindLast(2);
	cit = cstg.FindLast(2);
	it  = stg. FindLastLess(2);
	cit = cstg.FindLastLess(2);
	it  = stg. FindLastLessEq(2);
	cit = cstg.FindLastLessEq(2);

	cstg.Has(2);

	stg.GetRoot();
	stg.GetFirst();
	stg.GetLast();

	cstg.GetRoot();
	cstg.GetFirst();
	cstg.GetLast();
	
	stg.Add(2);
	stg.Add(2, 3);

	stg.Del(it);

	stg.Upd(it);

	cstg.IsEmpty();
	cstg.GetH();
	cstg.GetN();
}

DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(TArrayGrowableAllocator, avl_gap_arr)
DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(avl_gap_arr, avl_sfiha_arr)

DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(TDancerGrowableAllocator, avl_gap_dancer)
DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(avl_gap_dancer, avl_sfiha_dancer)

DECLARE_SPECIFIC_GROWABLE_ALLOCATOR_PTR(TFileMappingGrowableAllocator, avl_gap_fm)
DECLARE_SPECIFIC_SHARED_FIXED_ITEM_HEAP_ACCESSOR(avl_gap_fm, avl_sfiha_fm)

template <class HeapAllocatorType>
void TestInst2()
{
	TestInst3<FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, HeapAllocatorType, TPrivateFixedItemHeapAccessor)>();
	TestInst3<FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, HeapAllocatorType, avl_sfiha_arr)>();
	TestInst3<FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, HeapAllocatorType, avl_sfiha_fm)>();
}

void TestFixedItemHeapAVL_StorageInst()
{
	TestInst2<TArrayGrowableAllocator>		();
	TestInst2<TDancerGrowableAllocator>		();
	TestInst2<TFileMappingGrowableAllocator>();
}

template <class StorageType>
void print(const StorageType& stg, typename StorageType::TConstIterator i)
{
	if(!i.IsValid())
		return;
	
	printf("%d:%d", i.x, stg[i].a);
	printf("("), print(stg, stg.GetLeft(i)), printf(")");
	printf("("), print(stg, stg.GetRight(i)), printf(")");
}

template <class AllocatorType>
void reopen_proc(AllocatorType& Allocator);

template <>
void reopen_proc<TArrayGrowableAllocator>(TArrayGrowableAllocator& Allocator)
{
}

template <>
void reopen_proc<TDancerGrowableAllocator>(TDancerGrowableAllocator& Allocator)
{
	Allocator.SetN(Allocator.GetN(), true);
}

template <>
void reopen_proc<TFileMappingGrowableAllocator>(TFileMappingGrowableAllocator& Allocator)
{
	Allocator.Allocate(Allocator.GetFileName(), false);
}

template <class tstg>
void TestStorage(tstg& stg, bool bFastClean = false)
{
	const tstg& cstg = stg;

	int nt = 0;	

	for(int q=4*1024;q;q--)
	{
		tassert(nt == (int)stg.GetN());

		if(rand() % 1000 == 0)
			reopen_proc(stg.m_Accessor.GetHeap().m_Allocator);

		if(rand() % 100 == 0)
		{
			if(cstg.IsEmpty())
			{
				tassert(cstg.GetH() == 0);					
			}
			else
			{
				typename tstg::TIterator it;
				typename tstg::TConstIterator cit;

				for(it = stg.ToNext(make_temp(stg.GetFirst())()) ; it.IsValid() ; stg.ToNext(it))
					tassert(stg[make_temp(stg.GetPrev(it))()].q.a <= stg[it].q.a);

				for(it = stg.ToPrev(make_temp(stg.GetLast())()) ; it.IsValid() ; stg.ToPrev(it))
					tassert(stg[stg.GetNext(it)].q.a >= stg[it].q.a);

				for(cit = cstg.ToNext(make_temp(cstg.GetFirst())()) ; cit.IsValid() ; cstg.ToNext(cit))
					tassert(cstg[make_temp(cstg.GetPrev(cit))()].q.a <= cstg[cit].q.a);

				for(cit = cstg.ToPrev(make_temp(cstg.GetLast())()) ; cit.IsValid() ; cstg.ToPrev(cit))
					tassert(cstg[make_temp(cstg.GetNext(cit))()].q.a >= cstg[cit].q.a);
				
				int amt[32];
				int tamt = 0;
				memset(amt, 0, sizeof(amt));

				for(cit = cstg.GetFirst() ; cit.IsValid() ; cstg.ToNext(cit))
					amt[stg[cit].q.a]++, tamt++;

				tassert(tamt == nt);

				size_t i;

				for(i=0;i<32;i++)
				{
					if(amt[i])
					{
						tassert(cstg.Has(i));
						typename tstg::TConstIterator cit = cstg.Find(i);
						tassert(cit.IsValid() && cstg[cit].q.a == i);
					}
					else
					{
						tassert(!cstg.Has(i));
					}

					int v;

					for(v = 0, cit = cstg.FindFirst(i) ; cit.IsValid() && cstg[cit].q.a == i ; cstg.ToNext(cit), v++);
					tassert(v == amt[i]);

					for(v = 0, cit = cstg.FindLast(i) ; cit.IsValid() && cstg[cit].q.a == i ; cstg.ToPrev(cit), v++);
					tassert(v == amt[i]);
				}
			}
		}

		size_t v = rand() % 32;

		if(!stg.IsEmpty() && rand() % 2)
		{
			typename tstg::TIterator it = stg.Find(v);

			if(it.IsValid())
				stg.Del(it), nt--;
		}
		else
		{
			typename tstg::TIterator it = stg.Add(v);

			nt++;

			tassert(stg[it].q.a == v);
		}

		tassert((stg.IsEmpty() ? 0 : stg[stg.GetRoot()].q.n) == nt);

		tassert(nt <= (1 << (2*stg.GetH())));
	}

	stg.Clear();

	if(bFastClean)
	{
		tassert(stg.IsEmpty() &&
				stg.m_Accessor.GetHeap().IsTotallyCleanWithAux() &&
				stg.m_Accessor.GetHeap().IsTotallyCleanWithAuxDefaultCmp());
	}

	stg.Add(2);

	stg.Clear();

	{
		for(size_t i = 0 ; i < 4153 ; i++)
			stg.Add(1 + rand() % 16);		

		typename tstg::TIterator it;

		for(size_t i = 0 ; i <= 17 ; i++)
		{
			it = stg.Find(i);
			tassert(!it.IsValid() || stg[it].q.a == i);

			it = stg.FindFirstGreater(i);
			tassert(it.IsValid() ?
						(stg[it].q.a > i &&
							(!stg.GetPrev(it).IsValid() || stg[stg.GetPrev(it)].q.a <= i)) :
						(!stg.GetLast().IsValid() || stg[stg.GetLast()].q.a <= i));

			it = stg.FindFirstGreaterEq(i);
			tassert(it.IsValid() ?
						(stg[it].q.a >= i &&
							(!stg.GetPrev(it).IsValid() || stg[stg.GetPrev(it)].q.a < i)) :
						(!stg.GetLast().IsValid() || stg[stg.GetLast()].q.a < i));			

			it = stg.FindLastLess(i);
			tassert(it.IsValid() ?
						(stg[it].q.a < i &&
							(!stg.GetNext(it).IsValid() || stg[stg.GetNext(it)].q.a >= i)) :
						(!stg.GetFirst().IsValid() || stg[stg.GetFirst()].q.a >= i));

			it = stg.FindLastLessEq(i);
			tassert(it.IsValid() ?
						(stg[it].q.a <= i &&
							(!stg.GetNext(it).IsValid() || stg[stg.GetNext(it)].q.a > i)) :
						(!stg.GetFirst().IsValid() || stg[stg.GetFirst()].q.a > i));
		}

		stg.Clear();
	}
}

// ----------------
// Global routines
// ----------------
void TestArrayFixedItemHeapAVL_Storage()
{
	{
		puts("Testing array fixed item heap AVL storage with private heap...");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, TArrayGrowableAllocator, TPrivateFixedItemHeapAccessor) tstg;

		tstg stg(update_proc<tstg>);

		stg.m_Accessor.AllocateHeap();

		stg.Clear();

		TestStorage(stg, true);

		puts("Done");
	}

	{
		puts("Testing array fixed item heap AVL storage with shared heap, direct header ptr");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, TArrayGrowableAllocator, TSharedFixedItemHeapAccessor<TDirectPtr>::_) tstg;

		tstg stg(update_proc<tstg>);

		tstg::THeap heap;

		heap.Allocate();

		heap.Clear();

		tstg::THeader header;

		stg.m_Accessor.Allocate(heap, header);

		TestStorage(stg);

		puts("Done");
	}

	{
		puts("Testing array fixed item heap AVL storage with shared heap, heap header ptr");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl,
											TArrayGrowableAllocator,
											avl_sfiha_arr) tstg;

		tstg stg(update_proc<tstg>);

		tstg::THeap heap;

		heap.Allocate();

		heap.Clear();

		tstg::THeader& Header =
			reinterpret_cast<tstg::THeader&>
				(heap[heap.Reserve(tstg::TItem(0, 0))]);

		new(&Header) tstg::THeader();

		stg.m_Accessor.Allocate(heap, avl_gap_arr<tstg::THeader>(heap.m_Allocator, &Header));

		TestStorage(stg);

		puts("Done");
	}
}

void TestDancerFixedItemHeapAVL_Storage()
{
	{
		puts("Testing dancer fixed item heap AVL storage with private heap...");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, TDancerGrowableAllocator, TPrivateFixedItemHeapAccessor) tstg;

		tstg stg(update_proc<tstg>);
		
		stg.m_Accessor.AllocateHeap();

		stg.Clear();

		TestStorage(stg, true);

		puts("Done");
	}

	{
		puts("Testing dancer fixed item heap AVL storage with shared heap, direct header ptr");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl,
											TDancerGrowableAllocator,
											TSharedFixedItemHeapAccessor<TDirectPtr>::_) tstg;

		tstg stg(update_proc<tstg>);

		tstg::THeap heap;

		heap.Allocate();

		heap.Clear();

		tstg::THeader header;

		stg.m_Accessor.Allocate(heap, header);

		TestStorage(stg);

		puts("Done");
	}

	{
		puts("Testing dancer fixed item heap AVL storage with shared heap, heap header ptr");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl,
											TDancerGrowableAllocator,
											avl_sfiha_dancer) tstg;

		tstg stg(update_proc<tstg>);

		tstg::THeap heap;

		heap.Allocate();

		heap.Clear();

		tstg::THeader& Header =
			reinterpret_cast<tstg::THeader&>
				(heap[heap.Reserve(tstg::TItem(0, 0))]);

		new(&Header) tstg::THeader();

		stg.m_Accessor.Allocate(heap, avl_gap_dancer<tstg::THeader>(heap.m_Allocator, &Header));

		TestStorage(stg);

		puts("Done");
	}
}

void TestFileMappingFixedItemHeapAVL_Storage()
{
	{
		puts("Testing file mapping fixed item heap AVL storage with private heap...");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl, TFileMappingGrowableAllocator, TPrivateFixedItemHeapAccessor) tstg;

		tstg stg(update_proc<tstg>);

		stg.m_Accessor.GetHeapAllocator().Allocate("test.dat", false);

		stg.m_Accessor.AllocateHeap();

		stg.Clear();

		TestStorage(stg, true);

		puts("Done");
	}

	{
		puts("Testing file mapping fixed item heap AVL storage with shared heap, direct header ptr");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl,
											TFileMappingGrowableAllocator,
											TSharedFixedItemHeapAccessor<TDirectPtr>::_) tstg;

		tstg stg(update_proc<tstg>);

		tstg::THeap heap;

		heap.m_Allocator.Allocate("test.dat", false);

		heap.Allocate();

		heap.Clear();

		tstg::THeader header;

		stg.m_Accessor.Allocate(heap, header);

		TestStorage(stg);

		puts("Done");
	}

	{
		puts("Testing file mapping fixed item heap AVL storage with shared heap, heap header ptr");

		typedef FIXED_ITEM_HEAP_AVL_STORAGE(tq_fih_avl,
											TFileMappingGrowableAllocator,
											avl_sfiha_fm) tstg;

		tstg stg(update_proc<tstg>);

		tstg::THeap heap;

		heap.m_Allocator.Allocate("test.dat", false);

		heap.Allocate();

		heap.Clear();

		tstg::THeader& Header =
			reinterpret_cast<tstg::THeader&>
				(heap[heap.Reserve(tstg::TItem(0, 0))]);

		new(&Header) tstg::THeader();

		stg.m_Accessor.Allocate(heap, avl_gap_fm<tstg::THeader>(heap.m_Allocator, &Header));

		TestStorage(stg);

		puts("Done");
	}
}
