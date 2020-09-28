#ifndef for_each_h
#define for_each_h

// ----------------
// For-each macros
// ----------------

// C String
#define FOR_EACH_C_STRING(_s, _i) \
	for(size_t _i = 0 ; (_s)[_i] ; ++_i)

// String
#define FOR_EACH_STRING(_s, _i) \
	for(size_t _i = 0 ; _i < (_s).GetLength() ; ++_i)

// C array
#define FOR_EACH_C_ARRAY_FWD(_array, _index) \
	for(size_t _index = 0 ; _index < ARRAY_SIZE(_array) ; ++_index)

#define FOR_EACH_C_ARRAY_REV(_array, _index) \
	for(size_t _index = ARRAY_SIZE(_array) - 1 ; _index != -1 ; --_index)

#define FOR_EACH_C_ARRAY(_array, _index) \
	FOR_EACH_C_ARRAY_FWD((_array), _index)

// C array (external index)
#define FOR_EACH_C_ARRAY_FWD_EI(_array, _index) \
	for((_index) = 0 ; (_index) < ARRAY_SIZE(_array) ; ++(_index))

#define FOR_EACH_C_ARRAY_REV_EI(_array, _index) \
	for((_index) = ARRAY_SIZE(_array) - 1 ; (size_t)(_index) != -1 ; --(_index))

#define FOR_EACH_C_ARRAY_EI(_array, _index) \
	FOR_EACH_C_ARRAY_FWD_EI((_array), (_index))

// Array
#define FOR_EACH_ARRAY_FWD(_array, _index) \
	for(size_t _index = 0 ; _index < (_array).GetN() ; ++_index)

#define FOR_EACH_ARRAY_REV(_array, _index) \
	for(size_t _index = (_array).GetN() - 1 ; _index != -1 ; --_index)

#define FOR_EACH_ARRAY(_array, _index) \
	FOR_EACH_ARRAY_FWD((_array), _index)

// Array (external index)
#define FOR_EACH_ARRAY_FWD_EI(_array, _index) \
	for((_index) = 0 ; (_index) < (_array).GetN() ; ++(_index))

#define FOR_EACH_ARRAY_REV_EI(_array, _index) \
	for((_index) = (_array).GetN() - 1 ; (size_t)(_index) != -1 ; --(_index))

#define FOR_EACH_ARRAY_EI(_array, _index) \
	FOR_EACH_ARRAY_FWD_EI((_array), (_index))

// List
#define FOR_EACH_LIST_FWD(_list, _iter_type, _iter)				\
	for(_iter_type _iter = (_list).GetFirst(), _niter ;			\
		_iter.IsValid() && (_niter = _iter.GetNext(), true) ;	\
		_iter = _niter)											\

#define FOR_EACH_LIST_REV(_list, _iter_type, _iter)				\
	for(_iter_type _iter = (_list).GetLast(), _piter ;			\
		_iter.IsValid() && (_piter = _iter.GetPrev(), true) ;	\
		_iter = _piter)											\

#define FOR_EACH_LIST(_list, _iter_type, _iter) \
	FOR_EACH_LIST_FWD((_list), _iter_type, _iter)

// List (external iterator)
#define FOR_EACH_LIST_FWD_EI_NODEL(_list, _iter) \
	for((_iter) = (_list).GetFirst() ; (_iter).IsValid() ; ++(_iter))

#define FOR_EACH_LIST_REV_EI_NODEL(_list, _iter) \
	for((_iter) = (_list).GetLast() ; (_iter).IsValid() ; --(_iter))

#define FOR_EACH_LIST_EI_NODEL(_list, _iter) \
	FOR_EACH_LIST_FWD_EI_nodel((_list), (_iter))

// Tree
#define FOR_EACH_TREE_FWD(_tree, _iter_type, _iter)				\
	for(_iter_type _iter = (_tree).GetFirst(), _niter ;			\
		_iter.IsValid() && (_niter = _iter.GetNext(), true) ;	\
		_iter = _niter)											\

#define FOR_EACH_TREE_REV(_tree, _iter_type, _iter)				\
	for(_iter_type _iter = (_tree).GetLast(), _piter ;			\
		_iter.IsValid() && (_piter = _iter.GetPrev(), true) ;	\
		_iter = _piter)											\

#define FOR_EACH_TREE(_tree, _iter_type, _iter) \
	FOR_EACH_TREE_FWD((_tree), _iter_type, _iter)

#define FOR_EACH_TREE_FWD_NODEL(_tree, _iter_type, _iter) \
	for(_iter_type _iter = (_tree).GetFirst() ; _iter.IsValid() ; ++_iter)

#define FOR_EACH_TREE_REV_NODEL(_tree, _iter_type, _iter) \
	for(_iter_type _iter = (_tree).GetLast() ; _iter.IsValid() ; --_iter)

#define FOR_EACH_TREE_NODEL(_tree, _iter_type, _iter) \
	FOR_EACH_TREE_FWD((_tree), _iter_type, _iter)

// Tree (external iterator)
#define FOR_EACH_TREE_FWD_EI_NODEL(_tree, _iter) \
	for((_iter) = (_tree).GetFirst() ; (_iter).IsValid() ; ++(_iter))

#define FOR_EACH_TREE_REV_EI_NODEL(_tree, _iter) \
	for((_iter) = (_tree).GetLast() ; (_iter).IsValid() ; --(_iter))

#define FOR_EACH_TREE_EI_NODEL(_tree, _iter) \
	FOR_EACH_TREE_FWD_EI_NODEL((_tree), (_iter))

