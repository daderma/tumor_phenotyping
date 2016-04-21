@echo off
set DEPENDENCIES_PREFIX=%cd%\dependencies
set BOOST_PREFIX=%DEPENDENCIES_PREFIX%\boost
set ZLIB_PREFIX=%DEPENDENCIES_PREFIX%\zlib
set LIBTIFF_PREFIX=%DEPENDENCIES_PREFIX%\libtiff
set INSTALL_PREFIX=%DEPENDENCIES_PREFIX%\install
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64


if exist "%INSTALL_PREFIX%" (goto done)


pushd "%BOOST_PREFIX%"
call bootstrap.bat
b2 headers
set BOOST_LIBRARIES=--with-filesystem
b2 --toolset=msvc-12.0 --prefix="%INSTALL_PREFIX%\debug" --layout=tagged %BOOST_LIBRARIES% address-model=64 link=static runtime-link=shared variant=debug install
b2 --toolset=msvc-12.0 --prefix="%INSTALL_PREFIX%\release" --layout=tagged %BOOST_LIBRARIES% address-model=64 link=static runtime-link=shared variant=release install
popd


pushd "%ZLIB_PREFIX%"
git clean -d -f
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\debug .
cmake --build . --target install --config debug
git clean -d -f
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\release .
cmake --build . --target install --config release
popd


pushd "%LIBTIFF_PREFIX%"
git clean -d -f
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\debug -DBUILD_SHARED_LIBS=OFF .
cmake --build . --target install --config debug
git clean -d -f
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install\release -DBUILD_SHARED_LIBS=OFF .
cmake --build . --target install --config release
popd


:done
