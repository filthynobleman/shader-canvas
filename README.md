# Shader Canvas
This repository provides a small application for the execution of OpenGL fragment shaders on a blank canvas.  

## Building the Application
For the building process you need a working [CMake](https://cmake.org/) installation and any C compiler supported by CMake.  
Navigate inside the project directory and execute the following commands
```
mkdir build
cd build
cmake ..
cmake --build . --config release
```

## Running
The building process should leave you with an executable named `ShaderCanvas` (on Windows: `ShaderCanvas.exe`). The program can be executed as follows
```
./ShaderCanvas [FILENAME] [-w WIDTH] [-h HEIGHT] [-f]
```
The options can be specified in any order and have the following meaning:

 - `FILENAME` is the path to a valid OpenGL fragment shader. If not provided, the program will color the screen with a red/green gradient based on the screen coordinates.
 - `-w` and `-h` are used to specify the initial size of the window.
 - `-f` enables the fullscreen mode (**TODO:** this seems to not always work and will be fixed as soon as possible).  


While the program is running, the user can interact with the following keys:
 - `ESC` for closing the application;
 - `R` for reloading the shader from the same file