// Subtree
#define FOR_EACH_SUBTREE_FWD_NODEL(_parent, _iter_type, _iter) \
	for(_iter_type _iter = (_parent).GetFirst() ; _iter.IsValid() ; _iter.ToNext(_parent))

#define FOR_EACH_SUBTREE_REV_NODEL(_parent, _iter_type, _iter) \
	for(_iter_type _iter = (_parent).GetLast() ; _iter.IsValid() ; _iter.ToPrev(_parent))

#define FOR_EACH_SUBTREE_NODEL(_parent, _iter_type, _iter) \
	FOR_EACH_SUBTREE_FWD_NODEL((_parent), _iter_type, _iter)

// Subtree (external iterator)
#define FOR_EACH_SUBTREE_FWD_EI_NODEL(_parent, _iter) \
	for((_iter) = (_parent).GetFirst() ; (_iter).IsValid() ; (_iter).ToNext(_parent))

#define FOR_EACH_SUBTREE_REV_EI_NODEL(_parent, _iter) \
	for((_iter) = (_parent).GetLast() ; (_iter).IsValid() ; (_iter).ToPrev(_parent))

#define FOR_EACH_SUBTREE_EI_NODEL(_parent, _iter) \
	FOR_EACH_SUBTREE_FWD_EI_NODEL((_parent), (_iter))

// Tree level
#define FOR_EACH_TREE_LEVEL_FWD(_parent, _iter_type, _iter)				\
	for(_iter_type _iter = (_parent).GetFirstChild(), _niter ;			\
		_iter.IsValid() && (_niter = _iter.GetNextSibling(), true) ;	\
		_iter = _niter)													\

#define FOR_EACH_TREE_LEVEL_REV(_parent, _iter_type, _iter)				\
	for(_iter_type _iter = (_parent).GetLastChild(), _piter ;			\
		_iter.IsValid() && (_piter = _iter.GetPrevSibling(), true) ;	\
		_iter = _piter)													\

#define FOR_EACH_TREE_LEVEL(_parent, _iter_type, _iter) \
	FOR_EACH_TREE_LEVEL_FWD((_parent), _iter_type, _iter)

// Tree level (external iterator)
#define FOR_EACH_TREE_LEVEL_FWD_EI_NODEL(_parent, _iter) \
	for((_iter) = (_parent).GetFirstChild() ; (_iter).IsValid() ; (_iter).ToNextSibling())

#define FOR_EACH_TREE_LEVEL_REV_EI_NODEL(_parent, _iter) \
	for((_iter) = (_parent).GetLastChild() ; (_iter).IsValid() ; (_iter).ToPrevSibling())

#define FOR_EACH_TREE_LEVEL_EI_NODEL(_parent, _iter) \
	FOR_EACH_TREE_LEVEL_FWD_EI_NODEL((_parent),  (_iter))

// AVL storage
#define FOR_EACH_AVL_STORAGE_FWD(_storage, _iter_type, _iter)	\
	for(_iter_type _iter = (_storage).GetFirst(), _niter ;		\
		_iter.IsValid() && (_niter = _iter.GetNext(), true) ;	\
		_iter = _niter)											\

#define FOR_EACH_AVL_STORAGE_REV(_storage, _iter_type, _iter)	\
	for(_iter_type _iter = (_storage).GetLast(), _piter ;		\
		_iter.IsValid() && (_piter = _iter.GetPrev(), true) ;	\
		_iter = _piter)											\

#define FOR_EACH_AVL_STORAGE(_storage, _iter_type, _iter) \
	FOR_EACH_AVL_STORAGE_FWD((_storage), _iter_type, _iter)

// AVL storage (external iterator)
#define FOR_EACH_AVL_STORAGE_FWD_EI_NODEL(_storage, _iter) \
	for((_iter) = (_storage).GetFirst() ; (_iter).IsValid() ; ++(_iter))

#define FOR_EACH_AVL_STORAGE_REV_EI_NODEL(_storage, _iter) \
	for((_iter) = (_storage).GetLast() ; (_iter).IsValid() ; --(_iter))

#define FOR_EACH_AVL_STORAGE_EI_NODEL(_storage, _iter) \
	FOR_EACH_AVL_STORAGE_FWD_EI_NODEL((_storage), (_iter))

// Trie
#define FOR_EACH_TRIE(_trie, _walker_type, _walker) \
	for(_walker_type _walker(_trie) ; _walker ; ++_walker)

#define FOR_EACH_TRIE_LEVEL(_parent, _iter_type, _iter) \
	for(_iter_type _iter = (_parent).GetFirstChild(), _niter ;			\
		_iter.IsValid() && (_niter = _iter.GetNextSibling(), true) ;	\
		_iter = _niter)													\

// Trie
#define FOR_EACH_TRIE(_trie, _walker_type, _walker) \
	for(_walker_type _walker(_trie) ; _walker ; ++_walker)

// Trie level
#define FOR_EACH_TRIE_LEVEL(_parent, _iter_type, _iter) \
	for(_iter_type _iter = (_parent).GetFirstChild(), _niter ;			\
		_iter.IsValid() && (_niter = _iter.GetNextSibling(), true) ;	\
		_iter = _niter)													\

// Trie level (external iterator)
#define FOR_EACH_TRIE_LEVEL_EI_NODEL(_parent, _iter_type, _iter) \
	for((_iter) = (_parent).GetFirstChild() ; (_iter).IsValid() ; (_iter).ToNextSibling())

// --------------------------
// Global for-each functions
// --------------------------
template <class t>
void ForEach_Nullifier(t& v, void*)
	{ v = t(); }

template <class t>
void ForEach_Invalidator(t& v, void*)
	{ v.Invalidate(); }

#endif // for_each_h
