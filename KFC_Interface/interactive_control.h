#ifndef interactive_control_h
#define interactive_control_h

#include <KFC_Common\interpolator.h>
#include <KFC_Sound\sound.h>
#include "control.h"
#include "control_interpolated_state.h"

// ------------------------------------
// Interactive control creation struct
// ------------------------------------
struct TInteractiveControlCreationStruct : public TControlCreationStruct
{
	size_t m_szHotKey;

	float m_InteractiveTransitionDelays[3];

	bool	m_bAllowNonHoveredClicks;
	bool	m_bAllowMouseStealing;
	size_t	m_szPushClickFirstDelay;
	size_t	m_szPushClickRepeatDelay;


	TInteractiveControlCreationStruct();

	void Load(	TInfoNodeConstIterator	InfoNode,
				const TControl*			pParentControl,
				const FRECT&			Resolution);

	void SetImmediateInteractiveTransitions();
};

// ------------------------------------
// Interactive control sounds provider
// ------------------------------------
struct TInteractiveControlSoundsProvider
{
	TObjectPointer<TSound> m_PushSound;
	TObjectPointer<TSound> m_ClickSound;


	TInteractiveControlSoundsProvider();

	void Load(TInfoNodeConstIterator InfoNode);
};

// --------------------
// Interactive control
// --------------------
class TInteractiveControl : public TControl
{
public:
	enum TPushState
	{
		PS_NONE			= 0,
		PS_MOUSE		= 1,
		PS_KEYBOARD		= 2,
		PS_FORCE_UINT	= UINT_MAX,

	};

	enum TInteractiveState
	{
		IS_DISABLED		= 0,
		IS_NONHOVERED	= 1,
		IS_HOVERED		= 2,
		IS_PUSHED		= 3,
		IS_FORCE_UINT	= UINT_MAX,

	};
	
private:
	float m_InteractiveTransitionDelays[3];

	size_t m_szHotKey;

	size_t m_szPushClickFirstDelay;
	size_t m_szPushClickRepeatDelay;

	TTimer	m_PushClickTimer;
	bool	m_bFirstPushClickDelay;

	TPushState m_PushState;

	TControlInterpolatedState<TInteractiveState, 4> m_InteractiveState;

	
	TInteractiveState DetermineInteractiveState() const;

protected:
	// Allocation
	virtual void OnAllocate();

	virtual void OnSetInitialValues();

	// Update/render events
	virtual bool OnSuspend	();
	virtual bool OnResume	();

	virtual void OnPreUpdate();

	virtual void OnPostUpdate();

	// Appearance events
	virtual void OnEnable();

	virtual void OnDisable();
	
	// Mouse events
	virtual void OnMouseButtonDown(size_t szButton);

	virtual void OnMouseButtonUp(size_t szButton);

	virtual void OnMouseEnter();

	virtual void OnMouseLeave();

	virtual void OnMouseCaptureLost();

	// Keyboard events
	virtual void OnKeyboardKeyDown(size_t szKey);

	virtual void OnKeyboardKeyUp(size_t szKey);

	// Push/unpush events
	virtual void OnPush(bool bFromMouse);

	virtual void OnUnpush(bool bFromMouse);

	// Click events
	virtual void OnClick(bool bFromMouse);

	virtual void OnPushClick(bool bFromMouse);

	// Push/unpush routines
	void Push(bool bFromMouse);

	void Unpush(bool bFromMouse);

	// Click routines
	void Click(bool bFromMouse);

	void PushClick(bool bFromMouse);

public:
	bool m_bAllowNonHoveredClicks;
	bool m_bAllowMouseStealing;

	TObjectPointer<TSound> m_PushSound;
	TObjectPointer<TSound> m_ClickSound;
	
	TD3DColor m_StateColors[4];
	

	TInteractiveControl(const TInteractiveControlCreationStruct&	CreationStruct,
						TInteractiveControlSoundsProvider&			SoundsProvider);

	// Messages
	bool IsValidMessage(const TControlMessage* pMessage) const;

	// ---------------- TRIVIALS ----------------
	size_t GetHotKey() const { return m_szHotKey; }

	TPushState GetPushState() const { return m_PushState; }	

	TInteractiveState GetInteractiveState() const
		{ return m_InteractiveState; }

	float GetInterpolatedInteractiveState() const
		{ return m_InteractiveState.GetInterpolatedState(); }
};

// ----------------
// Global routines
// ----------------
bool IsInteractiveControl(const TControl* pControl);

#endif // interactive_control_h