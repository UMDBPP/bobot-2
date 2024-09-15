Hi, I'm going to explain how this project works since I made some changes. I hope that this won't be too complicated once I lay it out but please ask questions.

# Prereqs

You need to install some things to make it all work. Honest to god it should be very simple.

**If on Windows, use WSL for everything!** If on Linux you have easy mode, and macOS is very similar to Linux but the way you install things is slightly different.

Install the packages below. This is install CMake (used to compile and link things) and utilties needed to compile code for our ARM processor (RP2040).

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

Subsequent builds only require running `cmake -DPICO_BOARD=none ..` and `make` again. If things start to break, an easy fix is sometimes to do a new clean build. Sometimes changes you make are not properly picked up by the build system and the quickest solution is to do a clean build.

### How it Works

I'm not an expert but I'll try to give a good enough explanation to make it start to make sense.

At the top level, our project has a CMakeLists.txt file. This specifies a lot of things about our project such as source files, included libraries, desired final products, and more. I've tried to put some descriptive comments in there to make it clearer what it all means.

The command `cmake -DPICO_BOARD=none ..` does a lot of things but thankfully all of the details are super important to us. The most important thing that it does is go and find all of the needed libraries and source files specified in CMakeLists.txt. Once it finds all of them it generates a number of **Makefiles**.

Makefiles are just files that describe the final product of a build, all of the things those final products depend upon, and how to make the final product out of those dependencies. They're basically just recipes.

In the build directory you'll find the top level Makefile. Running `make` is what kicks off the steps specified in this file and eventually produces the final product. This file has many many steps in it, indcluding doing all of the steps specified by Makefiles that are deeper down in the build folder. As evidenced by the terminal output when you run `make` in this directory, a lot of stuff happens. 

If everything is written and set up correctly though it'll produce the final executable (.uf2 or .elf)!

# Improvements Over The Arduino IDE

This approach offer a number of advantages over writing your solution in the Arduino IDE. Namely, this solution is more flexible.

Code can be written and included however you want to do it. I think you'll find that this lends itself to more legible code since splitting functions out into their own headers and .c files is vastly simpler than it is in the Arduino IDE. Additionally, passing versions of code around on Github is far easier. Rather than having a single monolithic .ino file, you have a collection of .c, .cpp, and .h files that can be modified and changed. I think this lends itself to better design and documentation.

I also personally appreciate the flexbility of choosing C between and C++. This project actually features both. I understand that this probably doesn't matter to Aero and EE majors since the curriculum doesn't encourage the C purist approach I was exposed to as a CompE.

The other big improvement I see is portability. First of all, a single .uf2 or .elf executable can be built that anyone can upload to the device, **even without the toolchain installed.** This means that a single product can be released on Github and anyone with a USB port and an internet connection can reflash the device. There's no need to use a specific person's laptop because they happened to make particular changes to a library. Furthermore if changes are made, they are all tracked by Git and available on Github. This means changes are easy to see, track, and even revert if needed.

On top of that since the project is self contained, **all of the code needed to build the final product is in this folder.** This means that so long as you have the toolchain above installed (which is available for everyone because of how ubiquitous it is), you can build it with the instructions above.

This solution isn't the best one for everyone or for every payload. The Arduino Framework is king of abstraction. But for certain payloads such as this one that is penny pinching grams off the mass budget and trying to extract the best possible performance, safety, and reliabilty a solution like this is more optimal in my opinion. It provides better access to the hardware which enables that extra flexibility and performance that is the cost for the Arduino frameworks total abstraction.

## Downsides

Like any solution there are downsides. The notable one being that there is less abstraction here than there is with the Arduino Framework. I think that as you write code in the way specified by this example project you'll find that it's not **that** different though. Furthermore, having a better understanding of what the hardware is doing will lead to a better final result.

For one, the official documentation is very good for this space. It's not perfect but it's much more complete than you'd get elsewhere.

https://www.raspberrypi.com/documentation/pico-sdk/

Secondly, Arduino libraries are more difficult to use. You either need to port them (some of them make this easier than others), or you need to write something yourself. I think you'll find however, that there are already a number of barebones C libraries for the sensors and things you want to use.