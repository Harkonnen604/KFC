#include "kfc_cgi_pch.h"
#include "html_common.h"

#include "cgi_tokenizer.h"

// ----------------
// Global routines
// ----------------
void WriteHTML_Hidden(const KString& Name, const KString& Value)
{
	printf(	"<input type=hidden name=\"%s\" value=\"%s\">\n",
				(LPCTSTR)Name,
				(LPCTSTR)EncodeForHTML(Value));
}

void WriteHTML_HiddenTextArea(const KString& Name, const KString& Value)
{
	printf(	"<textarea name=\"%s\" style=display:none;>%s</textarea>\n",
				(LPCTSTR)Name,
				(LPCTSTR)EncodeForHTML_TextArea(Value));
}
