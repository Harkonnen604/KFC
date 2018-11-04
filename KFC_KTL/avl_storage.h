#ifndef avl_storage_h
#define avl_storage_h

// ------------
// AVL storage
// ------------

// Item
template <class ObjectType>
struct T_AVL_StorageItem
{
public:
	ObjectType data;

	T_AVL_StorageItem* l;
	T_AVL_StorageItem* r;
	T_AVL_StorageItem* p;

	size_t h;

public:
	template <class CreatorType>
	T_AVL_StorageItem(const CreatorType& sdata) :
		data(sdata), l(NULL), r(NULL), p(NULL), h(0) {}

	template <class CreatorType1, class CreatorType2>
	T_AVL_StorageItem(const CreatorType1& sdata1, const CreatorType2& sdata2) :
		data(sdata1, sdata2), l(NULL), r(NULL), p(NULL), h(0) {}
};

// Iterator
template <class ObjectType>
struct T_AVL_StorageIterator
{
public:
	typedef T_AVL_StorageItem<ObjectType> TItem;

public:
	void* x;

public:
	T_AVL_StorageIterator()
		{ Invalidate(); }

	T_AVL_StorageIterator(void* sx) : x(sx) {}

	bool IsValid() const
		{ return x != NULL; }

	void Invalidate()
		{ x = NULL; }

	ObjectType& operator * () const
		{ DEBUG_VERIFY(IsValid()); return ((TItem*)x)->data; }

	ObjectType* operator -> () const
		{ DEBUG_VERIFY(IsValid()); return &((TItem*)x)->data; }

	T_AVL_StorageIterator GetLeft() const
		{ DEBUG_VERIFY(IsValid()); return ((TItem*)x)->l; }

	T_AVL_StorageIterator GetRight() const
		{ DEBUG_VERIFY(IsValid()); return ((TItem*)x)->r; }

	T_AVL_StorageIterator GetParent() const
		{ DEBUG_VERIFY(IsValid()); return ((TItem*)x)->p; }

	T_AVL_StorageIterator GetPrev() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = (TItem*)x;

		if(p->l)
		{
			for(p = p->l ; p->r ; p = p->r);
		}
		else
		{
			TItem* pp;

			do
			{
				pp = p, p = p->p;

			}while(p && p->r != pp);
		}

		return p;
	}

	T_AVL_StorageIterator GetNext() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = (TItem*)x;

		if(p->r)
		{
			for(p = p->r ; p->l ; p = p->l);
		}
		else
		{
			TItem* pp;

			do
			{
				pp = p, p = p->p;

			}while(p && p->l != pp);
		}

		return p;
	}

	T_AVL_StorageIterator& ToLeft()
		{ DEBUG_VERIFY(IsValid()); x = ((TItem*)x)->l;  return *this;}

	T_AVL_StorageIterator& ToRight()
		{ DEBUG_VERIFY(IsValid()); x = ((TItem*)x)->r;  return *this;}

	T_AVL_StorageIterator& ToParent()
		{ DEBUG_VERIFY(IsValid()); x = ((TItem*)x)->p; return *this; }

	T_AVL_StorageIterator& ToPrev()
		{ return *this = GetPrev(); }

	T_AVL_StorageIterator& ToNext()
		{ return *this = GetNext(); }

	T_AVL_StorageIterator& operator -- ()
		{ return ToPrev(); }

	T_AVL_StorageIterator& operator ++ ()
		{ return ToNext(); }		

	T_AVL_StorageIterator operator -- (int)
		{ const T_AVL_StorageIterator Ret = *this; --*this; return Ret; }

	T_AVL_StorageIterator operator ++ (int)
		{ const T_AVL_StorageIterator Ret = *this; ++*this; return Ret; }

	size_t GetH() const
		{ DEBUG_VERIFY(IsValid()); return ((TItem*)x)->h; }
};

// Const iterator
template <class ObjectType>
struct T_AVL_StorageConstIterator
{
public:
	typedef T_AVL_StorageItem<ObjectType> TItem;

public:
	const void* x;

public:
	T_AVL_StorageConstIterator()
		{ Invalidate(); }

	T_AVL_StorageConstIterator(void* sx) : x(sx) {}

	T_AVL_StorageConstIterator(const T_AVL_StorageIterator<ObjectType>& Iter) : x(Iter.x) {}

	T_AVL_StorageConstIterator& operator = (T_AVL_StorageIterator<ObjectType> Iter)
		{ x = Iter.x; return *this; }

	bool IsValid() const
		{ return x != NULL; }

	void Invalidate()
		{ x = NULL; }

