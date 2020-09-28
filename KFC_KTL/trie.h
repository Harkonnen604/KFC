#ifndef trie_h
#define trie_h

#include "persistent_fixed_item_heap.h"
#include "for_each.h"

// Memory
#define TRIE_HEAP_BLOCK_SIZE    (32)

// -----
// Trie
// -----

// Node
template <class ObjectType>
struct TTrieNode
{
public:
    size_t m_szChar;

    ObjectType m_Data;

    TTrieNode* m_pFirstChild;
    TTrieNode* m_pNextSibling;

    size_t m_szNTerm;

public:
    TTrieNode(const TTrieNode& Node) :
        m_szChar        (Node.m_szChar),
        m_Data          (Node.m_Data),
        m_pFirstChild   (NULL),
        m_pNextSibling  (NULL),
        m_szNTerm       (Node.m_szNTerm)
    {
        assert(m_szNTerm);
    }

    void ZeroNTerm()
        { if(m_szNTerm) m_Data.~ObjectType(), m_szNTerm = 0; }

    size_t IncNTerm()
        { if(!m_szNTerm++) new(&m_Data) ObjectType(); return m_szNTerm; }

    size_t DecNTerm()
        { assert(m_szNTerm); if(!--m_szNTerm) m_Data.~ObjectType(); return m_szNTerm; }
};

// Iterator
template <class ObjectType>
struct TTrieIterator
{
public:
    typedef ObjectType TObject;

    typedef TTrieNode<ObjectType> TNode;

public:
    TNode* m_pNode;

public:
    TTrieIterator()
        { Invalidate(); }

    TTrieIterator(TNode* pNode) : m_pNode(pNode) {}

    TTrieIterator& FromPVoid(void* pData)
        { m_pNode = (TNode*)pData; return *this; }

    void* AsPVoid() const
        { return (void*)m_pNode; }

    bool IsValid() const
        { return m_pNode; }

    void Invalidate()
        { m_pNode = NULL; }

    TTrieIterator GetFirstChild() const
        { assert(IsValid()); return m_pNode->m_pFirstChild; }

    TTrieIterator GetNextSibling() const
        { assert(IsValid()); return m_pNode->m_pNextSibling; }

    TTrieIterator& ToFirstChild()
        { return *this = GetFirstChild(); }

    TTrieIterator& ToNextSibling()
        { return *this = GetNextSibling(); }

    TTrieIterator FindChild(size_t szChar) const
    {
        assert(IsValid());

        TTrieIterator Iter;

        for(Iter = GetFirstChild() ; ; Iter.ToNextSibling())
        {
            if(!Iter.IsValid() || Iter.GetChar() > szChar)
                return NULL;

            if(Iter.GetChar() == szChar)
                return Iter;
        }
    }

    TTrieIterator GetChild(size_t szChar) const
        { TTrieIterator Iter = FindChild(szChar); assert(Iter.IsValid()); return Iter; }

    TTrieIterator& ToChild(size_t szChar)
        { return *this = GetChild(szChar); }

    bool HasChild(size_t szChar) const
        { assert(IsValid()); return FindChild(szChar).IsValid(); }

    bool IsLeaf() const
        { assert(IsValid()); return !m_pNode->m_pFirstChild; }

    bool IsLastChild() const
        { assert(IsValid()); return !m_pNode->m_pNextSibling; }

    size_t GetChar() const
        { assert(IsValid()); return m_pNode->m_szChar; }

    size_t GetNTerm() const
        { assert(IsValid()); return m_pNode->m_szNTerm; }

    bool IsTerminal() const
        { assert(IsValid()); return m_pNode->m_szNTerm; }

    TObject* operator -> () const
        { assert(IsValid()); assert(IsTerminal()); return &m_pNode->m_Data; }

    TObject& operator * () const
        { assert(IsValid()); assert(IsTerminal()); return m_pNode->m_Data; }
};

// Const iterator
template <class ObjectType>
struct TTrieConstIterator
{
public:
    typedef ObjectType TObject;

    typedef TTrieNode<ObjectType> TNode;

public:
    const TNode* m_pNode;

public:
    TTrieConstIterator()
        { Invalidate(); }

    TTrieConstIterator(const TNode* pNode) : m_pNode(pNode) {}

    TTrieConstIterator(TTrieIterator<ObjectType> Iter) : m_pNode(Iter.m_pNode) {}

    TTrieConstIterator& operator = (TTrieIterator<ObjectType> Iter)
        { m_pNode = Iter.m_pNode; return *this; }

