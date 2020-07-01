#include "kfc_interface_pch.h"
#include "scroll_control.h"

#include <KFC_Sound\sound_storage.h>
#include "interactive_control.h"

#include "interface.h"

// -------------------------------
// Scroll control creation struct
// -------------------------------
void TScrollControlCreationStruct::Load(	TInfoNodeConstIterator	InfoNode,
											const TControl*			pParentControl,
											const FRECT&			Resolution)
{
	TControlCreationStruct::Load(InfoNode, pParentControl, Resolution);

	// {{{
	INITIATE_FAILURE;
}

// -------------------------------
// Scroll control sounds provider
// -------------------------------
TScrollControlSoundsProvider::TScrollControlSoundsProvider()
{
	// {{{ Load defaults here (tokenize/storagize'em first in interface_globals)
}

void TScrollControlSoundsProvider::Load(TInfoNodeConstIterator InfoNode)
{
	g_SoundStorage.LoadObject(InfoNode, TEXT("DescendingScrollSound"),	m_DescendingScrollSound,	true);
	g_SoundStorage.LoadObject(InfoNode, TEXT("AscendingScrollSound"),	m_AscendingScrollSound,		true);
}

// ---------------------------------
// Scroll control controls provider
// ---------------------------------
TScrollControlControlsProvider::TScrollControlControlsProvider()
{
	m_bAllocated = true;
}

void TScrollControlControlsProvider::Release()
{
	m_bAllocated = false;

	m_DescendingArrow.Release();
	m_AscendingArrow. Release();

	m_Items.Clear();
}

void TScrollControlControlsProvider::Load(	TInfoNodeConstIterator	InfoNode,
											const TControl*			pParentControl,
											const FRECT&			Resolution)
{
	// {{{
	INITIATE_FAILURE;
}

// ---------------
// Scroll control
// ---------------
TControl* TScrollControl::LoadControl(	type_t					tpType,
										TInfoNodeConstIterator	InfoNode,
										const TControl*			pParentControl,
										const FRECT&			Resolution)
{
	DEBUG_VERIFY(tpType == CONTROL_TYPE_SCROLL);

	DEBUG_VERIFY(InfoNode.IsValid());

	TScrollControlCreationStruct CreationStruct;
	CreationStruct.Load(InfoNode, pParentControl, Resolution);

	TScrollControlSoundsProvider SoundsProvider;
	SoundsProvider.Load(InfoNode);

	TScrollControlControlsProvider ControlsProvider;
	ControlsProvider.Load(InfoNode, pParentControl, Resolution);

	return new TScrollControl(CreationStruct, SoundsProvider, ControlsProvider);
}

TScrollControl::TScrollControl(	const TScrollControlCreationStruct&	CreationStruct,
								TScrollControlSoundsProvider&		SoundsProvider,
								TScrollControlControlsProvider&		ControlsProvider) :

	TControl(CreationStruct)

{
	m_pDescendingArrow	= NULL;
	m_pAscendingArrow	= NULL;

	// --- States ---
	m_szNVisibleItems			= CreationStruct.m_szNVisibleItems;
	m_FirstVisibleItemRect		= CreationStruct.m_FirstVisibleItemRect;
	m_VisibleItemRectsOffset	= CreationStruct.m_VisibleItemRectsOffset;
	m_fScrollDelay				= CreationStruct.m_fScrollDelay;

	// --- Sounds ---
	m_DescendingScrollSound.ReOwn(SoundsProvider.m_DescendingScrollSound);
	m_AscendingScrollSound.	ReOwn(SoundsProvider.m_AscendingScrollSound);

	// --- Controls ----
	DEBUG_VERIFY(ControlsProvider.IsAllocated());

	// Arrows
	m_ControlsProvider.m_DescendingArrow.	ReOwn(ControlsProvider.m_DescendingArrow);
	m_ControlsProvider.m_AscendingArrow.	ReOwn(ControlsProvider.m_AscendingArrow);

	// Items
	m_ControlsProvider.m_Items.Clear();

	for(size_t i = 0 ; i < ControlsProvider.m_Items.GetN() ; i++)
		m_ControlsProvider.m_Items.Add().ReOwn(ControlsProvider.m_Items[i]);
}