	const ObjectType& operator * () const
		{ DEBUG_VERIFY(IsValid()); return ((const TItem*)x)->data; }

	const ObjectType* operator -> () const
		{ DEBUG_VERIFY(IsValid()); return &((const TItem*)x)->data; }

	T_AVL_StorageConstIterator GetLeft() const
		{ DEBUG_VERIFY(IsValid()); return ((const TItem*)x)->l; }

	T_AVL_StorageConstIterator GetRight() const
		{ DEBUG_VERIFY(IsValid()); return ((const TItem*)x)->r; }

	T_AVL_StorageConstIterator GetParent() const
		{ DEBUG_VERIFY(IsValid()); return ((const TItem*)x)->p; }

	T_AVL_StorageConstIterator GetPrev() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = (TItem*)x;

		if(p->l)
		{
			for(p = p->l ; p->r ; p = p->r);
		}
		else
		{
			TItem* pp;

			do
			{
				pp = p, p = p->p;

			}while(p && p->r != pp);
		}

		return p;
	}

	T_AVL_StorageConstIterator GetNext() const
	{
		DEBUG_VERIFY(IsValid());

		TItem* p = (TItem*)x;

		if(p->r)
		{
			for(p = p->r ; p->l ; p = p->l);
		}
		else
		{
			TItem* pp;

			do
			{
				pp = p, p = p->p;

			}while(p && p->l != pp);
		}

		return p;
	}

	T_AVL_StorageConstIterator& ToLeft()
		{ DEBUG_VERIFY(IsValid()); x = ((const TItem*)x)->l;  return *this;}

	T_AVL_StorageConstIterator& ToRight()
		{ DEBUG_VERIFY(IsValid()); x = ((const TItem*)x)->r;  return *this;}

	T_AVL_StorageConstIterator& ToParent()
		{ DEBUG_VERIFY(IsValid()); x = ((const TItem*)x)->p; return *this; }

	T_AVL_StorageConstIterator& ToPrev()
		{ return *this = GetPrev(); }

	T_AVL_StorageConstIterator& ToNext()
		{ return *this = GetNext(); }

	T_AVL_StorageConstIterator& operator -- ()
		{ return ToPrev(); }

	T_AVL_StorageConstIterator& operator ++ ()
		{ return ToNext(); }		

	T_AVL_StorageConstIterator operator -- (int)
		{ const T_AVL_StorageConstIterator Ret = *this; --*this; return Ret; }

	T_AVL_StorageConstIterator operator ++ (int)
		{ const T_AVL_StorageConstIterator Ret = *this; ++*this; return Ret; }

	size_t GetH() const
		{ DEBUG_VERIFY(IsValid()); return ((const TItem*)x)->h; }
};

// AVL storage
template <class ObjectType>
class T_AVL_Storage
{
public:
	typedef ObjectType TObject;
	
	typedef T_AVL_StorageItem<ObjectType> TItem;
	
	typedef T_AVL_StorageIterator<ObjectType> TIterator;
	
	typedef T_AVL_StorageConstIterator<ObjectType> TConstIterator;

private:
	TItem* root;

	size_t n;

private:
	void Clean(TItem* x);

	void Fix(TItem* x);

	// Update proc
	typedef void TUpdateProc(TIterator Iter);

private:
	template <class KeyType>
	void Attach(TIterator i, const KeyType& key);

	void Detach(TIterator i);

	static void CopyBranch(const TItem* pItem1, TItem* pItem2);

private:
	TUpdateProc* m_pUpdateProc;

public:
	T_AVL_Storage(TUpdateProc* pUpdateProc = NULL);

	T_AVL_Storage(const T_AVL_Storage& Storage);

	~T_AVL_Storage()
		{ Clear(); }

	size_t GetN() const
		{ return n; }

	size_t GetH() const
		{ return IsEmpty() ? 0 : GetRoot().GetH(); }

	bool IsEmpty() const
		{ return root == NULL; }

	void Clear();

	template <class KeyType>
	TIterator Find(const KeyType& key);

	template <class KeyType>
	TConstIterator Find(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->Find(key); }

	template <class KeyType>
	TIterator Get(const KeyType& key)
	{
		TIterator Iter = Find(key);

		DEBUG_VERIFY(Iter.IsValid());

		return Iter;
	}

	template <class KeyType>
	TConstIterator Get(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->Get(key); }

	template <class KeyType>
	ObjectType& operator [] (const KeyType& key)
		{ return *Get(key); }

	template <class KeyType>
	const ObjectType& operator [] (const KeyType& key) const
		{ return *Get(key); }

