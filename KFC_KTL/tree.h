#ifndef tree_h
#define tree_h

#include "basic_types.h"
#include "stream.h"

// -----
// Tree
// -----

// Item
template <class ObjectType>
struct TTreeItem
{
private:
	TTreeItem();

	TTreeItem(const TTreeItem&);

	TTreeItem& operator = (const TTreeItem&);

public:
	ObjectType m_Data;

	TTreeItem*	m_pParent;
	TTreeItem*	m_pPrevSibling;
	TTreeItem*	m_pNextSibling;
	TTreeItem*	m_pFirstChild;
	TTreeItem*	m_pLastChild;
	size_t		m_szNChildren;
	size_t		m_szDepth;

public:
	TTreeItem(	TTreeItem* pParent,
				TTreeItem* pPrevSibling,
				TTreeItem* pNextSibling) :	m_pParent		(pParent),
											m_pPrevSibling	(pPrevSibling),
											m_pNextSibling	(pNextSibling),
											m_pFirstChild	(NULL),
											m_pLastChild	(NULL),
											m_szNChildren	(0),
											m_szDepth		(pParent ? pParent->m_szDepth + 1 : 0) {}

	template <class CreatorType>
	TTreeItem(	TTreeItem*				pParent,
				TTreeItem*				pPrevSibling,
				TTreeItem*				pNextSibling,
				const CreatorType&	Data) :	m_Data			(Data),
											m_pParent		(pParent),
											m_pPrevSibling	(pPrevSibling),
											m_pNextSibling	(pNextSibling),
											m_pFirstChild	(NULL),
											m_pLastChild	(NULL),
											m_szNChildren	(0),
											m_szDepth		(pParent ? pParent->m_szDepth + 1 : 0) {}
};

// Iterator
template <class ObjectType>
struct TTreeIterator
{
public:
	typedef TTreeItem<ObjectType> TItem;

public:
	TItem* m_pItem;

public:
	TTreeIterator() : m_pItem(NULL) {}

	TTreeIterator(TItem* pSItem) : m_pItem(pSItem) {}

	TTreeIterator& FromPVoid(void* pData)
		{ m_pItem = (TItem*)pData; return *this; }

	void* AsPVoid() const
		{ return (void*)m_pItem; }

	bool IsValid() const
		{ return m_pItem != NULL; }

	void Invalidate()
		{ m_pItem = NULL; }

	TTreeIterator GetParent() const
		{ assert(IsValid()); return m_pItem->m_pParent; }

	TTreeIterator GetPrevSibling() const
		{ assert(IsValid()); return m_pItem->m_pPrevSibling; }

	TTreeIterator GetNextSibling() const
		{ assert(IsValid()); return m_pItem->m_pNextSibling; }

	TTreeIterator GetFirstChild() const
		{ assert(IsValid()); return m_pItem->m_pFirstChild; }

	TTreeIterator GetLastChild() const
		{ assert(IsValid()); return m_pItem->m_pLastChild; }

	TTreeIterator GetPrev() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = m_pItem;

		if(p->m_pPrevSibling)
		{
			for(p = p->m_pPrevSibling ; p->m_pLastChild ; p = p->m_pLastChild);

			return p;
		}

		if(p->m_pParent)
			return p->m_pParent;

