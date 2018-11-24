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
bool writeONFile(py::list &, std::string, py::kwargs &);

void _readCurve(const ON_Geometry *, py::list &);
void _readSurface(const ON_Geometry *, py::list &);
void _readBrep(const ON_Geometry *, py::list &);
void _constructDict(const ON_Curve *, py::dict &);
void _constructDict(const ON_Surface *, py::dict &);

bool _constructCurve(ONX_Model &, py::dict &);
bool _constructSurface(ONX_Model &, py::dict &);

#endif /* RW3DM_H */