	template <class KeyType>
	TIterator FindFirst(const KeyType& key);

	template <class KeyType>
	TConstIterator FindFirst(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->FindFirst(key); }

	template <class KeyType>
	TIterator FindFirstGreater(const KeyType& key);

	template <class KeyType>
	TConstIterator FindFirstGreater(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->FindFirstGreater(key); }

	template <class KeyType>
	TIterator FindFirstGreaterEq(const KeyType& key);

	template <class KeyType>
	TConstIterator FindFirstGreaterEq(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->FindFirstGreaterEq(key); }

	template <class KeyType>
	TIterator FindLast(const KeyType& key);

	template <class KeyType>
	TConstIterator FindLast(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->FindLast(key); }

	template <class KeyType>
	TIterator FindLastLess(const KeyType& key);

	template <class KeyType>
	TConstIterator FindLastLess(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->FindLastLess(key); }

	template <class KeyType>
	TIterator FindLastLessEq(const KeyType& key);

	template <class KeyType>
	TConstIterator FindLastLessEq(const KeyType& key) const
		{ return (const_cast<T_AVL_Storage*>(this))->FindLastLessEq(key); }

	template <class KeyType>
	bool Has(const KeyType& key) const
		{ return Find(key).IsValid(); }

	template <class CreatorType>
	TIterator Add(const CreatorType& data)
	{
		TItem* p = new TItem(data);

		n++;

		Attach(p, data);

		return p;
	}

	template <class CreatorType1, class CreatorType2>
	TIterator Add(const CreatorType1& data1, const CreatorType2& data2)
	{
		TItem* p = new TItem(data1, data2);

		n++;

		Attach(p, data1);

		return p;
	}

	template <class CreatorType>
	T_AVL_Storage& operator << (const CreatorType& data)
		{ return Add(data), *this; }

	template <class CreatorType>
	TIterator FindOrAdd(const CreatorType& data);

	template <class CreatorType1, class CreatorType2>
	TIterator FindOrAdd(const CreatorType1& data1, const CreatorType2& data2);

	template <class KeyType>
	void ModifyArbitraryWithHint(TIterator i, const KeyType& Key)
	{
		DEBUG_VERIFY(i.IsValid());

		Detach(i);

		Attach(i, Key);
	}

	template <class KeyType>
	void ModifyArbitrary(TIterator i)
		{ ModifyArbitraryWithHint(i, KeyType(*i)); }

	void Modify(TIterator i)
	{
		DEBUG_VERIFY(i.IsValid());

		Detach(i);

		Attach(i, *i);
	}

	void Modify(const TIterator* pIters, size_t n)
	{
		DEBUG_VERIFY(!n || pIters);

		#ifdef _DEBUG
		{
			for(size_t i = 0 ; i < n ; i++)
				DEBUG_VERIFY(pIters[i].IsValid());
		}
		#endif // _DEBUG

		for(size_t i = n - 1 ; i != UINT_MAX ; i--)
			Detach(pIters[i]);

		for(size_t i = 0 ; i < n ; i++)
			Attach(pIters[i], *pIters[i]);
	}

	void Del(TIterator i)
	{
		DEBUG_VERIFY(i.IsValid());

		n--;

		Detach(i);

		delete (TItem*)i.x;
	}

	void Upd(TIterator i);

	TIterator GetRoot()
		{ return root; }

	TConstIterator GetRoot() const
		{ return (const_cast<T_AVL_Storage*>(this))->GetRoot(); }

	TIterator GetFirst();

	TConstIterator GetFirst() const
		{ return (const_cast<T_AVL_Storage*>(this))->GetFirst(); }

	TIterator GetLast();

	TConstIterator GetLast() const
		{ return (const_cast<T_AVL_Storage*>(this))->GetLast(); }

	T_AVL_Storage& operator = (const T_AVL_Storage& Storage);	
};

template <class ObjectType>
T_AVL_Storage<ObjectType>::T_AVL_Storage(TUpdateProc* pUpdateProc) :
	m_pUpdateProc(pUpdateProc)
{
	root = NULL;
	
	n = 0;
}

template <class ObjectType>
T_AVL_Storage<ObjectType>::T_AVL_Storage(const T_AVL_Storage& Storage) :
	m_pUpdateProc(NULL)
{
	root = NULL;

	n = 0;

	*this = Storage;
}

template <class ObjectType>
void T_AVL_Storage<ObjectType>::Clean(TItem* x)
{
	if(x == NULL)
		return;

	Clean(x->l), Clean(x->r);
	
	delete x;
}

