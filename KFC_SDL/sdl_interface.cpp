#include "kfc_sdl_pch.h"
#include "sdl_interface.h"

#include "sdl_consts.h"
#include "basic_controls.h"
#include "sdl_resource_storage.h"

T_SDL_InterfaceEventSink gs_DefaultSDL_InterfaceEventSink;

// -------------------------
// SDL interface definition
// -------------------------
void T_SDL_InterfaceDefinition::Reset()
{
	m_uiBG_Color = g_SDL_Consts.m_uiDefaultInterfaceBG_Color;

	m_pBG_Image = NULL;

	m_uiExtFocusColor = g_SDL_Consts.m_uiDefaultInterfaceExtFocusColor;

	m_ExtFocusSize = g_SDL_Consts.m_DefaultInterfaceExtFocusSize;

	m_ControlDefs.Clear();

	m_szInitialFocusID = UINT_MAX;

	m_ExtraValues.Clear();	
}

void T_SDL_InterfaceDefinition::Load(	TInfoNodeConstIterator			Node,
										const T_SDL_ResourceStorage&	Storage,
										const T_SDL_ResourceID_Map&		ID_Map,
										const TTokens&					ValueTokens,
										const TTokens&					ColorTokens,
										const TTokens&					StringTokens)
{
	Reset();

	FOR_EACH_TREE_LEVEL(Node, TInfoNodeConstIterator, CNode)
	{
		m_ControlDefs <<
			g_SDL_DeviceGlobals.GetControlLoader(CNode->m_Name)
			(CNode, Storage, ID_Map, ValueTokens, ColorTokens, StringTokens);
	}

	FOR_EACH_LIST(Node->m_Parameters, TInfoParameterConstIterator, PIter)
	{
		if(PIter->m_Name == "BG_Color")
		{
			m_uiBG_Color = ReadRGB(ColorTokens(PIter->m_Value));
		}
		else if(PIter->m_Name == "BG_Image")
		{
			KFC_VERIFY(m_pBG_Image = Storage.GetImage(ID_Map[PIter->m_Value]));
		}
		else if(PIter->m_Name == "ExtFocusColor")
		{
			m_uiExtFocusColor = ReadRGB(ColorTokens(PIter->m_Value));
		}
		else if(PIter->m_Name == "ExtFocusSize")
		{
			m_ExtFocusSize = ReadFromString<SZSIZE>(ValueTokens(PIter->m_Value));
		}
		else if(PIter->m_Name == "InitialFocus")
		{
			m_szInitialFocusID = ID_Map[PIter->m_Value];

			KFC_VERIFY(m_szInitialFocusID != UINT_MAX);

			T_SDL_Control::TDefinition* pFocusControlDef = NULL;

			FOR_EACH_LIST(m_ControlDefs, T_SDL_Control::TDefinitions::TConstIterator, Iter)
			{
				if((*Iter)->m_szID == m_szInitialFocusID)
				{
					pFocusControlDef = *Iter;
					break;
				}
			}

			if(!pFocusControlDef)
				INITIATE_DEFINED_FAILURE((KString)"Invalid initial focus control ID: \"" + PIter->m_Value + "\".");

			if(!pFocusControlDef->m_bVisible)
				INITIATE_DEFINED_FAILURE((KString)"Attempt to set initial focus to invisible control with ID \"" + PIter->m_Value + "\".");

			if(!pFocusControlDef->m_bEnabled)
				INITIATE_DEFINED_FAILURE((KString)"Attempt to set initial focus to disabled control with ID \"" + PIter->m_Value + "\".");
		}
		else
		{
			*m_ExtraValues.Add(PIter->m_Name) = ValueTokens(ColorTokens(StringTokens(PIter->m_Value)));
		}
	}
}

// --------------
// SDL interface
// --------------
void T_SDL_Interface::Init()
{
	m_pEventSink = &gs_DefaultSDL_InterfaceEventSink;

	m_Focus.Invalidate();

	m_uiBG_Color = g_SDL_Consts.m_uiDefaultInterfaceBG_Color;

	m_pBG_Image = NULL;

	m_uiExtFocusColor = g_SDL_Consts.m_uiDefaultInterfaceExtFocusColor;

	m_ExtFocusSize = g_SDL_Consts.m_DefaultInterfaceExtFocusSize;

	m_bActive = false;

	m_bActivating = false;

	m_szBlockRedrawCount = 0;
	
	m_bHasDelayedRedraw = false;
}

