#ifndef kfc_common_pch_h
#define kfc_common_pch_h

#include <KFC_KTL/kfc_ktl_pch.h>

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef _MSC_VER
	#include <direct.h>
	#include <io.h>
#endif // _MSC_VER

#include <time.h>
#include <KFC_KTL/win_inc.h>

#ifdef _MSC_VER
	#include <tchar.h>
	#include <shlwapi.h>
	#include <shlobj.h>
#endif // _MSC_VER

#ifdef _MSC_VER
	#include <OleCtl.h>
	#include <ObjBase.h>
#endif // _MSC_VER

#endif // kfc_common_pch_h
