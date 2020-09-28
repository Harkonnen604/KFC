#include "kfc_common_pch.h"
#include "cmd_line.h"

// ----------------
// Global routines
// ----------------
KStrings::TIterator ParseCmdLine(   LPCTSTR     pCmdLine,
                                    KStrings&   RTokens,
                                    bool        bClearFirst)
{
    KStrings::TIterator Ret = RTokens.SplitQuotedString(pCmdLine, bClearFirst);

    // Deleting filename
    if(bClearFirst && !RTokens.IsEmpty())
    {
        DEBUG_VERIFY(Ret.IsValid());
        ++Ret;

        RTokens.DelFirst();
    }

    return Ret;
}
