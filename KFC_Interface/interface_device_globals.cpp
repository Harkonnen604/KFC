#include "kfc_interface_pch.h"
#include "interface_device_globals.h"

#include <math.h>
#include <KFC_KTL\globals_collector.h>
#include <KFC_GUI\gui.h>
#include <KFC_Windows\windows_device_globals.h>
#include <KFC_Graphics\sprite_storage.h>
#include <KFC_Graphics\font_storage.h>
#include <KFC_Graphics\graphics_device_globals.h>
#include <KFC_Graphics\graphics_cfg.h>
#include <KFC_Input\input_device_globals.h>
#include <KFC_Sound\sound_storage.h>
#include "interface_consts.h"
#include "interface_cfg.h"
#include "interface_initials.h"
#include "interface_tokens.h"
#include "controls_factory.h"
#include "interface_sprite_defs.h"
#include "interface_font_defs.h"
#include "interface_sound_defs.h"
#include "interface.h"
#include "interface_input_message_defs.h"

TInterfaceDeviceGlobals g_InterfaceDeviceGlobals;

// -------------------------
// Interface device globals
// -------------------------
TInterfaceDeviceGlobals::TInterfaceDeviceGlobals() : TGlobals(TEXT("Interface device globals"))
{
	AddSubGlobals(g_InterfaceInitials);
	AddSubGlobals(g_InterfaceCfg);
	AddSubGlobals(g_InterfaceTokens);
	AddSubGlobals(g_ControlsFactory);

	// Mouse pointer
	m_bMousePointerVisible = true;	

	// Interface
	m_pMouseOwnerControl = NULL;
	m_pFocusOwnerControl = NULL;
}

void TInterfaceDeviceGlobals::OnUninitialize()
{
	size_t i;

	// Hot message
	m_HotMessageInterpolator.Release();

	m_HotMessageText.Empty();
	m_HotPointers.Clear();

	// Music segments
	m_ClickSoundRegisterer.		Release();
	m_PushSoundRegisterer.		Release();
	m_HotMessageSoundRegisterer.Release();

	// Sprites
	m_CancelSpriteRegisterer.	Release();
	m_OkSpriteRegisterer.		Release();

	m_CheckBoxSpriteRegisterer.Release();

	for(i = 3 ; i != UINT_MAX ; i--)
		m_HotPointerSpriteRegisterers[i].Release();

	m_MousePointerSpriteRegisterer.Release();

	// Fonts
	m_DefaultLabelFontRegisterer.	Release();
	m_HotMessageFontRegisterer.		Release();

	// Interface
	m_pFocusOwnerControl = NULL;
	m_pMouseOwnerControl = NULL;	

	// Messages
	m_InputMessageProcessor.Release();
}

