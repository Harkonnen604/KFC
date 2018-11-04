#ifndef sdl_control_h
#define sdl_control_h

#include <KFC_KTL/text_container.h>
#include <KFC_Common/structured_info.h>
#include "sdl_decl.h"

// ------------
// SDL control
// ------------

// SDL control own definition
struct T_SDL_ControlOwnDefinition
{
public:
	size_t m_szID;

	IRECT m_Rect;

	bool m_bVisible;
	bool m_bEnabled;

public:
	T_SDL_ControlOwnDefinition();
};

// SDL control
class T_SDL_Control : private T_SDL_ControlOwnDefinition
{
public:
	// Definition
	struct TDefinition : public T_SDL_ControlOwnDefinition
	{
		virtual ~TDefinition() {}

		virtual TDefinition* CreateCopy() const = 0;

		virtual T_SDL_Control* CreateControl() const = 0;
	};

	// Definitions
	typedef TList<TPtrHolder<TDefinition> > TDefinitions;

private:
	T_SDL_Interface* m_pInterface;

	T_SDL_ControlHandle m_Handle;

	size_t m_szBlockRedrawCount;

	mutable bool m_bHasDelayedRedraw;

private:
	void Attach(T_SDL_Interface& Interface, T_SDL_ControlHandle Handle);
	
	IRECT GetFocusRect(const SZSIZE& ExtFocusSize) const;

protected:
	bool CanDraw() const;

	virtual void OnAttach() {}

	virtual bool OnNeedExtFocus() const
		{ return false; }

	virtual void OnDraw(bool bFocusChange) const = 0;

	virtual void OnKNOB(size_t szKey) {}

	virtual void OnControlTimer(size_t szID) {}

public:
	T_SDL_Control(const TDefinition& Definition);

	virtual ~T_SDL_Control() {}
	
	bool IsAttached() const
		{ return m_pInterface && m_Handle.IsValid(); }

	size_t GetID() const
		{ return m_szID; }

	void SetID(size_t szID)
		{ m_szID = szID; }

	const IRECT& GetRect() const
		{ DEBUG_VERIFY(m_Rect.IsFlatValid()); return m_Rect; }
	
	void SetRect(const IRECT& Rect);

	void SetCoords(const IPOINT& Coords)
		{ SetRect(RectFromCS(Coords, (ISIZE)GetRect())); }

	void Show(bool bShow = true);

	void Hide()
		{ Show(false); }

	void ShowOverlaid(bool bSetFocus);

	void HideOverlaid();

	void Enable(bool bEnable = true);

	void Disable()
		{ Enable(false); }

	void SetFocus();

	void RemoveFocus();

	void Redraw(bool bFocusChange = false) const;

	bool IsRedrawBlocked() const
		{ return m_szBlockRedrawCount; }

	bool HasDelayedRedraw() const
		{ return m_bHasDelayedRedraw; }

	void BlockRedraw()
		{ m_szBlockRedrawCount++; }

	void UnblockRedraw()
	{
		DEBUG_VERIFY(m_szBlockRedrawCount > 0);

		if(!--m_szBlockRedrawCount && m_bHasDelayedRedraw)
			m_bHasDelayedRedraw = false, Redraw();
	}

	void AddTimer(size_t szID, size_t szPeriod);

	bool RemoveTimer(size_t szID);

	void RemoveAllTimers();

	size_t SuspendTimer(size_t szID);

	size_t ResumeTimer(size_t szID);

	void ResetTimer(size_t szID);

	KString GetText() const
	{
		const TTextContainer* pTextContainer = dynamic_cast<const TTextContainer*>(this);
		KFC_VERIFY_WITH_SOURCE(pTextContainer);

		return pTextContainer->GetText();
	}

	void SetText(const KString& Text)
	{
		TTextContainer* pTextContainer = dynamic_cast<TTextContainer*>(this);
		KFC_VERIFY_WITH_SOURCE(pTextContainer);

		pTextContainer->SetText(Text);
	}

	bool DoesNeedExtFocus() const
		{ return OnNeedExtFocus(); }

	T_SDL_Interface& GetInterface() const
		{ DEBUG_VERIFY(IsAttached()); return *m_pInterface; }

	T_SDL_ControlHandle GetHandle() const
		{ DEBUG_VERIFY(m_Handle.IsValid()); return m_Handle; }

	bool IsFocused() const;

	bool IsVisible() const
		{ return m_bVisible; }

	bool IsHidden() const
		{ return !IsVisible(); }

	bool IsEnabled() const
		{ return IsVisible() && m_bEnabled; }

	bool IsDisabled() const
		{ return !IsEnabled(); }

	friend class T_SDL_Interface;

	friend void InvokeSDL_ControlTimer(T_SDL_Control* pControl, size_t szID);
};

// ---------------------------
// SDL control redraw blocker
// ---------------------------
class T_SDL_ControlRedrawBlocker
{
private:
	T_SDL_Control* m_pControl;

public:
	T_SDL_ControlRedrawBlocker(T_SDL_Control* pControl) : m_pControl(pControl)
		{ DEBUG_VERIFY(m_pControl); m_pControl->BlockRedraw(); }

	~T_SDL_ControlRedrawBlocker()
		{ m_pControl->UnblockRedraw(); }
};

// -------------------
// SDL control loader
// -------------------
typedef T_SDL_Control::TDefinition*	T_SDL_ControlLoader(TInfoNodeConstIterator			Node,
														const T_SDL_ResourceStorage&	Storage,
														const T_SDL_ResourceID_Map&		ID_Map,
														const TTokens&					ValueTokens,
														const TTokens&					ColorTokens,
														const TTokens&					StringTokens);

#endif // sdl_control_h
