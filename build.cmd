@echo off
set DEPENDENCIES_ROOT=%cd%\dependencies
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64


goto :zlib


:prepare
echo Preparing directories
cd /d "%DEPENDENCIES_ROOT%"
rmdir /s /q install


:zlib
cd /d "%DEPENDENCIES_ROOT%"
robocopy /mir /nfl /ndl /mt zlib-1.2.8 zlib
cd zlib
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\debug .
cmake --build . --target install --config debug
cd /d "%DEPENDENCIES_ROOT%"
robocopy /mir /nfl /ndl /mt zlib-1.2.8 zlib
cd zlib
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\release .
cmake --build . --target install --config release


:libpng
cd /d "%DEPENDENCIES_ROOT%"
robocopy /mir /nfl /ndl /mt libpng-1.6.20 libpng
cd libpng
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\debug .
cmake --build . --target install --config debug
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\release .
cmake --build . --target install --config release


:boost
cd /d "%DEPENDENCIES_ROOT%"
robocopy /mir /nfl /ndl /mt boost_1_59_0 boost
cd boost
call bootstrap.bat
b2 --toolset=msvc-12.0 --prefix=..\install\debug --layout=tagged -j8 address-model=64 link=static runtime-link=shared variant=debug install
b2 --toolset=msvc-12.0 --prefix=..\install\release --layout=tagged -j8 address-model=64 link=static runtime-link=shared variant=release install


pause


