#ifndef sdl_interface_h
#define sdl_interface_h

#include <KFC_KTL/text_container.h>
#include <KFC_Common/time_globals.h>
#include <KFC_Common/structured_info.h>
#include "sdl_image.h"
#include "sge_font.h"
#include "sdl_common.h"

extern T_SDL_InterfaceEventSink gs_DefaultSDL_InterfaceEventSink;

// -------------------------
// SDL interface definition
// -------------------------
struct T_SDL_InterfaceDefinition
{
public:
	// Extra values
	typedef TTire<KString> TExtraValues;	

public:
	UINT32 m_uiBG_Color;

	const T_SDL_Image* m_pBG_Image;

	UINT32 m_uiExtFocusColor;

	SZSIZE m_ExtFocusSize;

	T_SDL_Control::TDefinitions m_ControlDefs;

	size_t m_szInitialFocusID;

	TExtraValues m_ExtraValues;

public:
	T_SDL_InterfaceDefinition()
		{ Reset(); }

	void Reset();

	void Load(	TInfoNodeConstIterator			Node,
				const T_SDL_ResourceStorage&	Storage,
				const T_SDL_ResourceID_Map&		ID_Map,
				const TTokens&					ValueTokens,
				const TTokens&					ColorTokens,
				const TTokens&					StringTokens);

	T_SDL_Control::TDefinitions::TIterator FindControlDef(size_t szID)
	{
		DEBUG_VERIFY(szID != UINT_MAX);

		FOR_EACH_LIST(m_ControlDefs, T_SDL_Control::TDefinitions::TIterator, Iter)
		{
			if((*Iter)->m_szID == szID)
				return Iter;
		}

		return NULL;
	}

	T_SDL_Control::TDefinitions::TIterator GetControlDef(size_t szID)
	{
		T_SDL_Control::TDefinitions::TIterator Iter = FindControlDef(szID);

		DEBUG_VERIFY(Iter.IsValid());

		return Iter;
	}

	T_SDL_Control::TDefinitions::TConstIterator FindControlDef(size_t szID) const
	{
		DEBUG_VERIFY(szID != UINT_MAX);

		FOR_EACH_LIST(m_ControlDefs, T_SDL_Control::TDefinitions::TConstIterator, Iter)
		{
			if((*Iter)->m_szID == szID)
				return Iter;
		}

		return NULL;
	}

	T_SDL_Control::TDefinitions::TConstIterator GetControlDef(size_t szID) const
	{
		T_SDL_Control::TDefinitions::TConstIterator Iter = FindControlDef(szID);

		DEBUG_VERIFY(Iter.IsValid());

		return Iter;
	}

	bool HasControlDef(size_t szID) const
		{ return FindControlDef(szID).IsValid(); }

	const KString& GetExtraValue(LPCTSTR pName) const
	{
		TExtraValues::TConstIterator Iter = m_ExtraValues.Find(pName);

		if(!Iter.IsValid())
			INITIATE_DEFINED_FAILURE((KString)"SDL interface definition extra value \"" + pName + "\" not found.");

		return *Iter;
	}

	const KString& GetExtraValue(LPCTSTR pName, const KString& DefaultValue) const
	{
		TExtraValues::TConstIterator Iter = m_ExtraValues.Find(pName);

		return Iter.IsValid() ? *Iter : DefaultValue;
	}
};

// -----------------------
// SDL interface password
// -----------------------
struct T_SDL_InterfacePassword
{
public:
	size_t m_szID;

	TArray<size_t, true> m_Sequence;

public:
	T_SDL_InterfacePassword() : m_szID(UINT_MAX) {}

	T_SDL_InterfacePassword(size_t szID) : m_szID(szID)
		{ DEBUG_VERIFY(m_szID != UINT_MAX); }

	T_SDL_InterfacePassword& operator << (size_t szKnob)
		{ m_Sequence << szKnob; return *this; }
};

