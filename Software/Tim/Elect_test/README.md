Hi, I'm going to explain how this project works since I made some changes. I hope that this won't be too complicated once I lay it out but please ask questions.

# Prereqs
You need to install some things to make it all work. Honest to god it should be very simple.

If on Windows, use **WSL** for everything! If on Linux you have easy mode, and macOS is very similar to Linux but the way you install things is slightly different.

Install the packages below. This is install CMake (used to compile and link things) and utilties needed to compile code for our ARM processor (RP2040)

`sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib`

Those should be the only prereqs you need to install.

# Structure
The project structure is straightforward.

- **src**: Your application code, your main function and supporting code that don't belong as standalone libraries in `lib`.
- **lib**: Libraries like the pico-sdk, peripheral drivers for devices like radios, other code you want to use from the internet.
- **build**: Where the final executables (.uf2, .elf) end up. Also contains intermediate files used to build the executables.
- **CMakeLists.txt**: Not a directory but it is where the build process starts. See comments for more info

# How To Use

## After Cloning
When you first pull this repo down you'll need to update the submodules within. Submodules are just other git repositories that exist inside this one.

`git submodule update --init --recursive`

This might take awhile.

## Building
To do a "clean build" do the following.

```
rm -rf build
mkdir build
cd build
cmake -DPICO_BOARD=none ..
make
```

Deleting the build directory is important since it probably contains cached files from other people when you first download it. Just using this folder to build won't work since it's expecting antoher person's machine. If you just want the final executable you can just grab it from the folder, you don't need to rebuild.

Subsequent builds only require running `cmake -DPICO_BOARD=none ..` and `make` again. If things start to break an easy fix is sometimes to do a new clean build.