		return NULL;
	}

	TTreeIterator GetNext() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = m_pItem;

		if(p->m_pFirstChild)
			return p->m_pFirstChild;

		do
		{
			if(p->m_pNextSibling)
				return p->m_pNextSibling;

			p = p->m_pParent;

		}while(p);

		return NULL;
	}

	TTreeIterator GetFirst() const
		{ DEBUG_VERIFY(IsValid()); return *this; }

	TTreeIterator GetLast() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p;

		for(p = m_pItem ; p->m_pLastChild ; p = p->m_pLastChild);

		return p;
	}

	TTreeIterator GetPrev(TTreeIterator Parent) const
	{
		DEBUG_VERIFY(IsValid());

		DEBUG_VERIFY(Parent.IsValid());

		if(m_pItem == Parent.m_pItem)
			return NULL;

		TItem* p = m_pItem;

		if(p->m_pPrevSibling)
		{
			for(p = p->m_pPrevSibling ; p->m_pLastChild ; p = p->m_pLastChild);

			return p;
		}

		if(p->m_pParent)
			return p->m_pParent;

		return NULL;
	}

	TTreeIterator GetNext(TTreeIterator Parent) const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = m_pItem;

		if(p->m_pFirstChild)
			return p->m_pFirstChild;

		do
		{
			if(p == Parent.m_pItem)
				return NULL;

			if(p->m_pNextSibling)
				return p->m_pNextSibling;

			p = p->m_pParent;

		}while(p);

		return NULL;
	}

	TTreeIterator& ToParent()
		{ return *this = GetParent(); }

	TTreeIterator& ToPrevSibling()
		{ return *this = GetPrevSibling(); }

	TTreeIterator& ToNextSibling()
		{ return *this = GetNextSibling(); }

	TTreeIterator& ToFirstChild()
		{ return *this = GetFirstChild(); }

	TTreeIterator& ToLastChild()
		{ return *this = GetLastChild(); }

	TTreeIterator& ToPrev()
		{ return *this = GetPrev(); }

	TTreeIterator& ToNext()
		{ return *this = GetNext(); }

	TTreeIterator& ToFirst()
		{ return *this = GetFirst(); }

	TTreeIterator& ToLast()
		{ return *this = GetLast(); }

	TTreeIterator& ToPrev(TTreeIterator Parent)
		{ return *this = GetPrev(Parent); }

	TTreeIterator& ToNext(TTreeIterator Parent)
		{ return *this = GetNext(Parent); }

	size_t GetNChildren() const
		{ assert(IsValid()); return m_pItem->m_szNChildren; }

	size_t GetDepth() const
		{ assert(IsValid()); return m_pItem->m_szDepth; }

	bool IsLeaf() const
		{ return GetNChildren() == 0; }

	bool IsRoot() const
		{ return !GetParent().IsValid(); }

	ObjectType* operator -> () const
		{ assert(IsValid()); return &m_pItem->m_Data; }

	ObjectType& operator * () const
		{ assert(IsValid()); return m_pItem->m_Data; }
};

// Const iterator
template <class ObjectType>
struct TTreeConstIterator
{
public:
	typedef TTreeItem<ObjectType> TItem;

public:
	TItem* m_pItem;

public:
	TTreeConstIterator() : m_pItem(NULL) {}

	TTreeConstIterator(TItem* pSItem) : m_pItem(pSItem) {}

	TTreeConstIterator(const TTreeIterator<ObjectType>& Iter) : m_pItem(Iter.m_pItem) {}

	TTreeConstIterator& operator = (TTreeIterator<ObjectType> Iter)
		{ m_pItem = Iter.m_pItem; return *this; }

	TTreeConstIterator& FromPVoid(void* pData)
		{ m_pItem = (TItem*)pData; return *this; }

	void* AsPVoid() const
		{ return (void*)m_pItem; }

	bool IsValid() const
		{ return m_pItem != NULL; }

	void Invalidate()
		{ m_pItem = NULL; }

	TTreeConstIterator GetParent() const
		{ assert(IsValid()); return m_pItem->m_pParent; }

	TTreeConstIterator GetPrevSibling() const
		{ assert(IsValid()); return m_pItem->m_pPrevSibling; }

	TTreeConstIterator GetNextSibling() const
		{ assert(IsValid()); return m_pItem->m_pNextSibling; }

	TTreeConstIterator GetFirstChild() const
		{ assert(IsValid()); return m_pItem->m_pFirstChild; }

	TTreeConstIterator GetLastChild() const
		{ assert(IsValid()); return m_pItem->m_pLastChild; }