    TTrieConstIterator& FromPVoid(void* pData)
        { m_pNode = (const TNode*)pData; return *this; }

    void* AsPVoid() const
        { return (void*)m_pNode; }

    bool IsValid() const
        { return m_pNode; }

    void Invalidate()
        { m_pNode = NULL; }

    TTrieConstIterator GetFirstChild() const
        { assert(IsValid()); return m_pNode->m_pFirstChild; }

    TTrieConstIterator GetNextSibling() const
        { assert(IsValid()); return m_pNode->m_pNextSibling; }

    TTrieConstIterator& ToFirstChild()
        { return *this = GetFirstChild(); }

    TTrieConstIterator& ToNextSibling()
        { return *this = GetNextSibling(); }

    TTrieConstIterator FindChild(size_t szChar) const
    {
        assert(IsValid());

        TTrieConstIterator Iter;

        for(Iter = GetFirstChild() ; ; Iter.ToNextSibling())
        {
            if(!Iter.IsValid() || Iter.GetChar() > szChar)
                return NULL;

            if(Iter.GetChar() == szChar)
                return Iter;
        }
    }

    TTrieConstIterator GetChild(size_t szChar) const
        { TTrieConstIterator Iter = FindChild(szChar); assert(Iter.IsValid()); return Iter; }

    TTrieConstIterator& ToChild(size_t szChar)
        { return *this = GetChild(szChar); }

    bool HasChild(size_t szChar) const
        { assert(IsValid()); return FindChild(szChar).IsValid(); }

    bool IsLeaf() const
        { assert(IsValid()); return !m_pNode->m_pFirstChild; }

    bool IsLastChild() const
        { assert(IsValid()); return !m_pNode->m_pNextSibling; }

    size_t GetChar() const
        { assert(IsValid()); return m_pNode->m_szChar; }

    size_t GetNTerm() const
        { assert(IsValid()); return m_pNode->m_szNTerm; }

    bool IsTerminal() const
        { assert(IsValid()); return m_pNode->m_szNTerm; }

    const TObject* operator -> () const
        { assert(IsValid()); assert(IsTerminal()); return &m_pNode->m_Data; }

    const TObject& operator * () const
        { assert(IsValid()); assert(IsTerminal()); return m_pNode->m_Data; }
};

template <class ObjectType>
inline int Compare(TTrieIterator<ObjectType> v1, TTrieIterator<ObjectType> v2)
    { return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TTrieIterator<ObjectType> v1, TTrieConstIterator<ObjectType> v2)
    { return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TTrieConstIterator<ObjectType> v1, TTrieIterator<ObjectType> v2)
    { return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TTrieConstIterator<ObjectType> v1, TTrieConstIterator<ObjectType> v2)
    { return Compare(v1.AsPVoid(), v2.AsPVoid()); }

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTrieIterator<ObjectType>,      TTrieIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTrieIterator<ObjectType>,      TTrieConstIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTrieConstIterator<ObjectType>, TTrieIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTrieConstIterator<ObjectType>, TTrieConstIterator<ObjectType>)

// Trie
template <class ObjectType>
class TTrie
{
public:
    typedef ObjectType TObject;

    typedef TTrieNode<ObjectType> TNode;

    typedef TTrieIterator<ObjectType> TIterator;

    typedef TTrieConstIterator<ObjectType> TConstIterator;

private:
    TPersistentFixedItemHeap<TNode> m_Heap;

    TNode* m_pRoot;

private:
    void Clean(TIterator Iter);

    TNode* CreateNode(size_t szChar, TNode* pNextSibling)
    {
        TNode* pNode = m_Heap.ReserveWithoutConstruction();

        pNode->m_szChar         = szChar;
        pNode->m_pFirstChild    = NULL;
        pNode->m_pNextSibling   = pNextSibling;
        pNode->m_szNTerm        = 0;

        return pNode;
    }

    TNode* CreateNode(const TNode& Node)
        { return m_Heap.Reserve(Node); }

    void CopyRec(TConstIterator SrcIter, TIterator DstIter);

public:
    TTrie();

    TTrie(const TTrie& Trie)
        { *this = Trie; }

    TTrie& operator = (const TTrie& Trie);

    ~TTrie()
        { Clean(GetRoot()); }

    bool IsEmpty() const
        { return GetRoot().IsLeaf() && !GetRoot().IsTerminal(); }

    void Clear();

    TIterator GetRoot()
        { return m_pRoot; }

    TConstIterator GetRoot() const
        { return m_pRoot; }

    TIterator AddChar(TIterator Parent, size_t szChar);

