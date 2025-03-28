@echo off

echo Building Python .pyd module...
python setup.py build_ext --inplace

echo Compiling C++ program...
g++ main.cpp -o main.exe -I"C:\Program Files\Python38\include" -L"C:\Program Files\Python38\libs" -lpython38

echo Build complete. Running the program...
main.exes