// ------------------------
// SDL interface passwords
// ------------------------
typedef TArray<T_SDL_InterfacePassword> T_SDL_InterfacePasswords;

// --------------
// SDL interface
// --------------
class T_SDL_Interface
{
private:
	T_SDL_Controls m_Controls;

	T_SDL_InterfaceEventSink* m_pEventSink;

	T_SDL_Timers m_Timers;

	T_SDL_InterfacePasswords m_Passwords;

	TArray<size_t, true> m_CurSeq;
	size_t m_szSeqPos;

	T_SDL_ControlHandle m_Focus;
	UINT32 m_uiBG_Color;

	const T_SDL_Image* m_pBG_Image;

	UINT32 m_uiExtFocusColor;

	SZSIZE m_ExtFocusSize;

	int m_iRetCode;

	bool m_bActive;

	bool m_bActivating;

	size_t m_szBlockRedrawCount;

	mutable bool m_bHasDelayedRedraw;

private:
	void DrawExtFocus(T_SDL_ControlHandle Handle) const;

	void EraseExtFocus(T_SDL_ControlHandle Handle) const;

	void EraseControl(T_SDL_ControlHandle Handle, bool bWithExtFocus, const IRECT* pSubNewRect = NULL)  const;

private:
	void ShowControl(T_SDL_ControlHandle Handle, bool bShow);
	
	void ShowOverlaidControl(T_SDL_ControlHandle Handle, bool bShow, bool bSetFocus);

	void EnableControl(T_SDL_ControlHandle Handle, bool bEnable);	

public:
	T_SDL_ControlHandle GetPrevFocus(T_SDL_ControlHandle Handle);

	T_SDL_ControlHandle GetPrevFocus()
		{ return GetPrevFocus(m_Focus); }

	T_SDL_ControlHandle GetNextFocus(T_SDL_ControlHandle Handle);

	T_SDL_ControlHandle GetNextFocus()
		{ return GetNextFocus(m_Focus); }

public:
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

	bool CanDraw() const
	{
		if(!IsActive() || IsActivating())
			return false;

		if(IsRedrawBlocked())
		{
			m_bHasDelayedRedraw = true;
			return false;
		}

		return true;
	}

public: // methods used by controls
	void DrawBG() const;

	void DrawBG(const IRECT& Rect) const;

	T_SDL_InterfaceEventSink& GetEventSink() const
		{ return *m_pEventSink; }

	T_SDL_Timers& GetTimers()
		{ return m_Timers; }

private:
	void Init();

public:
	T_SDL_Interface()
		{ Init(); }

	T_SDL_Interface(const T_SDL_InterfaceDefinition& Definition)
		{ Init(); Setup(Definition); }

	void Setup(const T_SDL_InterfaceDefinition& Definition);

	void AddPassword(const T_SDL_InterfacePassword& Password);

	UINT32 GetBG_Color() const
		{ return m_uiBG_Color; }

	void SetBG_Color(UINT32 uiColor);
	
	const T_SDL_Image* GetBG_Image() const
		{ return m_pBG_Image; }

	void SetBG_Image(const T_SDL_Image* pImage);

	void Redraw(bool bDrawBG = true) const;

	void Clear();

	T_SDL_ControlHandle AddControl(T_SDL_Control* pControl);

	void RemoveControl(T_SDL_ControlHandle Handle);

	T_SDL_ControlHandle GetFocus() const
		{ return m_Focus; }

	size_t GetFocusID() const
		{ return m_Focus.IsValid() ? (*m_Focus)->GetID() : UINT_MAX; }

	void SetFocus(T_SDL_ControlHandle Handle, bool bFireEvent = false);

	void SetFocus(size_t szID, bool bFireEvent = false)
		{ SetFocus(szID == UINT_MAX ? T_SDL_ControlHandle() : GetControl(szID), bFireEvent); }
	
private:
	size_t HandleEvent(T_KSDL_Event Event, size_t szParam);

public:
	int Activate(T_SDL_InterfaceEventSink& EventSink, const T_SDL_Interface* pPrevInterface);

