#ifndef interface_input_message_map_h
#define interface_input_message_map_h

#include "interface_device_globals.h"
#include "interface_input_message_defs.h"

// ----------------------------
// Interface input message map
// ----------------------------
template <class ClassType>
class TInterfaceInputMessageMap
{
private:
	bool m_bAllocated;

public:
	typedef void (ClassType::*TKeyboardKeyHandler)(	size_t	szKey,
													bool	bNewState);

	typedef void (ClassType::*TMouseButtonHandler)(	size_t			szButton,
													bool			bNewState,
													const FPOINT&	Coords);

	typedef void (ClassType::*TMouseMovementHandler)(const FSIZE& Delta);

	typedef void (ClassType::*TMouseCoordsChangeHandler)(	const FPOINT&	DstCoords,
															const FSIZE&	Delta);

private:
	ClassType* m_pObject;

	struct TKeyboardKeyEntry
	{
		TKeyboardKeyHandler	m_Handler;
		SZSEGMENT			m_Keys;

		
		void Set(	TKeyboardKeyHandler	SHandler,
					size_t				szFirstKey,
					size_t				szAmt = 1)
		{
			m_Handler = SHandler;
			m_Keys.Set(szFirstKey, szFirstKey + szAmt);
		}
	};
	
	struct TMouseButtonEntry
	{
		TMouseButtonHandler	m_Handler;
		SZSEGMENT			m_Buttons;


		void Set(	TMouseButtonHandler	SHandler,
					size_t				szFirstButton,
					size_t				szAmt = 1)
		{
			m_Handler = SHandler;
			m_Buttons.Set(szFirstButton, szFirstButton + szAmt);
		}
	};

	struct TMouseMovementEntry
	{
		TMouseMovementHandler m_Handler;
		
		
		void Set(TMouseMovementHandler SHandler)
		{
			m_Handler = SHandler;
		}
	};

	struct TMouseCoordsChangeEntry
	{
		TMouseCoordsChangeHandler m_Handler;

		void Set(TMouseCoordsChangeHandler SHandler)
		{
			m_Handler = SHandler;
		}
	};

	TArray<TKeyboardKeyEntry,		true> m_KeyboardKeyEntries;
	TArray<TMouseButtonEntry,		true> m_MouseButtonEntries;
	TArray<TMouseMovementEntry,		true> m_MouseMovementEntries;
	TArray<TMouseCoordsChangeEntry,	true> m_MouseCoordsChangeEntries;

public:
	TInterfaceInputMessageMap();
	
	~TInterfaceInputMessageMap()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(ClassType& SObject);

	void AddKeyboardKeyEntry(	TKeyboardKeyHandler	Handler,
								size_t				szFirstKey,
								size_t				szAmt = 1);

	void AddMouseButtonEntry(	TMouseButtonHandler	Handler,
								size_t				szButton,
								size_t				szAmt = 1);

	void AddMouseMovementEntry(TMouseMovementHandler Handler);

	void AddMouseCoordsChangeEntry(TMouseCoordsChangeHandler Handler);

	void ProcessMessages() const;
};

