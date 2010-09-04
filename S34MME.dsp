# Microsoft Developer Studio Project File - Name="S34MME" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=S34MME - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "S34MME.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "S34MME.mak" CFG="S34MME - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "S34MME - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "S34MME - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "Desktop"
# PROP WCE_FormatVersion ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "S34MME - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "S34MME_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G6 /MD /W3 /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DLL_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib jpeglib.lib /nologo /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy .\release\s34mme.dll ..\bin	copy .\release\s34mme.lib ..\bin	copy *.h ..\include	del ..\include\Jconfig.h	del ..\include\Jmorecfg.h	del ..\include\Jpeglib.h	del ..\include\stdafx.h	del ..\include\basic.h	del ..\include\resource.h
# End Special Build Tool

!ELSEIF  "$(CFG)" == "S34MME - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "S34MME_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /ML /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DLL_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib jpeglib.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "S34MME - Win32 Release"
# Name "S34MME - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CDAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\gif.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageManipulation.cpp
# End Source File
# Begin Source File

SOURCE=.\JPEG.cpp
# End Source File
# Begin Source File

SOURCE=.\MIDIMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\PixelFormatConvertor.cpp
# End Source File
# Begin Source File

SOURCE=.\S34Anim.cpp
# End Source File
# Begin Source File

SOURCE=.\S34AnimFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\S34AnimPathManager.cpp
# End Source File
# Begin Source File

SOURCE=.\S34AnimScript.cpp
# End Source File
# Begin Source File

SOURCE=.\S34Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\s34Curve.cpp
# End Source File
# Begin Source File

SOURCE=.\S34DblList.cpp
# End Source File
# Begin Source File

SOURCE=.\S34Dither.cpp
# End Source File
# Begin Source File

SOURCE=.\S34Image.cpp
# End Source File
# Begin Source File

SOURCE=.\S34ImageMask.cpp
# End Source File
# Begin Source File

SOURCE=.\S34MME.cpp
# End Source File
# Begin Source File

SOURCE=.\s34mme.rc
# End Source File
# Begin Source File

SOURCE=.\S34Palette.cpp
# End Source File
# Begin Source File

SOURCE=.\s34Point.cpp
# End Source File
# Begin Source File

SOURCE=.\s34Rect.cpp
# End Source File
# Begin Source File

SOURCE=.\S34Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\s34Size.cpp
# End Source File
# Begin Source File

SOURCE=.\S34String.cpp
# End Source File
# Begin Source File

SOURCE=.\S34Text.cpp
# End Source File
# Begin Source File

SOURCE=.\S34xml.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Basic.h
# End Source File
# Begin Source File

SOURCE=.\CDAudio.h
# End Source File
# Begin Source File

SOURCE=.\gif.h
# End Source File
# Begin Source File

SOURCE=.\ImageManipulation.h
# End Source File
# Begin Source File

SOURCE=.\JCONFIG.H
# End Source File
# Begin Source File

SOURCE=.\Jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\JPEG.h
# End Source File
# Begin Source File

SOURCE=.\Jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\MIDIMusic.h
# End Source File
# Begin Source File

SOURCE=.\PixelFormatConvertor.h
# End Source File
# Begin Source File

SOURCE=.\S34Anim.h
# End Source File
# Begin Source File

SOURCE=.\S34AnimFactory.h
# End Source File
# Begin Source File

SOURCE=.\S34AnimPathManager.h
# End Source File
# Begin Source File

SOURCE=.\S34AnimScript.h
# End Source File
# Begin Source File

SOURCE=.\S34ANMErrors.h
# End Source File
# Begin Source File

SOURCE=.\S34Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\S34Curve.h
# End Source File
# Begin Source File

SOURCE=.\S34DblList.h
# End Source File
# Begin Source File

SOURCE=.\S34Dither.h
# End Source File
# Begin Source File

SOURCE=.\S34GRAErrors.h
# End Source File
# Begin Source File

SOURCE=.\S34Image.h
# End Source File
# Begin Source File

SOURCE=.\S34ImageDef.h
# End Source File
# Begin Source File

SOURCE=.\S34ImageMask.h
# End Source File
# Begin Source File

SOURCE=.\S34Palette.h
# End Source File
# Begin Source File

SOURCE=.\s34Point.h
# End Source File
# Begin Source File

SOURCE=.\s34Rect.h
# End Source File
# Begin Source File

SOURCE=.\S34Scene.h
# End Source File
# Begin Source File

SOURCE=.\s34Size.h
# End Source File
# Begin Source File

SOURCE=.\S34String.h
# End Source File
# Begin Source File

SOURCE=.\S34Text.h
# End Source File
# Begin Source File

SOURCE=.\S34xml.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
