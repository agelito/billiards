@echo off

cd %~dp0

if not exist bin (
   mkdir bin
)

if not exist obj (
   mkdir obj
)

set SRC_FILES=src/win32_main.c
set LIBRARIES=kernel32.lib user32.lib opengl32.lib

echo "Compiling..."
cl -EHa- /Od /Zi /W3 /D_CRT_SECURE_NO_WARNINGS /Isrc /Foobj/ /Febin/racera.exe %SRC_FILES% %LIBRARIES% /link /subsystem:windows

echo "Copying Game Assets..."
xcopy assets\* bin\ /v /d /y /s /e /f