template <class ClassType>
TInterfaceInputMessageMap<ClassType>::TInterfaceInputMessageMap()
{
	m_bAllocated = false;
	
	m_pObject = NULL;
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_MouseMovementEntries.	Release();
		m_MouseButtonEntries.	Release();
		m_KeyboardKeyEntries.	Release();

		m_pObject = NULL;
	}
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::Allocate(ClassType& SObject)
{
	Release();

	try
	{
		m_pObject = &SObject;

		m_KeyboardKeyEntries.	Allocate(g_CommonConsts.m_MessageMapAllocation);
		m_MouseButtonEntries.	Allocate(g_CommonConsts.m_MessageMapAllocation);
		m_MouseMovementEntries.	Allocate(g_CommonConsts.m_MessageMapAllocation);

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::AddKeyboardKeyEntry(	TKeyboardKeyHandler	Handler,
																size_t				szFirstKey,
																size_t				szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstKey != DIK_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_KeyboardKeyEntries.Add().Set(Handler, szFirstKey, szAmt);
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::AddMouseButtonEntry(	TMouseButtonHandler	Handler,
																size_t				szFirstButton,
																size_t				szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstButton != DIM_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_MouseButtonEntries.Add().Set(Handler, szFirstButton, szAmt);
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::AddMouseMovementEntry(TMouseMovementHandler Handler)
{
	DEBUG_VERIFY_ALLOCATION;

	m_MouseMovementEntries.Add().Set(Handler);
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::AddMouseCoordsChangeEntry(TMouseCoordsChangeHandler Handler)
{
	DEBUG_VERIFY_ALLOCATION;

	m_MouseCoordsChangeEntries.Add().Set(Handler);
}

template <class ClassType>
void TInterfaceInputMessageMap<ClassType>::ProcessMessages() const
{
	DEBUG_VERIFY_ALLOCATION;

	size_t i;

	for(TMessageIterator Iter = g_InterfaceDeviceGlobals.GetFirstInputMessage() ;
		Iter.IsValid() ;
		++Iter)
	{
		// Key key mesage
		{
			const TInterfaceInputKeyboardKeyMessage* pMessage;

			if(pMessage = dynamic_cast<const TInterfaceInputKeyboardKeyMessage*>(Iter.GetDataPtr()))
			{
				if(	g_InputDeviceGlobals.IsKeyboardKeyPressed(pMessage->m_szKey) ==
						pMessage->m_bNewState)
				{
					for(i = 0 ; i < m_KeyboardKeyEntries.GetN() ; i++)
					{
						if(HitsSegment(	pMessage->m_szKey,
										m_KeyboardKeyEntries[i].m_Keys))
						{
							(m_pObject->*m_KeyboardKeyEntries[i].m_Handler)
								(pMessage->m_szKey, pMessage->m_bNewState);
						}
					}
				}
			}
		}

		// Mouse button message
		{
			const TInterfaceInputMouseButtonMessage* pMessage;

			if(pMessage = dynamic_cast<const TInterfaceInputMouseButtonMessage*>(Iter.GetDataPtr()))
			{
				if(	g_InputDeviceGlobals.IsMouseButtonPressed(pMessage->m_szButton) ==
					pMessage->m_bNewState)
				{
					for(i = 0 ; i < m_MouseButtonEntries.GetN() ; i++)
					{
						if(HitsSegment(	pMessage->m_szButton,
										m_MouseButtonEntries[i].m_Buttons))
						{
							(m_pObject->*m_MouseButtonEntries[i].m_Handler)
								(pMessage->m_szButton, pMessage->m_bNewState, pMessage->m_Coords);
						}
					}
				}
			}
		}

		// Mouse movement message
		{
			const TInterfaceInputMouseMovementMessage* pMessage;

			if(pMessage = dynamic_cast<const TInterfaceInputMouseMovementMessage*>(Iter.GetDataPtr()))
			{
				if(!g_InputDeviceGlobals.m_MouseDelta.IsPoint())
				{
					for(i = 0 ; i < m_MouseMovementEntries.GetN() ; i++)
					{
						(m_pObject->*m_MouseMovementEntries[i].m_Handler)
							(pMessage->m_Delta);
					}
				}
			}
		}

		// Mouse input mouse coords change
		{
			const TInterfaceInputMouseCoordsChangeMessage* pMessage;

			if(pMessage = dynamic_cast<const TInterfaceInputMouseCoordsChangeMessage*>(Iter.GetDataPtr()))
			{
				if(!g_InputDeviceGlobals.m_MouseCoordsDelta.IsPoint())
				{
					for(i = 0 ; i < m_MouseCoordsChangeEntries.GetN() ; i++)
					{
						(m_pObject->*m_MouseCoordsChangeEntries[i].m_Handler)
							(pMessage->m_DstCoords, pMessage->m_Delta);
					}
				}
			}
		}
	}
}

#endif // interface_input_message_map_h