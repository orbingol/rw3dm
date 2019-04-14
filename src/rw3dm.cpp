/*
Copyright (c) 2018-2019 Onur Rauf Bingol

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

#include "rw3dm.h"


void initializeRwExt()
{
    ON::Begin();
}

void finalizeRwExt()
{
    ON::End();
}

void extractCurveData(const ON_Geometry* geometry, Config &cfg, Json::Value &data, double *paramOffset, double *paramLength)
{
    // We expect a curve object
    if (ON::object_type::curve_object != geometry->ObjectType())
        return;

    // We know that "geometry" is a curve object
    const ON_Curve *curve = (ON_Curve *)geometry;

    // Try to get the NURBS form of the curve object
    ON_NurbsCurve nurbsCurve;
    if (curve->NurbsCurve(&nurbsCurve))
    {
        // Get dimension
        data["dimension"] = nurbsCurve.Dimension();

        // Get rational
        data["rational"] = nurbsCurve.IsRational();

        // Get degree
        data["degree"] = nurbsCurve.Degree();

        // Get knot vector
        Json::Value knotVector;
        if (cfg.normalize())
        {
            // Get parametric domain
            double *d = nurbsCurve.Domain().m_t;

            // Normalize knot vector
            knotVector.append((nurbsCurve.SuperfluousKnot(false) - d[0]) / (d[1] - d[0]));
            for (int idx = 0; idx < nurbsCurve.KnotCount(); idx++)
                knotVector[idx + 1] = (nurbsCurve.Knot(idx) - d[0]) / (d[1] - d[0]);
            knotVector.append((nurbsCurve.SuperfluousKnot(true) - d[0]) / (d[1] - d[0]));
        }
        else
        {
            knotVector.append(nurbsCurve.SuperfluousKnot(false));
            for (int idx = 0; idx < nurbsCurve.KnotCount(); idx++)
                knotVector[idx + 1] = nurbsCurve.Knot(idx);
            knotVector.append(nurbsCurve.SuperfluousKnot(true));
        }
        data["knotvector"] = knotVector;

        Json::Value controlPoints;
        Json::Value points;

        // Get control points
        for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
        {
            double *vertex = nurbsCurve.CV(idx);
            Json::Value point;
            for (int c = 0; c < nurbsCurve.Dimension(); c++)
            {
                if (paramOffset != nullptr && paramLength != nullptr && cfg.normalize())
                    point[c] = (vertex[c] - paramOffset[c]) / paramLength[c];
                else
                    point[c] = vertex[c];
            }
            points[idx] = point;
        }
        controlPoints["points"] = points;

        // Check if the NURBS curve is rational
        if (nurbsCurve.IsRational())
        {
            Json::Value weights;
            // Get weights
            for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
                weights[idx] = nurbsCurve.Weight(idx);
            controlPoints["weights"] = weights;
        }
        data["control_points"] = controlPoints;
    }
}

void extractSurfaceData(const ON_Geometry* geometry, Config &cfg, Json::Value &data)
{
    // We expect a surface object
    if (ON::object_type::surface_object != geometry->ObjectType())
        return;

    // We know that "geometry" is a surface object
    const ON_Surface *surface = (ON_Surface *)geometry;

    // Try to get the NURBS form of the surface object
    ON_NurbsSurface nurbsSurface;
    if (surface->NurbsSurface(&nurbsSurface))
    {
        // Get dimension
        data["dimension"] = nurbsSurface.Dimension();

        // Get rational
        data["rational"] = nurbsSurface.IsRational();

        // Get degrees
        data["degree_u"] = nurbsSurface.Degree(0);
        data["degree_v"] = nurbsSurface.Degree(1);

        // Get knot vectors
        Json::Value knotVectorU;
        if (cfg.normalize())
        {
            // Get parametric domain
            double *dU = nurbsSurface.Domain(0).m_t;

            // Normalize knot vector
            knotVectorU.append((nurbsSurface.SuperfluousKnot(0, false) - dU[0]) / (dU[1] - dU[0]));
            for (int idx = 0; idx < nurbsSurface.KnotCount(0); idx++)
                knotVectorU[idx + 1] = (nurbsSurface.Knot(0, idx) - dU[0]) / (dU[1] - dU[0]);
            knotVectorU.append((nurbsSurface.SuperfluousKnot(0, true) - dU[0]) / (dU[1] - dU[0]));
        }
        else
        {
            knotVectorU.append(nurbsSurface.SuperfluousKnot(0, false));
            for (int idx = 0; idx < nurbsSurface.KnotCount(0); idx++)
                knotVectorU[idx + 1] = nurbsSurface.Knot(0, idx);
            knotVectorU.append(nurbsSurface.SuperfluousKnot(0, true));
        }
        data["knotvector_u"] = knotVectorU;

        Json::Value knotVectorV;
        if (cfg.normalize())
        {
            // Get parametric domain
            double *dV = nurbsSurface.Domain(1).m_t;

            // Normalize knot vector
            knotVectorV.append((nurbsSurface.SuperfluousKnot(1, false) - dV[0]) / (dV[1] - dV[0]));
            for (int idx = 0; idx < nurbsSurface.KnotCount(1); idx++)
                knotVectorV[idx + 1] = (nurbsSurface.Knot(1, idx) - dV[0]) / (dV[1] - dV[0]);
            knotVectorV.append((nurbsSurface.SuperfluousKnot(1, true) - dV[0]) / (dV[1] - dV[0]));
        }
        else
        {
            knotVectorV.append(nurbsSurface.SuperfluousKnot(1, false));
            for (int idx = 0; idx < nurbsSurface.KnotCount(1); idx++)
                knotVectorV[idx + 1] = nurbsSurface.Knot(1, idx);
            knotVectorV.append(nurbsSurface.SuperfluousKnot(1, true));
        }
        data["knotvector_v"] = knotVectorV;

        Json::Value controlPoints;
        Json::Value points;

        // Get control points
        int sizeU = nurbsSurface.CVCount(0);
        int sizeV = nurbsSurface.CVCount(1);
        for (int idxU = 0; idxU < sizeU; idxU++)
        {
            for (int idxV = 0; idxV < sizeV; idxV++)
            {
                unsigned int idx = idxV + (idxU * sizeV);
                double *vertex = nurbsSurface.CV(idxU, idxV);
                Json::Value point;
                for (int c = 0; c < nurbsSurface.Dimension(); c++)
                    point[c] = vertex[c];
                points[idx] = point;
            }
        }
        controlPoints["points"] = points;

        // Check if the NURBS surface is rational
        if (nurbsSurface.IsRational())
        {
            Json::Value weights;
            // Get weights
            for (int idxU = 0; idxU < sizeU; idxU++)
            {
                for (int idxV = 0; idxV < sizeV; idxV++)
                {
                    unsigned int idx = idxV + (idxU * sizeV);
                    weights[idx] = nurbsSurface.Weight(idxU, idxV);
                }
            }
            controlPoints["weights"] = weights;
        }
        data["size_u"] = sizeU;
        data["size_v"] = sizeV;
        data["control_points"] = controlPoints;
    }
}

void extractBrepData(const ON_Geometry* geometry, Config &cfg, Json::Value &data)
{
    // We expect a BRep object
    if (ON::object_type::brep_object != geometry->ObjectType())
        return;

    // We know that "geometry" is a BRep object
    ON_Brep *brep = (ON_Brep *)geometry;

    // Standardize relationships of all surfaces, edges and trims in the BRep object
    brep->Standardize();

    // Delete unnecessary curves and surfaces after "standardize"
    brep->Compact();

    // Face loop
    unsigned int faceIdx = 0;
    unsigned int surfIdx = 0;
    ON_BrepFace *brepFace;
    while (brepFace = brep->Face(faceIdx))
    {
        const ON_Surface *faceSurf = brepFace->SurfaceOf();
        if (faceSurf)
        {
            Json::Value surfData;
            extractSurfaceData(faceSurf, cfg, surfData);
            // Only add to the array if JSON output is not empty
            if (!surfData.empty())
            {
                data[surfIdx] = surfData;
                surfIdx++;
            }
        }
        faceIdx++;
    }

    // Trim loop
    if (cfg.trims())
    {
        Json::Value trimCurvesData;
        unsigned int trimIdx = 0;
        unsigned int curveIdx = 0;
        ON_BrepTrim *brepTrim;
        while (brepTrim = brep->Trim(trimIdx))
        {
            const ON_Curve *trimCurve = brepTrim->TrimCurveOf();
            if (trimCurve)
            {
                // Get surface domain for normalization of trimming curves
                ON_Interval dom_u = brepTrim->SurfaceOf()->Domain(0);
                ON_Interval dom_v = brepTrim->SurfaceOf()->Domain(1);

                // Prepare parameter space offset and length
                double paramOffset[2] = { dom_u.m_t[0], dom_v.m_t[0] };
                double paramLength[2] = { dom_u.Length(), dom_v.Length() };

                // Extract trim curve data
                Json::Value curveData;
                extractCurveData(trimCurve, cfg, curveData, paramOffset, paramLength);
                // Only add to the array if JSON output is not empty
                if (!curveData.empty())
                {
                    curveData["type"] = "spline";
                    trimCurvesData[curveIdx] = curveData;
                    curveIdx++;
                }
            }
            trimIdx++;
        }

        // Because of the standardization, there should be 1 surface
        if (trimCurvesData.size() > 0)
        {
            Json::Value trimData;
            trimData["count"] = curveIdx;
            trimData["data"] = trimCurvesData;

            // Assign trims to the first surface
            data[0]["trims"] = trimData;
        }
    }
}

ON_ModelComponentReference
constructCurveData(Json::Value &data, Config &cfg, ONX_Model &model)
{
    // Control points array
    Json::Value ctrlpts = data["control_points"];

    // Spatial dimension
    int dimension = (data.isMember("dimension")) ? data["dimension"].asInt() : ctrlpts["points"][0].size();
 
    // Number of control points
    int numCtrlpts = data["control_points"]["points"].size();

    // Create a curve instance
    ON_NurbsCurve nurbsCurve(
        dimension,
        (data.isMember("rational")) ? data["rational"].asInt() : 1,
        data["degree"].asInt() + 1,
        numCtrlpts
    );

    // Set knot vector
    for (int idx = 0; idx < nurbsCurve.KnotCount(); idx++)
        nurbsCurve.SetKnot(idx, data["knotvector"][idx].asDouble());

    // Set control points
    for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
    {
        Json::Value cptData = ctrlpts["points"][idx];
        ON_3dPoint cpt(cptData[0].asDouble(), cptData[1].asDouble(), (dimension == 2) ? 0.0 : cptData[2].asDouble());
        nurbsCurve.SetCV(idx, cpt);
        if (ctrlpts.isMember("weights"))
            nurbsCurve.SetWeight(idx, ctrlpts["weights"].asDouble());
    }

    // Add curve to the model
    return model.AddModelGeometryComponent(&nurbsCurve, nullptr);
}

ON_ModelComponentReference
constructSurfaceData(Json::Value &data, Config &cfg, ONX_Model &model)
{
    // Control points array
    Json::Value ctrlpts = data["control_points"];

    // Spatial dimension
    int dimension = (data.isMember("dimension")) ? data["dimension"].asInt() : ctrlpts["points"][0].size();

    // Number of control points
    int sizeU = data["size_u"].asInt();
    int sizeV = data["size_v"].asInt();

    // Create a surface instance
    ON_NurbsSurface nurbsSurface(
        dimension,
        (data.isMember("rational")) ? data["rational"].asInt() : 1,
        data["degree_u"].asInt() + 1,
        data["degree_v"].asInt() + 1,
        sizeU,
        sizeV
    );

    // Set knot vectors
    for (int idx = 0; idx < nurbsSurface.KnotCount(0); idx++)
        nurbsSurface.SetKnot(0, idx, data["knotvector_u"][idx].asDouble());
    for (int idx = 0; idx < nurbsSurface.KnotCount(1); idx++)
        nurbsSurface.SetKnot(1, idx, data["knotvector_v"][idx].asDouble());

    // Set control points
    for (int idxU = 0; idxU < nurbsSurface.CVCount(0); idxU++)
    {
        for (int idxV = 0; idxV < nurbsSurface.CVCount(1); idxV++)
        {
            unsigned int idx = idxV + (idxU * sizeV);
            Json::Value cptData = ctrlpts["points"][idx];
            ON_3dPoint cpt(cptData[0].asDouble(), cptData[1].asDouble(), cptData[2].asDouble());
            nurbsSurface.SetCV(idxU, idxV, cpt);
            if (ctrlpts.isMember("weights"))
                nurbsSurface.SetWeight(idxU, idxV, ctrlpts["weights"].asDouble());
        }
    }

    // Each surface (and the trim curves) should belong to a BRep object
    ON_Brep brep;

    // Add surface to the BRep object
    brep.NewFace(nurbsSurface);

    // Process trims
    if (data.isMember("trims"))
    {
        Json::Value trims = data["trims"];
        if (trims.isMember("data"))
        {
            ONX_Model trimModel;
            for (auto trim : trims["trims"]["data"])
            {
                // Construct the trim curve and get its reference
                ON_ModelComponentReference trimRef = constructCurveData(trim, cfg, trimModel);
                // Start converting reference to the curve object
                const ON_ModelGeometryComponent *trimGeomComp = ON_ModelGeometryComponent::FromModelComponentRef(trimRef, nullptr);
                ON_Geometry *trimGeom = trimGeomComp->ExclusiveGeometry();
                ON_Curve *trimCurve = ON_Curve::Cast(trimGeom);
                // Add trim curve to the BRep object
                brep.AddTrimCurve(trimCurve);
            }
        }
    }

    // Add BRep to the model
    return model.AddModelGeometryComponent(&brep, nullptr);
}
