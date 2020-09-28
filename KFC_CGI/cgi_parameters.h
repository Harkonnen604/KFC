#ifndef cgi_parameters_h
#define cgi_parameters_h

#include <KFC_KTL/variable_list.h>

// --------------
// CGI parameter
// --------------
typedef TVariable T_CGI_Parameter;

// -----------------
// CGI read options
// -----------------
#define CGI_RO_GET      (0x0001)
#define CGI_RO_POST     (0x0002)

#define CGI_RO_NONE     (0)
#define CGI_RO_ALL      (UINT_MAX)

// ---------------
// CGI parameters
// ---------------
class T_CGI_Parameters : public TVariableList
{
public:
    T_CGI_Parameters(bool bCaseSensitive) : TVariableList(bCaseSensitive) {}

    T_CGI_Parameters(bool bCaseSensitive, kflags_t flReadOptions) : TVariableList(bCaseSensitive)
        { Read(flReadOptions); }

    T_CGI_Parameters(bool bCaseSensitive, LPCTSTR s) : TVariableList(bCaseSensitive)
        { Parse(s); }

    T_CGI_Parameters::TIterator Parse(LPCTSTR s, bool bClearFirst = true);

    T_CGI_Parameters::TIterator Read(kflags_t flReadOptions, bool bClearFirst = true);

    KString Write() const;

    bool HasIndexed(LPCTSTR pName, size_t& szRIndex) const;

    bool HasIndexed(LPCTSTR pName, size_t* pRIndices, size_t szN) const;

    T_CGI_Parameters& operator () (const KString& Name, const KString& Value)
        { Add(Name, Value); return *this; }
};

#endif // cgi_parameters_h
