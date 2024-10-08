rmdir "build" /S /Q
mkdir "build"
cmake -G "Unix Makefiles" -S . -B build/
cd build/
make
pause