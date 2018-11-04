#ifndef message_loop_h
#define message_loop_h

#ifdef _MSC_VER

// -------------
// Aux messages
// -------------
typedef TArray<CWPSTRUCT, true> TAuxMessages;

// ----------------
// Global routines
// ----------------
void RunMessageLoop(HWND hAccelWnd = NULL, HACCEL hAccel = NULL);

void PostAuxMessage(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

#endif // _MSC_VER

#endif // message_loop_h
