#ifndef debug_h
#define debug_h

#include "kfc_mem.h"
#include "basic_macros.h"
#include "kstring.h"
#include "debug_message_box.h"
#include "termination.h"

// ---------------
// dputs, dprintf
// ---------------
#ifdef _DEBUG
    #define dputs       puts
    #define dprintf     printf
#else // _DEBUG
    #ifdef _MSC_VER
        #define dputs       __noop
        #define dprintf     __noop
    #else // _MSC_VER
        inline int dputs    (const char*)       { return 1; }
        inline int dprintf  (const char*, ...)  { return 0; }
    #endif // _MSC_VER
#endif // _DEBUG

// ------------
// Test assert
// ------------
void _tassert(LPCTSTR pExp, LPCTSTR pFile, int iLine);

#define tassert(exp) \
    ((exp) || (_tassert(TEXT(#exp), TEXT(__FILE__), __LINE__), 0))

// -------
// Consts
// -------
extern const bool g_bDebug;
extern const bool g_bDevelop;

// --------------
// KFC exception
// --------------
struct T_KFC_Exception {};

// -----------
// Error text
// -----------
KString& GetErrorText();

void ShowErrorText(LPCTSTR pText = GetErrorText());

KString GetErrorDescription(DWORD dwCode);

// ------------------
// Outer block guard
// ------------------
class TOuterBlockGuard
{
public:
    TOuterBlockGuard();

    ~TOuterBlockGuard();
};

// -----------------------
// Fast outer block guard
// -----------------------
class TFastOuterBlockGuard
{
public:
    TFastOuterBlockGuard();

    ~TFastOuterBlockGuard();
};

// -----------
// Test block
// -----------
#define TEST_BLOCK_BEGIN                                try { {
#define TEST_BLOCK_CUSTOM_HANDLER(Type)                 } GetErrorText().Empty(); } catch(Type) { {
#define TEST_BLOCK_KFC_EXCEPTION_HANDLER                TEST_BLOCK_CUSTOM_HANDLER(T_KFC_Exception)
#define TEST_BLOCK_TERMINATION_EXCEPTION_HANDLER(Name)  TEST_BLOCK_CUSTOM_HANDLER(const TTerminationException& Name)
#define TEST_BLOCK_ANY_EXCEPTION_HANDLER                TEST_BLOCK_CUSTOM_HANDLER(...)
#define TEST_BLOCK_END                                  } GetErrorText().Empty(); }

// -----------------
// Debug test block
// -----------------
#ifdef _DEBUG

    #define DEBUG_TEST_BLOCK_BEGIN                                  TEST_BLOCK_BEGIN
    #define DEBUG_TEST_BLOCK_CUSTOM_HANDLER(Type)                   TEST_BLOCK_CUSTOM_HANDLER(Type)
    #define DEBUG_TEST_BLOCK_KFC_EXCEPTION_HANDLER                  TEST_BLOCK_KFC_EXCEPTION_HANDLER
    #define DEBUG_TEST_BLOCK_TERMINATION_EXCEPTION_HANDLER(Name)    TEST_BLOCK_TERMINATION_EXCEPTION_HANDLER(Name)
    #define DEBUG_TEST_BLOCK_ANY_EXCEPTION_HANDLER                  TEST_BLOCK_ANY_EXCEPTION_HANDLER
    #define DEBUG_TEST_BLOCK_END                                    TEST_BLOCK_END

#else // _DEBUG

    #define DEBUG_TEST_BLOCK_BEGIN                                  {
    #define DEBUG_TEST_BLOCK_CUSTOM_HANDLER(Type)                   } if(false) {
    #define DEBUG_TEST_BLOCK_KFC_EXCEPTION_HANDLER                  } if(false) {
    #define DEBUG_TEST_BLOCK_TERMINATION_EXCEPTION_HANDLER(Name)    } if(false) {
    #define DEBUG_TEST_BLOCK_ANY_EXCEPTION_HANDLER                  } if(false) {
    #define DEBUG_TEST_BLOCK_END                                    }

#endif // _DEBUG

// -----------
// Safe block
// -----------
#define SAFE_BLOCK_BEGIN    TEST_BLOCK_BEGIN
#define SAFE_BLOCK_END      TEST_BLOCK_ANY_EXCEPTION_HANDLER TEST_BLOCK_END

// -------------------
// Release safe block
// -------------------
#ifdef _DEBUG

    #define RELEASE_SAFE_BLOCK_BEGIN    {
    #define RELEASE_SAFE_BLOCK_END      }

#else // _DEBUG

    #define RELEASE_SAFE_BLOCK_BEGIN    SAFE_BLOCK_BEGIN
    #define RELEASE_SAFE_BLOCK_END      SAFE_BLOCK_END

#endif // _DEBUG

// ---------------------------
// Defined failure initiation
// ---------------------------
inline void EmptyErrorText()
    { GetErrorText().Empty(); }

void UpdateErrorText(LPCTSTR pText);

#define INITIATE_DEFINED_FAILURE(Text)  \
    do                                  \
    {                                   \
        UpdateErrorText(Text);          \
                                        \
        throw T_KFC_Exception();        \
                                        \
    }while(false)                       \

// --------------------------------
// Defined code failure initiation
// --------------------------------
#define INITIATE_DEFINED_CODE_FAILURE(Text, Code)                                                           \
    do                                                                                                      \
    {                                                                                                       \
        size_t _code = (size_t)(Code);                                                                      \
                                                                                                            \
        INITIATE_DEFINED_FAILURE(   KString::Formatted( TEXT("%s (error code: 0x%.8X - %s)."),              \
                                                            (LPCTSTR)(KString)(Text),                       \
                                                            _code,                                          \
                                                            (LPCTSTR)GetErrorDescription((size_t)(Code)))); \
    }while(false)                                                                                           \

// -------------------
// Failure initiation
// -------------------
#define INITIATE_FAILURE                                                                                                \
    INITIATE_DEFINED_FAILURE((KString)TEXT("Failure condition at: ") + TEXT(__FILE__) + TEXT(" , ") + (int)__LINE__);   \

// -------------------------
// Debug failure initiation
// -------------------------
#ifdef _DEBUG
    #define DEBUG_INITIATE_FAILURE  INITIATE_FAILURE
#else // _DEBUG
    #define DEBUG_INITIATE_FAILURE  KFC_NOP
#endif // _DEBUG

// -----------------
// KFC verification
// -----------------
#define KFC_VERIFY_WITH_SOURCE(v)                                                           \
    do                                                                                      \
    {                                                                                       \
        if(!(v))                                                                            \
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Verification \"")    +               \
                                            TEXT(#v) + TEXT("\" failed at: ")   +           \
                                            TEXT(__FILE__) + TEXT(" , ") + (int)__LINE__);  \
                                                                                            \
    }while(false)                                                                           \

// -----------------
// KFC verification
// -----------------
#define KFC_VERIFY(v)                                                               \
    do                                                                              \
    {                                                                               \
        if(!(v))                                                                    \
        {                                                                           \
            INITIATE_DEFINED_FAILURE(   (KString)TEXT("Verification failed: \"") +  \
                                            TEXT(#v) + TEXT("\"."));                \
        }                                                                           \
                                                                                    \
    }while(false)                                                                   \

// -------------------
// Debug verification
// -------------------
#ifdef _DEBUG

    #define DEBUG_VERIFY(v)     KFC_VERIFY_WITH_SOURCE(v)

#else // _DEBUG

    #define DEBUG_VERIFY(v)     KFC_NOP

#endif // _DEBUG

// -----------------------
// Evaluated verification
// -----------------------
#ifdef _DEBUG

    #define DEBUG_EVALUATE_VERIFY(v)    DEBUG_VERIFY(v)

#else //_DEBUG

    #define DEBUG_EVALUATE_VERIFY(v)    (v)

#endif // _DEBUG

#define DEBUG_EVERIFY(v)    DEBUG_EVALUATE_VERIFY(v)

// ----------------------------
// Initialization verification
// ----------------------------
#ifdef _DEBUG

    #define DEBUG_VERIFY_INITIALIZATION                                                                                                     \
        do                                                                                                                                  \
        {                                                                                                                                   \
            if(!IsInitialized())                                                                                                            \
                INITIATE_DEFINED_FAILURE((KString)TEXT("Globals are not initialized: ") + TEXT(__FILE__) + TEXT(" , ") + (int)__LINE__);    \
                                                                                                                                            \
        }while(false)                                                                                                                       \

#else // _DEBUG

    #define DEBUG_VERIFY_INITIALIZATION

#endif // _DEBUG

// ------------------------
// Allocation verification
// ------------------------
#ifdef _DEBUG

    #define DEBUG_VERIFY_ALLOCATION                                                             \
        do                                                                                      \
        {                                                                                       \
            if(!IsAllocated())                                                                  \
            {                                                                                   \
                INITIATE_DEFINED_FAILURE(   (KString)TEXT("Object was not allocated: ") +       \
                                                TEXT(__FILE__) + TEXT(" , ") + (int)__LINE__);  \
            }                                                                                   \
                                                                                                \
        }while(false)                                                                           \

#else // _DEBUG

    #define DEBUG_VERIFY_ALLOCATION

#endif // _DEBUG

// ------------------------------
// Local allocation verification
// ------------------------------
#ifdef _DEBUG

    #define DEBUG_VERIFY_LOCAL_ALLOCATION(ClassType)                                                                                \
        do                                                                                                                          \
        {                                                                                                                           \
            if(!ClassType::IsAllocated())                                                                                           \
                INITIATE_DEFINED_FAILURE((KString)TEXT("Sub-object is empty: ") + TEXT(__FILE__) + TEXT(" , ") + (int)__LINE__);    \
                                                                                                                                    \
        }while(false)                                                                                                               \

#else // _DEBUG

    #define DEBUG_VERIFY_LOCAL_ALLOCATION(ClassType)

#endif // _DEBUG

// -----------------
// Ptr verification
// -----------------
#ifdef _DEBUG

    template <class t>
    inline t* DebugVerifyPtr(t* p)
        { DEBUG_VERIFY(p); return p; }

    template <class t>
    inline const t* DebugVerifyPtr(const t* p)
        { DEBUG_VERIFY(p); return p; }

    #define DEBUG_VERIFY_PTR(p) (DebugVerifyPtr(p))

#else // _DEBUG

    #define DEBUG_VERIFY_PTR(p) (p)

#endif // _DEBUG

// ---------------------
// Globals failure text
// ---------------------
#define GLOBALS_FAILURE_TEXT(Text)                                                                  \
    ((KString)TEXT("\"") + (m_Name) + TEXT("\" (RefCount = ") + m_szRefCount + TEXT(") ") + (Text)) \

// ------------------------------
// Report incorrect value format
// ------------------------------
#define REPORT_INCORRECT_VALUE_FORMAT(String, ValueName)                                                                \
    INITIATE_DEFINED_FAILURE((KString)TEXT("Incorrect ") + (ValueName) + TEXT(" format (\"") + (String) + TEXT("\").")) \

// ---------------------
// Report invalid value
// ---------------------
#define REPORT_INVALID_VALUE(String, ValueName)                                                                 \
    INITIATE_DEFINED_FAILURE((KString)TEXT("Invalid ") + (ValueName) + TEXT(" (\"") + (String) + TEXT("\")."))  \

// ------------
// Outer block
// ------------
#define KFC_OUTER_BLOCK_BEGIN               \
    {                                       \
        TOuterBlockGuard _KFC_Guard0;       \
                                            \
        TEST_BLOCK_BEGIN                    \
        {                                   \

#define KFC_FAST_OUTER_BLOCK_BEGIN          \
    {                                       \
        TFastOuterBlockGuard _KFC_Guard0;   \
                                            \
        TEST_BLOCK_BEGIN                    \
        {                                   \

#define KFC_OUTER_BLOCK_END                                         \
        }                                                           \
        TEST_BLOCK_KFC_EXCEPTION_HANDLER                            \
        {                                                           \
            ShowErrorText();                                        \
        }                                                           \
        TEST_BLOCK_CUSTOM_HANDLER(T_KFC_MemoryException)            \
        {                                                           \
            ShowErrorText(TEXT("Memory exception detected."));      \
        }                                                           \
        TEST_BLOCK_TERMINATION_EXCEPTION_HANDLER(NONE)              \
        {                                                           \
        }                                                           \
        TEST_BLOCK_ANY_EXCEPTION_HANDLER                            \
        {                                                           \
            ShowErrorText(TEXT("Unhandled exception detected."));   \
        }                                                           \
        TEST_BLOCK_END                                              \
    }                                                               \

// ----------------------
// Memory leaks tracking
// ----------------------
#ifdef _DEBUG
    #ifdef _MSC_VER
        #define TRACK_MEMORY_LEAKS  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
    #else // _MSC_VER
        #define TRACK_MEMORY_LEAKS  KFC_NOP
    #endif // _MSC_VER
#else // _DEBUG
    #define TRACK_MEMORY_LEAKS KFC_NOP
#endif // _DEBUG

#endif // debug_h
