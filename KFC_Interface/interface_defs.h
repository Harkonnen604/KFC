#ifndef interface_defs_h
#define interface_defs_h

#include <KFC_Graphics\d3d_color.h>

// ------------------
// Main declarations
// ------------------
class TControl;
class TInterface;

// -----------------------
// Control pointer holder
// -----------------------
typedef TPtrHolder<TControl> TControlPtrHolder;

// ---------
// Controls
// ---------
typedef TTree<TControlPtrHolder> TControls;

// ----------------
// Control pointer
// ----------------
typedef TObjectPointer<TControl> TControlPointer;

// -----------------
// Control pointers
// -----------------
typedef TArray<TControlPointer> TControlPointers;

// --------------
// Control state
// --------------
struct TControlState;

// --------------------
// Control input state
// --------------------
struct TControlInputState;

#endif // interface_defs_h
