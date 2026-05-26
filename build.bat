@echo off
setlocal

:: -----------------------------
:: Defaults
:: -----------------------------
set BUILD_CONFIG=debug
set OPEN_DEBUGGER=false
set TARGET_ENV=vs2022

:: -----------------------------
:: Parse arguments
:: -----------------------------
if /I "%2"=="debugger" (
    set OPEN_DEBUGGER=true
) else if /I "%2"=="debug" (
    set BUILD_CONFIG=debug
) else if /I "%2"=="release" (
    set BUILD_CONFIG=release
) else if /I "%2"=="dist" (
    set BUILD_CONFIG=dist
) else if not "%2"=="" (
    echo ERROR: Unknown argument "%2"
    exit /b 1
)

if /I "%3"=="debugger" (
    set OPEN_DEBUGGER=true
) else if not "%3"=="" (
    echo ERROR: Unknown argument "%3"
    exit /b 1
)

:: -----------------------------
:: Setup VS environment
:: -----------------------------

call 3rd\premake\premake5.exe --file="generate.lua" "%TARGET_ENV%"

if errorlevel 1 (
    echo ERROR: Failed to generate project files.
    exit /b 1
)

:: -----------------------------
:: Build
:: -----------------------------
set MSBUILD="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"

call %MSBUILD% ".project-files/game.sln" ^
    /t:"%1" ^
    /p:Configuration="%BUILD_CONFIG%" ^
    /p:Platform=x64

if errorlevel 1 (
    echo.
    echo BUILD FAILED
    exit /b 1
)

echo.
echo BUILD SUCCEEDED

endlocal