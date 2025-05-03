@echo off
echo Compiling Tetris...
g++ main.cpp src\*.cpp -Iinclude -o tetris.exe
if %errorlevel% neq 0 (
    echo Compilation failed.
) else (
    echo Compilation successful. Output: tetris.exe
)
pause