    TIterator AddTermChar(TIterator Parent, size_t szChar)
    {
        TIterator Iter = AddChar(Parent, szChar);

        Iter.m_pNode->IncNTerm();

        return Iter;
    }

    TIterator Add(LPCTSTR pText, bool& bRNew = temp<bool>())
    {
        assert(pText);

        TIterator Iter;

        for(Iter = GetRoot() ; *pText ; Iter = AddChar(Iter, *pText++));

        bRNew = (Iter.m_pNode->IncNTerm() == 1);

        return Iter;
    }

    void Del(LPCTSTR pText);

    TIterator Find(LPCTSTR pText, size_t szMaxLength = -1)
    {
        TIterator Iter;

        for(Iter = GetRoot() ; *pText && szMaxLength && Iter.IsValid() ; Iter = Iter.FindChild(*pText++), szMaxLength--);

        return Iter.IsValid() && Iter.IsTerminal() ? Iter : NULL;
    }

    TConstIterator Find(LPCTSTR pText, size_t szMaxLength = -1) const
    {
        TConstIterator Iter;

        for(Iter = GetRoot() ; *pText && szMaxLength && Iter.IsValid() ; Iter = Iter.FindChild(*pText++), szMaxLength--);

        return Iter.IsValid() && Iter.IsTerminal() ? Iter : NULL;
    }

    TIterator Get(LPCTSTR pText, size_t szMaxLength = -1)
        { TIterator Iter = Find(pText, szMaxLength); assert(Iter.IsValid()); return Iter; }

    TConstIterator Get(LPCTSTR pText, size_t szMaxLength = -1) const
        { TConstIterator Iter = Find(pText, szMaxLength); assert(Iter.IsValid()); return Iter; }

    ObjectType Get(LPCTSTR pText, const ObjectType& DefaultValue) const
        { TConstIterator Iter = Find(pText); return Iter.IsValid() ? *Iter : DefaultValue; }

    bool Has(LPCTSTR pText, size_t szMaxLength = -1) const
        { return Find(pText, szMaxLength).IsValid(); }

    TObject& operator [] (LPCTSTR pText)
        { return *Get(pText); }

    const TObject& operator [] (LPCTSTR pText) const
        { return *Get(pText); }
};

// ------------
// Trie walker
// ------------
template <class ObjectType, bool bTerminalOnly = true>
class TTrieWalker
{
public:
    typedef TArray<typename TTrie<ObjectType>::TIterator, true> TPath;

private:
     TPath m_Path;

public:
    TTrieWalker(TTrie<ObjectType>& Trie)
    {
        m_Path.Add() = Trie.GetRoot();

        if(bTerminalOnly && !m_Path.GetLastItem().IsTerminal())
            ++*this;
    }

    TTrieWalker& operator ++ ()
    {
        DEBUG_VERIFY(*this);

        do
        {
            if(!m_Path.GetLastItem().IsLeaf())
            {
                m_Path.Add(), m_Path.GetLastItem() = m_Path.GetPreLastItem().GetFirstChild();
            }
            else
            {
                do
                {
                    if(!m_Path.GetLastItem().IsLastChild())
                    {
                        m_Path.GetLastItem().ToNextSibling();
                        break;
                    }

                    m_Path.DelLastNoFix();

                }while(!m_Path.IsEmpty());
            }

            if(!bTerminalOnly)
                break;

        }while(!m_Path.IsEmpty() && !m_Path.GetLastItem().IsTerminal());

        return *this;
    }

    TTrieWalker& ToNextSibling()
    {
        DEBUG_VERIFY(*this);

        do
        {
            do
            {
                if(!m_Path.GetLastItem().IsLastChild())
                {
                    m_Path.GetLastItem().ToNextSibling();
                    break;
                }

                m_Path.DelLastNoFix();

            }while(!m_Path.IsEmpty());

            if(!bTerminalOnly)
                break;

        }while(!m_Path.IsEmpty() && !m_Path.GetLastItem().IsTerminal());

        return *this;
    }

    const TPath& GetInternalPath() const
        { return m_Path; }

    KString GetPath() const
    {
        DEBUG_VERIFY(*this);

        KString String;
        String.Allocate(m_Path.GetN() - 1);

        for(size_t i = 1 ; i < m_Path.GetN() ; i++)
            String.SetChar(i - 1, (TCHAR)m_Path[i].GetChar());

        return String;
    }

    operator bool () const
        { return !m_Path.IsEmpty(); }

    typename TTrie<ObjectType>::TIterator GetIter() const
        { DEBUG_VERIFY(*this); return m_Path.GetLastItem(); }

