# Extract NURBS curves and surfaces from .3DM files

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
5. Run CMake to generate build configuration files
6. Compile and install (it will also compile OpenNURBS)
7. Go to the install directory and run: `python setup.py install`

### Updating RW3DM

1. Pull the latest commits: `git pull --rebase`
2. Update submodules: `git submodule update`
3. Compile and install

### Notes on installing and updating

#### For Windows

* Visual Studio 2017 is required to compile OpenNURBS and pybind11 modules
* Please don't forget to install C++ support on Visual Studio
* It is also possible to use MSBuild (CMake detects it as Visual Studio)
* Visual Studio 32-bit and 64-bit are processed differently in CMake and it should be selected with respect to the Python architecture that you are using. You can find your Python architecture by typing:

`python -c "import platform; print(platform.architecture()[0])"`

* Please choose **Release** as the build configuration inside Visual Studio before you compile the module
* In case of any import errors, check build configuration and compiler architecture again to make sure that they were correctly selected

#### For Linux

* The package compilation is tested on Ubuntu 18.04 with gcc 7.3
* For Ubuntu, please don't forget to install the compiler packages: `apt install build-essential`

## Using RW3DM

### Reading .3DM files

The following code snippet reads a .3DM file and extracts shape information contained in the file.

```python
from rw3dm import rw3dm

# Create an empty list for storing the models in the .3DM file
data_out = []

# Read the models contained in the .3DM file into the list "data_out" 
rw3dm.read("Models_In.3dm", data_out)
```

### Writing .3DM files

The following code snippet writes NURBS shape information to a .3DM file.

```python
from rw3dm import rw3dm

# Assuming that you have created "data_in" list as explained below
# Write the surfaces and curves contained in the "data_in" list to the .3DM file 
rw3dm.write(data_in, "Models_Out.3dm")
```

### Data structure for curve

```yaml
- shape_type: "curve"   # should be the string "curve"
- dimension: (int)      # dimension of the curve; e.g. 2D or 3D (optional)
- degree: (int)         # degree of the curve
- knotvector: (list)    # knot vector
- control_points:
  - points: (list)      # Cartesian coordinates of the control points
  - weights: (list)     # weights vector (optional)
```

### Data structure for surface

```yaml
- shape_type: "surface"   # should be the string "surface"
- dimension: 3            # dimension of the surface (optional) 
- degree_u: (int)         # degree (u-direction)
- degree_v: (int)         # degree (v-direction)
- knotvector_u: (list)    # knot vector (u-direction)
- knotvector_v: (list)    # knot vector (v-direction)
- size_u: (int)           # number of control points on the u-direction
- size_v: (int)           # number of control points on the v-direction
- control_points:
  - points: (list)        # Cartesian coordinates of the control points
  - weights: (list)       # weights vector (optional)
```

## Function reference

* `rw3dm.read(filename, data_out)`: Reads .3DM files
* `rw3dm.write(data_in, filename)`: Writes .3DM files

`filename` is a string, `data_in` and `data_out` are lists of dictionaries containing the NURBS curve and surface data.
Please refer to the Data Structures section above for details on the structure of the surface and curve dictionaries.

## Author

* Onur Rauf Bingol ([@orbingol](https://github.com/orbingol))

## License

* RW3DM project is available under the MIT License.
* [OpenNURBS](https://github.com/mcneel/opennurbs) is available under the MIT license
* [pybind11](https://github.com/pybind/pybind11) is available under a BSD-style license
