# RW3DM - Rhino 3DM file reader/writer in Python

RW3DM is a Python module for reading and writing OpenNURBS/Rhino .3DM files in Python. It uses
[OpenNURBS](https://github.com/mcneel/opennurbs) library for reading and writing .3DM files
and [pybind11](https://github.com/pybind/pybind11) to generate Python wrappers for the reading
and writing functions.

## Installation

1. Clone the repository: `git clone https://github.com/orbingol/rw3dm.git`
2. Change to the directory of the local repository: `cd rw3dm`
3. Update submodules: `git submodule update --init`
4. Run CMake to generate build configuration files
5. Compile and install (it will also compile OpenNURBS)
5. Go to install directory and run: `python setup.py install`

## Using

The following code segment illustrates the usage of the Python module:

```python
from rw3dm import rw3dm

# Create an empty list for storing the models in the .3DM file
data = []

# Read the models inside the .3DM file into the list "data" 
rw3dm.read("Surface.3dm", data)
```

## Function Reference

* `rw3dm.read()`: Reads .3DM files
* `rw3dm.write()`: Writes .3DM files

## Author

* Onur Rauf Bingol ([@orbingol](https://github.com/orbingol))

## License

* RW3DM project is available under the MIT License.
* [OpenNURBS](https://github.com/mcneel/opennurbs) is available under the MIT license
* [pybind11](https://github.com/pybind/pybind11) is available under a BSD-style license