template <class ObjectType>
void T_AVL_Storage<ObjectType>::Fix(TItem* x)
{
	while(x)
	{
		TItem* const p = x->p;

		const size_t h1 = x->l ? x->l->h : 0;
		const size_t h2 = x->r ? x->r->h : 0;

		if(h1 > h2 + 1 || h1 == h2 + 1 && p && p->r == x) // right rotation
		{
			TItem* const a = x->l;
			TItem* const b = a->r;

			a->r = x, x->p = a;

			if(p)
			{
				if(p->l == x)
				{
					p->l = a;
				}
				else
				{
					DEBUG_VERIFY(p->r == x);

					p->r = a;
				}
			}
			else
			{
				root = a;
			}

			a->p = p;

			if(x->l = b)
				b->p = x, x = b;
		}
		else if(h2 > h1 + 1 || h2 == h1 + 1 && p && p->l == x) // left rotation
		{
			TItem* const a = x->r;
			TItem* const b = a->l;

			a->l = x, x->p = a;			

			if(p)
			{
				if(p->l == x)
				{
					p->l = a;
				}
				else
				{
					DEBUG_VERIFY(p->r == x);

					p->r = a;
				}
			}
			else
			{
				root = a;
			}

			a->p = p;

			if(x->r = b)
				b->p = x, x = b;
		}
		else
		{
			x->h = Max(h1, h2) + 1;

			if(m_pUpdateProc)
				m_pUpdateProc(x);

			x = p;
		}
	}
}

template <class ObjectType>
void T_AVL_Storage<ObjectType>::Clear()
{
	Clean(root);

	root = NULL, n = 0;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::Find(const KeyType& key)
{
	TItem* x = root;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d > 0)
			x = x->l;
		else if(d < 0)
			x = x->r;
		else
			return x;
	}

	return NULL;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindFirst(const KeyType& key)
{
	TItem* x = root;
	TItem* y = NULL;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d > 0)
			x = x->l;
		else if(d < 0)
			x = x->r;
		else
			y = x, x = x->l;
	}

	return y;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindFirstGreater(const KeyType& key)
{
	TItem* x = root;
	TItem* y = NULL;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d <= 0)
			x = x->r;
		else if(d > 0)
			y = x, x = x->l;
	}

	return y;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindFirstGreaterEq(const KeyType& key)
{
	TItem* x = root;
	TItem* y = NULL;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d >= 0)
			y = x, x = x->l;
		else
			x = x->r;
	}

	return y;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindLast(const KeyType& key)
{
	TItem* x = root;
	TItem* y = NULL;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d > 0)
			x = x->l;
		else if(d < 0)
			x = x->r;
		else
			y = x, x = x->r;
	}

	return y;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindLastLess(const KeyType& key)
{
	TItem* x = root;
	TItem* y = NULL;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d >= 0)
			x = x->l;
		else
			y = x, x = x->r;		
	}

	return y;
}

template <class ObjectType>
template <class KeyType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindLastLessEq(const KeyType& key)
{
	TItem* x = root;
	TItem* y = NULL;

	while(x)
	{
		const int d = Compare(x->data, key);

		if(d <= 0)
			y = x, x = x->r;
		else
			x = x->l;
	}

	return y;
}

template <class ObjectType>
template <class KeyType>
void T_AVL_Storage<ObjectType>::Attach(TIterator i, const KeyType& key)
{
	TItem* p = (TItem*)i.x;

	p->l = p->r = NULL;	

	if(!root)
	{
		p->p = NULL, root = p;
	}
	else
	{
		TItem* x = root;

		for(;;)
		{
			const int d = Compare(x->data, key);

			if(d >= 0)
			{
				if(x->l == NULL)
				{
					p->p = x, x->l = p;
					break;
				}

				x = x->l;
			}
			else
			{
				if(x->r == NULL)
				{
					p->p = x, x->r = p;
					break;
				}

				x = x->r;
			}
		}
	}

	Fix(p);
}

