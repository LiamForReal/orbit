@echo off
setlocal

REM Set install directory
set VCPKG_DIR=%CD%\vcpkg

REM Clone vcpkg if not already present
if not exist "%VCPKG_DIR%" (
    echo Cloning vcpkg into %VCPKG_DIR%...
    git clone https://github.com/microsoft/vcpkg.git %VCPKG_DIR%
)

REM Bootstrap vcpkg
cd /d %VCPKG_DIR%
echo Bootstrapping vcpkg...
call bootstrap-vcpkg.bat

REM Install Boost
echo Installing Boost via vcpkg...
vcpkg install boost

echo.
echo Done! Boost has been installed using vcpkg.
pause