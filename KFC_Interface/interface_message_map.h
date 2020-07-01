#ifndef interface_message_map
#define interface_message_map

#include "interface_message_defs.h"
#include "control_defs.h"
#include "checkbox_control.h"
#include "scroll_control.h"

// ----------------------
// Interface message map
// ----------------------
template <class ClassType>
class TInterfaceMessageMap
{
private:
	bool m_bAllocated;

public:
	typedef void (ClassType::*TInteractiveControlPushHandler)(	TControl*	pControl,
																bool		bNewState);

	typedef void (ClassType::*TInteractiveControlClickHandler)(TControl* pControl);

	typedef void (ClassType::*TInteractiveControlPushClickHandler)(TControl* pControl);

	// Checkbox control handlers
	typedef void (ClassType::*TCheckBoxControlCheckHandler)(TControl*	pControl,
															bool		bNewState);

	// Scroll control handlers
	typedef void (ClassType::*TScrollControlScrollHandler)(	TControl*	pControl,
															int			iDirection);

private:
	ClassType* m_pObject;

	const TInterface* m_pInterface;


	struct TInteractiveControlPushEntry
	{
		TInteractiveControlPushHandler	m_Handler;
		SZSEGMENT						m_IDs;


		void Set(	TInteractiveControlPushHandler	SHandler,
					size_t							szFirstID,
					size_t							szAmt = 1)
		{
			m_Handler = SHandler;
			m_IDs.Set(szFirstID, szFirstID + szAmt);
		}
	};

	struct TInteractiveControlClickEntry
	{
		TInteractiveControlClickHandler	m_Handler;
		SZSEGMENT						m_IDs;


		void Set(	TInteractiveControlClickHandler	SHandler,
					size_t							szFirstID,
					size_t							szAmt = 1)
		{
			m_Handler = SHandler;
			m_IDs.Set(szFirstID, szFirstID + szAmt);
		}
	};

	struct TInteractiveControlPushClickEntry
	{
		TInteractiveControlPushClickHandler	m_Handler;
		SZSEGMENT							m_IDs;


		void Set(	TInteractiveControlPushClickHandler	SHandler,
					size_t								szFirstID,
					size_t								szAmt = 1)
		{
			m_Handler = SHandler;
			m_IDs.Set(szFirstID, szFirstID + szAmt);
		}
	};

	struct TCheckBoxControlCheckEntry
	{
		TCheckBoxControlCheckHandler	m_Handler;
		SZSEGMENT						m_IDs;


		void Set(	TCheckBoxControlCheckHandler	SHandler,
					size_t							szFirstID,
					size_t							szAmt = 1)
		{
			m_Handler = SHandler;
			m_IDs.Set(szFirstID,  szFirstID + szAmt);
		}
	};

	struct TScrollControlScrollEntry
	{
		TScrollControlScrollHandler	m_Handler;
		SZSEGMENT					m_IDs;


		void Set(	TScrollControlScrollHandler	SHandler,
					size_t						szFirstID,
					size_t						szAmt = 1)
		{
			m_Handler = SHandler;
			m_IDs.Set(szFirstID, szFirstID + szAmt);
		}
	};

	TArray<TInteractiveControlPushEntry>		m_InteractiveControlPushEntries;
	TArray<TInteractiveControlClickEntry>		m_InteractiveControlClickEntries;
	TArray<TInteractiveControlPushClickEntry>	m_InteractiveControlPushClickEntries;
	TArray<TCheckBoxControlCheckEntry>			m_CheckBoxControlCheckEntries;
	TArray<TScrollControlScrollEntry>			m_ScrollControlScrollEntries;

public:
	TInterfaceMessageMap();
	
	~TInterfaceMessageMap()
		{ Release(); }

