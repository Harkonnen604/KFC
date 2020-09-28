#include "pch.h"
#include "avl_storage_test.h"

#include <KFC_KTL/avl_storage.h>

struct tq_avl
{
    size_t a;
    double v;

    int n;

    tq_avl(size_t sa) : a(sa) {}

    ~tq_avl() { a = 3; }
};

typedef T_AVL_Storage<tq_avl> tstg;

inline int Compare(const tq_avl& v1, size_t v2)
    { return Compare(v1.a, v2); }

static void update(tstg::TIterator it)
{
    it->n = (it.GetLeft ().IsValid() ? it.GetLeft ()->n : 0) +
            (it.GetRight().IsValid() ? it.GetRight()->n : 0) + 1;
}

void TestAVL_StorageInst()
{
    tstg stg(update);
    tstg stg2;
    const tstg cstg;

    tstg::TIterator it;
    tstg::TConstIterator cit;

    it = tstg::TIterator(NULL);

    it.IsValid();
    it.Invalidate();

    (*it).a = 3;
    it->a = 2;

    it.GetLeft();
    it.GetRight();
    it.GetParent();
    it.GetPrev();
    it.GetNext();

    it.ToLeft();
    it.ToRight();
    it.ToParent();
    it.ToPrev();
    it.ToNext();

    ++it, --it, it++, it--;

    it.GetH();

    if((*cit).a == 3) {}
    if(cit->a == 2) {}

    cit.GetLeft();
    cit.GetRight();
    cit.GetParent();
    cit.GetPrev();
    cit.GetNext();

    cit.ToLeft();
    cit.ToRight();
    cit.ToParent();
    cit.ToPrev();
    cit.ToNext();

    ++cit, --cit, cit++, cit--;

    cit.GetH();

    stg.Clear();

    stg.Find(2);
    cstg.Find(2);

    stg.FindFirst(2);
    cstg.FindFirst(2);

    stg.FindLast(2);
    cstg.FindLast(2);

    cstg.Has(2);

    stg.GetRoot();
    stg.GetFirst();
    stg.GetLast();

    cstg.GetRoot();
    cstg.GetFirst();
    cstg.GetLast();

    stg.Add(2);

    stg.Del(it);

    stg.Upd(it);

    cstg.IsEmpty();
    cstg.GetN();
    cstg.GetH();
}

#include <KFC_KTL/growable_allocator.h>

// ----------------
// Global routines
// ----------------
void Test_AVL_Storage()
{
    puts("Testing AVL storage...");

    {
        tstg stg(update);

        const tstg& cstg = stg;

        int nt = 0;

        for(int q=4*1024;q;q--)
        {
            tassert((int)stg.GetN() == nt);

            if(rand() % 100 == 0)
            {
                if(stg.IsEmpty())
                {
                    tassert(stg.GetH() == 0);
                }
                else
                {
                    tstg::TIterator it;
                    tstg::TConstIterator cit;

                    for(it = ++stg.GetFirst() ; it.IsValid() ; ++it)
                        tassert(it.GetPrev()->a <= it->a);

                    for(it = --stg.GetLast() ; it.IsValid() ; --it)
                        tassert(it.GetNext()->a >= it->a);

                    for(cit = ++cstg.GetFirst() ; cit.IsValid() ; ++cit)
                        tassert(cit.GetPrev()->a <= cit->a);

                    for(cit = --cstg.GetLast() ; cit.IsValid() ; --cit)
                        tassert(cit.GetNext()->a >= cit->a);

                    int amt[32];
                    int tamt = 0;
                    memset(amt, 0, sizeof(amt));

                    for(it = stg.GetFirst() ; it.IsValid() ; ++it)
                        amt[it->a]++, tamt++;

                    tassert(tamt == nt);

                    size_t i;

                    for(i=0;i<32;i++)
                    {
                        if(amt[i])
                        {
                            tassert(cstg.Has(i));
                            tstg::TConstIterator cit = cstg.Find(i);
                            tassert(cit.IsValid() && cit->a == i);
                        }
                        else
                        {
                            tassert(!cstg.Has(i));
                        }

                        int v;

                        for(v = 0, cit = stg.FindFirst(i) ; cit.IsValid() && cit->a == i ; ++cit, v++);
                        tassert(v == amt[i]);

                        for(v = 0, cit = stg.FindLast(i) ; cit.IsValid() && cit->a == i ; --cit, v++);
                        tassert(v == amt[i]);
                    }
                }
            }

            size_t v = rand() % 32;

            if(!stg.IsEmpty() && rand() % 2)
            {
                tstg::TIterator it = stg.Find(v);

                if(it.IsValid())
                    stg.Del(it), nt--;
            }
            else
            {
                tstg::TIterator it = stg.Add(v);

                nt++;

                tassert(it->a == v);
            }

            tassert((stg.IsEmpty() ? 0 : stg.GetRoot()->n) == nt);

            tassert(nt <= (1 << (2*stg.GetH())));
        }

        stg. Clear();
    }

    puts("Done");
}