void TInterfaceDeviceGlobals::OnInitialize()
{
	TStructuredInfo Info(FILENAME_TOKENS.
		Process(TEXT("[StartFolder][DefinitionsFolder]Interface.Definition")));

	// Messages
	m_InputMessageProcessor.Allocate();

	size_t i;

	// Mouse data
	if(g_GraphicsCfg.m_bFullScreen)
		CenterMouseCursor();

	m_MouseRect.Set(0.0f, 0.0f, 1.0f, 1.0f);

	m_MouseCoords = GetWindowedMouseCoords();

	m_MouseDelta.Set(0.0f, 0.0f);

	m_MouseCoordsDelta.Set(0.0f, 0.0f);

	m_MouseSensitivity.Set(1.0f, 1.0f);

	// Sprites
	try
	{
		const TInfoNodeConstIterator InfoNode =
			Info.GetNode(Info.GetRootNode(), TEXT("Sprites"));

		TObjectPointer<TSprite> Sprite;

		// Mouse pointer
		g_SpriteStorage.LoadObject(InfoNode, TEXT("MousePointer"), Sprite, false);

		m_MousePointerSpriteRegisterer.Allocate(SPRITES_REGISTRATION_MANAGER,
												Sprite,
												MOUSE_POINTER_SPRITE_INDEX);

		// Hot pointers
		{
			TObjectPointer<TSprite> Sprites[4];

			const size_t szLimits = 4;

			g_SpriteStorage.LoadObjects(InfoNode,
										TEXT("HotPointer?"),
										TEXT("?"),
										Sprites,
										&szLimits,
										NULL);

			for(i = 0 ; i < 4 ; i++)
			{
				m_HotPointerSpriteRegisterers[i].Allocate(	SPRITES_REGISTRATION_MANAGER,
															Sprites[i],
															HOT_POINTER_SPRITES_START_INDEX + i);
			}
		}

		// Checkbox
		g_SpriteStorage.LoadObject(InfoNode, TEXT("CheckBox"), Sprite, false);

		m_CheckBoxSpriteRegisterer.Allocate(SPRITES_REGISTRATION_MANAGER,
											Sprite,
											CHECKBOX_SPRITE_INDEX);

		// Ok
		g_SpriteStorage.LoadObject(InfoNode, TEXT("Ok"), Sprite, false);

		m_OkSpriteRegisterer.Allocate(	SPRITES_REGISTRATION_MANAGER,
										Sprite,
										OK_SPRITE_INDEX);

		// Cancel
		g_SpriteStorage.LoadObject(InfoNode, TEXT("Cancel"), Sprite, false);

		m_CancelSpriteRegisterer.Allocate(	SPRITES_REGISTRATION_MANAGER,
											Sprite,
											CANCEL_SPRITE_INDEX);
	}

	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error loading basic interface sprites."));
	}

	// Fonts
	try
	{
		const TInfoNodeConstIterator InfoNode =
			Info.GetNode(Info.GetRootNode(), TEXT("Fonts"));

		TObjectPointer<TFont> Font;

		// Hot message
		g_FontStorage.LoadObject(InfoNode, TEXT("HotMessage"), Font, false);

		m_HotMessageFontRegisterer.Allocate(FONTS_REGISTRATION_MANAGER,
											Font,
											HOT_MESSAGE_FONT_INDEX);

		// Default label
		g_FontStorage.LoadObject(InfoNode, TEXT("DefaultLabel"), Font, false);

		m_DefaultLabelFontRegisterer.Allocate(	FONTS_REGISTRATION_MANAGER,
												Font,
												DEFAULT_LABEL_FONT_INDEX);
	}

	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error loading basic interface fonts."));
	}

	// Sounds
	try
	{
		const TInfoNodeConstIterator InfoNode =
			Info.GetNode(Info.GetRootNode(), TEXT("Sounds"));

		TObjectPointer<TSound> Sound;

		// Hot message
		g_SoundStorage.LoadObject(InfoNode, TEXT("HotMessage"), Sound, false);

		m_HotMessageSoundRegisterer.Allocate(	SOUNDS_REGISTRATION_MANAGER,
												Sound,
												HOT_MESSAGE_SOUND_INDEX);

		// Push
		g_SoundStorage.LoadObject(InfoNode, TEXT("Push"), Sound, false);

		m_PushSoundRegisterer.Allocate(	SOUNDS_REGISTRATION_MANAGER,
										Sound,
										PUSH_SOUND_INDEX);

		// Click
		g_SoundStorage.LoadObject(InfoNode, TEXT("Click"), Sound, false);

		m_ClickSoundRegisterer.Allocate(SOUNDS_REGISTRATION_MANAGER,
										Sound,
										CLICK_SOUND_INDEX);
	}
	
	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error loading interface sounds."));
	}

	// Hot message interpolator
	try
	{
		float SrcValues[3] = {0.0f};
		SrcValues[1] = SrcValues[0] + g_InterfaceConsts.m_fHotMessageAppearanceDelay;
		SrcValues[2] = SrcValues[1] + 1000.0f; // varies due to speed coefs

		float DstValues[3] = {0.0f, 1.0f, 1.0f};

		m_HotMessageInterpolator.Allocate();

		m_HotMessageInterpolator.GetSrcValueGetter().Allocate(	FSEGMENT(	SrcValues[0],
																			SrcValues[2]),
																SrcValues[0]);

		m_HotMessageInterpolator.GetDstValueSetter().Allocate(DstValues[0]);

		m_HotMessageInterpolator.GetValueMapper().Allocate(SrcValues, DstValues);
	}

	catch(...)
	{
		INITIATE_DEFINED_FAILURE(TEXT("Error allocating hot message interpolator."));
	}

	// Interface
	m_bMousePointerVisible = false;

	// Interface
	InvalidateMouseOwnership(false);
	InvalidateFocusOwnership(false);
}

