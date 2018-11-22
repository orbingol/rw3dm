#ifndef RW3DM_H
#define RW3DM_H

#include <iostream>
#include <string>

#include <pybind11/pybind11.h>
#include <opennurbs_public.h>

namespace py = pybind11;

void startON();
void stopON();
bool readONFile(std::string, py::list &);
bool writeONFile(py::list &, std::string);

void _readCurve(const ON_Geometry *, py::dict &);
void _readSurface(const ON_Geometry *, py::dict &);
void _readBrep(const ON_Geometry *, py::list &);
void _constructDict(const ON_Curve *, py::dict &);
void _constructDict(const ON_Surface *, py::dict &);

#endif /* RW3DM_H */
