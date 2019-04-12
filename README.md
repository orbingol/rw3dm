# RW3DM - Rhino Extensions for NURBS-Python (geomdl)

RW3DM is a Python module for extracting NURBS curves and surfaces from OpenNURBS/Rhino .3DM files. It uses
[OpenNURBS](https://github.com/mcneel/opennurbs) library for reading and writing .3DM files
and [pybind11](https://github.com/pybind/pybind11) to generate Python wrappers for the reading
and writing functions.

## Installing & updating RW3DM

### Installing RW3DM

1. Clone the repository: `git clone https://github.com/orbingol/rw3dm.git`
2. Change to the directory of the local repository: `cd rw3dm`
3. Update submodules: `git submodule update --init`
4. Create a build directory for CMake-generated files, e.g. `mkdir build`
5. Enter the build directory, e.g. `cd build`
6. Run CMake to generate build configuration files, e.g. `cmake ..` or use CMake GUI
7. Compile and install
 * For Windows, you will find the Visual Studio project files under `build` directory.
 * After opening Visual Studio, choose `Release` from the build combobox, then right click on `INSTALL` and choose `Rebuild`
 * For Linux, run `make install` inside the `build` directory
 * The install directory will be `build/install` by default.
 * You can modify the install directory using `RW3DM_INSTALL_PREFIX` variable while configuring the project with CMake
8. Go to the install directory, e.g. `cd install` or the one you configured with CMake during step 6.
9. Run: `python setup.py install`
 * Please note that on Linux systems, there will be `python2` and `python3`. `python` might be linked to one of these.
 * Please use the Python version that you linked the module. Otherwise, you will encounter import errors.

### Updating RW3DM

1. Pull the latest commits: `git pull --rebase`
2. Update submodules: `git submodule update`
3. Repeat steps from 5 to 9

### Notes on installing and updating

#### General information

* The minimum CMake version required is 3.8. Please visit [cmake.org](https://cmake.org/) to download the binary for your operating system.
* Please make sure that you are compiling against the correct Python library versions (mixing versions can cause some issues)

#### For Windows

* Visual Studio 2017 is required to compile OpenNURBS and pybind11 modules
* Don't forget to install C++ support on Visual Studio (not installed by default)
* It is also possible to use MSBuild (CMake detects it as Visual Studio)
* Visual Studio 32-bit and 64-bit are processed differently in CMake and it should be selected with respect to the Python architecture that you are using. You can find your Python architecture by typing:

`python -c "import platform; print(platform.architecture()[0])"`

* Choose **Release** as the build configuration inside Visual Studio before you compile the module
* In case of any import errors, check build configuration and compiler architecture again to make sure that they were correctly selected

#### For Ubuntu 18.04

* It is tested with gcc 7.3
* Don't forget to install the compiler packages: `apt install build-essential`
* You might also need to install the Python development libraries

## Using RW3DM

### Reading .3DM files



### Writing .3DM files



## Author

* Onur Rauf Bingol ([@orbingol](https://github.com/orbingol))

## License

* RW3DM is licensed under the terms of the [MIT License](LICENSE).
* [JsonCpp](https://github.com/open-source-parsers/jsoncpp) is licensed under the terms of the [MIT License](https://github.com/open-source-parsers/jsoncpp/blob/master/LICENSE)
* [OpenNURBS](https://github.com/mcneel/opennurbs) is licensed under the terms of the [MIT license](https://www.rhino3d.com/opennurbs)
