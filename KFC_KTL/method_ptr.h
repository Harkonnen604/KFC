#ifndef method_ptr_h
#define method_ptr_h

#include "basic_macros.h"
#include "basic_templates.h"

// ---------------
// Method pointer
// ---------------
template <class ClassType, class MethodType>
struct TMethodPtr
{
private:
    void Copy(const TMethodPtr<ClassType, MethodType>& SMethodPtr)
        { m_pObject = SMethodPtr.m_pObject, m_pMethod = SMethodPtr.m_pMethod; }

public:
    ClassType*  m_pObject;
    MethodType  m_pMethod;


    TMethodPtr() :  m_pObject(NULL),
                    m_pMethod(NULL) {}

    TMethodPtr(const TMethodPtr<ClassType, MethodType>& SMethodPtr)
        { Copy(SMethodPtr); }

    TMethodPtr( ClassType* pSObject,
                MethodType pSMethod) :  m_pObject(pSObject),
                                        m_pMethod(pSMethod) {}

    bool IsValid() const { return m_pObject && m_pMethod; }

    TMethodPtr& operator = (const TMethodPtr& SMethodPtr)
        { Copy(SMethodPtr); return *this; }

    bool operator == (const TMethodPtr& SMethodPtr) const
        { return m_pObject == SMethodPtr.m_pObject && m_pMethod == SMethodPtr.m_pMethod; }

    bool operator != (const TMethodPtr& SMethodPtr) const
        { return m_pObject != SMethodPtr.m_pObject || m_pMethod != SMethodPtr.m_pMethod; }
};

