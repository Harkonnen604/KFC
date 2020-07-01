#ifndef dummy_control_h
#define dummy_control_h

#include "control.h"

// ------------------------------
// Dummy control creation struct
// ------------------------------
struct TDummyControlCreationStruct : public TControlCreationStruct
{
	TDummyControlCreationStruct();

	void Load(	TInfoNodeConstIterator	InfoNode,
				const TControl*			pParentControl,
				const FRECT&			Resolution);
};

// --------------
// Dummy control
// --------------
class TDummyControl : public TControl
{
public:
	static TControl* LoadControl(	type_t					tpType,
									TInfoNodeConstIterator	InfoNode,
									const TControl*			pParentControl,
									const FRECT&			Resolution);

	TDummyControl(const TDummyControlCreationStruct& CreationStruct);
};

#endif // dummy_control_h