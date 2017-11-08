# ReboundU
A utility library for the Rebound n-body solver code, found [here](http://github.com/hannorein/rebound). This project is a derivative of [luabound](https://github.com/mossseank/luabound), with the output code from that ported to be usable from C without needing to use the lua interface.

The interface is written in C, but includes C++ wrapper code as well. This allows for this library to be used in both C and C++ code freely, without the user needed to include different files, or use special include guards. The library itself is implemented in C++11.

The code is licensed under the GNU GPL v3 license, the full text of which can be found in the LICENSE file in this repository. Unless otherwise stated, all code in this repository is copyright Sean Moss (contact info below). All licensing and copyright information for the Rebound library can be found in the Rebound repository (link above).

## How to Build from Source
Unfortunately, because of the highly cpu-extension-dependent nature of Rebound, luabound must be built anew on nearly every system that it will be used on.

#### Supported Systems/Compilers
ReboundU has only been tested on the following system/compiler pairs, and is not officially supported on other compilers or operating systems:  
* Linux Mint 18.1 (Ubuntu 16.04 LTS) with g++ 5.4.0 (although any equivalent Linux distro should work)  

#### Required Packages
In order to build from source, you must have the following packages installed on your system (these are the Ubuntu names, but most distributions should have their own versions):
* GLFW3 - `libglfw3` and `libglfw3-dev` (only if using the visualizer)

If you are working on a restricted system that does not allow installation of these packages, you will have to build them from source yourself, and then add the path to the libraries into the `reboundu.build` file to properly link them.

Compiler support for OpenMP must be enabled as well, if you are using OpenMP. This is usually default on Linux, but the default MacOS compilers will sometimes not have OpenMP support.

#### Compiling the Code
*Note: this library uses C++11 features. __You must use GCC 4.8.0+ or Clang 3.2+__ to get these features.*

To actually build the code:
```bash
./build.sh
cd BUILD
make config=<cfg>
```
where `<cfg>` defines the features that are available to Rebound, and can have one of the following values:
* *basic* - Do not include the OpenGL visualizer or OpenMP acceleration.
* *gl* - Include only the OpenGL visualizer.
* *mp* - Include only the OpenMP acceleration.
* *full* - Include both the OpenGL visualizer and OpenMP acceleration.

#### Installing
Running the `install.sh` script in the root of the repo will copy the minimum required files into the `INSTALL` folder that you need to use ReboundU. Two header files, and two libraries will be copied, which can then easily be moved around to use in your own projects.

## How to Use
Documentation on how to build, use, and customize Luabound can be found on the [Github Wiki](https://github.com/mossseank/ReboundU/wiki).

## Contact
For questions, comments, or concerns regarding the code, or the project itself, please contact:
* Sean Moss (*sean.moss@colorado.edu*) - Primary developer and maintainer of the code.
