# luabound
Allows simulations for the [Rebound](http://github.com/hannorein/rebound) n-body code to be defined in Lua, and run on the C API. This code is developed primarily by [Sean Moss](mailto:sean.moss@colorado.edu), at the University of Colorado at Boulder.

This project aims to produce a stand-alone executable and lightweight Lua API, that allows Rebound simulations to be fully described in Lua and then run on the Rebound C API. This makes the use of Rebound more accessable, allowing simulations to be defined in an easy to use declarative fashion. This provides the ease of implementation that is seen in the Python version, but with the speed and features seen in the C version. It also comes with predefined file output settings, allowing for tracking of a wide range of simulation parameters, without the user needing to write their own file logging functionality. Included with this is a python script to easily read and extract the data from these output files, with a single function call. Furthermore, if C code is needed for the simulation callbacks, Luabound supports runtime loading of plugins written in C for performant code and callback use.

The code is licensed under the GNU GPL v3 license, the full text of which can be found in the LICENSE file in this repository. Unless otherwise stated, all code in this repository is copyright Sean Moss (contact info below). All licensing and copyright information for the Rebound library can be found in the Rebound repository (link above).

## How to Build from Source
Unfortunately, because of the highly cpu-extension-dependent nature of Rebound, luabound must be built anew on nearly every system that it will be used on. This is discussed a bit more in [this file](https://github.com/mossseank/luabound/blob/master/extlib/rebound/README.md).

#### Supported Systems/Compilers
Luabound has only been tested on the following system/compiler pairs, and is not officially supported on other compilers or operating systems:  
* Linux Mint 18.1 (Ubuntu 16.04 LTS) with g++ 5.4.0 (although any equivalent Linux distro should work)  

#### Required Packages
In order to build from source, you must have the following packages installed on your system (these are the Ubuntu names, but most distributions should have their own versions):
* Lua 5.3 - `lua5.3` and `lua5.3-dev` (__only for Linux__, MacOS has the lua binaries included in this distribution already)
* GLFW3 - `libglfw3` and `libglfw3-dev` (only if using the visualizer)

If you are working on a restricted system that does not allow installation of these packages, you will have to build them from source yourself, and put the resulting binaries in the `extlib/lib` directory.

Compiler support for OpenMP must be enabled as well, if you are using OpenMP. This is usually default on Linux, but the default MacOS compilers will sometimes not have OpenMP support.

#### Compiling the Code
*Note: sol2, the C++ wrapper for Lua, requires C++14 features to compile properly. __You must use GCC 5.2.0+ or Clang 3.5+__ to get these features.*

To actually build the code:
```bash
./build.sh
cd BUILD
make config=<cfg>
```
where `<cfg>` defines the features that are available to Rebound, and can have one of the following values:
* *basic* - Do not include the OpenGL visualizer or OpenMP acceleration. (Output: OUT/luabound)
* *vis* - Include only the OpenGL visualizer. (Output: OUT/luaboundv)
* *omp* - Include only the OpenMP acceleration. (Output: OUT/luaboundm)
* *visomp* - Include both the OpenGL visualizer and OpenMP acceleration. (Output: OUT/luaboundvm)

## How to Use
Documentation on how to build, use, and customize Luabound can be found on the [Github Wiki](https://github.com/mossseank/luabound/wiki).

## How to Contribute
If you are looking to contribute to the code, please make your own fork of the repository, and submit any changes through a pull request. Please follow all coding styles and standards as seen in the code. Specifically, the code is formatted in the [K&R Style](https://en.wikipedia.org/wiki/Indent_style#K.26R), with the use of tab characters for indentation (PRs that use spaces will be rejected without a second thought).

Please fully explain in detail exactly what changes you have made in the PR. Make sure to reference any sources, such as the Github issue tracker, that your PR may reference or close. We welcome bugfixes, code cleanup, or even the addition of entirely new features.

If you notice a bug in the code, or have a feature that you would like to request, please open an issue on Github.

## Contact
For questions, comments, or concerns regarding the code, or the project itself, please contact:
* Sean Moss (*sean.moss@colorado.edu*) - Primary developer and maintainer of the code.