    ObjectType* operator -> () const
        { DEBUG_VERIFY(*this); return &*m_Path.GetLastItem(); }

    ObjectType& operator * () const
        { DEBUG_VERIFY(*this); return *m_Path.GetLastItem(); }
};

// ------------------
// Const trie walker
// ------------------
template <class ObjectType, bool bTerminalOnly = true>
class TConstTrieWalker
{
public:
    typedef TArray<typename TTrie<ObjectType>::TConstIterator, true> TPath;

private:
    TPath m_Path;

public:
    TConstTrieWalker(const TTrie<ObjectType>& Trie)
    {
        m_Path.Add() = Trie.GetRoot();

        if(bTerminalOnly && !m_Path.GetLastItem().IsTerminal())
            ++*this;
    }

    TConstTrieWalker(const TTrieWalker<ObjectType, bTerminalOnly>& Walker)
        { *this = Walker; }

    TConstTrieWalker& operator = (const TTrieWalker<ObjectType, bTerminalOnly>& Walker)
    {
        kfc_static_assert(sizeof(typename TTrie<ObjectType>::TIterator) == sizeof(typename TTrie<ObjectType>::TConstIterator));

        m_Path.SetN(Walker.GetInternalPath().GetN(), true);

        memcpy( m_Path.GetDataPtr(),
                Walker.GetInternalPath().GetDataPtr(),
                Walker.GetInternalPath().GetN() * sizeof(typename TTrie<ObjectType>::TConstIterator));

        return *this;
    }

    TConstTrieWalker& operator ++ ()
    {
        DEBUG_VERIFY(*this);

        do
        {
            if(!m_Path.GetLastItem().IsLeaf())
            {
                m_Path.Add(), m_Path.GetLastItem() = m_Path.GetPreLastItem().GetFirstChild();
            }
            else
            {
                do
                {
                    if(!m_Path.GetLastItem().IsLastChild())
                    {
                        m_Path.GetLastItem().ToNextSibling();
                        break;
                    }

                    m_Path.DelLastNoFix();

                }while(!m_Path.IsEmpty());
            }

            if(!bTerminalOnly)
                break;

        }while(!m_Path.IsEmpty() && !m_Path.GetLastItem().IsTerminal());

        return *this;
    }

    TConstTrieWalker& ToNextSibling()
    {
        DEBUG_VERIFY(*this);

        do
        {
            do
            {
                if(!m_Path.GetLastItem().IsLastChild())
                {
                    m_Path.GetLastItem().ToNextSibling();
                    break;
                }

                m_Path.DelLastNoFix();

            }while(!m_Path.IsEmpty());

            if(!bTerminalOnly)
                break;

        }while(!m_Path.IsEmpty() && !m_Path.GetLastItem().IsTerminal());

        return *this;
    }

    const TPath& GetInternalPath() const
        { return m_Path; }

    KString GetPath() const
    {
        DEBUG_VERIFY(*this);

        KString String;
        String.Allocate(m_Path.GetN() - 1);

        for(size_t i = 1 ; i < m_Path.GetN() ; i++)
            String.SetChar(i - 1, (TCHAR)m_Path[i].GetChar());

        return String;
    }

    operator bool () const
        { return !m_Path.IsEmpty(); }

    typename TTrie<ObjectType>::TConstIterator GetIter() const
        { DEBUG_VERIFY(*this); return m_Path.GetLastItem(); }

    const ObjectType* operator -> () const
        { DEBUG_VERIFY(*this); return &*m_Path.GetLastItem(); }

    const ObjectType& operator * () const
        { DEBUG_VERIFY(*this); return *m_Path.GetLastItem(); }
};

// -----
// Trie
// -----
template <class ObjectType>
TTrie<ObjectType>::TTrie()
{
    m_Heap.Allocate(TRIE_HEAP_BLOCK_SIZE);

    m_pRoot = CreateNode(0, NULL);
}

template <class ObjectType>
void TTrie<ObjectType>::CopyRec(TConstIterator SrcIter, TIterator DstIter)
{
    TNode** ppPrev = &DstIter.m_pNode->m_pFirstChild;

    FOR_EACH_TRIE_LEVEL(SrcIter, TConstIterator, Iter)
    {
        *ppPrev =   Iter.IsTerminal() ?
                        CreateNode(*Iter.m_pNode) :
                        CreateNode(Iter.GetChar(), NULL);

        CopyRec(Iter, *ppPrev);

        ppPrev = &(*ppPrev)->m_pNextSibling;
    }
}

