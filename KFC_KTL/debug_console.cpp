#include "kfc_ktl_pch.h"
#include "debug_console.h"

#include "ktl_consts.h"

// ----------------
// Global routines
// ----------------
void CreateDebugConsole()
{
	#ifdef _MSC_VER
	{
		AllocConsole();
	
		HANDLE hScreen = CreateConsoleScreenBuffer(	GENERIC_READ | GENERIC_WRITE,
													FILE_SHARE_READ | FILE_SHARE_WRITE,
													NULL,
													CONSOLE_TEXTMODE_BUFFER,
													NULL);
	
		if(hScreen != INVALID_HANDLE_VALUE)
		{
			CONSOLE_SCREEN_BUFFER_INFO Info;
			if(GetConsoleScreenBufferInfo(hScreen, &Info))
			{
				COORD Size = {Info.dwSize.X, 4096};
				SetConsoleScreenBufferSize(hScreen, Size);
			}
	
			SetConsoleActiveScreenBuffer(hScreen);		
	
			freopen("CONIN$",	"rt", stdin);
			freopen("CONOUT$",	"wt", stdout);
			freopen("CONOUT$",	"wt", stderr);
		}
	}
	#endif // _MSC_VER
}

#ifdef _MSC_VER
	void DebugConsolePause(LPCTSTR pMessage, bool bFlushFirst)
#else // _MSC_VER
	void DebugConsolePause(LPCTSTR pMessage)	
#endif // _MSC_VER
{
	#ifdef _MSC_VER
	{
		if(bFlushFirst)
		{
			while(kbhit())
				getch();
		}
		
		if(pMessage)
			_putts(pMessage);
		
		getch();
	}
	#else // _MSC_VER
	{
		if(pMessage)
			_putts(pMessage);
		
		getc(stdin);
	}
	#endif // _MSC_VER
}

inline void putc_safe(char c)
{
	putc(c < ' ' ? '.' : c, stdout);
}

void DebugConsoleHexDump(const void* p, size_t n)
{
	const BYTE* pp = (const BYTE*)p;
	const BYTE* qq;

	size_t i, j;

	for(i = 0 ; i < n ; )
	{
		// Values
		{
			for(j = 0, qq = pp ; j < 8 && i + j < n ; j++, qq++)
				printf("%.2X ", (size_t)*qq);
			
			for( ; j < 8 ; j++)
				printf("   ");

			printf("| ");

			for( ; j < 16 && i + j < n ; j++, qq++)
				printf("%.2X ", (size_t)*qq);

			for( ; j < 16 ; j++)
				printf("   ");
		}

		printf("| ");

		// Chars
		{
			for(j = 0, qq = pp; j < 16 && i + j < n ; j++, qq++)
				putc_safe(*qq);
		}
	
		putch('\n');

		i += j, pp = qq;
	}
}
