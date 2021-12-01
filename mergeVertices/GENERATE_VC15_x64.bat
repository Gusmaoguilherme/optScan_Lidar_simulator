set PATH=%CD%\dependencies\geogram\bin;%PATH%
set CMAKE_PREFIX_PATH=%CD%\dependencies\geogram;
mkdir build
chdir build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX=install ../

@pause