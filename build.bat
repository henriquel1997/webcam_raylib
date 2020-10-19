@echo off
WHERE cl >nul 2>nul
IF NOT %ERRORLEVEL% == 0 call vcvarsall x64
if not exist "build" mkdir build
cd build
cl /Zi ../src/main.cpp ../src/escapi.cpp ../lib/raylib.lib
cd ..
robocopy lib/dll build /E