template <class ObjectType>
void T_AVL_Storage<ObjectType>::Detach(TIterator i)
{
	TItem* x = (TItem*)i.x;
	TItem* p = x->p;
	TItem* y;	

	if(y = x->l)
	{
		if(p)
		{
			if(p->l == x)
			{
				p->l = y;
			}
			else
			{
				DEBUG_VERIFY(p->r == x);

				p->r = y;
			}
		}
		else
		{
			root = y;
		}

		y->p = p;

		while(y->r)
			y = y->r;

		if(y->r = x->r)
			x->r->p = y;		

		Fix(y);
	}
	else if(y = x->r)
	{
		if(p)
		{
			if(p->l == x)
			{
				p->l = y;
			}
			else
			{
				DEBUG_VERIFY(p->r == x);

				p->r = y;
			}
		}
		else
		{
			root = y;
		}

		y->p = p;

		while(y->l)
			y = y->l;

		if(y->l = x->l)
			x->l->p = y;

		Fix(y);
	}
	else
	{
		if(p)
		{
			if(p->l == x)
			{
				p->l = NULL;
			}
			else
			{
				DEBUG_VERIFY(p->r == x);

				p->r = NULL;
			}

			Fix(p);
		}
		else
		{
			root = NULL;
		}
	}
}

template <class ObjectType>
template <class CreatorType>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindOrAdd(const CreatorType& data)
{
	TIterator Iter = Find(data);

	if(!Iter.IsValid())
		Iter = Add(data);

	return Iter;
}

template <class ObjectType>
template <class CreatorType1, class CreatorType2>
typename T_AVL_Storage<ObjectType>::TIterator
	T_AVL_Storage<ObjectType>::FindOrAdd(const CreatorType1& data1, const CreatorType2& data2)
{
	TIterator Iter = Find(data1);

	if(!Iter.IsValid())
		Iter = Add(data1, data2);

	return Iter;
}

template <class ObjectType>
void T_AVL_Storage<ObjectType>::Upd(TIterator i)
{
	DEBUG_VERIFY(i.IsValid());

	if(!m_pUpdateProc)
		return;	

	for( ; i.IsValid() ; i.ToParent())
		m_pUpdateProc(i);
}

template <class ObjectType>
typename T_AVL_Storage<ObjectType>::TIterator T_AVL_Storage<ObjectType>::GetFirst()
{
	if(IsEmpty())
		return NULL;

	TItem* x = root;

	while(x->l)
		x = x->l;

	return x;
}

template <class ObjectType>
typename T_AVL_Storage<ObjectType>::TIterator T_AVL_Storage<ObjectType>::GetLast()
{
	if(IsEmpty())
		return NULL;

	TItem* x = root;

	while(x->r)
		x = x->r;

	return x;
}

template <class ObjectType>
void T_AVL_Storage<ObjectType>::CopyBranch(const TItem* pSrcItem, TItem* pDstItem)
{
	TItem* pItem;

	pDstItem->h = pSrcItem->h;

	if(pSrcItem->l)
	{
		pItem = new TItem(pSrcItem->l->data), pItem->p = pDstItem;

		pDstItem->l = pItem;

		CopyBranch(pSrcItem->l, pDstItem->l);
	}

	if(pSrcItem->r)
	{
		pItem = new TItem(pSrcItem->r->data), pItem->p = pDstItem;

		pDstItem->r = pItem;

		CopyBranch(pSrcItem->r, pDstItem->r);
	}
}

template <class ObjectType>
T_AVL_Storage<ObjectType>& T_AVL_Storage<ObjectType>::operator =
	(const T_AVL_Storage<ObjectType>& Storage)
{
	if(&Storage == this)
		return *this;

	Clear();

	m_pUpdateProc = Storage.m_pUpdateProc;

	if(Storage.root)
	{
		CopyBranch(Storage.root, root = new TItem(Storage.root->data));

		n = Storage.n;
	}

	return *this;
}

template <class ObjectType>
inline int Compare(T_AVL_StorageIterator<ObjectType> Iter1, T_AVL_StorageIterator<ObjectType> Iter2)
{
	return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare(T_AVL_StorageIterator<ObjectType> Iter1, T_AVL_StorageConstIterator<ObjectType> Iter2)
{
	return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare(T_AVL_StorageConstIterator<ObjectType> Iter1, T_AVL_StorageIterator<ObjectType> Iter2)
{
	return Compare(Iter1.x, Iter2.x);
}

template <class ObjectType>
inline int Compare(T_AVL_StorageConstIterator<ObjectType> Iter1, T_AVL_StorageConstIterator<ObjectType> Iter2)
{
	return Compare(Iter1.x, Iter2.x);
}

DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, T_AVL_StorageIterator<ObjectType>,		T_AVL_StorageIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, T_AVL_StorageIterator<ObjectType>,		T_AVL_StorageConstIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, T_AVL_StorageConstIterator<ObjectType>,	T_AVL_StorageIterator<ObjectType>)
DECLARE_TEMPLATE_COMPARISON_OPERATORS(class ObjectType, T_AVL_StorageConstIterator<ObjectType>,	T_AVL_StorageConstIterator<ObjectType>)

#endif // avl_storage_h
