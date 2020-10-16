@echo off
if not exist "build" mkdir build
cd build
cl /Zi ../src/main.cpp ../src/escapi.cpp ../lib/raylib.lib
cd ..
robocopy lib/dll build /E