bool TInterfaceDeviceGlobals::OnSuspend()
{
	if(!TSuspendable::OnSuspend())
		return false;

	m_HotMessageInterpolator.Suspend();

	return true;
}

bool TInterfaceDeviceGlobals::OnResume()
{
	if(!TSuspendable::OnResume())
		return false;

	m_HotMessageInterpolator.Resume();

	return true;
}

void TInterfaceDeviceGlobals::OnUpdate()
{
	TGlobals::OnUpdate();	

	size_t i;

	// Termination
#ifdef _DEBUG

	if(	g_InputDeviceGlobals.IsKeyboardKeyPressed(DIK_CTRL) &&
		g_InputDeviceGlobals.IsKeyboardKeyPressed(DIK_ESCAPE))
	{
		g_GlobalsCollector.Terminate();
	}

#endif // _DEBUG

	// Messages
	m_InputMessageProcessor.Clear();	

	// Mouse
	if(g_GraphicsCfg.m_bFullScreen)
	{
		const IPOINT Coords = GetKCursorPos();

		m_MouseDelta.cx = ((float)Coords.x / GetSystemMetrics(SM_CXSCREEN) - 0.5f) * m_MouseRect.GetWidth	();
		m_MouseDelta.cy = ((float)Coords.y / GetSystemMetrics(SM_CYSCREEN) - 0.5f) * m_MouseRect.GetHeight	();

		CenterMouseCursor();
	}
	else
	{
		m_MouseCoords = GetWindowedMouseCoords();

		m_MouseDelta.Set(0, 0);
	}

	FPOINT OldMouseCoords = m_MouseCoords;

	m_MouseCoords.x += m_MouseDelta.cx;
	m_MouseCoords.y += m_MouseDelta.cy;

	BoundPoint(m_MouseCoords, m_MouseRect);

	m_MouseCoordsDelta.cx = m_MouseCoords.x - OldMouseCoords.x;
	m_MouseCoordsDelta.cy = m_MouseCoords.y - OldMouseCoords.y;

	// Messages
	{
		// Keyboard key messages
		for(i = 0 ; i < g_InputDeviceGlobals.m_ChangedKeyboardKeyStates.GetN() ; i++)
		{
			const size_t szValue = g_InputDeviceGlobals.m_ChangedKeyboardKeyStates[i];

			new(m_InputMessageProcessor)
				TInterfaceInputKeyboardKeyMessage(	(szValue & INPUT_STATE_CHANGE_ITEM_MASK),
													(szValue & INPUT_STATE_CHANGE_VALUE_MASK) ? true : false);
		}

		// Mouse button messages
		for(i = 0 ; i < g_InputDeviceGlobals.m_ChangedMouseButtonStates.GetN() ; i++)
		{
			const size_t szValue = g_InputDeviceGlobals.m_ChangedMouseButtonStates[i];

			new(m_InputMessageProcessor)
				TInterfaceInputMouseButtonMessage(	(szValue & INPUT_STATE_CHANGE_ITEM_MASK),
													(szValue & INPUT_STATE_CHANGE_VALUE_MASK) ? true : false,
													m_MouseCoords);
		}

		// Mouse movement messages
		if(!m_MouseDelta.IsPoint())
		{
			new(m_InputMessageProcessor)
				TInterfaceInputMouseMovementMessage(m_MouseDelta);
		}

		// Mouse coords change message
		if(!m_MouseCoordsDelta.IsPoint())
		{
			new(m_InputMessageProcessor)
				TInterfaceInputMouseCoordsChangeMessage(m_MouseCoords, m_MouseCoordsDelta);
		}

		// Hot message interpolator
		if(m_HotMessageInterpolator.Update())
		{
			const float* pSrcValues = m_HotMessageInterpolator.GetSrcValues();

			if(m_HotMessageInterpolator.GetCurrentSrcValue() == pSrcValues[0])
			{
				// Interpolation finished
				m_HotMessageText.Empty();
				m_HotPointers.Clear();
			}
			else if(m_HotMessageInterpolator.GetCurrentSrcValue() == pSrcValues[1])
			{
				// Entering visibility state
				m_HotMessageInterpolator.SetSpeedCoef(1000.0f / m_fHotMessageVisibilityDelay);

				m_HotMessageInterpolator.Activate(pSrcValues[2]);
			}
			else
			{
				// Entering hiding state
				m_HotMessageInterpolator.SetSpeedCoef(g_InterfaceConsts.m_fHotMessageHideSpeedCoef);
				
				m_HotMessageInterpolator.SetCurrentSrcValue(pSrcValues[1]);
				m_HotMessageInterpolator.Activate(pSrcValues[0]);
			}
		}
	}

	// Screenshot
	if(	g_InputDeviceGlobals.IsKeyboardKeyPressed		(g_InterfaceConsts.m_szScreenShotHotKey) &&
		g_InputDeviceGlobals.HasKeyboardKeyStateChanged	(g_InterfaceConsts.m_szScreenShotHotKey))
	{
		g_GraphicsDeviceGlobals.InitiateScreenShot();

		SetHotMessage(	TEXT("SCREEN CAPTURED"),
						FPOINT(g_GraphicsDeviceGlobals.m_ScreenCenter.x, 8.0f),
						WhiteColor(),
						1000.0f,
						false);
	}
}

