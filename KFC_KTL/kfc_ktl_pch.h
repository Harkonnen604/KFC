#ifndef kfc_ktl_pch_h
#define kfc_ktl_pch_h

#ifdef _MSC_VER
	#pragma warning( disable : 4091 4345 4800 4996 )
#endif // _MSC_VER

#ifdef _DEBUG
	#define _DEVELOP
#endif // _DEBUG

#define WTEXT(s)	L##s

#include "basic_macros.h"

#ifdef _MSC_VER

	#define COM_STDMETHOD_CAN_THROW

/*	#if WINVER < 0x500
		#define WINVER			(0x0500)
		#define _WIN32_WINDOWS	(0x0500)
		#define _WIN32_WINNT	(0x0500)
	#endif // WINVER

	#define _WIN32_DCOM*/

#endif // _MSC_VER

#include <typeinfo>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

using namespace std;

#ifdef _MSC_VER
	#include <conio.h>
	#include <tchar.h>
	#include <process.h>
#else // _MSC_VER
	#include <unistd.h>
	#include <dirent.h>
	#include <pthread.h>
	#include <wchar.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/mman.h>
	#include <sys/file.h>
	#include <netinet/tcp.h>
	#include <netinet/udp.h>
#endif // _MSC_VER

#include <limits.h>
// #include <stdint.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#ifdef _MSC_VER
#ifdef _DEBUG
	#include <crtdbg.h>
#endif // _DEBUG
#endif // _MSC_VER

#include "win_inc.h"
#include "postfixes.h"
#include "data_alignment.h"
#include "kfc_mem.h"
#include "kfc_env.h"
#include "stream.h"
#include "placement_allocation.h"
#include "basic_types.h"
#include "basic_bitypes.h"
#include "basic_wintypes.h"
#include "basic_templates.h"
#include "kstring.h"
#include "debug.h"
#include "debug_file.h"
#include "kstrings.h"
#include "hex.h"
#include "string_conv.h"
#include "flag_setter.h"
#include "rects.h"
#include "array.h"
#include "list.h"
#include "tree.h"
#include "avl_storage.h"
#include "vfifo.h"
#include "trie.h"
#include "stacked_storage.h"
#include "object_pointer.h"
#include "object_reference.h"
#include "ptr_holder.h"
#include "stream.h"
#include "for_each.h"
#include "auto_holder.h"
#include "debug_console.h"
#include "critical_section.h"
#include "file_names.h"
#include "date_time.h"

#endif // kfc_ktl_pch_h
