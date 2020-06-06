# RW3DM - Rhino Extensions for NURBS-Python (geomdl)

[![Appveyor Status](https://ci.appveyor.com/api/projects/status/gdawoy927oeex1dj?svg=true)](https://ci.appveyor.com/project/orbingol/rw3dm) 
[![Azure Status](https://dev.azure.com/quantumworx/rw3dm/_apis/build/status/orbingol.rw3dm?branchName=master)](https://dev.azure.com/quantumworx/rw3dm/_build/latest?definitionId=1&branchName=master)

RW3DM project provides executables for exporting and importing OpenNURBS/Rhino .3DM files in JSON format.
It utilizes [OpenNURBS](https://github.com/mcneel/opennurbs) library for reading and writing .3DM files
and [JsonCpp](https://github.com/open-source-parsers/jsoncpp) for importing and exporting
[geomdl](https://github.com/orbingol/NURBS-Python) geometry data.

## Installation and Updating RW3DM

### Downloading RW3DM

You can download the precompiled Windows 64-bit binaries under [Actions](https://github.com/orbingol/rw3dm/actions) tab.

### Compiling RW3DM

If you prefer to compile the binaries on your own, you can follow the following list as a reference:

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
 * The install directory will be `build/install` by default
 * You can modify the install directory using `RW3DM_INSTALL_DIR` variable while configuring the project with CMake
8. Go to the install directory, e.g. `cd install` or the one you configured with CMake during step 6
9. You will find the executables inside the install directory

### Updating RW3DM

If you prefer to update the compiled binaries on your own, you can follow the following list as a reference:

1. Pull the latest commits: `git pull --rebase`
2. Update submodules: `git submodule update`
3. Repeat steps from 5 to 9

### Notes on installing and updating

#### For Windows

* Visual Studio 2017 or newer is required to compile OpenNURBS
* Please install C++ support on Visual Studio as it is not installed by default
* Choose **Release** as the build configuration inside Visual Studio before you compile the package

#### For Linux

* It is tested with gcc 7.3 and it should be compatible with any gcc version supporting C++17
* Please remember to install the compiler packages for your Linux distribution

## Using RW3DM

### Reading .3DM files

`on2json` executable can be used to convert .3DM files to JSON format supported by [geomdl](https://github.com/orbingol/NURBS-Python).
The JSON files can be imported via [geomdl](https://github.com/orbingol/NURBS-Python)'s `exchange.import_json` API call.

### Writing .3DM files

`json2on` executable can be used to convert JSON format supported by [geomdl](https://github.com/orbingol/NURBS-Python) to .3DM files.
The JSON files can be exported via [geomdl](https://github.com/orbingol/NURBS-Python)'s `exchange.export_json` API call.

### Available arguments

Run `on2json` and `json2on` to see the available command-line arguments:

* `extract_curves`: Extract curves (Default is extract surfaces)
* `normalize`: Normalize knot vectors and scale trim curves to [0,1] domain
* `sense`: Extract surface and trim curve direction w.r.t. the face
* `show_config`: Print the configuration
* `silent`: Disable all printed messages
* `trims`: Extract trim curves

**Example**: `on2json MyONFile.3dm extract_curves=True`, extracts curves from *MyONFile.3dm*

## Author

* Onur Rauf Bingol ([@orbingol](https://github.com/orbingol))

## License

* RW3DM is licensed under the terms of the [MIT License](LICENSE).
* [JsonCpp](https://github.com/open-source-parsers/jsoncpp) is licensed under the terms of the [MIT License](https://github.com/open-source-parsers/jsoncpp/blob/master/LICENSE)
* [OpenNURBS](https://github.com/mcneel/opennurbs) is licensed under the terms of the [MIT license](https://www.rhino3d.com/opennurbs)