void TInterfaceDeviceGlobals::OnPostRender(bool bFailureRoolBack) const
{
	TGlobals::OnPostRender(bFailureRoolBack);

	size_t i;

	if(!bFailureRoolBack)
	{
		// Hot message
		if(!m_HotMessageText.IsEmpty() && m_HotMessageInterpolator.IsActive())
		{
			// Color
			TD3DColor Color = m_HotMessageColor;
			Color.m_fAlpha *= m_HotMessageInterpolator.GetCurrentDstValue();

			// Pointers
			for(i = 0 ; i < m_HotPointers.GetN() ; i++)
			{
				SPRITES_REGISTRATION_MANAGER[	HOT_POINTER_SPRITES_START_INDEX +
												m_HotPointers[i].m_Direction]->
					DrawNonScaled(m_HotPointers[i].m_Coords, Color);
			}

			// Message
			FONTS_REGISTRATION_MANAGER[HOT_MESSAGE_FONT_INDEX]->
				DrawText(m_HotMessageText, m_HotMessageCoords, Color);			
		}

		// Performance
		if(g_InputDeviceGlobals.IsKeyboardKeyPressed(g_InterfaceConsts.m_szPerformanceHotKey))
		{
			const TFont* pFont = FONTS_REGISTRATION_MANAGER[DEFAULT_FONT_INDEX];

			KString String;

			// Run time
			String += TEXT("Run time: ");
			String += KString::Formatted(TEXT("%.6u"), (UINT)g_WindowsDeviceGlobals.GetRunTime());
			String += TEXT("\n");
			
			// Last frame time
			String += TEXT("Last frame time: ");
			String += KString::Formatted(TEXT("%.3u"), (UINT)g_WindowsDeviceGlobals.GetLastFrameTime());
			String += TEXT("\n");

			// Frames rendered
			String += TEXT("Frames rendered: ");
			String += KString::Formatted(TEXT("%.6u"), (UINT)g_WindowsDeviceGlobals.GetNFrames());
			String += TEXT("\n");

			// Splitter
			String += TEXT("------------------------------------------------\n");

			// Immediate FPS
			String += TEXT("Immediate FPS: ");

			if(g_WindowsDeviceGlobals.GetLastFrameTime())
			{
				String += KString::Formatted(TEXT("%.3u"), 1000 / (UINT)(g_WindowsDeviceGlobals.GetLastFrameTime()));
			}
			else
			{
				String += TEXT("---");
			}

			String += TEXT("\n");

			// Medium FPS
			String += TEXT("Medium FPS: ");
			if(g_WindowsDeviceGlobals.GetRunTime())
			{
				String += KString::Formatted(TEXT("%.3u"),	1000 *
															(UINT)g_WindowsDeviceGlobals.GetNFrames() /
															(UINT)g_WindowsDeviceGlobals.GetRunTime());
			}
			else
			{
				String += TEXT("---");
			}

			pFont->DrawText(String,
							TO_F(IPOINT(g_GraphicsCfg.m_Resolution.cx - 150,
										g_GraphicsCfg.m_Resolution.cy - 100)),
							g_InterfaceConsts.m_PerformanceColor);
		}

		// Mouse pointer
		bool bDrawPointerSprite	= false;
		bool bShowSystemCursor	= false;

		if(g_GraphicsCfg.m_bFullScreen)
		{
			bDrawPointerSprite	= true;
			bShowSystemCursor	= false;
		}
		else
		{
			const HWND hWnd = g_WindowsDeviceGlobals.GetWindow();

			const IPOINT WinCursorPos = GetKCursorPos();

			if(KWindowFromPoint(WinCursorPos) == hWnd)
			{
				if(HitsRect(KScreenToClient(hWnd, IPOINT(WinCursorPos)),
							TO_I(g_GraphicsDeviceGlobals.m_ScreenRect)))
				{
					bDrawPointerSprite	= true;
					bShowSystemCursor	= false;
				}
				else
				{
					bDrawPointerSprite	= false;
					bShowSystemCursor	= true;
				}					
			}
		}

		ShowCursorNoAcc(bShowSystemCursor);

		if(m_bMousePointerVisible && bDrawPointerSprite)
		{
			FPOINT Coords = m_MouseCoords;

			ResizePoint(m_MouseRect,
						g_GraphicsDeviceGlobals.m_ScreenRect,
						Coords);

			RoundUp(Coords);

			SPRITES_REGISTRATION_MANAGER[MOUSE_POINTER_SPRITE_INDEX]->DrawNonScaled(Coords);

			// Mouse coords
#ifdef _DEBUG

			if(g_InputDeviceGlobals.IsKeyboardKeyPressed(DIK_F11))
			{
				FPOINT Coords;

				ResizePoint(m_MouseRect,
							g_GraphicsDeviceGlobals.m_ScreenRect,
							m_MouseCoords,
							Coords);

				FONTS_REGISTRATION_MANAGER[DEFAULT_FONT_INDEX]->
					DrawText(	Coords,
								g_GraphicsDeviceGlobals.m_ScreenCenter,
								g_InterfaceConsts.m_PerformanceColor);
			}

#endif // _DEBUG
		}
	}
}

