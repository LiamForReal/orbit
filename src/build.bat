rmdir "build" /S /Q
mkdir "build"
cmake -G "MinGW Makefiles" -S . -B build/
cd build/
make
pause