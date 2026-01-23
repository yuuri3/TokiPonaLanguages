@echo off
setlocal

pushd "%~dp0"
g++ -o a header/Utility.cpp header/Random.cpp header/Language.cpp main.cpp -std=c++2a
start a.exe
popd

pause