void TInterfaceDeviceGlobals::OnChangeMode()
{
	RemoveHotMessage();

	InvalidateMouseOwnership(true);
}

void TInterfaceDeviceGlobals::RemoveHotMessage()
{
	DEBUG_VERIFY_INITIALIZATION;
	
	// Message parameters
	m_HotMessageText.Empty();
	m_HotPointers.Clear();

	// Interpolator
	m_HotMessageInterpolator.
		SetCurrentSrcValue(m_HotMessageInterpolator.GetSrcValues()[0]);

	m_HotMessageInterpolator.Activate();

	m_HotMessageInterpolator.Update();
}

void TInterfaceDeviceGlobals::SetHotMessage(const KString&				MessageText,
											const FPOINT&				MessageCoords,
											const TD3DColor&			Color,
											float						fVisibilityDelay,
											const FRECT*				pPointerTargets,
											const THotPointerDirection*	pPointerDirections,
											size_t						szNPointers,
											bool						bPlayDefaultSound)
{
	DEBUG_VERIFY_INITIALIZATION;

	size_t i;

	DEBUG_VERIFY(Compare(fVisibilityDelay, 0.0f) > 0);
	
	// Setting parameters
	m_HotMessageText				= MessageText;
	m_HotMessageColor				= Color;
	m_fHotMessageVisibilityDelay	= fVisibilityDelay;

	m_HotMessageCoords = MessageCoords;

	m_HotPointers.Clear();

	if(szNPointers > 0)
	{
		DEBUG_VERIFY(pPointerTargets != NULL && pPointerDirections != NULL);

		for(i = 0 ; i < szNPointers ; i++)
		{
			const FRECT&				Target		= pPointerTargets	[i];
			const THotPointerDirection	Direction	= pPointerDirections[i];

			DEBUG_VERIFY(Target.IsFlatValid());

			const TSprite* pSprite =
				SPRITES_REGISTRATION_MANAGER[HOT_POINTER_SPRITES_START_INDEX + Direction];

			FPOINT Coords;

			if(Direction == HPD_UP)
			{
				Coords.x =	(Target.m_Left +
								Target.m_Right -
								pSprite->GetDefaultSize().cx) * 0.5f;

				Coords.y =	Target.m_Bottom +
								g_InterfaceConsts.m_HotPointerSpacing.cy;
			}
			else if(Direction == HPD_RIGHT)
			{
				Coords.x =	Target.m_Left -
								g_InterfaceConsts.m_HotPointerSpacing.cx -
								pSprite->GetDefaultSize().cx;

				Coords.y =	(Target.m_Top +
								Target.m_Bottom -
								pSprite->GetDefaultSize().cy) * 0.5f;
			}
			else if(Direction == HPD_DOWN)
			{
				Coords.x =	(Target.m_Left +
								Target.m_Right -
								pSprite->GetDefaultSize().cx) * 0.5f;

				Coords.y =	Target.m_Top -
								g_InterfaceConsts.m_HotPointerSpacing.cy -
								pSprite->GetDefaultSize().cy;
			}
			else if(Direction == HPD_LEFT)
			{
				Coords.x =	Target.m_Right +
								g_InterfaceConsts.m_HotPointerSpacing.cx;

				Coords.y =	(Target.m_Top +
								Target.m_Bottom -
								pSprite->GetDefaultSize().cy) * 0.5f;
			}
			else
			{
				INITIATE_FAILURE;
			}

			m_HotPointers.Add().Set(Coords, Direction);
		}
	}

	FSIZE TextSize;
	FONTS_REGISTRATION_MANAGER[HOT_MESSAGE_FONT_INDEX]->
		GetTextSize(m_HotMessageText, TextSize);

	m_HotMessageCoords.x -= TextSize.cx * 0.5f;
	m_HotMessageCoords.y -= TextSize.cy * 0.5f;

	// Activating the interpolator
	const float* pSrcValues = m_HotMessageInterpolator.GetSrcValues();

	float fStartValue = m_HotMessageInterpolator.GetCurrentSrcValue();
	BoundValue(fStartValue, FSEGMENT(pSrcValues[0], pSrcValues[1]));

	// Entering showing state
	m_HotMessageInterpolator.SetSpeedCoef(g_InterfaceConsts.m_fHotMessageShowSpeedCoef);

	m_HotMessageInterpolator.SetCurrentSrcValue(fStartValue);
	m_HotMessageInterpolator.Activate(pSrcValues[1]);

	if(bPlayDefaultSound)
	{
		SOUNDS_REGISTRATION_MANAGER[HOT_MESSAGE_SOUND_INDEX]->Play();
	}
}

