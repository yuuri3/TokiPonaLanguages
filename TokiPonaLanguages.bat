@echo off
setlocal

pushd "%~dp0"
g++ -o ignore/a Utility.cpp Random.cpp Language.cpp TokiPonaLanguages.cpp -std=c++2a -mwindows -lcomdlg32
start ignore/a.exe input.csv
popd

pause