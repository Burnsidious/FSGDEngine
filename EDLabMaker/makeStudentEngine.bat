@echo off

REM This batch file should copy over all projects of the engine and all necessary files for the student
REM  engine. The FSGDGame_Student.sln should be able to open in visual studio without error. If errors
REM  occur when opening FSGDGame_Student.sln then the automation will fail.
REM Arguments:
REM   %1 - Path to student engine
REM   %2 - Path to engine

IF %1 == "" goto FailArguments
IF %2 == "" goto FailArguments

md %1

xcopy %2*.h %1 /S /C /Y
xcopy %2*.cpp %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.filters %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.hpp %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.c %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.rc %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.ico %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.vcproj %1 /S /C /Y 
xcopy %2*.vcxproj %1 /S /C /Y
xcopy %2*.csproj %1 /S /C /Y
xcopy %2*.inc %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.hlsl %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.hlsli %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.cso %1 /S /C /Y /EXCLUDE:excludes.txt
xcopy %2*.props %1 /S /C /Y

xcopy %2FSGDGame\Resources %1FSGDGame\Resources\ /S /C /Y 

xcopy %2debug\EDCollision.lib %1DebugLibsWin32\ /C /Y 
xcopy %2x64\debug\EDCollision.lib %1DebugLibsx64\ /C /Y 
xcopy %2release\EDCollision.lib %1ReleaseLibsWin32\ /C /Y 
xcopy %2x64\release\EDCollision.lib %1ReleaseLibsx64\ /C /Y 

xcopy %2debug\*solution*.lib %1DebugLibsWin32\ /C /Y 
xcopy %2x64\debug\*solution*.lib %1DebugLibsx64\ /C /Y 
xcopy %2release\*solution*.lib %1ReleaseLibsWin32\ /C /Y 
xcopy %2x64\release\*solution*.lib %1ReleaseLibsx64\ /C /Y 

xcopy %2debug\*solution*.dll %1DebugLibsWin32\ /C /Y 
xcopy %2x64\debug\*solution*.dll %1DebugLibsx64\ /C /Y 
xcopy %2release\*solution*.dll %1ReleaseLibsWin32\ /C /Y 
xcopy %2x64\release\*solution*.dll %1ReleaseLibsx64\ /C /Y 

copy %2FSGDGame.sln %1FSGDGame_Student.sln /Y

goto Complete

:FailArguments
echo Missing argument: "PathToSolutionOutput" "PathToWorkingSolution"

:Complete