void T_SDL_Interface::Setup(const T_SDL_InterfaceDefinition& Definition)
{
	DEBUG_VERIFY(!IsActive() && !IsActivating());

	Clear();

	SetBG_Color(Definition.m_uiBG_Color);

	SetBG_Image(Definition.m_pBG_Image);

	m_uiExtFocusColor = Definition.m_uiExtFocusColor;

	m_ExtFocusSize = Definition.m_ExtFocusSize;

	FOR_EACH_LIST(Definition.m_ControlDefs, T_SDL_Control::TDefinitions::TConstIterator, Iter)
		AddControl((*Iter)->CreateControl());

	SetFocus(Definition.m_szInitialFocusID);
}

void T_SDL_Interface::DrawBG() const
{
	if(m_pBG_Image)
		BlitSDL_Image(*m_pBG_Image, g_pSDL_FB, IPOINT(0, 0));
	else
		FillSDL_Image(g_pSDL_FB, m_uiBG_Color);
}

void T_SDL_Interface::DrawBG(const IRECT& Rect) const
{
	if(m_pBG_Image)
		BlitSDL_Image(*m_pBG_Image, Rect, g_pSDL_FB, Rect.GetTopLeft());
	else
		FillSDL_Image(g_pSDL_FB, Rect, m_uiBG_Color);
}

void T_SDL_Interface::DrawExtFocus(T_SDL_ControlHandle Handle) const
{
	if(!Handle.IsValid() || !(*Handle)->DoesNeedExtFocus() || !(*Handle)->IsEnabled())
		return;

	IRECT  Rects[4];
	size_t szN;

	IRECT FocusRect = (*Handle)->GetFocusRect(m_ExtFocusSize);

	szN = SubtractRect(FocusRect, (*Handle)->GetRect(), Rects);

	for(size_t i = 0 ; i < szN ; i++)
		FillSDL_Image(g_pSDL_FB, Rects[i], m_uiExtFocusColor);
}

void T_SDL_Interface::EraseExtFocus(T_SDL_ControlHandle Handle) const
{
	if(!Handle.IsValid() || !(*Handle)->DoesNeedExtFocus() || !(*Handle)->IsEnabled())
		return;

	IRECT  Rects[4];
	size_t szN;
	
	IRECT FocusRect = (*Handle)->GetFocusRect(m_ExtFocusSize);

	szN = SubtractRect(FocusRect, (*Handle)->GetRect(), Rects);
	
	for(size_t i = 0 ; i < szN ; i++)
		DrawBG(Rects[i]);
}

void T_SDL_Interface::EraseControl(T_SDL_ControlHandle Handle, bool bWithExtFocus, const IRECT* pSubNewRect) const
{
	DEBUG_VERIFY(Handle.IsValid());

	IRECT EraseRect = bWithExtFocus ? (*Handle)->GetFocusRect(m_ExtFocusSize) : (*Handle)->GetRect();

	if(pSubNewRect)
	{
		IRECT Rects[4];

		size_t szN = SubtractRect(EraseRect, EraseRect + (*pSubNewRect - (*Handle)->GetRect()), Rects);

		for(size_t i = 0 ; i < szN ; i++)
			DrawBG(Rects[i]);
	}
	else
	{
		DrawBG(EraseRect);
	}
}

void T_SDL_Interface::ShowControl(T_SDL_ControlHandle Handle, bool bShow)
{
	DEBUG_VERIFY(Handle.IsValid());

	if((*Handle)->m_bVisible == bShow)
		return;

	if(!m_bActive)
	{
		(*Handle)->m_bVisible = bShow;

		return;
	}

	if(bShow) // show
	{
		(*Handle)->m_bVisible = true;

		(*Handle)->Redraw();
	}
	else // hide
	{
		T_SDL_ControlHandle NewFocus;

		bool bSetFocus;

		if((*Handle)->IsFocused())
			bSetFocus = true;
		else
			bSetFocus = false;

		EraseControl(Handle, (*Handle)->DoesNeedExtFocus() && (*Handle)->IsFocused());

		(*Handle)->m_bVisible = false;

		if(bSetFocus)
		{
			m_Focus.Invalidate();

			SetFocus(NewFocus);
		}
	}
}