// Allocation
void TScrollControl::OnAllocate()
{
	TControl::OnAllocate();

	size_t i;
	
	DEBUG_VERIFY(m_ControlsProvider.IsAllocated());

	// Items
	m_Items.Clear();

	for(i = 0 ; i < m_ControlsProvider.m_Items.GetN() ; i++)
	{
		m_Items.Add() =
			GetInterface().AddLastChildControl(	m_ControlsProvider.m_Items[i].MakeExternal(),
												this);
	}

	// Arrows
	if(m_ControlsProvider.m_DescendingArrow.IsAllocated())
	{
		DEBUG_VERIFY(IsInteractiveControl(m_ControlsProvider.m_DescendingArrow.GetDataPtr()));

		m_pDescendingArrow =
			GetInterface().AddLastChildControl(	m_ControlsProvider.m_DescendingArrow.MakeExternal(),
												this);
	}
	else
	{
		m_pDescendingArrow = NULL;
	}

	if(m_ControlsProvider.m_AscendingArrow.IsAllocated())
	{
		DEBUG_VERIFY(IsInteractiveControl(m_ControlsProvider.m_AscendingArrow.GetDataPtr()));

		m_pAscendingArrow =
			GetInterface().AddLastChildControl(	m_ControlsProvider.m_AscendingArrow.MakeExternal(),
												this);
	}
	else
	{
		m_pAscendingArrow = NULL;
	}

	m_ControlsProvider.Release();

	// Interpolator
	m_ScrollInterpolator.Allocate();

	// Sub-interpolators
	(new(m_ScrollInterpolator) TItemHiderInterpolator)->	Allocate();
	(new(m_ScrollInterpolator) TItemShowerInterpolator)->	Allocate();

	for(i = 0 ; i <= m_szNVisibleItems ; i++)
		(new(m_ScrollInterpolator) TItemSliderInterpolator)->Allocate();
	
	// Resolving sub-interpolator IDs
	{
		TInterpolatorHandle Handle = m_ScrollInterpolator.GetFirst();

		m_ItemHider  = Handle, ++Handle;
		m_ItemShower = Handle, ++Handle;

		m_ItemSliders.Clear();

		for(i = m_szNVisibleItems + 1 ; i ; i--)
			m_ItemSliders.Add() = Handle, ++Handle;
	}
}

void TScrollControl::OnSetInitialValues()
{
	TControl::OnSetInitialValues();

	// Visible items
	m_VisibleItems.Set(0, m_Items.GetN());
	IntersectSegments(m_VisibleItems, SZSEGMENT(0, m_szNVisibleItems));

	// Items
	FRECT ItemRect = m_FirstVisibleItemRect;

	for(size_t i = 0 ; i < m_Items.GetN() ; i++)
	{
		TControlState State = m_Items[i]->GetCurrentClientState();

		State.m_Rect.m_CommonPart = ItemRect;
		State.m_Visibility.m_CommonPart = i < m_szNVisibleItems ? 1.0f : 0.0f;

		GetInterface().SetControlClientState(m_Items[i], State, true);

		if(IsInteractiveControl(m_Items[i]))
		{
			TInteractiveControl& Control =
				(TInteractiveControl&)*m_Items[i];

			Control.m_StateColors[TInteractiveControl::IS_DISABLED] =
				Control.m_StateColors[TInteractiveControl::IS_NONHOVERED];
		}
		
		if(i < m_szNVisibleItems)
			ShiftRect(ItemRect, m_VisibleItemRectsOffset);
	}

	// Scrolling states
	m_iCurrentScrolling			= 0;
	m_iAccumulatedScrolling		= 0;
	m_fCurrentScrollerSpeedCoef	= 1.0f;
	m_bScrollerHeadingBack		= false;

	// Interpolator
	m_ScrollInterpolator.SetSuspendCount(GetSuspendCount());
	m_ScrollInterpolator.Deactivate();

	// Arrows
	UpdateArrowsAppearance(true);
}

// Update/render events
void TScrollControl::OnPreUpdate()
{
	TControl::OnPreUpdate();

	if(m_ScrollInterpolator.Update())
		OnScrollerFinished();
}

