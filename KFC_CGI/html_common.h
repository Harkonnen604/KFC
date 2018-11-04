#ifndef html_common_h
#define html_common_h

#include "cgi_parameters.h"

// ----------------
// Global routines
// ----------------
void WriteHTML_Hidden(const KString& Name, const KString& Value);

void WriteHTML_HiddenTextArea(const KString& Name, const KString& Value);

inline void ForwardHTML_Hidden(const T_CGI_Parameters& Parameters, const KString& Name)
	{ WriteHTML_Hidden(Name, Parameters[Name]); }

inline void ForwardHTML_HiddenTextArea(const T_CGI_Parameters& Parameters, const KString& Name)
	{ WriteHTML_HiddenTextArea(Name, Parameters[Name]); }


#endif // html_common_h