void TInterfaceDeviceGlobals::SetHotMessage(const KString&		MessageText,
											const FPOINT&		MessageCoords,
											const TD3DColor&	Color,
											float				fVisibilityDelay,
											bool				bPlayDefaultSound)
{
	DEBUG_VERIFY_INITIALIZATION;

	SetHotMessage(	MessageText,
					MessageCoords,
					Color,
					fVisibilityDelay,
					NULL,
					NULL,
					0,
					bPlayDefaultSound);
}

void TInterfaceDeviceGlobals::SetHotMessage(const KString&			MessageText,
											const FPOINT&			MessageCoords,
											const TD3DColor&		Color,
											float					fVisibilityDelay,
											const FRECT&			PointerTarget,
											THotPointerDirection	PointerDirection,
											bool					bPlayDefaultSound)
{
	DEBUG_VERIFY_INITIALIZATION;

	SetHotMessage(	MessageText,
					MessageCoords,
					Color,
					fVisibilityDelay,
					&PointerTarget,
					&PointerDirection,
					1,
					bPlayDefaultSound);
}

void TInterfaceDeviceGlobals::SetHotMessage(const KString&		MessageText,
											const FPOINT&		MessageCoords,
											const TD3DColor&	Color,
											float				fVisibilityDelay,
											const TControl*		pPointerTarget,
											bool				bPlayDefaultSound)
{
	DEBUG_VERIFY_INITIALIZATION;

	DEBUG_VERIFY(pPointerTarget);

	SetHotMessage(	MessageText,
					MessageCoords,
					Color,
					fVisibilityDelay,
					pPointerTarget->GetCurrentScreenState().m_HotPointerRect,
					pPointerTarget->m_HotPointerDirection,
					bPlayDefaultSound);
}

