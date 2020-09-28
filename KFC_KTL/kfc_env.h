#ifndef kfc_env_h
#define kfc_env_h

// ----------------
// Global routines
// ----------------
inline LPCTSTR getenv_safe(LPCTSTR pName)
{
    LPCTSTR p = _tgetenv(pName);

    return p ? p : TEXT("");
}

#endif // kfc_env_h
