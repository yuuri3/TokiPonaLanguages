@echo off
setlocal

pushd "%~dp0"

del /q "ignore\test_data\*"

g++ -o ignore/a Utility.cpp Random.cpp Language.cpp test.cpp -std=c++2a
start /wait "" ignore/a.exe

dir "ignore\test_data"

popd

pause