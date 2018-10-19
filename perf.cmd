@echo off

call :_test vc142
call :_test vc141
call :_test vc140
call :_test vc120

echo Ok
goto :_end
:_test

set _Tool=%1
echo Toolset:  %_Tool%

if "%_Tool%"=="vc120" set _Compiler=Visual Studio 2013 v12.0.40629.00 Update 5
if "%_Tool%"=="vc140" set _Compiler=Visual Studio 2015 v14.0.25431.01 Update 3
if "%_Tool%"=="vc141" set _Compiler=Visual Studio 2017 v15.6.7
if "%_Tool%"=="vc142" set _Compiler=Visual Studio 2019 v16.0.3
if "%_Compiler%"=="" echo Unknwon toolset&& exit /b 1
echo Compiler: %_Compiler%

set _File=perf\%_Tool%_win.md
echo File:     %_File%

if exist "%_File%" del /f /q "%_File%"
call :_test2 x86
call :_test2 x64
goto :_end

:_test2
set _Arch=%1
echo Architecture: %_Arch%

set _Exe=obj.%_Tool%.%_Arch%\test\Release\utf-cpp-test.exe
echo Executable:  %_Exe%

echo #### Windows 10 %_Arch% (%_Compiler%):>>"%_File%"
echo ```>>"%_File%"
"%_Exe%" >>"%_File%" 2>&1
echo ```>>"%_File%"

:_end
