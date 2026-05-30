# Reaction Aim
Reaction Aim is a minimalist arcade game and reaction trainer made with raylib. I do not intend it as an advance tool as this is just a side project of mine.

## Installation
You can download pre-built binaries from the [itch.io page](https://fenixthexd.itch.io/reaction-aim). Alternatively, you can build it yourself if you want to make changes. 

## Building from Source
1. Make sure you have a C compiler, git, CMake, and any libraries raylib needs installed.
2. Clone this repo or [download it](https://github.com/fenixthexd/reaction-aim/archive/refs/heads/main.zip).
```
git clone https://github.com/fenixthexd/reaction-aim.git
```
3. Initialize the project with CMake.
```
cmake -S . -B build
```
4. Compile the program.
```
cmake --build build
```
5. Finally, run the program found in `build/`.