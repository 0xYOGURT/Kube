@echo off
REM Build and run the OpenGL GLFW project

REM Set compiler and paths (adjust these if your setup differs)
set COMPILER=g++
set GLAD_DIR=glad
set GLFW_DIR=glfw
set INCLUDE_DIRS=-I"%GLAD_DIR%\include" -I"%GLFW_DIR%\include"
set LIB_DIRS=-L"%GLFW_DIR%\lib-mingw" 
set LIBS=-lglfw3 -lopengl32 -lgdi32

REM Output executable
set OUTPUT=mess.exe

echo Compiling...
%COMPILER% main.cpp %INCLUDE_DIRS% %LIB_DIRS% %LIBS% -o %OUTPUT%

if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed!
    pause
    exit /b 1
)

echo Running...
%OUTPUT%
pause