void T_SDL_Interface::ShowOverlaidControl(T_SDL_ControlHandle Handle, bool bShow, bool bSetFocus)
{
	DEBUG_VERIFY(Handle.IsValid());

	if(bShow)
	{
		DEBUG_VERIFY((*Handle)->IsHidden());

		if(bSetFocus)
			SetFocus(T_SDL_ControlHandle());

		ShowControl(Handle, true);

		if(bSetFocus)
			SetFocus(Handle);
	}
	else
	{
		DEBUG_VERIFY((*Handle)->IsVisible());

		DEBUG_VERIFY(!bSetFocus);		

		IRECT Rect = (*Handle)->DoesNeedExtFocus() && (*Handle)->IsFocused() ?
			(*Handle)->GetFocusRect(m_ExtFocusSize) : (*Handle)->GetRect();

		{
			T_SDL_Clipper Clipper0(g_pSDL_FB, Rect);

			ShowControl(Handle, false);

			Redraw(false);
		}
	}
}

void T_SDL_Interface::EnableControl(T_SDL_ControlHandle Handle, bool bEnable)
{
	DEBUG_VERIFY(Handle.IsValid());

	if((*Handle)->m_bEnabled == bEnable)
		return;

	if(!m_bActive || !(*Handle)->m_bVisible)
	{
		(*Handle)->m_bEnabled = bEnable;
		return;
	}

	if(bEnable) // enable
	{
		(*Handle)->m_bEnabled = true;

		(*Handle)->Redraw();
	}
	else // disable
	{
		T_SDL_ControlHandle NewFocus;

		bool bSetFocus;

		if((*Handle)->IsFocused())
		{
			EraseExtFocus(Handle);

			bSetFocus = true;
		}
		else
		{
			bSetFocus = false;
		}

		(*Handle)->m_bEnabled = false;

		(*Handle)->Redraw();

		if(bSetFocus)
		{
			m_Focus.Invalidate();

			SetFocus(NewFocus);
		}
	}
}

void T_SDL_Interface::SetControlRect(T_SDL_ControlHandle Handle, const IRECT& Rect)
{
	DEBUG_VERIFY(Handle.IsValid());

	DEBUG_VERIFY(Rect.IsFlatValid());

	if((*Handle)->m_Rect == Rect)
		return;

	if(!m_bActive || !(*Handle)->IsVisible())
	{
		(*Handle)->m_Rect = Rect;
		return;
	}

	EraseControl(Handle, (*Handle)->DoesNeedExtFocus() && (*Handle)->IsFocused(), &Rect);

	(*Handle)->m_Rect = Rect;

	(*Handle)->Redraw();

	if((*Handle)->DoesNeedExtFocus() && (*Handle)->IsFocused())
		DrawExtFocus(Handle);
}

void T_SDL_Interface::SetControlCoords(T_SDL_ControlHandle Handle, const IPOINT& Coords)
{
	DEBUG_VERIFY(Handle.IsValid());

	SetControlRect(Handle, RectFromCS(Coords, (ISIZE)(*Handle)->GetRect()));
}

T_SDL_ControlHandle T_SDL_Interface::GetPrevFocus(T_SDL_ControlHandle Handle)
{
	if(Handle.IsValid())
	{	
		T_SDL_ControlHandle PrevHandle = Handle;

		for(;;)
		{			
			if(!(--PrevHandle).IsValid())
				PrevHandle = m_Controls.GetLast();
				
			if(PrevHandle == Handle)
				break;

			if((*PrevHandle)->IsEnabled())
				return PrevHandle;
		}

		return (*Handle)->IsEnabled() ? Handle : NULL;
	}
	else
	{
		FOR_EACH_LIST_REV(m_Controls, T_SDL_ControlHandle, PrevHandle)
		{
			if((*PrevHandle)->IsEnabled())
				return PrevHandle;
		}

		return NULL;
	}
}

T_SDL_ControlHandle T_SDL_Interface::GetNextFocus(T_SDL_ControlHandle Handle)
{
	if(Handle.IsValid())
	{
		T_SDL_ControlHandle NextHandle = Handle;

		for(;;)
		{
			if(!(++NextHandle).IsValid())
				NextHandle = m_Controls.GetFirst();
				
			if(NextHandle == Handle)
				break;

			if((*NextHandle)->IsEnabled())
				return NextHandle;
		}

		return (*Handle)->IsEnabled() ? Handle : NULL;
	}
	else
	{
		FOR_EACH_LIST(m_Controls, T_SDL_ControlHandle, NextHandle)
		{
			if((*NextHandle)->IsEnabled())
				return NextHandle;
		}

		return NULL;
	}
}

