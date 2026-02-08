@echo off
setlocal

pushd "%~dp0"
g++ -o ignore/a Utility.cpp Random.cpp Language.cpp TokiPonaLanguages.cpp -std=c++2a -lcomdlg32
popd

pause