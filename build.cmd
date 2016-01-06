@echo off
set DEPENDENCIES_ROOT=%cd%\dependencies
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64


goto :prepare


:prepare
echo Preparing directories
cd /d "%DEPENDENCIES_ROOT%"
rmdir /s /q install


:boost
cd /d "%DEPENDENCIES_ROOT%"
robocopy /mir /nfl /ndl /mt boost_1_59_0 boost
cd boost
call bootstrap.bat
b2 --toolset=msvc-12.0 --prefix=..\install\debug --layout=tagged -j8 address-model=64 link=static runtime-link=shared variant=debug install
b2 --toolset=msvc-12.0 --prefix=..\install\release --layout=tagged -j8 address-model=64 link=static runtime-link=shared variant=release install


pause


