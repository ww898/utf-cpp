@echo off

set _MsBuild=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe
set _CMake=C:\Program Files\CMake\bin\cmake.exe

call :_build vc142 x86 Release
call :_build vc142 x86 Debug
call :_build vc142 x64 Release
call :_build vc142 x64 Debug

call :_build vc141 x86 Release
call :_build vc141 x86 Debug
call :_build vc141 x64 Release
call :_build vc141 x64 Debug

call :_build vc140 x86 Release
call :_build vc140 x86 Debug
call :_build vc140 x64 Release
call :_build vc140 x64 Debug

call :_build vc120 x86 Release
call :_build vc120 x86 Debug
call :_build vc120 x64 Release
call :_build vc120 x64 Debug

echo Ok
goto :_end

:_build

set _Tool=%1
set _Arch=%2
set _Conf=%3

echo Toolset:            %_Tool%
echo Architecture:       %_Arch%
echo Configuration:      %_Conf%

set _Dir=obj.%_Tool%.%_Arch%

echo Subdirectory:       %_Dir%

if "%_Tool%"=="vc120" set _Gen=Visual Studio 12 2013
if "%_Tool%"=="vc140" set _Gen=Visual Studio 14 2015
if "%_Tool%"=="vc141" set _Gen=Visual Studio 15 2017
if "%_Tool%"=="vc142" set _Gen=Visual Studio 16 2019
if "%_Gen%"=="" echo Unknwon toolset&& exit /b 1

echo CMake generator:    %_Gen%

if "%_Arch%"=="x86" set _GenArch=Win32
if "%_Arch%"=="x64" set _GenArch=x64
if "%_GenArch%"=="" echo Unknwon architecture&& exit /b 1

echo CMake architecture: %_GenArch%

if not exist "%_Dir%" mkdir "%_Dir%"

pushd "%_Dir%"
"%_CMake%" -G "%_Gen%" -A %_GenArch% ..
popd
if errorlevel 1 exit /b 1

pushd "%_Dir%"
"%_MsBuild%" /v:m /p:Configuration=%_Conf% utf-cpp.sln
if errorlevel 1 exit /b 1
popd

:_end
