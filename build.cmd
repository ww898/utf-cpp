@echo off

set _MsBuild=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\MSBuild\15.0\Bin\MSBuild.exe

set _Arch=
set _Tool=
set _Gen0=
set _Gen1=
set _Conf=Release

:_loop
if "%1"=="" goto :_run

if %1==vc120 set _Tool=%1&& set _Gen0=Visual Studio 12 2013&& goto :_next
if %1==vc140 set _Tool=%1&& set _Gen0=Visual Studio 14 2015&& goto :_next
if %1==vc141 set _Tool=%1&& set _Gen0=Visual Studio 15 2017&& goto :_next

if %1==x86 set _Arch=%1&& set _Gen1=&& goto :_next
if %1==x64 set _Arch=%1&& set _Gen1= Win64&& goto :_next

if %1==Release set _Conf=%1&& goto :_next
if %1==Debug   set _Conf=%1&& goto :_next

echo "Invalid argument %1"
exit /b 1

:_next
shift
goto :_loop

:_run
if "%_Tool%"=="" echo Toolset was not defined&& exit /b 1
if "%_Arch%"=="" echo Architecture was not defined&& exit /b 1
if "%_Conf%"=="" echo Configuration was not defined&& exit /b 1

set _Dir=obj.%_Tool%.%_Arch%
set _Gen=%_Gen0%%_Gen1%

echo CMake binary directory: %_Dir%
echo CMake generator:        %_Gen%

if not exist "%_Dir%" mkdir "%_Dir%"

pushd "%_Dir%"
cmake -G "%_Gen%" ..
popd
if errorlevel 1 exit /b 1

pushd "%_Dir%"
"%_MsBuild%" /v:m /p:Configuration=%_Conf% utf-cpp.sln
if errorlevel 1 exit /b 1
popd

echo Ok