void TScrollControl::OnInterfaceUpdated()
{
	TControl::OnInterfaceUpdated();

	for(TMessageIterator Iter = GetInterface().GetMessageProcessor().GetFirst() ;
		Iter.IsValid() ;
		++Iter)
	{
		// Push click messsage
		{
			const TInteractiveControlPushClickMessage* pMessage;

			if(pMessage = dynamic_cast<const TInteractiveControlPushClickMessage*>(Iter.GetDataPtr()))
			{
				TControl* pControl = pMessage->m_pControl;
				
				if(!pControl->GetCurrentScreenState().IsEnabled())
					continue;

				if(m_pDescendingArrow && pControl == m_pDescendingArrow)
					OnDescendingArrowClick();
				else if(m_pAscendingArrow && pControl == m_pAscendingArrow)
					OnAscendingArrowClick();
			}
		}
	}
}

bool TScrollControl::OnSuspend()
{
	if(!TControl::OnSuspend())
		return false;

	m_ScrollInterpolator.Suspend();

	return true;
}

bool TScrollControl::OnResume()
{
	if(!TControl::OnResume())
		return false;

	m_ScrollInterpolator.Resume();

	return true;
}

// Arrow click events
void TScrollControl::OnDescendingArrowClick()
{
	if(m_iCurrentScrolling)
	{
		if(m_iCurrentScrolling > 0)
		{
			m_iAccumulatedScrolling = 0;

			HeadScrollerBack();
		}
		else
		{
			if((int)m_VisibleItems.m_First + m_iAccumulatedScrolling > 0)
				m_iAccumulatedScrolling--;

			SetScrollerSpeedCoef(m_fCurrentScrollerSpeedCoef + 1.0f);
		}

		UpdateArrowsAppearance();
	}
	else
	{
		DescendingScroll();
	}

	// Sound
	if(m_DescendingScrollSound.IsAllocated())
		m_DescendingScrollSound->Play();

	// Message
	new(GetInterface().GetMessageProcessor())
		TScrollControlScrollMessage(this, -1);
}

void TScrollControl::OnAscendingArrowClick()
{
	if(m_iCurrentScrolling)
	{
		if(m_iCurrentScrolling < 0)
		{
			m_iAccumulatedScrolling = 0;

			HeadScrollerBack();
		}
		else
		{
			if((int)m_VisibleItems.m_Last + m_iAccumulatedScrolling < (int)m_Items.GetN())
				m_iAccumulatedScrolling++;

			SetScrollerSpeedCoef(m_fCurrentScrollerSpeedCoef + 1.0f);
		}
	}
	else
	{
		AscendingScroll();
	}

	// Sound
	if(m_AscendingScrollSound.IsAllocated())
		m_AscendingScrollSound->Play();

	// Message
	new(GetInterface().GetMessageProcessor())
		TScrollControlScrollMessage(this, +1);
}

// Scrolling events
void TScrollControl::OnScrollerFinished()
{
	m_iCurrentScrolling = 0;

	if(m_iAccumulatedScrolling < 0)
	{
		DescendingScroll();
		m_iAccumulatedScrolling++;
	}
	else if(m_iAccumulatedScrolling > 0)
	{
		AscendingScroll();
		m_iAccumulatedScrolling--;
	}
	else
	{
		SetScrollerSpeedCoef(1.0f);
	}
					
	UpdateArrowsAppearance();
}

// Scrolling methods
void TScrollControl::UpdateArrowsAppearance(bool bFirstCall)
{
	// Visibility
	{
		const float fVisibility =
			m_Items.GetN() <= m_VisibleItems.GetLength() ? 0.0f : 1.0f;

		if(m_pDescendingArrow)
		{
			TInterface::SetControlCommonVisibility(	m_pDescendingArrow,
													fVisibility,
													bFirstCall);
		}

		if(m_pAscendingArrow)
		{
			TInterface::SetControlCommonVisibility(	m_pAscendingArrow,
													fVisibility,
													bFirstCall);
		}
	}

	// Enablement
	{
		if(m_pDescendingArrow)
		{
			// Descenging arrow
			TInterface::SetControlCommonEnablement(	m_pDescendingArrow,
													m_iCurrentScrolling ||
													m_VisibleItems.m_First > 0 ?
														1.0f : 0.0f,
													bFirstCall);
		}

		if(m_pAscendingArrow)
		{
			// Ascending arrow
			TInterface::SetControlCommonEnablement(	m_pAscendingArrow,
													m_iCurrentScrolling ||
													m_VisibleItems.m_Last < m_Items.GetN() ?
														1.0f : 0.0f,
													bFirstCall);
		}
	}
}