	TTreeConstIterator GetPrev() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = m_pItem;

		if(p->m_pPrevSibling)
		{
			for(p = p->m_pPrevSibling ; p->m_pLastChild ; p = p->m_pLastChild);

			return p;
		}

		if(p->m_pParent)
			return p->m_pParent;

		return NULL;
	}

	TTreeConstIterator GetNext() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = m_pItem;

		if(p->m_pFirstChild)
			return p->m_pFirstChild;

		do
		{
			if(p->m_pNextSibling)
				return p->m_pNextSibling;

			p = p->m_pParent;

		}while(p);

		return NULL;
	}

	TTreeConstIterator GetFirst() const
		{ DEBUG_VERIFY(IsValid()); return *this; }

	TTreeConstIterator GetLast() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p;

		for(p = m_pItem ; p->m_pLastChild ; p = p->m_pLastChild);

		return p;
	}

	TTreeConstIterator GetPrev(TTreeConstIterator Parent) const
	{
		DEBUG_VERIFY(IsValid());

		DEBUG_VERIFY(Parent.IsValid());

		if(m_pItem == Parent.m_pItem)
			return NULL;

		TItem* p = m_pItem;

		if(p->m_pPrevSibling)
		{
			for(p = p->m_pPrevSibling ; p->m_pLastChild ; p = p->m_pLastChild);

			return p;
		}

		if(p->m_pParent)
			return p->m_pParent;

		return NULL;
	}

	TTreeConstIterator GetNext(TTreeConstIterator Parent) const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = m_pItem;

		if(p->m_pFirstChild)
			return p->m_pFirstChild;

		do
		{
			if(p == Parent.m_pItem)
				return NULL;

			if(p->m_pNextSibling)
				return p->m_pNextSibling;

			p = p->m_pParent;

		}while(p);

		return NULL;
	}

	TTreeConstIterator& ToParent()
		{ return *this = GetParent(); }

	TTreeConstIterator& ToPrevSibling()
		{ return *this = GetPrevSibling(); }

	TTreeConstIterator& ToNextSibling()
		{ return *this = GetNextSibling(); }

	TTreeConstIterator& ToFirstChild()
		{ return *this = GetFirstChild(); }

	TTreeConstIterator& ToLastChild()
		{ return *this = GetLastChild(); }

	TTreeConstIterator& ToPrev()
		{ return *this = GetPrev(); }

	TTreeConstIterator& ToNext()
		{ return *this = GetNext(); }

	TTreeConstIterator& ToFirst()
		{ return *this = GetFirst(); }

	TTreeConstIterator& ToLast()
		{ return *this = GetLast(); }

	TTreeConstIterator& ToPrev(TTreeConstIterator Parent)
		{ return *this = GetPrev(Parent); }

	TTreeConstIterator& ToNext(TTreeConstIterator Parent)
		{ return *this = GetNext(Parent); }

	size_t GetNChildren() const
		{ assert(IsValid()); return m_pItem->m_szNChildren; }

	size_t GetDepth() const
		{ assert(IsValid()); return m_pItem->m_szDepth; }

	bool IsLeaf() const
		{ return GetNChildren() == 0; }

	bool IsRoot() const
		{ return !GetParent().IsValid(); }

	const ObjectType* operator -> () const
		{ assert(IsValid()); return &m_pItem->m_Data; }

	const ObjectType& operator * () const
		{ assert(IsValid()); return m_pItem->m_Data; }
};

// Tree
template <class ObjectType>
class TTree
{
public:
	typedef ObjectType TObject;

	typedef TTreeItem<ObjectType> TItem;

	typedef TTreeIterator<ObjectType> TIterator;

	typedef TTreeConstIterator<ObjectType> TConstIterator;

public:
	// For each routine
	typedef void TForEach(ObjectType& v, void* pParam);

