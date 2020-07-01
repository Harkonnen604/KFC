# Microsoft Developer Studio Project File - Name="KFC_Sound" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KFC_Sound - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KFC_Sound.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KFC_Sound.mak" CFG="KFC_Sound - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KFC_Sound - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "KFC_Sound - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KFC_Sound - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\_out_rel"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"kfc_sound_pch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KFC_Sound - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\_out_deb"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"kfc_sound_pch.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "KFC_Sound - Win32 Release"
# Name "KFC_Sound - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\kfc_sound_pch.cpp
# ADD CPP /Yc"kfc_sound_pch.h"
# End Source File
# Begin Source File

SOURCE=.\plain_sounds.cpp
# End Source File
# Begin Source File

SOURCE=.\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_consts.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_device_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_initials.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_storage.cpp
# End Source File
# Begin Source File

SOURCE=.\sound_tokens.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dmusic_inc.h
# End Source File
# Begin Source File

SOURCE=.\dsound_inc.h
# End Source File
# Begin Source File

SOURCE=.\kfc_sound_pch.h
# End Source File
# Begin Source File

SOURCE=.\plain_sounds.h
# End Source File
# Begin Source File

SOURCE=.\sound.h
# End Source File
# Begin Source File

SOURCE=.\sound_buffer.h
# End Source File
# Begin Source File

SOURCE=.\sound_cfg.h
# End Source File
# Begin Source File

SOURCE=.\sound_consts.h
# End Source File
# Begin Source File

SOURCE=.\sound_defs.h
# End Source File
# Begin Source File

SOURCE=.\sound_device_globals.h
# End Source File
# Begin Source File

SOURCE=.\sound_globals.h
# End Source File
# Begin Source File

SOURCE=.\sound_initials.h
# End Source File
# Begin Source File

SOURCE=.\sound_storage.h
# End Source File
# Begin Source File

SOURCE=.\sound_tokens.h
# End Source File
# End Group
# End Target
# End Project
