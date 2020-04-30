/*
Copyright (c) 2018-2019 IDEA Lab, Iowa State University
Copyright (c) 2018-2020 Onur Rauf Bingol

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef RW3DM_H
#define RW3DM_H

#include "common.h"
#include <opennurbs_public.h>
#include <json/json.h>

// Framework initialization
void initializeRwExt();
void finalizeRwExt();

// Geometry extraction (3DM -> geomdl)
void extractCurveData(const ON_Geometry *, Config &, Json::Value &, double * = nullptr, double * = nullptr);
void extractNurbsSurfaceData(const ON_NurbsSurface *, Config &, Json::Value &);
void extractSurfaceData(const ON_Geometry *, Config &, Json::Value &);
void extractBrepData(const ON_Geometry *, Config &, Json::Value &);
void extractExtrusionData(const ON_Geometry*, Config&, Json::Value&);

// Geometry conversion (geomdl -> 3DM)
void constructCurveData(Json::Value &, Config &, ON_NurbsCurve *&);
void constructSurfaceData(Json::Value &, Config &, ON_Brep *&);

// Helper functions
bool checkLinearBoundaryTrim(ON_NurbsCurve *);

#endif /* RW3DM_H */
