#include "pch.h"
#include "cgi_test.h"

#include <KFC_CGI/cgi_tokenizer.h>

// ----------------
// Global routines
// ----------------
void TestCGI()
{
    puts("Testing CGI...");

    TTokens Tokens("[e]", 3);

    printf("'%s'\n", (LPCTSTR)ProcessCGI_Tokens("res/test.txt", &Tokens));

    puts("Done");
}
