@echo off

REM This batch file will cleanup the student engine once the automation has occured and saved the
REM  FSGDGame_Student.sln solution. The FSGDGame_Student.sln solution no longer has the unwanted 
REM  projects linked so feel free to delete them along with any unwanted source files.
REM Arguments:
REM   %1 - Path to student engine
REM   %2 - Path to engine

IF %1 == "" goto FailArguments
IF %2 == "" goto FailArguments

rmdir /S /Q %1"ED GUI Creator"
rmdir /S /Q %1"EDHeapSolution"
rmdir /S /Q %1"Editor"
rmdir /S /Q %1"EDLabMaker"
rmdir /S /Q %1"EDSolutions"
rmdir /S /Q %1"EDTree"
rmdir /S /Q %1"FSGDEngineToolsInterface"
rmdir /S /Q %1"GameObjectEditor"
rmdir /S /Q %1"GDExporter"

del /Q %1"*.sdf"
del /AH /Q %1"*.suo"

REM Delete all empty directories:
REM NOTE: rmdir cannot delete directories containing files so the only directories
REM       that will be deleted are the empty directories.
for /F %%f in ('dir /S /AD /B "%1*Debug"') do rmdir "%%f"

goto Complete

:FailArguments
echo Missing argument: "PathToSolutionOutput" "PathToWorkingSolution"

:Complete