// Mouse ownership
bool TInterfaceDeviceGlobals::IsMouseOwned() const
{
	// DEBUG_VERIFY_INITIALIZATION;

	return m_pMouseOwnerControl != NULL;
}

TControl* TInterfaceDeviceGlobals::GetMouseOwnerControl()
{
	// DEBUG_VERIFY_INITIALIZATION;

	return m_pMouseOwnerControl;
}

TInterface* TInterfaceDeviceGlobals::GetMouseOwnerInterface()
{
	// DEBUG_VERIFY_INITIALIZATION;
	
	return IsMouseOwned() ? &m_pMouseOwnerControl->GetInterface() : NULL;
}

void TInterfaceDeviceGlobals::SetMouseOwnerControl(	TControl*	pControl,
													bool		bSendMouseCaptureLostEvent)
{
	DEBUG_VERIFY_INITIALIZATION;
	
	DEBUG_VERIFY(pControl);

	if(m_pMouseOwnerControl == pControl)
		return;

	InvalidateMouseOwnership(bSendMouseCaptureLostEvent);

	m_pMouseOwnerControl = pControl;
}

void TInterfaceDeviceGlobals::InvalidateMouseOwnership(bool bSendMouseCaptureLostEvent)
{
	if(IsMouseOwned() && bSendMouseCaptureLostEvent)
		m_pMouseOwnerControl->OnMouseCaptureLost();

	m_pMouseOwnerControl = NULL;
}

void TInterfaceDeviceGlobals::InvalidateMouseOwnership(	TInterface*	pInterface,
														bool		bSendMouseCaptureLostEvent)
{
	if(pInterface && GetMouseOwnerInterface() == pInterface)
		InvalidateMouseOwnership(bSendMouseCaptureLostEvent);
}

void TInterfaceDeviceGlobals::InvalidateMouseOwnership(	TControl*	pControl,
														bool		bSendMouseCaptureLostEvent)
{
	if(pControl && GetMouseOwnerControl() == pControl)
		InvalidateMouseOwnership(bSendMouseCaptureLostEvent);
}