// ---------
// Callback
// ---------
#define DECLARE_CALLBACK_ARG_N(ArgNum, ArgClassDef, ArgClassList, ArgDef, ArgList)  \
                                                                                    \
    /* Callback base */                         \
    template <class RetType ArgClassDef>        \
    class TCallbackBaseArg##ArgNum              \
    {                                           \
    public:                                     \
        virtual bool IsValid() const = 0;       \
                                                \
        virtual RetType Call(ArgDef) const = 0; \
    };                                          \
                                                \
    /* NULL callback implementation */                          \
    template <class RetType ArgClassDef>                        \
    class T_NULL_CallbackImplArg##ArgNum :                      \
        public TCallbackBaseArg##ArgNum<RetType ArgClassList>   \
    {                                                           \
    public:                                                     \
        bool IsValid() const                                    \
            { return false; }                                   \
                                                                \
        RetType Call(ArgDef) const                              \
            { INITIATE_FAILURE; }                               \
    };                                                          \
                                                                \
    /* Function callback implementation */                                  \
    template <class RetType ArgClassDef>                                    \
    class TFunctionCallbackImplArg##ArgNum :                                \
        public TCallbackBaseArg##ArgNum<RetType ArgClassList>               \
    {                                                                       \
    private:                                                                \
        RetType (*m_pFunction)(ArgDef);                                     \
                                                                            \
    public:                                                                 \
        TFunctionCallbackImplArg##ArgNum(RetType (*pFunction)(ArgDef)) :    \
            m_pFunction(pFunction) {}                                       \
                                                                            \
        bool IsValid() const                                                \
            { return true; }                                                \
                                                                            \
        RetType Call(ArgDef) const                                          \
            { return m_pFunction(ArgList); }                                \
    };                                                                      \
                                                                            \
    /* Method callback implementation */                                                            \
    template <class RetType ArgClassDef, class ObjectType>                                          \
    class TMethodCallbackImplArg##ArgNum :                                                          \
        public TCallbackBaseArg##ArgNum<RetType ArgClassList>                                       \
    {                                                                                               \
    private:                                                                                        \
        ObjectType* m_pObject;                                                                      \
        RetType (ObjectType::*m_pMethod)(ArgDef);                                                   \
                                                                                                    \
    public:                                                                                         \
        TMethodCallbackImplArg##ArgNum( ObjectType* pObject,                                        \
                                        RetType (ObjectType::*pMethod)(ArgDef)) :                   \
                                                                                                    \
            m_pObject(pObject),                                                                     \
            m_pMethod(pMethod) {}                                                                   \
                                                                                                    \
        bool IsValid() const                                                                        \
            { return true; }                                                                        \
                                                                                                    \
        RetType Call(ArgDef) const                                                                  \
            { return (m_pObject->*m_pMethod)(ArgList); }                                            \
    };                                                                                              \
                                                                                                    \
    /* Callback */                                                                                  \
    template <class RetType ArgClassDef>                                                            \
    class TCallbackArg##ArgNum                                                                      \
    {                                                                                               \
    public:                                                                                         \
        union                                                                                       \
        {                                                                                           \
            void* _Aligner;                                                                         \
                                                                                                    \
            char m_Data                                                                             \
            [   union_size_t                                                                        \
                <   T_NULL_CallbackImplArg##ArgNum  <RetType ArgClassList>,                         \
                    TFunctionCallbackImplArg##ArgNum<RetType ArgClassList>,                         \
                    TMethodCallbackImplArg##ArgNum  <RetType ArgClassList, null_virt_t> >::res];    \
        };                                                                                          \
                                                                                                    \
    public:                                                                                         \
        TCallbackArg##ArgNum()                                                                      \
            { SetNULL(); }                                                                          \
                                                                                                    \
        operator bool () const                                                                      \
            { return ((TCallbackBaseArg##ArgNum<RetType ArgClassList>*)m_Data)->IsValid(); }        \
                                                                                                    \
        RetType operator () (ArgDef) const                                                          \
        {                                                                                           \
            assert(*this);                                                                          \
                                                                                                    \
            return ((TCallbackBaseArg##ArgNum<RetType ArgClassList>*)m_Data)->Call(ArgList);        \
        }                                                                                           \
                                                                                                    \
        void SetNULL()                                                                              \
            { new(m_Data) T_NULL_CallbackImplArg##ArgNum<RetType ArgClassList>; }                   \
                                                                                                    \
        void Set(RetType (*pFunction)(ArgDef))                                                      \
        {                                                                                           \
            assert(pFunction);                                                                      \
                                                                                                    \
            new(m_Data) TFunctionCallbackImplArg##ArgNum<RetType ArgClassList>(pFunction);          \
        }                                                                                           \
                                                                                                    \
        template <class ObjectType>                                                                 \
        void Set(ObjectType* pObject, RetType (ObjectType::*pMethod)(ArgDef))                       \
        {                                                                                           \
            assert(pObject);                                                                        \
            assert(pMethod);                                                                        \
                                                                                                    \
            new(m_Data)                                                                             \
                TMethodCallbackImplArg##ArgNum<RetType ArgClassList, ObjectType>                    \
                    (pObject, pMethod);                                                             \
        }                                                                                           \
    };                                                                                              \

DECLARE_CALLBACK_ARG_N( 0,
                        COMMA_LIST1(NONE),
                        COMMA_LIST1(NONE),
                        COMMA_LIST0(),
                        COMMA_LIST0())

DECLARE_CALLBACK_ARG_N( 1,
                        COMMA_LIST2(NONE,
                                    class ArgType0),
                        COMMA_LIST2(NONE,
                                    ArgType0),
                        COMMA_LIST1(ArgType0 Arg0),
                        COMMA_LIST1(Arg0))

DECLARE_CALLBACK_ARG_N( 2,
                        COMMA_LIST3(NONE,
                                    class ArgType0,
                                    class ArgType1),
                        COMMA_LIST3(NONE,
                                    ArgType0,
                                    ArgType1),
                        COMMA_LIST2(ArgType0 Arg0,
                                    ArgType1 Arg1),
                        COMMA_LIST2(Arg0,
                                    Arg1))

DECLARE_CALLBACK_ARG_N( 3,
                        COMMA_LIST4(NONE,
                                    class ArgType0,
                                    class ArgType1,
                                    class ArgType2),
                        COMMA_LIST4(NONE,
                                    ArgType0,
                                    ArgType1,
                                    ArgType2),
                        COMMA_LIST3(ArgType0 Arg0,
                                    ArgType1 Arg1,
                                    ArgType2 Arg2),
                        COMMA_LIST3(Arg0,
                                    Arg1,
                                    Arg2))

DECLARE_CALLBACK_ARG_N( 4,
                        COMMA_LIST5(NONE,
                                    class ArgType0,
                                    class ArgType1,
                                    class ArgType2,
                                    class ArgType3),
                        COMMA_LIST5(NONE,
                                    ArgType0,
                                    ArgType1,
                                    ArgType2,
                                    ArgType3),
                        COMMA_LIST4(ArgType0 Arg0,
                                    ArgType1 Arg1,
                                    ArgType2 Arg2,
                                    ArgType3 Arg3),
                        COMMA_LIST4(Arg0,
                                    Arg1,
                                    Arg2,
                                    Arg3))

#endif // method_ptr_h
