#ifndef ctrl_deref_h
#define ctrl_deref_h

#include "interface_defs.h"
#include "control.h"

// -----------------------
// Dereferencing routines
// -----------------------
inline TControl* IterToCtrl(TControls::TIterator Iter)
	{ return Iter.IsValid() ? &**Iter : NULL; }

inline const TControl* IterToCtrl(TControls::TConstIterator Iter)
	{ return Iter.IsValid() ? &**Iter : NULL; }

inline TControls::TIterator CtrlToIter(TControl* pControl)
	{ return pControl ? pControl->GetIter() : NULL; }

inline TControls::TConstIterator CtrlToIter(const TControl* pControl)
	{ return pControl ? pControl->GetIter() : NULL; }

#endif // ctrl_deref_h