// Focus ownership
bool TInterfaceDeviceGlobals::IsFocusOwned() const
{
	// DEBUG_VERIFY_INITIALIZATION;

	return m_pFocusOwnerControl != NULL;
}

TControl* TInterfaceDeviceGlobals::GetFocusOwnerControl()
{
	// DEBUG_VERIFY_INITIALIZATION;

	return m_pFocusOwnerControl;
}

TInterface* TInterfaceDeviceGlobals::GetFocusOwnerInterface()
{
	// DEBUG_VERIFY_INITIALIZATION;
	
	return IsFocusOwned() ? &m_pFocusOwnerControl->GetInterface() : NULL;
}

void TInterfaceDeviceGlobals::SetFocusOwnerControl(	TControl*	pControl,
													bool		bSendFocusLostEvent)
{
	DEBUG_VERIFY_INITIALIZATION;
	
	DEBUG_VERIFY(pControl);

	if(m_pFocusOwnerControl == pControl)
		return;

	InvalidateFocusOwnership(bSendFocusLostEvent);

	m_pFocusOwnerControl = pControl;
}

void TInterfaceDeviceGlobals::InvalidateFocusOwnership(bool bSendFocusLostEvent)
{
	if(IsFocusOwned() && bSendFocusLostEvent)
		m_pFocusOwnerControl->OnFocusLost();

	m_pFocusOwnerControl = NULL;
}

void TInterfaceDeviceGlobals::InvalidateFocusOwnership(	TInterface*	pInterface,
														bool		bSendFocusLostEvent)
{
	if(pInterface && GetFocusOwnerInterface() == pInterface)
		InvalidateFocusOwnership(bSendFocusLostEvent);
}

void TInterfaceDeviceGlobals::InvalidateFocusOwnership(	TControl*	pControl,
														bool		bSendFocusLostEvent)
{
	if(pControl && GetFocusOwnerControl() == pControl)
		InvalidateFocusOwnership(bSendFocusLostEvent);
}

FPOINT TInterfaceDeviceGlobals::GetWindowedMouseCoords()
{
	const HWND hWnd = g_WindowsDeviceGlobals.GetWindow();

	const IPOINT CursorPos = GetKCursorPos();

	const IRECT ClientRect = KClientToScreen(hWnd, GetKClientRect(hWnd));

	return	FPOINT(	m_MouseRect.m_Left + (float)(CursorPos.x - ClientRect.m_Left) * m_MouseRect.GetWidth () / ClientRect.GetWidth(),
					m_MouseRect.m_Top  + (float)(CursorPos.y - ClientRect.m_Top)  * m_MouseRect.GetHeight() / ClientRect.GetHeight());
}

void TInterfaceDeviceGlobals::CenterMouseCursor()
{
	SetKCursorPos(GetKWindowRect(g_WindowsDeviceGlobals.GetWindow()).GetCenter());
}

void TInterfaceDeviceGlobals::SetMouseRect(const FRECT& SMouseRect)
{
	DEBUG_VERIFY_INITIALIZATION;

	DEBUG_VERIFY(!SMouseRect.IsFlat());

	ResizeSize(	FSIZE(m_MouseRect),
				FSIZE(SMouseRect),
				m_MouseSensitivity);

	ResizeSize(	FSIZE(m_MouseRect),
				FSIZE(SMouseRect),
				m_MouseDelta);

	ResizePoint(m_MouseRect,
				SMouseRect,
				m_MouseCoords);

	ResizeSize(	FSIZE(m_MouseRect),
				FSIZE(SMouseRect),
				m_MouseCoordsDelta);

	m_MouseRect = SMouseRect;
}

TMessageIterator TInterfaceDeviceGlobals::GetFirstInputMessage()
{
	DEBUG_VERIFY_INITIALIZATION;

	return m_InputMessageProcessor.GetFirst();
}