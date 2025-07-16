# Mega Mario SFML C++

## Description

Implementation of a platformer game using SFML and C++  
Based on [**COMP4300 - Game Programming - Lecture 11 - Assignment 3**](https://www.youtube.com/watch?v=k2ckoLsD7ZU) by Dave Churchill  
Uses an implementation of an Entity-Component-System (ECS) architecture  

### Gameplay

WIP

## Installation

### Prerequisites

- C++/C compiler (gcc, g++, cl, clang)
- CMake 3.28 or newer

For Linux/Ubuntu, you also need to install the following dependencies:  

```bash
sudo apt-get update
sudo apt-get install -y libx11-dev \
                        libxcursor-dev \
                        libxi-dev \
                        libxrandr-dev \
                        libudev-dev \
                        libgl1-mesa-dev \
                        libfreetype6-dev \
                        libjpeg-dev \
                        libopenal-dev \
                        libflac-dev \
                        libvorbis-dev \
                        libxcb1-dev \
                        libxcb-image0-dev \
                        libxcb-randr0-dev \
                        libxcb-xtest0-dev
```

### Clone the repository

```bash
git clone https://github.com/Corentin-Mzr/Megamario-SFML
```

### Build the project

From the root folder, execute the following commands

```bash
cmake -B build
cmake --build build
```

### Run the program

To run the program, launch it from the build/bin folder

```bash
cd build/bin
./Megamario-SFML.exe
```

## Libraries

The following libraries have been used for this program

- [**SFML 3.0**](https://github.com/SFML/SFML)
- [**toml11**](https://github.com/ToruNiina/toml11)

## Controls

WIP

## License

This program is under the [**MIT License**](LICENSE.md)
