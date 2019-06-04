#include "kfc_common_pch.h"
#include "startup_globals.h"

#include <KFC_KTL/ktl_consts.h>
#include <KFC_KTL/file_names.h>
#include "common_device_globals.h"
#include "common_consts.h"
#include "module.h"
#include "file.h"
#include "cmd_line.h"

TStartupGlobals g_StartupGlobals;

// ----------------
// Startup globals
// ----------------
TStartupGlobals::TStartupGlobals() : TGlobals(TEXT("Startup globals"))
{
	AddSubGlobals(g_CommonDeviceGlobals);
}

void TStartupGlobals::OnUninitialize()
{
	// Parameters
	m_Parameters.Clear();

	// Folders
	#ifdef _MSC_VER
	{
		m_DesktopFolder.			Empty();
		m_StartMenuProgramsFolder.	Empty();
		m_ProgramFilesFolder.		Empty();
		m_SystemFolder.				Empty();
		m_WindowsFolder.			Empty();
	}
	#endif // _MSC_VER

	m_TempFolder.	Empty();
	m_StartFolder.	Empty();

	// Files
	m_StartFile.Empty();
}

void TStartupGlobals::OnInitialize()
{
	#ifndef _MSC_VER
		DEBUG_VERIFY(!g_CommonConsts.m_MainProcArgs.IsEmpty());
	#endif // _MSC_VER

	#ifdef _MSC_VER
	{
		TCHAR	Buf[1024];
		size_t	szLength;

		// Start file
		m_StartFile = GetKModuleFileName(NULL);

		// Start folder
		m_StartFolder = GetFilePath(m_StartFile);

		// Windows folder
		szLength = GetWindowsDirectory(Buf, sizeof(Buf) - 1);

		if(szLength == 0)
		{
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error fetching windows folder"), GetLastError());
		}

		Buf[szLength] = 0, m_WindowsFolder = SlashedFolderName(Buf);

		// System folder
		szLength = GetSystemDirectory(Buf, sizeof(Buf) - 1);

		if(szLength == 0)
		{
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error fetching system folder"), GetLastError());
		}

		Buf[szLength] = 0, m_SystemFolder = SlashedFolderName(Buf);

		// Temp folder
		szLength = GetTempPath(sizeof(Buf) - 1, Buf);
		if(szLength == 0)
		{
			INITIATE_DEFINED_CODE_FAILURE(TEXT("Error fetching temp folder"), GetLastError());
		}

		Buf[szLength] = 0, m_TempFolder = SlashedFolderName(Buf);

		// Program files folder
		{
			const LPCTSTR pEnv = _tgetenv(TEXT("ProgramFiles"));

			m_ProgramFilesFolder =	pEnv ?
										SlashedFolderName(pEnv) :
										m_WindowsFolder.Left(3) + TEXT("Program Files\\");
		}

		// Start menu programs folder
		{
			TCHAR Buf[MAX_PATH];
			SHGetSpecialFolderPath(NULL, Buf, CSIDL_STARTMENU, TRUE);

			m_StartMenuProgramsFolder = SlashedFolderName(Buf) + TEXT("Programs\\");
		}

		// Desktop folder
		{
			TCHAR Buf[MAX_PATH];
			SHGetSpecialFolderPath(NULL, Buf, CSIDL_DESKTOP, TRUE);

			m_DesktopFolder = SlashedFolderName(Buf);
		}

		// Parameters
		ParseCmdLine(GetCommandLine(), m_Parameters);
	}
	#else
	{
		KFC_VERIFY(!g_CommonConsts.m_MainProcArgs.IsEmpty());

		// Start folder
		m_StartFolder = FollowPath(GetWorkingDirectory(), GetFilePath(g_CommonConsts.m_MainProcArgs[0]));

		// Temp folder
		m_TempFolder = m_StartFolder;

		// Start file
		m_StartFile = m_StartFolder + GetFileName(g_CommonConsts.m_MainProcArgs[0]);

		// Parameters
		for(KStrings::TConstIterator Iter = g_CommonConsts.m_MainProcArgs.GetFirst().GetNext() ; Iter.IsValid() ; ++Iter)
			m_Parameters << *Iter;
	}
	#endif // _MSC_VER
}
