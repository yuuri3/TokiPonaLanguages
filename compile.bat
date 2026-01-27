@echo off
setlocal

pushd "%~dp0"
g++ -o ignore/a header/Utility.cpp header/Random.cpp header/Language.cpp main.cpp -std=c++2a
start ignore/a.exe input.csv
popd

pause