	typedef void TConstForEach(const ObjectType& v, void* pParam);

private:
	TItem* m_pRoot;

private:
	static TIterator Add(TItem* pItem);

	static void CopyBranch(TConstIterator SrcIter, TIterator DstIter);

	static void DelBranch(TIterator Iter);

	static void LoadBranch(TIterator Iter, TStream& Stream);

	static void SaveBranch(TConstIterator Iter, TStream& Stream);

public:
	TTree();

	TTree(const TTree& STree);

	~TTree()
		{ Clear(); }

	bool IsEmpty() const
		{ return !m_pRoot; }

	void Clear()
		{ if(!IsEmpty()) DelRoot(); }

	TIterator AddRoot()
		{ return Add(m_pRoot = new TItem(NULL, NULL, NULL)); }

	template <class CreatorType>
	TIterator AddRoot(const CreatorType& Data)
		{ return Add(m_pRoot = new TItem(NULL, NULL, NULL, Data)); }

	static TIterator Add(TIterator Parent, TIterator PrevSibling, TIterator NextSibling)
		{ return Add(new TItem(Parent.m_pItem, PrevSibling.m_pItem, NextSibling.m_pItem)); }

	template <class CreatorType>
	static TIterator Add(TIterator Parent, TIterator PrevSibling, TIterator NextSibling, const CreatorType& Data)
		{ return Add(new TItem(Parent.m_pItem, PrevSibling.m_pItem, NextSibling.m_pItem, Data)); }

	static TIterator AddChildBefore(TIterator Iter)
	{
		assert(Iter.IsValid());
		assert(!Iter.IsRoot());
		return Add(Iter.GetParent(), Iter.GetPrevSibling(), Iter);
	}

	template <class CreatorType>
	static TIterator AddChildBefore(TIterator Iter, const CreatorType& Data)
	{
		assert(Iter.IsValid());
		assert(!Iter.IsRoot());
		return Add(Iter.GetParent(), Iter.GetPrevSibling(), Iter, Data);
	}

	static TIterator AddChildAfter(TIterator Iter)
	{
		assert(Iter.IsValid());
		assert(!Iter.IsRoot());
		return Add(Iter.GetParent(), Iter, Iter.GetNextSibling());
	}

	template <class CreatorType>
	static TIterator AddChildAfter(TIterator Iter, const CreatorType& Data)
	{
		assert(Iter.IsValid());
		assert(!Iter.IsRoot());
		return Add(Iter.GetParent(), Iter, Iter.GetNextSibling(), Data);
	}

	static TIterator AddFirstChild(TIterator Parent)
		{ return Add(new TItem(Parent.m_pItem, NULL, Parent.GetFirstChild().m_pItem)); }

	template <class CreatorType>
	static TIterator AddFirstChild(TIterator Parent, const CreatorType& Data)
		{ return Add(new TItem(Parent.m_pItem, NULL, Parent.GetFirstChild(), Data)); }

	static TIterator AddLastChild(TIterator Parent)
		{ return Add(new TItem(Parent.m_pItem, Parent.GetLastChild().m_pItem, NULL)); }

	template <class CreatorType>
	static TIterator AddLastChild(TIterator Parent, const CreatorType& Data)
		{ return Add(new TItem(Parent.m_pItem, Parent.GetLastChild().m_pItem, NULL, Data)); }

  static void AddSubTree(TIterator NewParent, TIterator OldParent);

	static void Del(TIterator Iter);

	void DelRoot();

	static void ForEach(TIterator Iter, TForEach* pForEach, void* pParam = NULL);

	static void ForEach(TConstIterator Iter, TConstForEach* pForEach, void* pParam = NULL);

	TIterator GetRoot()
		{ return m_pRoot; }

	TConstIterator GetRoot() const
		{ return m_pRoot; }

	TIterator GetFirst()
		{ return m_pRoot; }

	TConstIterator GetFirst() const
		{ return m_pRoot; }