template <class ObjectType>
TTrie<ObjectType>& TTrie<ObjectType>::operator = (const TTrie& Trie)
{
    if(&Trie == this)
        return *this;

    if(m_Heap.IsAllocated())
        Clean(GetRoot());

    m_Heap.Allocate(TRIE_HEAP_BLOCK_SIZE);

    m_pRoot =   Trie.GetRoot().IsTerminal() ?
                    CreateNode(*Trie.m_pRoot) :
                    CreateNode(0, NULL);

    CopyRec(Trie.GetRoot(), GetRoot());

    return *this;
}

template <class ObjectType>
void TTrie<ObjectType>::Clean(TIterator Iter)
{
    FOR_EACH_TRIE_LEVEL(Iter, TIterator, Iter2)
        Clean(Iter2);

    Iter.m_pNode->ZeroNTerm();

    m_Heap.FreeWithoutDestruction(Iter.m_pNode);
}

template <class ObjectType>
void TTrie<ObjectType>::Clear()
{
    if(!m_Heap.IsAllocated())
        return;

    Clean(GetRoot());

    m_Heap.Allocate(TRIE_HEAP_BLOCK_SIZE);

    m_pRoot = CreateNode(0, NULL);
}

template <class ObjectType>
typename TTrie<ObjectType>::TIterator TTrie<ObjectType>::AddChar(TIterator Parent, size_t szChar)
{
    assert(Parent.IsValid());

    TIterator PIter, Iter;

    for(PIter = NULL, Iter = Parent.GetFirstChild() ; ; PIter = Iter, Iter.ToNextSibling())
    {
        if(!Iter.IsValid() || Iter.GetChar() > szChar)
        {
            TNode* pNewNode = CreateNode(szChar, Iter.m_pNode);

            (PIter.IsValid() ? PIter.m_pNode->m_pNextSibling : Parent.m_pNode->m_pFirstChild) = pNewNode;

            return pNewNode;
        }

        if(Iter.GetChar() == szChar)
            return Iter;
    }
}

template <class ObjectType>
void TTrie<ObjectType>::Del(LPCTSTR pText)
{
    assert(Has(pText));

    TNode*  Path[64];
    TNode** Prev[64];
    size_t  szN = 0;

    TArray<TNode*,  true> ArrayPath;
    TArray<TNode**, true> ArrayPrev;

    TNode* pNode;

    for(pNode = m_pRoot ; *pText ; pText++)
    {
        TNode*  pNode2;
        TNode** ppPrev2;

        for(ppPrev2 = &pNode  -> m_pFirstChild,  pNode2 = *ppPrev2 ; ;
            ppPrev2 = &pNode2 -> m_pNextSibling, pNode2 = *ppPrev2)
        {
            assert(pNode2 && pNode2->m_szChar <= (size_t)*pText);

            if(pNode2->m_szChar == (size_t)*pText)
                break;
        }

        if(szN < ARRAY_SIZE(Path))
        {
            Path[szN] = pNode2;
            Prev[szN] = ppPrev2;
        }
        else if(szN == ARRAY_SIZE(Path))
        {
            memcpy(&ArrayPath.SetN(ARRAY_SIZE(Path) + 1), Path, szN * sizeof(*Path));
            ArrayPath[szN] = pNode2;

            memcpy(&ArrayPrev.SetN(ARRAY_SIZE(Prev) + 1), Prev, szN * sizeof(*Prev));
            ArrayPrev[szN] = ppPrev2;
        }
        else
        {
            ArrayPath.Add() = pNode2;
            ArrayPrev.Add() = ppPrev2;
        }

        szN++;

        pNode = pNode2;
    }

    pNode->DecNTerm();

    if(szN <= ARRAY_SIZE(Path))
    {
        for(size_t i = szN - 1 ; i != -1 ; i--)
        {
            if(!Path[i]->m_szNTerm && !Path[i]->m_pFirstChild)
                *Prev[i] = Path[i]->m_pNextSibling, m_Heap.FreeWithoutDestruction(Path[i]);
        }
    }
    else
    {
        assert(ArrayPath.GetN() == szN);
        assert(ArrayPrev.GetN() == szN);

        for(size_t i = szN - 1 ; i != -1 ; i--)
        {
            if(!ArrayPath[i]->m_szNTerm && !ArrayPath[i]->m_pFirstChild)
                *ArrayPrev[i] = ArrayPath[i]->m_pNextSibling, m_Heap.FreeWithoutDestruction(ArrayPath[i]);
        }
    }
}

#endif // trie_h