void TScrollControl::DescendingScroll()
{
	if(m_VisibleItems.m_First == 0) // safety
		return;

	ScrollItems(SZSEGMENT(	m_VisibleItems.m_First - 1,
							m_VisibleItems.m_Last),
				m_VisibleItems.m_Last - 1,
				m_VisibleItems.m_First - 1,
				m_VisibleItemRectsOffset);

	// Visible segment
	--m_VisibleItems;

	// Current scrolling
	m_iCurrentScrolling = -1;

	// Arrows
	UpdateArrowsAppearance();
}

void TScrollControl::AscendingScroll()
{
	if(m_VisibleItems.m_Last == m_Items.GetN()) // safety
		return;

	ScrollItems(SZSEGMENT(	m_VisibleItems.m_First,
							m_VisibleItems.m_Last + 1),
				m_VisibleItems.m_First,
				m_VisibleItems.m_Last,
				-m_VisibleItemRectsOffset);

	// Visible segment
	++m_VisibleItems;

	// Current scrolling
	m_iCurrentScrolling = 1;
	
	// Arrows
	UpdateArrowsAppearance();
}

void TScrollControl::ScrollItems(	const SZSEGMENT&	SlideSegment,
									size_t				szHideItemIndex,
									size_t				szShowItemIndex,
									const FSIZE&		RectsDelta)
{
	DEBUG_VERIFY(SlideSegment.GetLength() == m_szNVisibleItems + 1);

	size_t i;

	float SrcValues[2] = {0, m_fScrollDelay};

	// Item hider
	{
		TItemHiderInterpolator& SubInterpolator =
			(TItemHiderInterpolator&)*m_ItemHider;

		float DstValues[2] = {1.0f, 0.0f};

		SubInterpolator.GetSrcValueGetter().Allocate(	FSEGMENT(	SrcValues[0],
																	SrcValues[1]),
														SrcValues[0]);

		SubInterpolator.GetDstValueSetter().Allocate(m_Items[szHideItemIndex]);

		SubInterpolator.GetValueMapper().Allocate(SrcValues, DstValues);
	}

	// Item shower
	{
		TItemShowerInterpolator& SubInterpolator =
			(TItemShowerInterpolator&)*m_ItemShower;

		float DstValues[2] = {0.0f, 1.0f};

		SubInterpolator.GetSrcValueGetter().Allocate(	FSEGMENT(	SrcValues[0],
																	SrcValues[1]),
														SrcValues[0]);

		SubInterpolator.GetDstValueSetter().Allocate(m_Items[szShowItemIndex]);

		SubInterpolator.GetValueMapper().Allocate(SrcValues, DstValues);
	}

	// Sliders
	for(i = SlideSegment.m_First ; i < SlideSegment.m_Last ; i++)
	{
		TItemSliderInterpolator& Slider =
			(TItemSliderInterpolator&)*m_ItemSliders[i - SlideSegment.m_First];

		FRECT DstValues[2];

		DstValues[0] = m_Items[i]->GetCurrentClientState().m_Rect.m_CommonPart;

		ShiftRect(DstValues[1] = DstValues[0], RectsDelta);

		Slider.GetSrcValueGetter().Allocate(FSEGMENT(	SrcValues[0],
														SrcValues[1]),
											SrcValues[0]);

		Slider.GetDstValueSetter().Allocate(m_Items[i]);

		Slider.GetValueMapper().Allocate(SrcValues, DstValues);
	}

	m_ScrollInterpolator.SetSpeedCoef(m_fCurrentScrollerSpeedCoef);

	m_ScrollInterpolator.Activate(SrcValues[1]);

	m_bScrollerHeadingBack = false;
}
	
void TScrollControl::HeadScrollerBack()
{
	if(m_iCurrentScrolling == 0) // safety
		return;

	m_iCurrentScrolling = -m_iCurrentScrolling;

	if(m_iCurrentScrolling < 0)
		--m_VisibleItems;
	else
		++m_VisibleItems;

	m_bScrollerHeadingBack = !m_bScrollerHeadingBack;

	m_ScrollInterpolator.Activate(m_bScrollerHeadingBack ? 0.0f : m_fScrollDelay);
}

void TScrollControl::SetScrollerSpeedCoef(float fSpeedCoef)
{
	m_ScrollInterpolator.SetSpeedCoef(m_fCurrentScrollerSpeedCoef = fSpeedCoef);
}