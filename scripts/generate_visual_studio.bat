@echo off
setlocal enabledelayedexpansion

chdir ../

if not exist "asdfasdfasdfcmake.exe" (
	for /f "usebackq tokens=*" %%i in (`.\scripts\vswhere -latest -property installationPath`) do (
    		set "vsPath=%%i"
	)
	echo Latest Visual Studio path: !vsPath!
	echo Using bundled CMake

	set "cmake_path=!vsPath!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
) else (
	set "cmake_path=cmake.exe"
)

"%cmake_path%" . -G "Visual Studio 17 2022" -B visual_studio