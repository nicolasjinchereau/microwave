@echo off

set MICROWAVE_DIR=D:/Repositories/microwave
set PROJECT_NAME=TestApp

:: generate the project with premake
call "%MICROWAVE_DIR%\premake\premake5.exe" vs2022 --os=windows

:: find MSVC install path
set VSWHERE_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
call "%VSWHERE_PATH%" -latest -utf8 -property installationPath>vs-install-path-tmp
set /p VS_INSTALL_PATH=<vs-install-path-tmp
del vs-install-path-tmp

:: run dev command prompt setup script
call "%VS_INSTALL_PATH%\Common7\Tools\VsDevCmd.bat"

:: build the project
msbuild projects/windows/%PROJECT_NAME%.sln /p:Configuration=Debug /t:Clean;Rebuild

exit /b