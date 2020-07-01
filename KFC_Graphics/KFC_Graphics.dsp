# Microsoft Developer Studio Project File - Name="KFC_Graphics" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=KFC_Graphics - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "KFC_Graphics.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "KFC_Graphics.mak" CFG="KFC_Graphics - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "KFC_Graphics - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "KFC_Graphics - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "KFC_Graphics - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"kfc_graphics_pch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "KFC_Graphics - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"kfc_graphics_pch.h" /FD /GZ /c
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

# Name "KFC_Graphics - Win32 Release"
# Name "KFC_Graphics - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\2d_effects.cpp
# End Source File
# Begin Source File

SOURCE=.\2d_fonts.cpp
# End Source File
# Begin Source File

SOURCE=.\2d_sprites.cpp
# End Source File
# Begin Source File

SOURCE=.\border_sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\composite_sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\compound_sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_color.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_light.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_material.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_state_block.cpp
# End Source File
# Begin Source File

SOURCE=.\d3d_vector.cpp
# End Source File
# Begin Source File

SOURCE=.\dummy_sprites.cpp
# End Source File
# Begin Source File

SOURCE=.\effect_sprites.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\font_storage.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_cfg.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_consts.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_device_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_effect_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_initials.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_state_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\graphics_tokens.cpp
# End Source File
# Begin Source File

SOURCE=.\index_buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\kfc_graphics_pch.cpp
# ADD CPP /Yc"kfc_graphics_pch.h"
# End Source File
# Begin Source File

SOURCE=.\pixel_formats.cpp
# End Source File
# Begin Source File

SOURCE=.\pixel_shader.cpp
# End Source File
# Begin Source File

SOURCE=.\sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\sprite_helpers.cpp
# End Source File
# Begin Source File

SOURCE=.\sprite_storage.cpp
# End Source File
# Begin Source File

SOURCE=.\stated_texture.cpp
# End Source File
# Begin Source File

SOURCE=.\surface.cpp
# End Source File
# Begin Source File

SOURCE=.\text_params.cpp
# End Source File
# Begin Source File

SOURCE=.\text_sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# Begin Source File

SOURCE=.\texture_image.cpp
# End Source File
# Begin Source File

SOURCE=.\texture_mapper.cpp
# End Source File
# Begin Source File

SOURCE=.\transition_sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\vertex_buffer.cpp
# End Source File
# Begin Source File

SOURCE=.\vertex_formats.cpp
# End Source File
# Begin Source File

SOURCE=.\vertex_shader.cpp
# End Source File
# Begin Source File

SOURCE=.\visual_model.cpp
# End Source File
# Begin Source File

SOURCE=.\visual_part.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\2d_effects.h
# End Source File
# Begin Source File

SOURCE=.\2d_fonts.h
# End Source File
# Begin Source File

SOURCE=.\2d_sprites.h
# End Source File
# Begin Source File

SOURCE=.\border_sprite.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\color_defs.h
# End Source File
# Begin Source File

SOURCE=.\composite_sprite.h
# End Source File
# Begin Source File

SOURCE=.\compound_sprite.h
# End Source File
# Begin Source File

SOURCE=.\d3d_color.h
# End Source File
# Begin Source File

SOURCE=.\d3d_inc.h
# End Source File
# Begin Source File

SOURCE=.\d3d_light.h
# End Source File
# Begin Source File

SOURCE=.\d3d_material.h
# End Source File
# Begin Source File

SOURCE=.\d3d_matrix.h
# End Source File
# Begin Source File

SOURCE=.\d3d_state_block.h
# End Source File
# Begin Source File

SOURCE=.\d3d_vector.h
# End Source File
# Begin Source File

SOURCE=.\dummy_sprites.h
# End Source File
# Begin Source File

SOURCE=.\effect_sprites.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\font_defs.h
# End Source File
# Begin Source File

SOURCE=.\font_storage.h
# End Source File
# Begin Source File

SOURCE=.\graphics_cfg.h
# End Source File
# Begin Source File

SOURCE=.\graphics_consts.h
# End Source File
# Begin Source File

SOURCE=.\graphics_device_globals.h
# End Source File
# Begin Source File

SOURCE=.\graphics_effect_globals.h
# End Source File
# Begin Source File

SOURCE=.\graphics_globals.h
# End Source File
# Begin Source File

SOURCE=.\graphics_initials.h
# End Source File
# Begin Source File

SOURCE=.\graphics_limits.h
# End Source File
# Begin Source File

SOURCE=.\graphics_state_manager.h
# End Source File
# Begin Source File

SOURCE=.\graphics_tokens.h
# End Source File
# Begin Source File

SOURCE=.\index_buffer.h
# End Source File
# Begin Source File

SOURCE=.\kfc_graphics_pch.h
# End Source File
# Begin Source File

SOURCE=.\pixel_formats.h
# End Source File
# Begin Source File

SOURCE=.\pixel_shader.h
# End Source File
# Begin Source File

SOURCE=.\sprite.h
# End Source File
# Begin Source File

SOURCE=.\sprite_defs.h
# End Source File
# Begin Source File

SOURCE=.\sprite_helpers.h
# End Source File
# Begin Source File

SOURCE=.\sprite_storage.h
# End Source File
# Begin Source File

SOURCE=.\stated_texture.h
# End Source File
# Begin Source File

SOURCE=.\surface.h
# End Source File
# Begin Source File

SOURCE=.\text_params.h
# End Source File
# Begin Source File

SOURCE=.\text_sprite.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# Begin Source File

SOURCE=.\texture_image.h
# End Source File
# Begin Source File

SOURCE=.\texture_mapper.h
# End Source File
# Begin Source File

SOURCE=.\transition_sprite.h
# End Source File
# Begin Source File

SOURCE=.\vertex_buffer.h
# End Source File
# Begin Source File

SOURCE=.\vertex_formats.h
# End Source File
# Begin Source File

SOURCE=.\vertex_shader.h
# End Source File
# Begin Source File

SOURCE=.\visual_model.h
# End Source File
# Begin Source File

SOURCE=.\visual_part.h
# End Source File
# End Group
# End Target
# End Project
