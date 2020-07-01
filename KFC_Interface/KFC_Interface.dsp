# Microsoft Developer Studio Project File - Name="KFC_Interface" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KFC_Interface - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KFC_Interface.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KFC_Interface.mak" CFG="KFC_Interface - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KFC_Interface - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "KFC_Interface - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KFC_Interface - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"kfc_interface_pch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KFC_Interface - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"kfc_interface_pch.h" /FD /GZ /c
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

# Name "KFC_Interface - Win32 Release"
# Name "KFC_Interface - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\button_control.cpp
# End Source File
# Begin Source File

SOURCE=.\check_container.cpp
# End Source File
# Begin Source File

SOURCE=.\checkbox_control.cpp
# End Source File
# Begin Source File

SOURCE=.\control.cpp
# End Source File
# Begin Source File

SOURCE=.\control_interpolated_state.cpp
# End Source File
# Begin Source File

SOURCE=.\controls_factory.cpp
# End Source File
# Begin Source File

SOURCE=.\ctrl_deref.cpp
# End Source File
# Begin Source File

SOURCE=.\dummy_control.cpp
# End Source File
# Begin Source File

SOURCE=.\hot_pointer.cpp
# End Source File
# Begin Source File

SOURCE=.\image_control.cpp
# End Source File
# Begin Source File

SOURCE=.\interactive_control.cpp
# End Source File
# Begin Source File

SOURCE=.\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_consts.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_defs.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_device_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_initials.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_input_message_defs.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_input_message_map.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_interpolators.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_message_defs.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_message_map.cpp
# End Source File
# Begin Source File

SOURCE=.\interface_tokens.cpp
# End Source File
# Begin Source File

SOURCE=.\kfc_interface_pch.cpp
# ADD CPP /Yc"kfc_interface_pch.h"
# End Source File
# Begin Source File

SOURCE=.\scroll_control.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\button_control.h
# End Source File
# Begin Source File

SOURCE=.\check_container.h
# End Source File
# Begin Source File

SOURCE=.\checkbox_control.h
# End Source File
# Begin Source File

SOURCE=.\control.h
# End Source File
# Begin Source File

SOURCE=.\control_defs.h
# End Source File
# Begin Source File

SOURCE=.\control_interpolated_state.h
# End Source File
# Begin Source File

SOURCE=.\controls_factory.h
# End Source File
# Begin Source File

SOURCE=.\ctrl_deref.h
# End Source File
# Begin Source File

SOURCE=.\dummy_control.h
# End Source File
# Begin Source File

SOURCE=.\hot_pointer.h
# End Source File
# Begin Source File

SOURCE=.\image_control.h
# End Source File
# Begin Source File

SOURCE=.\interactive_control.h
# End Source File
# Begin Source File

SOURCE=.\interface.h
# End Source File
# Begin Source File

SOURCE=.\interface_cfg.h
# End Source File
# Begin Source File

SOURCE=.\interface_consts.h
# End Source File
# Begin Source File

SOURCE=.\interface_defs.h
# End Source File
# Begin Source File

SOURCE=.\interface_device_globals.h
# End Source File
# Begin Source File

SOURCE=.\interface_font_defs.h
# End Source File
# Begin Source File

SOURCE=.\interface_globals.h
# End Source File
# Begin Source File

SOURCE=.\interface_initials.h
# End Source File
# Begin Source File

SOURCE=.\interface_input_message_defs.h
# End Source File
# Begin Source File

SOURCE=.\interface_input_message_map.h
# End Source File
# Begin Source File

SOURCE=.\interface_interpolator_macros.h
# End Source File
# Begin Source File

SOURCE=.\interface_interpolators.h
# End Source File
# Begin Source File

SOURCE=.\interface_message_defs.h
# End Source File
# Begin Source File

SOURCE=.\interface_message_map.h
# End Source File
# Begin Source File

SOURCE=.\interface_sound_defs.h
# End Source File
# Begin Source File

SOURCE=.\interface_sprite_defs.h
# End Source File
# Begin Source File

SOURCE=.\interface_tokens.h
# End Source File
# Begin Source File

SOURCE=.\kfc_interface_pch.h
# End Source File
# Begin Source File

SOURCE=.\scroll_control.h
# End Source File
# End Group
# End Target
# End Project