	TIterator GetLast()
	{
		if(!m_pRoot)
			return NULL;

		TItem* p;

		for(p = m_pRoot ; p->m_pLastChild ; p = p->m_pLastChild);

		return p;
	}

	TConstIterator GetLast() const
	{
		if(!m_pRoot)
			return NULL;

		TItem* p;

		for(p = m_pRoot ; p->m_pLastChild ; p = p->m_pLastChild);

		return p;
	}

	TIterator& ToRoot(TIterator& Iter)
		{ return Iter = GetRoot(); }

	TConstIterator& ToRoot(TConstIterator& Iter)
		{ return Iter = GetRoot(); }

	TIterator& ToFirst(TIterator& Iter)
		{ return Iter = GetFirst(); }

	TConstIterator& ToFirst(TConstIterator& Iter)
		{ return Iter = GetFirst(); }

	TIterator& ToLast(TIterator& Iter)
		{ return Iter = GetLast(); }

	TConstIterator& ToLast(TConstIterator& Iter)
		{ return Iter = GetLast(); }

	TTree& operator = (const TTree& STree);

	TTree& Load(TStream& Stream);

	const TTree& Save(TStream& Stream) const;
};

// Iterator / const iterator
template <class ObjectType>
inline int Compare(TTreeIterator<ObjectType> v1, TTreeIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TTreeIterator<ObjectType> v1, TTreeConstIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TTreeConstIterator<ObjectType> v1, TTreeIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

template <class ObjectType>
inline int Compare(TTreeConstIterator<ObjectType> v1, TTreeConstIterator<ObjectType> v2)
	{ return Compare(v1.AsPVoid(), v2.AsPVoid()); }

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTreeIterator<ObjectType>,		TTreeIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTreeIterator<ObjectType>,		TTreeConstIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTreeConstIterator<ObjectType>,	TTreeIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, TTreeConstIterator<ObjectType>,	TTreeConstIterator<ObjectType>)

// Tree
template <class ObjectType>
TTree<ObjectType>::TTree()
{
	m_pRoot = NULL;
}

template <class ObjectType>
TTree<ObjectType>::TTree(const TTree& STree)
{
	m_pRoot = NULL;

	*this = STree;
}

template <class ObjectType>
typename TTree<ObjectType>::TIterator TTree<ObjectType>::Add(TItem* pItem)
{
	if(pItem->m_pParent)
	{
		assert((pItem->m_pPrevSibling ? pItem->m_pPrevSibling->m_pNextSibling : pItem->m_pParent->m_pFirstChild) == pItem->m_pNextSibling);
		assert((pItem->m_pNextSibling ? pItem->m_pNextSibling->m_pPrevSibling : pItem->m_pParent->m_pLastChild)  == pItem->m_pPrevSibling);

		(pItem->m_pPrevSibling ? pItem->m_pPrevSibling->m_pNextSibling : pItem->m_pParent->m_pFirstChild) = pItem;
		(pItem->m_pNextSibling ? pItem->m_pNextSibling->m_pPrevSibling : pItem->m_pParent->m_pLastChild)  = pItem;

		pItem->m_pParent->m_szNChildren++;
	}

	return pItem;
}

template <class ObjectType>
void TTree<ObjectType>::AddSubTree(TIterator NewParent, TIterator OldParent)
{
  for(TIterator Iter = OldParent.GetFirstChild(); Iter.IsValid(); Iter.ToNextSibling())
    AddSubTree(AddLastChild(NewParent, *Iter), Iter);
}

template <class ObjectType>
void TTree<ObjectType>::DelBranch(TIterator Iter)
{
	TIterator Iter2, NIter2;

	for(Iter2 = Iter.GetLastChild() ; Iter2.IsValid() ; Iter2 = NIter2)
	{
		NIter2 = Iter2.GetPrevSibling();

		DelBranch(Iter2);
	}

	delete Iter.m_pItem;
}

