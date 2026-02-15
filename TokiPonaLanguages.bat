@echo off
setlocal

pushd "%~dp0"
g++ -o ignore/a headers/Utility.cpp headers/Random.cpp headers/Meaning.cpp headers/Word.cpp headers/Language.cpp TokiPonaLanguages.cpp -std=c++2a -lcomdlg32
popd

pause