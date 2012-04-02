# Microsoft Developer Studio Project File - Name="gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vfdwin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vfdwin.mak" CFG="gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gui - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "gui - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\..\..\build\"
# PROP BASE Intermediate_Dir "..\..\..\..\build\Release_vfdwin"
# PROP BASE Target_Dir "..\..\..\..\build\"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\..\build\"
# PROP Intermediate_Dir "..\..\..\..\build\Release_vfdwin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "..\..\..\..\build\"
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../inc" /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D _WIN32_WINNT=0x500 /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /d "NDEBUG"
# ADD RSC /l 0x400 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 advapi32.lib comctl32.lib comdlg32.lib ole32.lib shell32.lib user32.lib  vfd.lib /nologo /version:2.5 /subsystem:windows /pdb:none /machine:I386 /libpath:"..\..\..\..\build\\" /release

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\..\..\build\"
# PROP BASE Intermediate_Dir "..\..\..\..\build\Release_vfdwin"
# PROP BASE Target_Dir "..\..\..\..\build\"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\..\build\"
# PROP Intermediate_Dir "..\..\..\..\build\Debug_vfdwin"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "..\..\..\..\build\"
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /Zi /Od /I "../inc" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D _WIN32_WINNT=0x500 /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x400 /d "_DEBUG"
# ADD RSC /l 0x400 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 advapi32.lib comctl32.lib comdlg32.lib ole32.lib shell32.lib user32.lib  vfd.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /libpath:"..\..\..\..\build\\"

!ENDIF 

# Begin Target

# Name "gui - Win32 Release"
# Name "gui - Win32 Debug"
# Begin Group "Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\registry.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\vfdwin.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter "c;cpp"
# Begin Source File

SOURCE=.\aboutdlg.c
# End Source File
# Begin Source File

SOURCE=.\assocdlg.c
# End Source File
# Begin Source File

SOURCE=.\driverdlg.c
# End Source File
# Begin Source File

SOURCE=.\imagedlg.c
# End Source File
# Begin Source File

SOURCE=.\maindlg.c
# End Source File
# Begin Source File

SOURCE=.\registry.c
# End Source File
# Begin Source File

SOURCE=.\shelldlg.c
# End Source File
# Begin Source File

SOURCE=.\vfdwin.c
# End Source File
# End Group
# Begin Group "GUI Resource"

# PROP Default_Filter "ico;bmp;rc"
# Begin Source File

SOURCE=..\lib\res\config.ico
# End Source File
# Begin Source File

SOURCE=..\lib\res\image.ico
# End Source File
# Begin Source File

SOURCE=.\res\imagelist.bmp
# End Source File
# Begin Source File

SOURCE=..\lib\res\vfd.ico
# End Source File
# Begin Source File

SOURCE=.\vfdwin.rc
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\vfdmsg.mc

!IF  "$(CFG)" == "gui - Win32 Release"

# Begin Custom Build - Compiling Message - $(InputPath)
InputPath=.\vfdmsg.mc
InputName=vfdmsg

BuildCmds= \
	mc $(InputName)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG0409.BIN" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG0411.BIN" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# Begin Custom Build - Compiling Message - $(InputPath)
InputPath=.\vfdmsg.mc
InputName=vfdmsg

BuildCmds= \
	mc $(InputName)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG0409.BIN" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"MSG0411.BIN" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\vfdwin.rs

!IF  "$(CFG)" == "gui - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__VFDWI="../inc/vfdver.h"	"../inc/vfdver.rc"	"vfdmsg.rc"	"vfdwin.rc"	
# Begin Custom Build - Compiling Resource - $(InputPath)
IntDir=.\..\..\..\..\build\Release_vfdwin
InputPath=.\vfdwin.rs
InputName=vfdwin

"$(IntDir)\$(InputName).res" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	rc /fo"$(IntDir)\$(InputName).res" /i "../inc" /d "NDEBUG" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "gui - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__VFDWI="../inc/vfdver.h"	"../inc/vfdver.rc"	"vfdmsg.rc"	"vfdwin.rc"	
# Begin Custom Build - Compiling Resource - $(InputPath)
IntDir=.\..\..\..\..\build\Debug_vfdwin
InputPath=.\vfdwin.rs
InputName=vfdwin

"$(IntDir)\$(InputName).res" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	rc /fo"$(IntDir)\$(InputName).res" /i "../inc" /d "_DEBUG" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