void T_SDL_Interface::AddPassword(const T_SDL_InterfacePassword& Password)
{
	DEBUG_VERIFY(Password.m_szID != UINT_MAX);
	DEBUG_VERIFY(!Password.m_Sequence.IsEmpty());

	m_Passwords.Add() = Password;

	m_CurSeq.EnsureN(Password.m_Sequence.GetN());

	m_CurSeq.ZeroData(), m_szSeqPos = KNOB_NONE;
}

void T_SDL_Interface::SetBG_Color(UINT32 uiColor)
{
	if(m_uiBG_Color == uiColor)
		return;

	m_uiBG_Color = uiColor;
	
	if(m_bActive && !m_pBG_Image)
		Redraw();
}

void T_SDL_Interface::SetBG_Image(const T_SDL_Image* pImage)
{
	if(!m_pBG_Image && !pImage)
		return;

	DEBUG_VERIFY(!(pImage && pImage->GetSize() < g_SDL_Consts.m_Resolution));

	m_pBG_Image = pImage;

	if(m_bActive)
		Redraw();
}

void T_SDL_Interface::Redraw(bool bDrawBG) const
{
	if(!CanDraw())
		return;

	// BG
	if(bDrawBG)
		DrawBG();

	// Controls
	FOR_EACH_LIST(m_Controls, T_SDL_ControlConstHandle, Handle)
		(*Handle)->Redraw();

	// Focus
	DrawExtFocus(m_Focus);

	m_bHasDelayedRedraw = false;
}

void T_SDL_Interface::Clear()
{
	if(m_Controls.IsEmpty())
		return;

	m_Focus.Invalidate();

	m_Timers.Clear();

	m_Controls.Clear();

	m_Passwords.Clear();

	m_CurSeq.Clear(), m_szSeqPos = 0;

	if(m_bActive)
		Redraw();
}

T_SDL_ControlHandle T_SDL_Interface::AddControl(T_SDL_Control* pControl)
{
	DEBUG_VERIFY(pControl);

	T_SDL_ControlHandle Handle = m_Controls.AddLast(pControl);

	pControl->Attach(*this, Handle);

	if(m_bActive)
		pControl->Redraw();

	return Handle;
}

void T_SDL_Interface::RemoveControl(T_SDL_ControlHandle Handle)
{
	DEBUG_VERIFY(Handle.IsValid());

	m_Timers.Remove(*Handle);

	T_SDL_ControlHandle NewFocus;

	bool bSetFocus;

	if((*Handle)->IsFocused())
		bSetFocus = true;
	else
		bSetFocus = false;

	if(m_bActive && (*Handle)->IsVisible())
		EraseControl(Handle, (*Handle)->DoesNeedExtFocus() && (*Handle)->IsFocused());

	m_Controls.Del(Handle);

	if(bSetFocus)
	{
		m_Focus.Invalidate();

		SetFocus(NewFocus);
	}
}

void T_SDL_Interface::SetFocus(T_SDL_ControlHandle Handle, bool bFireEvent)
{
	DEBUG_VERIFY(!(Handle.IsValid() && !(*Handle)->IsEnabled()));

	if(Handle == m_Focus)
		return;

	T_SDL_ControlHandle OldFocus = m_Focus;

	m_Focus = Handle;

	if(m_bActive)
	{
		// Erasing old
		{
			if(OldFocus.IsValid())
				(*OldFocus)->Redraw(true);
	
			EraseExtFocus(OldFocus);
		}
			
		// Drawing new
		{
			if(m_Focus.IsValid())
				(*m_Focus)->Redraw(true);

			DrawExtFocus(m_Focus);
		}
	}

	if(bFireEvent)
		m_pEventSink->OnFocusChange(OldFocus.IsValid() ? &**OldFocus : NULL, m_Focus.IsValid() ? &**m_Focus : NULL);
}

