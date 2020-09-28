#include "pch.h"
#include "file_chars_tree_test.h"

#include <KFC_Common/file_chars_tree.h>

struct tq
{
    int a;

    tq() : a(2) {}

    ~tq() { a=3; }
};

void TestFileCharsTreeInst()
{
    typedef TFileCharsTree<tq> tchr;

    tchr chr;
    const tchr cchr;

    tchr::TIterator it;
    tchr::TConstIterator cit;

    it.IsRoot();
    it.IsValid();
    it.Invalidate();

    cchr.IsAllocated();
    chr.Release();
    chr.Create("a");
    chr.Open("a", true);
    chr.Clear();

    it = chr.GetRoot();
    cit = cchr.GetRoot();

    chr.ToRoot(it);
    cchr.ToRoot(cit);

    it = chr.GetFirstChild(it);
    it = chr.GetNextSibling(it);

    cit = cchr.GetFirstChild(cit);
    cit = cchr.GetNextSibling(cit);

    chr.ToFirstChild(it);
    chr.ToNextSibling(it);

    cchr.ToFirstChild(cit);
    cchr.ToNextSibling(cit);

    chr.IsLeaf(it);
    chr.IsLastChild(it);

    cchr.IsLeaf(cit);
    cchr.IsLastChild(cit);

    it = chr.InsertFirstChild(it, 0);
    it = chr.InsertNextSibling(it, 0);
    it = chr.InsertFirstChild(it, 0);

    chr.DelFirstChild(it);
    chr.DelNextSibling(it);

    chr.GetDataPtr(it)->a = 2;
    chr.GetDataRef(it).a = 2;
    chr[it].a = 2;

    if( cchr.GetDataPtr(cit)->a == 2 ||
        cchr.GetDataRef(cit).a == 2 ||
        cchr[cit].a == 2)
    {
    }

    chr.IsReadOnly();

    it = chr.FindChild(it, ' ');
    it = chr.GetChild(it, ' ');

    cit = cchr.FindChild(cit, ' ');
    cit = cchr.GetChild(cit, ' ');

    cchr.HasChild(it, ' ');
    cchr.HasChild(cit, ' ');

//  chr.GetHeap();
}

// ----------------
// Global routines
// ----------------
static void print(TFileCharsTree<tq>& tree, TFileCharsTree<tq>::TIterator it)
{
    putchar(it.IsRoot() ? '*' : tree.GetChar(it));

    if(tree.IsLeaf(it))
        return;

    putchar('(');

    bool f = true;

    for(tree.ToFirstChild(it) ; it.IsValid() ; tree.ToNextSibling(it))
    {
        if(f)
            f = false;
        else
            putchar(',');

        print(tree, it);
    }

    putchar(')');
}

void TestFileCharsTree()
{
    puts("Testing file chars tree,,,");

    {
        TFileCharsTree<tq> chr;

        chr.Create("test.chr");

        for(int q=4;q;q--)
        {
            TFileCharsTree<tq>::TIterator it, it2, tit;

            it = chr.InsertFirstChild(chr.GetRoot(), 'a');
            it = chr.InsertNextSibling(it, 'b'), tit = it;
            it = chr.InsertNextSibling(it, 'd');

                  chr.InsertFirstChild(it, '4');
            it2 = chr.InsertFirstChild(it, '2');
                  chr.InsertFirstChild(it, '1');

            chr.InsertNextSibling(it2, '3');

            tit = chr.InsertNextSibling(tit, 'c');

            chr.DelNextSibling(tit);

            size_t osz = chr.GetHeap().m_Allocator.GetN();

            chr.InsertFirstChild(chr.GetRoot(), '0');

            tassert(chr.GetHeap().m_Allocator.GetN() == osz);

            print(chr, chr.GetRoot()), puts("");

            printf("sz = %d\n", chr.GetHeap().m_Allocator.GetN());

            while(!chr.IsLeaf(chr.GetRoot()))
                chr.DelFirstChild(chr.GetRoot());
        }

        chr.Open("test.chr", true);

        print(chr, chr.GetRoot()), puts("");
    }

    puts("Done");
}
