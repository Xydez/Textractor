@echo off

if exist "build" (
  rmdir /s /q "build" || goto :error
)

if exist "builds" (
  rmdir /s /q "builds" || goto :error
)

REM === BUILD INFO ===
REM may either be `x64` or `Win32`
set PLATFORM=Win32

REM === QT INFO ===
REM `5.15.2`
set QT_VERSION=5.15.2
REM `msvc2019` or `msvc2019_64`
set QT_BIN=msvc2019

set Qt5_DIR=%LocalAppData%\QT\%QT_VERSION%\%QT_BIN%\lib\cmake\Qt5

cmake ^
  --log-level=DEBUG ^
  -G "Visual Studio 16 2019" ^
  -A %PLATFORM% ^
  -DQt5_DIR=%Qt5_DIR% ^
  -DCMAKE_BUILD_TYPE="RelWithDebInfo" ^
  -DVERSION="" ^
  -S . -B build

goto :end

:error
echo An error occurred, exiting.

:end
pause