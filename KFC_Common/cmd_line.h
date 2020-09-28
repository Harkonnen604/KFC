#ifndef cmd_line_h
#define cmd_line_h

// ----------------
// Global routines
// ----------------
KStrings::TIterator ParseCmdLine(   LPCTSTR     pCmdLine,
                                    KStrings&   RTokens,
                                    bool        bClearFirst = true);

#endif //cmd_line_h
