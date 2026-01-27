@echo off
setlocal

pushd "%~dp0"

del /q "ignore\test_data\*"

g++ -o ignore/a header/Utility.cpp header/Random.cpp header/Language.cpp main.cpp -std=c++2a
start /wait "" ignore/a.exe test_data/Input_FullData.csv
start /wait "" ignore/a.exe test_data/Input_NoOldTokiPona.csv
start /wait "" ignore/a.exe test_data/Input_NoMap.csv
start /wait "" ignore/a.exe test_data/Input_NoPhonetics.csv
start /wait "" ignore/a.exe test_data/Input_SoundChange.csv
start /wait "" ignore/a.exe test_data/Input_SoundChange_NoRemove.csv
start /wait "" ignore/a.exe test_data/Input_SoundRemove.csv

dir "ignore\test_data"

popd

pause