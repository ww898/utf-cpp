@echo off

call build.cmd vc120 x86 Release
if errorlevel 1 exit /b 1

call build.cmd vc120 x86 Debug
if errorlevel 1 exit /b 1

call build.cmd vc120 x64 Release
if errorlevel 1 exit /b 1

call build.cmd vc120 x64 Debug
if errorlevel 1 exit /b 1

call build.cmd vc140 x86 Release
if errorlevel 1 exit /b 1

call build.cmd vc140 x86 Debug
if errorlevel 1 exit /b 1

call build.cmd vc140 x64 Release
if errorlevel 1 exit /b 1

call build.cmd vc140 x64 Debug
if errorlevel 1 exit /b 1

call build.cmd vc141 x86 Release
if errorlevel 1 exit /b 1

call build.cmd vc141 x86 Debug
if errorlevel 1 exit /b 1

call build.cmd vc141 x64 Release
if errorlevel 1 exit /b 1

call build.cmd vc141 x64 Debug
if errorlevel 1 exit /b 1

echo Ok
