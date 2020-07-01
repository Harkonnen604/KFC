#ifndef interface_input_message_defs_h
#define interface_input_message_defs_h

#include <KFC_Common\message.h>

// -------------------------------------
// Interface input keyboard key message
// -------------------------------------
struct TInterfaceInputKeyboardKeyMessage : public TMessage
{
	size_t	m_szKey;
	bool	m_bNewState;


	TInterfaceInputKeyboardKeyMessage(	size_t	szSKey,
										bool	bSNewState) :

		m_szKey		(szSKey),
		m_bNewState	(bSNewState) {}	
};

// -------------------------------------
// Interface input mouse button message
// -------------------------------------
struct TInterfaceInputMouseButtonMessage : public TMessage
{
	size_t	m_szButton;
	bool	m_bNewState;
	FPOINT	m_Coords;


	TInterfaceInputMouseButtonMessage(	size_t			szSButton,
										bool			bSNewState,
										const FPOINT&	SCoords) :

		m_szButton	(szSButton),
		m_bNewState	(bSNewState),
		m_Coords	(SCoords) {}
};

// ---------------------------------------
// Interface input mouse movement message
// ---------------------------------------
struct TInterfaceInputMouseMovementMessage : public TMessage
{
	FSIZE m_Delta;


	TInterfaceInputMouseMovementMessage(const FSIZE& SDelta) :
		m_Delta	(SDelta) {}
};

// --------------------------------------------
// Interface input mouse coords change message
// --------------------------------------------
struct TInterfaceInputMouseCoordsChangeMessage : public TMessage
{
	FPOINT	m_DstCoords;
	FSIZE	m_Delta;


	TInterfaceInputMouseCoordsChangeMessage(const FPOINT&	SDstCoords,
											const FSIZE&	SDelta) :

		m_DstCoords	(SDstCoords),
		m_Delta		(SDelta) {}
};

#endif // interface_input_message_defs_h