	void Deactivate(int iCode);

	void AddTimer(size_t szID, size_t szPeriod)
		{ m_Timers.Add(NULL, szID, szPeriod); }

	bool RemoveTimer(size_t szID)
		{ return m_Timers.Remove(NULL, szID); }

	void RemoveAllTimers()
		{ m_Timers.Remove(NULL); }

	size_t SuspendTimer(size_t szID)
		{ return m_Timers.Suspend(NULL, szID); }

	size_t ResumeTimer(size_t szID)
		{ return m_Timers.Resume(NULL, szID); }

	void ResetTimer(size_t szID)
		{ return m_Timers.Reset(NULL, szID); }

	bool IsActive() const
		{ return m_bActive; }

	bool IsActivating() const
		{ return m_bActivating; }

	T_SDL_ControlHandle FindControl(size_t szID);

	T_SDL_ControlHandle GetControl(size_t szID)
	{
		DEBUG_VERIFY(szID != UINT_MAX);

		T_SDL_ControlHandle Handle = FindControl(szID);

		KFC_VERIFY("SDL control not found." && Handle.IsValid());

		return Handle;
	}

	bool HasControl(size_t szID)
		{ return FindControl(szID).IsValid(); }

	void SetControlRect(T_SDL_ControlHandle Handle, const IRECT& Rect);

	void SetControlCoords(T_SDL_ControlHandle Handle, const IPOINT& Coords);
	
	KString GetControlText(size_t szID)
	{
		DEBUG_VERIFY(szID != UINT_MAX);

		const TTextContainer* pTextContainer = dynamic_cast<const TTextContainer*>(&**GetControl(szID));

		KFC_VERIFY("Text container SDL control expected." && pTextContainer);

		return pTextContainer->GetText();
	}

	void SetControlText(size_t szID, const KString& Text)
	{
		DEBUG_VERIFY(szID != UINT_MAX);

		TTextContainer* pTextContainer = dynamic_cast<TTextContainer*>(&**GetControl(szID));

		KFC_VERIFY("Text container SDL control expected." && pTextContainer);

		pTextContainer->SetText(Text);
	}

	T_SDL_Control* operator [] (size_t szID)
		{ return *GetControl(szID); }	

	friend class T_SDL_Control;
};

// -----------------------------
// SDL interface redraw blocker
// -----------------------------
class T_SDL_InterfaceRedrawBlocker
{
private:
	T_SDL_Interface& m_Interface;

public:
	T_SDL_InterfaceRedrawBlocker(T_SDL_Interface& Interface) : m_Interface(Interface)
		{ m_Interface.BlockRedraw(); }

	~T_SDL_InterfaceRedrawBlocker()
		{ m_Interface.UnblockRedraw(); }
};

// ----------------------------
// SDL label highlight manager
// ----------------------------
class T_SDL_LabelHighlightManager
{
private:
	// Item
	struct TItem
	{
	public:
		size_t m_szFocusID;
		size_t m_szLabelID;

	public:
		TItem& Set(size_t szFocusID, size_t szLabelID)
		{
			m_szFocusID = szFocusID;
			m_szLabelID = szLabelID;

			return *this;
		}
	};

private:
	T_SDL_Interface& m_Interface;

	TArray<TItem, true> m_Items;

	size_t m_szHL_LabelID;

public:
	T_SDL_LabelHighlightManager(T_SDL_Interface& Interface);

	void Add(size_t szFocusID, size_t szLabelID)
	{
		DEBUG_VERIFY(szFocusID != UINT_MAX && szLabelID != UINT_MAX);

		DEBUG_VERIFY(m_Interface.HasControl(szFocusID));
		DEBUG_VERIFY(m_Interface.HasControl(szLabelID));

		m_Items.Add().Set(szFocusID, szLabelID);
	}

	void Update();
};

#endif // sdl_interface_h