	bool IsAllocated() const
		{ return m_bAllocated; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(ClassType& SObject, const TInterface& SInterface);

	void AddInteractiveControlPushEntry(TInteractiveControlPushHandler	Handler,
										size_t							szFirstID,
										size_t							szAmt = 1);

	void AddInteractiveControlClickEntry(	TInteractiveControlClickHandler	Handler,
											size_t								szFirstID,
											size_t								szAmt = 1);

	void AddInteractiveControlPushClickEntry(	TInteractiveControlPushClickHandler	Handler,
												size_t								szFirstID,
												size_t								szAmt = 1);

	void AddCheckBoxControlCheckEntry(	TCheckBoxControlCheckHandler	Handler,
										size_t							szFirstID,
										size_t							szAmt = 1);

	void AddScrollControlScrollEntry(	TScrollControlScrollHandler	Handler,
										size_t						szFirstID,
										size_t						szAmt = 1);
	
	void ProcessMessages() const;
};

template <class ClassType>
TInterfaceMessageMap<ClassType>::TInterfaceMessageMap()
{
	m_bAllocated = false;
	
	m_pObject		= NULL;
	m_pInterface	= NULL;
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::Release(bool bFromAllocatorException)
{
	if(m_bAllocated || bFromAllocatorException)
	{
		m_bAllocated = false;

		m_ScrollControlScrollEntries.			Clear();
		m_CheckBoxControlCheckEntries.			Clear();
		m_InteractiveControlPushClickEntries.	Clear();
		m_InteractiveControlClickEntries.		Clear();
		m_InteractiveControlPushEntries.		Clear();		

		m_pObject		= NULL;
		m_pInterface	= NULL;
	}
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::Allocate(ClassType& SObject, const TInterface& SInterface)
{
	Release();

	try
	{
		m_pObject		= &SObject;
		m_pInterface	= &SInterface;

		m_bAllocated = true;
	}

	catch(...)
	{
		Release(true);
		throw;
	}
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::AddInteractiveControlPushEntry(	TInteractiveControlPushHandler	Handler,
																		size_t							szFirstID,
																		size_t							szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstID != CONTROL_ID_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_InteractiveControlPushEntries.Add().Set(Handler, szFirstID, szAmt);
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::AddInteractiveControlClickEntry(	TInteractiveControlClickHandler	Handler,
																		size_t							szFirstID,
																		size_t							szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstID != CONTROL_ID_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_InteractiveControlClickEntries.Add().Set(Handler, szFirstID, szAmt);
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::AddInteractiveControlPushClickEntry(	TInteractiveControlPushClickHandler	Handler,
																			size_t								szFirstID,
																			size_t								szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstID != CONTROL_ID_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_InteractiveControlPushClickEntries.Add().Set(Handler, szFirstID, szAmt);
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::AddCheckBoxControlCheckEntry(	TCheckBoxControlCheckHandler	Handler,
																	size_t							szFirstID,
																	size_t							szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstID != CONTROL_ID_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_CheckBoxControlCheckEntries.Add().Set(Handler, szFirstID, szAmt);
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::AddScrollControlScrollEntry(	TScrollControlScrollHandler	Handler,
																	size_t						szFirstID,
																	size_t						szAmt)
{
	DEBUG_VERIFY_ALLOCATION;

	DEBUG_VERIFY(szFirstID != CONTROL_ID_NONE);
	DEBUG_VERIFY(szAmt > 0);	

	m_ScrollControlScrollEntries.Add().Set(Handler, szFirstID, szAmt);
}

template <class ClassType>
void TInterfaceMessageMap<ClassType>::ProcessMessages() const
{
	DEBUG_VERIFY_ALLOCATION;

	size_t i;

	for(TMessageIterator Iter = m_pInterface->GetFirstMessage() ;
		Iter.IsValid() ;
		++Iter)
	{
		const TControlMessage* pControlMessage =
			dynamic_cast<const TControlMessage*>(Iter.GetDataPtr());

		DEBUG_VERIFY(pControlMessage);

		if(!pControlMessage->m_pControl->IsValidMessage(pControlMessage))
			continue;

		// Interactive contorol push message
		{
			const TInteractiveControlPushMessage* pMessage;

			if(pMessage = dynamic_cast<const TInteractiveControlPushMessage*>(Iter.GetDataPtr()))
			{
				TControl* const pControl = pMessage->m_pControl;

				for(i = 0 ; i < m_InteractiveControlPushEntries.GetN() ; i++)
				{
					if(HitsSegment(	pControl->GetID(),
									m_InteractiveControlPushEntries[i].m_IDs))
					{
						(m_pObject->*m_InteractiveControlPushEntries[i].m_Handler)
							(pControl, pMessage->m_bNewState);
					}
				}
			}
		}
		
		// Interactive control click message
		{
			const TInteractiveControlClickMessage* pMessage;

			if(pMessage = dynamic_cast<const TInteractiveControlClickMessage*>(Iter.GetDataPtr()))
			{
				TControl* const pControl = pMessage->m_pControl;

				for(i = 0 ; i < m_InteractiveControlClickEntries.GetN() ; i++)
				{
					if(HitsSegment(	pControl->GetID(),
									m_InteractiveControlClickEntries[i].m_IDs))
					{
						(m_pObject->*m_InteractiveControlClickEntries[i].m_Handler)
							(pControl);
					}
				}
			}
		}

		// Interactive contorl push click message		
		{
			const TInteractiveControlPushClickMessage* pMessage;

			if(pMessage = dynamic_cast<const TInteractiveControlPushClickMessage*>(Iter.GetDataPtr()))
			{			
				TControl* const pControl = pMessage->m_pControl;

				for(i = 0 ; i < m_InteractiveControlPushClickEntries.GetN() ; i++)
				{
					if(HitsSegment(	pControl->GetID(),
									m_InteractiveControlPushClickEntries[i].m_IDs))
					{
						(m_pObject->*m_InteractiveControlPushClickEntries[i].m_Handler)
							(pControl);
					}
				}
			}
		}

		// Checkbox control check message
		{
			const TCheckBoxControlCheckMessage* pMessage;

			if(pMessage = dynamic_cast<const TCheckBoxControlCheckMessage*>(Iter.GetDataPtr()))
			{
				TControl* const pControl = pMessage->m_pControl;

				if(pControl->GetCurrentScreenState().IsEnabled())
				{
					for(i = 0 ; i < m_CheckBoxControlCheckEntries.GetN() ; i++)
					{
						if(HitsSegment(	pControl->GetID(),
										m_CheckBoxControlCheckEntries[i].m_IDs))
						{
							(m_pObject->*m_CheckBoxControlCheckEntries[i].m_Handler)
								(pControl, pMessage->m_bNewState);
						}
					}
				}
			}
		}

		// Scrol control scroll message
		{
			const TScrollControlScrollMessage* pMessage;

			if(pMessage = dynamic_cast<const TScrollControlScrollMessage*>(Iter.GetDataPtr()))
			{
				TControl* const pControl = pMessage->m_pControl;

				for(i = 0 ; i < m_ScrollControlScrollEntries.GetN() ; i++)
				{
					if(HitsSegment(	pControl->GetID(),
									m_ScrollControlScrollEntries[i].m_IDs))
					{
						(m_pObject->*m_ScrollControlScrollEntries[i].m_Handler)
							(pControl, pMessage->m_iDirection);
					}
				}
			}
		}
	}
}

#endif // interface_message_map