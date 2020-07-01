#ifndef interface_message_defs_h
#define interface_message_defs_h

#include <KFC_Common\message.h>
#include "interface_defs.h"

// ----------------
// Control message
// ----------------
struct TControlMessage : public TMessage
{
	TControl* m_pControl;


	TControlMessage(TControl* pSControl) :

		m_pControl(pSControl) {}
};

// -----------------------------------
// Control control visibility message
// -----------------------------------
struct TControlVisibilityMessage : public TControlMessage
{
	bool m_bNewState;


	TControlVisibilityMessage(	TControl*	pSControl,
								bool		bSNewState) :
	
		TControlMessage(pSControl),

		m_bNewState(bSNewState) {}
};

// ---------------------------
// Control enablement message
// ---------------------------
struct TControlEnablementMessage : public TControlMessage
{
	bool m_bNewState;


	TControlEnablementMessage(	TControl*	pSControl,
								bool		bSNewState) :
	
		TControlMessage(pSControl),

		m_bNewState(bSNewState) {}	
};

// -----------------------------
// Control keyboard key message
// -----------------------------
struct TControlKeyboardKeyMessage : public TControlMessage
{
	size_t	m_szKey;
	bool	m_bNewState;


	TControlKeyboardKeyMessage(	TControl*	pSControl,
								size_t		szSKey,
								bool		bSNewState) :

		TControlMessage(pSControl),

		m_szKey		(szSKey),
		m_bNewState	(bSNewState) {}
};

// ----------------------
// Control focus message
// ----------------------
struct TControlFocusMessage : public TControlMessage
{
	bool m_bNewState;


	TControlFocusMessage(	TControl*	pSControl,
							bool		bSNewState) :

		TControlMessage	(pSControl),
		m_bNewState		(bSNewState) {}
};

// -----------------------------
// Control mouse button message
// -----------------------------
struct TControlMouseButtonMessage : public TControlMessage
{
	size_t	m_szButton;
	bool	m_bNewState;
	FPOINT	m_Coords;


	TControlMouseButtonMessage(	TControl*		pSControl,
								size_t			szSButton,
								bool			bSNewState,
								const FPOINT&	SCoords) :

		TControlMessage(pSControl),

		m_szButton	(szSButton),
		m_bNewState	(bSNewState),
		m_Coords	(SCoords) {}
};

// -------------------------------
// Control mouse movement message
// -------------------------------
struct TControlMouseMovementMessage : public TControlMessage
{
	FPOINT	m_DstCoords;
	FSIZE	m_Delta;


	TControlMouseMovementMessage(	TControl*		pSControl,
									const FSIZE&	SDelta,
									const FPOINT&	SDstCoords) :

		TControlMessage(pSControl),

		m_DstCoords	(SDstCoords),
		m_Delta		(SDelta) {}
};

// -------------------------------
// Control mouse hovering message
// -------------------------------
struct TControlMouseHoveringMessage : public TControlMessage
{
	FPOINT	m_Coords;
	bool	m_bNewState;


	TControlMouseHoveringMessage(	TControl*		pSControl,
									bool			bSNewState,
									const FPOINT&	SCoords) :

		TControlMessage(pSControl),

		m_Coords	(SCoords),
		m_bNewState	(bSNewState){}
};

// --------------------------------
// Iteractive control push message
// --------------------------------
struct TInteractiveControlPushMessage : public TControlMessage
{
	bool m_bNewState;
	bool m_bFromMouse;


	TInteractiveControlPushMessage(	TControl*	pSControl,
									bool		bSNewState,
									bool		bSFromMouse) :

		TControlMessage(pSControl),

		m_bNewState	(bSNewState),
		m_bFromMouse(bSFromMouse) {}
};

// ----------------------------------
// Interactive control click message
// ----------------------------------
struct TInteractiveControlClickMessage : public TControlMessage
{
	bool m_bFromMouse;


	TInteractiveControlClickMessage(TControl*	pSControl,
									bool		bSFromMouse) :

		TControlMessage(pSControl),

		m_bFromMouse(bSFromMouse) {}
};

// ---------------------------------------
// Interactive control push click message
// ---------------------------------------
struct TInteractiveControlPushClickMessage : public TControlMessage
{
	bool m_bFromMouse;


	TInteractiveControlPushClickMessage(TControl*	pSControl,
										bool		bSFromMouse) :

		TControlMessage(pSControl),

		m_bFromMouse(bSFromMouse) {}
};

// -------------------------------
// Checkbox control check message
// -------------------------------
struct TCheckBoxControlCheckMessage : public TControlMessage
{
	bool m_bNewState;
	bool m_bFromMouse;


	TCheckBoxControlCheckMessage(	TControl*	pSControl,
									bool		bSNewState,
									bool		bSFromMouse) :

		TControlMessage(pSControl),

		m_bNewState	(bSNewState),
		m_bFromMouse(bSFromMouse) {}
};

// ------------------------------
// Scroll control scroll message
// ------------------------------
struct TScrollControlScrollMessage : public TControlMessage
{
	int m_iDirection;


	TScrollControlScrollMessage(TControl*	pSControl,
								int			iSDirection) :

		TControlMessage(pSControl),

		m_iDirection(iSDirection) {}
};

#endif // interface_message_defs_h
