#ifndef sdl_decl_h
#define sdl_decl_h

// SDL interface return codes
#define SDL_INTR_OK         (1)
#define SDL_INTR_CANCEL     (2)
#define SDL_INTR_YES        (3)
#define SDL_INTR_NO         (4)
#define SDL_INTR_RETRY      (5)
#define SDL_INTR_ABORT      (6)
#define SDL_INTR_IGNORE     (7)

#define SDL_INTR_PASSWORD(id)   (128 + (id))

// Declarations
class T_SDL_Control;
class T_SDL_Interface;

// -------------------------
// SDL interface event sink
// -------------------------
class T_SDL_InterfaceEventSink
{
public:
    virtual ~T_SDL_InterfaceEventSink() {}

    virtual void OnActivate() {}

    virtual bool OnPush(T_SDL_Control* pControl) { return true; }

    virtual void OnClick(T_SDL_Control* pControl) {}

    virtual void OnTimer(size_t szID) {}

    virtual void OnFocusChange(T_SDL_Control* pOldFocus, T_SDL_Control* pNewFocus) {}

    virtual void OnPassword(size_t szID) {}

    virtual void OnIdle() {}
};

struct T_SDL_InterfacePassword;

typedef TList<TPtrHolder<T_SDL_Control> > T_SDL_Controls;

typedef T_SDL_Controls::TIterator T_SDL_ControlHandle;

typedef T_SDL_Controls::TConstIterator T_SDL_ControlConstHandle;

class T_SDL_ResourceStorage;
class T_SDL_ResourceID_Map;

struct T_SDL_Timer;
class  T_SDL_Timers;

#endif // sdl_decl_h
