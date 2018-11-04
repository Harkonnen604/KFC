#ifndef sdl_tls_item_h
#define sdl_tls_item_h

#include <KFC_KTL/tls_storage.h>

// Select SDL list item callback
typedef void TSelelectSDL_ListItemCallback(size_t szOldIndex, size_t szNewIndex, void* pParam);

// -------------
// SDL TLS item
// -------------
struct T_SDL_TLS_Item : public T_TLS_Storage::TItem
{
private:
	static size_t ms_szIndex;

private:
	static T_TLS_Storage::TItem* Creator()
		{ return new T_SDL_TLS_Item; }

public:
	bool* m_pCurSDL_MessageLoopBreaker;

	TSelelectSDL_ListItemCallback* m_pSelectListItemCallback;
	void* m_pSelectListItemCallbackParam; 

public:
	T_SDL_TLS_Item()
	{
		m_pCurSDL_MessageLoopBreaker = NULL;

		m_pSelectListItemCallback = NULL;
		m_pSelectListItemCallbackParam = NULL;
	}

	static void Register()
	{
		if(ms_szIndex == UINT_MAX)
			ms_szIndex = g_TLS_Storage.ReserveItemType(Creator);
	}

	static void Free()
	{
		if(ms_szIndex != UINT_MAX)
			g_TLS_Storage.FreeItemType(ms_szIndex);
	}

	static T_SDL_TLS_Item& Get()
		{ return (T_SDL_TLS_Item&)g_TLS_Storage[ms_szIndex]; }
};

// ----------------
// Global routines
// ----------------
inline TSelelectSDL_ListItemCallback* GetThreadSelectSDL_ListCallback()
{
	return T_SDL_TLS_Item::Get().m_pSelectListItemCallback;
}

inline void* GetThreadSelectSDL_ListItemCallbackParam()
{
	return T_SDL_TLS_Item::Get().m_pSelectListItemCallbackParam;
}

// ---------------------------------------
// Thread SDL message loop breaker setter
// ---------------------------------------
class TThreadSDL_MessageLoopBreakerSetter
{
private:
	bool* m_pOldBreaker;

public:
	TThreadSDL_MessageLoopBreakerSetter(bool& bBreaker)
	{
		T_SDL_TLS_Item& Item = T_SDL_TLS_Item::Get();

		m_pOldBreaker = Item.m_pCurSDL_MessageLoopBreaker;

		Item.m_pCurSDL_MessageLoopBreaker = &bBreaker;
	}

	~TThreadSDL_MessageLoopBreakerSetter()
	{
		T_SDL_TLS_Item::Get().m_pCurSDL_MessageLoopBreaker = m_pOldBreaker;
	}
};

// --------------------------------------------
// Thread SDL list select item callback setter
// --------------------------------------------
class TThreadSelectSDL_ListItemCallbackSetter
{
private:
	TSelelectSDL_ListItemCallback* m_pOldCallback;
	void* m_pOldCallbackParam;

public:
	TThreadSelectSDL_ListItemCallbackSetter(TSelelectSDL_ListItemCallback* pCallback, void* pCallbackParam)
	{
		T_SDL_TLS_Item& Item = T_SDL_TLS_Item::Get();

		m_pOldCallback		= Item.m_pSelectListItemCallback;
		m_pOldCallbackParam	= Item.m_pSelectListItemCallbackParam;

		Item.m_pSelectListItemCallback		= pCallback;
		Item.m_pSelectListItemCallbackParam	= pCallbackParam;
	}

	~TThreadSelectSDL_ListItemCallbackSetter()
	{
		T_SDL_TLS_Item& Item = T_SDL_TLS_Item::Get();

		Item.m_pSelectListItemCallback		= m_pOldCallback;
		Item.m_pSelectListItemCallbackParam	= m_pOldCallbackParam;
	}
};

#endif // sdl_tls_item_h