template <class ObjectType>
void TTree<ObjectType>::DelRoot()
{
	assert(!IsEmpty());

	DelBranch(GetRoot());

	m_pRoot = NULL;
}

template <class ObjectType>
void TTree<ObjectType>::Del(TIterator Iter)
{
	assert(Iter.IsValid() && Iter.m_pItem->m_pParent);

	Iter.m_pItem->m_pParent->m_szNChildren--;

	(Iter.m_pItem->m_pPrevSibling ? Iter.m_pItem->m_pPrevSibling->m_pNextSibling : Iter.m_pItem->m_pParent->m_pFirstChild) = Iter.m_pItem->m_pNextSibling;
	(Iter.m_pItem->m_pNextSibling ? Iter.m_pItem->m_pNextSibling->m_pPrevSibling : Iter.m_pItem->m_pParent->m_pLastChild)  = Iter.m_pItem->m_pPrevSibling;

	DelBranch(Iter);
}

template <class ObjectType>
void TTree<ObjectType>::CopyBranch(TConstIterator SrcIter, TIterator DstIter)
{
	*DstIter = *SrcIter;

	for(SrcIter.ToFirstChild() ; SrcIter.IsValid() ; SrcIter.ToNextSibling())
		CopyBranch(SrcIter, AddLastChild(DstIter));
}

template <class ObjectType>
void TTree<ObjectType>::ForEach(TIterator Iter, TForEach* pForEach, void* pParam)
{
	assert(Iter.IsValid() && pForEach);

	pForEach(*Iter, pParam);

	for(Iter.ToFirstChild() ; Iter.IsValid() ; Iter.ToNextSibling())
		ForEach(Iter, pForEach, pParam);
}

template <class ObjectType>
void TTree<ObjectType>::ForEach(TConstIterator Iter, TConstForEach* pForEach, void* pParam)
{
	assert(Iter.IsValid() && pForEach);

	pForEach(*Iter, pParam);

	for(Iter.ToFirstChild() ; Iter.IsValid() ; Iter.ToNextSibling())
		ForEach(Iter, pForEach, pParam);
}

template <class ObjectType>
TTree<ObjectType>& TTree<ObjectType>::operator = (const TTree& STree)
{
	if(this == &STree)
		return *this;

	Clear();

	if(!STree.IsEmpty())
		CopyBranch(STree.GetRoot(), AddRoot());

	return *this;
}

template <class ObjectType>
void TTree<ObjectType>::LoadBranch(TIterator Iter, TStream& Stream)
{
	Stream >> *Iter;

	size_t szN;

	for(szN = ReadVar<size_t>(Stream) ; szN ; szN--)
		LoadBranch(AddLastChild(Iter), Stream);
}

template <class ObjectType>
TTree<ObjectType>& TTree<ObjectType>::Load(TStream& Stream)
{
	Clear();

	if(!ReadVar<bool>(Stream))
		return *this;

	LoadBranch(AddRoot(), Stream);

	return *this;
}

template <class ObjectType>
void TTree<ObjectType>::SaveBranch(TConstIterator Iter, TStream& Stream)
{
	Stream << *Iter;

	Stream << Iter.GetNChildren();

	for(Iter.ToFirstChild() ; Iter.IsValid() ; Iter.ToNextSibling())
		SaveBranch(Iter, Stream);
}

template <class ObjectType>
const TTree<ObjectType>& TTree<ObjectType>::Save(TStream& Stream) const
{
	if(IsEmpty())
	{
		Stream << false;
		return *this;
	}

	Stream << true;

	SaveBranch(GetRoot(), Stream);

	return *this;
}

template <class ObjectType>
inline TStream& operator >> (TStream& Stream, TTree<ObjectType>& RTree)
	{ return RTree.Load(Stream), Stream; }

template <class ObjectType>
inline TStream& operator << (TStream& Stream, const TTree<ObjectType>& Tree)
	{ return Tree.Save(Stream), Stream; }

#endif // tree_h