size_t T_SDL_Interface::HandleEvent(T_KSDL_Event Event, size_t szParam)
{
	if(Event == KSDLE_KNOB)
	{
		// Password test
		if(!m_CurSeq.IsEmpty())
		{
			m_CurSeq[m_szSeqPos] = szParam;
			
			if(++m_szSeqPos == m_CurSeq.GetN())
				m_szSeqPos = 0;

			FOR_EACH_ARRAY(m_Passwords, i)
			{
				const TArray<size_t, true>& PassSeq = m_Passwords[i].m_Sequence;

				size_t cp = PassSeq.GetN();
				size_t cs = m_szSeqPos;

				while(cp > 0)
				{
					size_t l = Min(cp, cs);

					if(memcmp(PassSeq.GetDataPtr() + cp - l, m_CurSeq.GetDataPtr() + cs - l, l * sizeof(size_t)))
						break;

					cp -= l;
					
					if(!(cs -= l))
						cs = m_CurSeq.GetN();
				}

				if(!cp)
				{
					m_pEventSink->OnPassword(m_Passwords[i].m_szID);
					m_CurSeq.ZeroData(), m_szSeqPos = 0;
					return 0;
				}
			}
		}

		// Prev focus
		if(szParam == KNOB_LEFT)
		{
			SetFocus(GetPrevFocus(), true);
			return 0;
		}

		// Next focus
		if(szParam == KNOB_RIGHT)
		{
			SetFocus(GetNextFocus(), true);
			return 0;
		}

		// Control or event sink gets the event
		if(m_Focus.IsValid())
			(*m_Focus)->OnKNOB(szParam);

		return 0;
	}

	return 0;
}

int T_SDL_Interface::Activate(T_SDL_InterfaceEventSink& EventSink, const T_SDL_Interface* pPrevInterface)
{
	DEBUG_VERIFY(!m_bActive);

	#ifdef _DEBUG
	{
		FOR_EACH_LIST(m_Controls, T_SDL_ControlHandle, Handle)
			DEBUG_VERIFY((*Handle)->m_Rect.IsFlatValid());
	}
	#endif // _DEBUG

	m_pEventSink = &EventSink;

	m_iRetCode = SDL_INTR_OK; // for safety

	m_bActivating = true;

	GetEventSink().OnActivate();

	if(!m_bActivating) // Deactivate() was called inside OnActivate()
	{
		if(pPrevInterface)
			pPrevInterface->Redraw();

		return m_iRetCode;
	}

	m_bActivating = false;

	m_bActive = true;

	Redraw();

	RunSDL_MessageLoop(*this, &T_SDL_Interface::HandleEvent, m_Timers, *m_pEventSink, m_bActive);

	if(pPrevInterface)
		pPrevInterface->Redraw();

	return m_iRetCode;
}

void T_SDL_Interface::Deactivate(int iCode)
{
	DEBUG_VERIFY(m_bActive != m_bActivating);

	m_iRetCode = iCode;

	m_bActive = m_bActivating = false;
}

T_SDL_ControlHandle T_SDL_Interface::FindControl(size_t szID)
{
	DEBUG_VERIFY(szID != UINT_MAX);

	FOR_EACH_LIST(m_Controls, T_SDL_ControlHandle, Handle)
	{
		if((*Handle)->GetID() == szID)
			return Handle;
	}
	
	return NULL;
}

// ----------------------------
// SDL label highlight manager
// ----------------------------
T_SDL_LabelHighlightManager::T_SDL_LabelHighlightManager(T_SDL_Interface& Interface) : m_Interface(Interface)
{
	m_szHL_LabelID = UINT_MAX;
}

void T_SDL_LabelHighlightManager::Update()
{
	size_t szOldHL_LabelID = m_szHL_LabelID;

	m_szHL_LabelID = UINT_MAX;

	{
		size_t szFocusID = m_Interface.GetFocusID();

		if(szFocusID != UINT_MAX)
		{
			FOR_EACH_ARRAY(m_Items, i)
			{
				if(m_Items[i].m_szFocusID == szFocusID)
				{
					m_szHL_LabelID = m_Items[i].m_szLabelID;
					break;
				}
			}
		}
	}

	if(m_szHL_LabelID == szOldHL_LabelID)
		return;

	if(szOldHL_LabelID != UINT_MAX)
	{
		TButtonSDL_Control* pButton = dynamic_cast<TButtonSDL_Control*>(m_Interface[szOldHL_LabelID]);
		KFC_VERIFY_WITH_SOURCE(pButton);
		pButton->SetBG_Color(UINT_MAX);
	}

	if(m_szHL_LabelID != UINT_MAX)
	{
		TButtonSDL_Control* pButton = dynamic_cast<TButtonSDL_Control*>(m_Interface[m_szHL_LabelID]);
		KFC_VERIFY_WITH_SOURCE(pButton);
		pButton->SetBG_Color(g_SDL_Consts.m_uiLabelSelectionBG_Color);
	}
}
