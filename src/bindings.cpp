#include <pybind11/pybind11.h>

#include "rw3dm.h"


PYBIND11_MODULE(rw3dm, m)
{
    m.doc() = "Reads and writes OpenNURBS/Rhino .3DM files"; // module docstring
    m.def("start", &startON, "Starts OpenNURBS library");
    m.def("stop", &stopON, "Stops OpenNURBS library");
    m.def("read", &readONFile, "Reads OpenNURBS/Rhino .3DM files");
    m.def("write", &writeONFile, "Writes OpenNURBS/Rhino .3DM files");
}
