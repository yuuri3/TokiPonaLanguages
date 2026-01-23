@echo off
setlocal

pushd "%~dp0"
g++ -o a Utility.cpp Random.cpp Language.cpp main.cpp -std=c++2a
start a.exe
popd

pause