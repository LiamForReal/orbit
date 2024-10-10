REM COPYRIGHT (C) Made by ORBit Team 2024

REM Donwload Boost Source Code
curl -O https://archives.boost.io/release/1.86.0/source/boost_1_86_0.zip
tar -xf "boost_1_86_0.zip"
del /f /q "boost_1_86_0.zip"
cd ./boost_1_86_0

REM Download B2
curl -LO https://github.com/bfgroup/b2/releases/download/5.2.1/b2-5.2.1.zip
tar -xf b2-5.2.1.zip
del /f /q "b2-5.2.1.zip"

REM Setup B2
cd "./b2-5.2.1"
bootstrap.bat
b2 install --prefix=.
copy "./b2.exe" "../boost_1_86_0/tools/build/src/engine"

REM Setup Boost
cd ..
cd ./boost_1_86_0
bootstrap.bat gcc
b2 link=static runtime-link=static --prefix=../boost install

pause