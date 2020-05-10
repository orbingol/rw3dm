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

#include "rw3dm.h"


void initializeRwExt()
{
    ON::Begin();
}

void finalizeRwExt()
{
    ON::End();
}

void extractNurbsCurveData(const ON_Geometry* geometry, Config &cfg, Json::Value &data, double *paramOffset, double *paramLength)
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
        Json::Value weights;

        // Get control points and weights
        for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
        {
            double *vertex = nurbsCurve.CV(idx);
            double weight = nurbsCurve.Weight(idx);
            Json::Value point;
            for (int c = 0; c < nurbsCurve.Dimension(); c++)
            {
                double cp = vertex[c] / weight;
                if (paramOffset != nullptr && paramLength != nullptr && cfg.normalize())
                    point[c] = (cp - paramOffset[c]) / paramLength[c];
                else
                    point[c] = cp;
            }
            points[idx] = point;
            weights[idx] = weight;
        }
        controlPoints["points"] = points;
        controlPoints["weights"] = weights;

        data["control_points"] = controlPoints;
    }
}

void extractNurbsSurfaceData(const ON_NurbsSurface* nurbsSurface, Config& cfg, Json::Value& data) {
    // Get dimension
    data["dimension"] = nurbsSurface->Dimension();

    // Get rational
    data["rational"] = nurbsSurface->IsRational();

    // Get degrees
    data["degree_u"] = nurbsSurface->Degree(0);
    data["degree_v"] = nurbsSurface->Degree(1);

    // Get knot vectors
    Json::Value knotVectorU;
    if (cfg.normalize())
    {
        // Get parametric domain
        double* dU = nurbsSurface->Domain(0).m_t;

        // Normalize knot vector
        knotVectorU.append((nurbsSurface->SuperfluousKnot(0, false) - dU[0]) / (dU[1] - dU[0]));
        for (int idx = 0; idx < nurbsSurface->KnotCount(0); idx++)
            knotVectorU[idx + 1] = (nurbsSurface->Knot(0, idx) - dU[0]) / (dU[1] - dU[0]);
        knotVectorU.append((nurbsSurface->SuperfluousKnot(0, true) - dU[0]) / (dU[1] - dU[0]));
    }
    else
    {
        knotVectorU.append(nurbsSurface->SuperfluousKnot(0, false));
        for (int idx = 0; idx < nurbsSurface->KnotCount(0); idx++)
            knotVectorU[idx + 1] = nurbsSurface->Knot(0, idx);
        knotVectorU.append(nurbsSurface->SuperfluousKnot(0, true));
    }
    data["knotvector_u"] = knotVectorU;

    Json::Value knotVectorV;
    if (cfg.normalize())
    {
        // Get parametric domain
        double* dV = nurbsSurface->Domain(1).m_t;

        // Normalize knot vector
        knotVectorV.append((nurbsSurface->SuperfluousKnot(1, false) - dV[0]) / (dV[1] - dV[0]));
        for (int idx = 0; idx < nurbsSurface->KnotCount(1); idx++)
            knotVectorV[idx + 1] = (nurbsSurface->Knot(1, idx) - dV[0]) / (dV[1] - dV[0]);
        knotVectorV.append((nurbsSurface->SuperfluousKnot(1, true) - dV[0]) / (dV[1] - dV[0]));
    }
    else
    {
        knotVectorV.append(nurbsSurface->SuperfluousKnot(1, false));
        for (int idx = 0; idx < nurbsSurface->KnotCount(1); idx++)
            knotVectorV[idx + 1] = nurbsSurface->Knot(1, idx);
        knotVectorV.append(nurbsSurface->SuperfluousKnot(1, true));
    }
    data["knotvector_v"] = knotVectorV;

    Json::Value controlPoints;
    Json::Value points;
    Json::Value weights;

    // Get control points adn weights
    int sizeU = nurbsSurface->CVCount(0);
    int sizeV = nurbsSurface->CVCount(1);
    for (int idxU = 0; idxU < sizeU; idxU++)
    {
        for (int idxV = 0; idxV < sizeV; idxV++)
        {
            int idx = surfaceCvIndex(idxU, idxV, sizeU, sizeV);
            double* vertex = nurbsSurface->CV(idxU, idxV);
            double weight = nurbsSurface->Weight(idxU, idxV);
            Json::Value point;
            for (int c = 0; c < nurbsSurface->Dimension(); c++)
            {
                point[c] = vertex[c] / weight;
            }
            points[idx] = point;
            weights[idx] = weight;
        }
    }
    controlPoints["points"] = points;
    controlPoints["weights"] = weights;

    data["size_u"] = sizeU;
    data["size_v"] = sizeV;
    data["control_points"] = controlPoints;
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
        // Extract NURBS surface data
        extractNurbsSurfaceData(&nurbsSurface, cfg, data);
    }
}

void extractExtrusionData(const ON_Geometry* geometry, Config& cfg, Json::Value& data)
{
    // We expect an extrusion object
    if (ON::object_type::extrusion_object != geometry->ObjectType())
        return;

    // We know that "geometry" is an extrusion object
    ON_Extrusion* extr = (ON_Extrusion*)geometry;

    // Try to get the NURBS surface form of the extrusion object
    ON_NurbsSurface nurbsSurface;
    if (extr->NurbsSurface(&nurbsSurface))
    {
        // Extract NURBS surface data
        extractNurbsSurfaceData(&nurbsSurface, cfg, data);
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
                // Add face sense
                if (cfg.sense())
                    surfData["reversed"] = !brepFace->m_bRev;

                // Process trims
                if (cfg.trims())
                {
                    Json::Value trimCurvesData;
                    unsigned int loopIdx = 0;
                    ON_BrepLoop *brepLoop;

                    // Use loops to get trim information
                    while (brepLoop = brepFace->Loop(loopIdx))
                    {
                        Json::Value trimLoopData;
                        unsigned int trimIdx = 0;
                        ON_BrepTrim *brepTrim;

                        // Extract the trim inside the loop
                        while (brepTrim = brepLoop->Trim(trimIdx))
                        {
                            // Try to get the trim curve from the BRep structure
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
                                extractNurbsCurveData(trimCurve, cfg, curveData, paramOffset, paramLength);

                                // Only add to the array if JSON output is not empty
                                if (!curveData.empty())
                                {
                                    if (cfg.sense())
                                        curveData["reversed"] = !brepTrim->m_bRev3d;
                                    curveData["type"] = "spline";
                                    trimLoopData.append(curveData);
                                }
                            }

                            // Increment trim traversing index
                            trimIdx++;
                        }

                        // Create a container type trim
                        if (trimLoopData.size() > 0)
                        {
                            Json::Value trimData;
                            trimData["type"] = "container";
                            trimData["data"] = trimLoopData;
                            trimData["count"] = trimLoopData.size();
                            // Detect the sense
                            trimData["reversed"] = (brepLoop->m_type == ON_BrepLoop::TYPE::outer) ? true : false;

                            // Add trim container to the JSON array
                            trimCurvesData.append(trimData);
                        }

                        // Increment loop traversing index
                        loopIdx++;
                    }

                    // Due to the standardization, there should be 1 surface
                    if (trimCurvesData.size() > 0)
                    {
                        Json::Value trimData;
                        trimData["count"] = trimCurvesData.size();
                        trimData["data"] = trimCurvesData;

                        // Assign trims to the first surface
                        surfData["trims"] = trimData;
                    }
                }

                // Add extracted surface to the JSON array
                data.append(surfData);
            }
        }

        // Increment face traversing index
        faceIdx++;
    }
}

void constructNurbsCurveData(Json::Value &data, Config &cfg, ON_NurbsCurve *&nurbsCurve)
{
    // Control points array
    Json::Value ctrlpts = data["control_points"];

    // Spatial dimension
    int dimension = (data.isMember("dimension")) ? data["dimension"].asInt() : ctrlpts["points"][0].size();

    // Number of control points
    int numCtrlpts = data["control_points"]["points"].size();

    // Create a curve instance
    nurbsCurve = ON_NurbsCurve::New(
        dimension,
        (data.isMember("rational")) ? data["rational"].asInt() : 1,
        data["degree"].asInt() + 1,
        numCtrlpts
    );

    // Set knot vector
    for (int idx = 0; idx < nurbsCurve->KnotCount(); idx++)
        nurbsCurve->SetKnot(idx, data["knotvector"][idx + 1].asDouble());

    // Set control points
    for (int idx = 0; idx < nurbsCurve->CVCount(); idx++)
    {
        // Extract control point
        Json::Value cptData = ctrlpts["points"][idx];
        // Extract weight
        double w = (ctrlpts.isMember("weights")) ? ctrlpts["weights"][idx].asDouble() : 1.0;

        // Create a control vertex
        ON_4dPoint cptw;
        cptw.x = cptData[0].asDouble() * w;
        cptw.y = cptData[1].asDouble() * w;
        cptw.z = (dimension == 2) ? 0.0 : cptData[2].asDouble() * w;
        cptw.w = w;

        // Set control vertex
        nurbsCurve->SetCV(idx, cptw);
    }
}

void constructNurbsSurfaceData(Json::Value &data, Config &cfg, ON_Brep *&brep)
{
    // Control points array
    Json::Value ctrlpts = data["control_points"];

    // Spatial dimension
    int dimension = (data.isMember("dimension")) ? data["dimension"].asInt() : ctrlpts["points"][0].size();

    // Number of control points
    int sizeU = data["size_u"].asInt();
    int sizeV = data["size_v"].asInt();

    // Create a surface instance
    ON_NurbsSurface *nurbsSurface = ON_NurbsSurface::New(
        dimension,
        (data.isMember("rational")) ? data["rational"].asInt() : 1,
        data["degree_u"].asInt() + 1,
        data["degree_v"].asInt() + 1,
        sizeU,
        sizeV
    );

    // Set knot vectors
    for (int idx = 0; idx < nurbsSurface->KnotCount(0); idx++)
        nurbsSurface->SetKnot(0, idx, data["knotvector_u"][idx + 1].asDouble());
    for (int idx = 0; idx < nurbsSurface->KnotCount(1); idx++)
        nurbsSurface->SetKnot(1, idx, data["knotvector_v"][idx + 1].asDouble());


    // Set control points
    for (int idxU = 0; idxU < nurbsSurface->CVCount(0); idxU++)
    {
        for (int idxV = 0; idxV < nurbsSurface->CVCount(1); idxV++)
        {
            int idx = surfaceCvIndex(idxU, idxV, sizeU, sizeV);
            // Extract P
            Json::Value cptData = ctrlpts["points"][idx];
            // Extract weight
            double w = (ctrlpts.isMember("weights")) ? ctrlpts["weights"][idx].asDouble() : 1.0;
            // OpenNURBS uses Pw format
            ON_4dPoint cptw(cptData[0].asDouble() * w, cptData[1].asDouble() * w, cptData[2].asDouble() * w, w);
            // Set control vertex in OpenNURBS data
            nurbsSurface->SetCV(idxU, idxV, cptw);
        }
    }

    // Each surface (and the trim curves) should belong to a BRep object
    brep = ON_Brep::New();

    // Create a BRep object from the NURBS surface
    if (!brep->Create(nurbsSurface))
    {
        delete nurbsSurface;
        delete brep;
        nurbsSurface = nullptr;
        brep = nullptr;
        return;
    }

    // Process trims
    if (data.isMember("trims"))
    {
        Json::Value trims = data["trims"];
        if (trims.isMember("data"))
        {
            // Loop trims array
            ONX_Model trimModel;
            for (auto trim : trims["data"])
            {
                // Get trim type
                std::string trimType = trim["type"].asString();

                // Add trim curve to brep
                if (trimType == "spline")
                    constructBsplineTrimCurve(trim, cfg, brep);
                else if (trimType == "freeform")
                    constructFreeformTrimCurve(trim, cfg, brep);
                else if (trimType == "container")
                    constructContainerTrimCurve(trim, cfg, brep);
                else
                {
                    // Skip unsupported trim format
                    continue;
                }
            }
        }
        // Set necessary trim flags
        brep->SetTolerancesBoxesAndFlags(true);
    }

    // Check if the BRep is valid
    bool isValidBrep;
#if _DEBUG
    ON_TextLog logger;
    isValidBrep = brep->IsValid(&logger);
#else
    isValidBrep = brep->IsValid();
#endif
    if (!isValidBrep)
    {
        //brep->Destroy();
        std::cout << "[WARNING] Please check the trims (invalid BREP)" << std::endl;
    }
}


void constructBsplineTrimCurve(Json::Value& trim, Config& cfg, ON_Brep*& brep)
{
    // Construct the trim curve
    ON_NurbsCurve* trimCurve;
    constructNurbsCurveData(trim, cfg, trimCurve);

    // Try to understand if the extracted trim curve is the edge of the surface
    if (checkLinearBoundaryTrim(trimCurve))
        return;

    // Add trim curve to the BRep object
    int t2i = brep->AddTrimCurve(trimCurve);
    // If trim is valid, add trim sense, i.e. trim direction w.r.t. the face
    if (t2i > -1)
    {
        // Get the surface
        ON_Surface* surf = brep->m_S[0];

        // Get surface domain
        double st0_u, st1_u;
        surf->GetDomain(0, &st0_u, &st1_u);
        double st0_v, st1_v;
        surf->GetDomain(1, &st0_v, &st1_v);


        // Get domain of the 2-dimensional trim curve
        double t0, t1;
        trimCurve->GetDomain(&t0, &t1);

        // Construct 3-dimensional mapping of the trim curve
        double t = t0;
        double delta = 0.001;
        ON_3dPointArray eptArray;
        ON_SimpleArray<double> paramsArray;
        ON_3dPoint uv;
        ON_3dPoint ept;
        // Skip final parametric position to cheat floating point error
        while (t < t1)
        {
            trimCurve->EvPoint(t, uv);
            // Evaluate surface only if trim curve is on the surface
            // Trim curve domain range == surface domain range
            if ((uv.x >= st0_u) && (uv.x <= st1_u) && (uv.y >= st0_v) && (uv.y <= st1_v))
            {
                surf->EvPoint(uv.x, uv.y, ept);
                eptArray.Append(ept);
                //paramsArray.Append(t);
            }
            t += delta;
        }

        // Evaluate the last parametric position separately
        // to make sure last curve point == first curve point
        trimCurve->EvPoint(t1, uv);
        if ((uv.x >= st0_u) && (uv.x <= st1_u) && (uv.y >= st0_v) && (uv.y <= st1_v))
        {
            surf->EvPoint(uv.x, uv.y, ept);
            eptArray.Append(ept);
            //paramsArray.Append(t);
        }

        ON_PolylineCurve* trimCurve3d = new ON_PolylineCurve(eptArray, paramsArray);
        //ON_NurbsCurve *trimCurve3dNurbs = trimCurve3d.NurbsCurve(nullptr, tolerance);

        // Append mapping to the 3-dimensional curve array
        int t3i = brep->AddEdgeCurve(trimCurve3d);

        // Construct start and end vertices of the 3-dimensional edge
        ON_BrepVertex& edgeVertexStart = brep->NewVertex(trimCurve3d->PointAtStart(), RW3DM_VAR_TOLERANCE);
        //ON_BrepVertex& edgeVertexEnd = brep->NewVertex(trimCurve3d->PointAtEnd(), tolerance);

        // Construct a closed BRep edge
        //ON_BrepEdge &edge = brep->NewEdge(edgeVertexStart, edgeVertexEnd, t3i);
        ON_BrepEdge& edge = brep->NewEdge(edgeVertexStart, edgeVertexStart, t3i);
        edge.m_tolerance = RW3DM_VAR_TOLERANCE;

        // Construct BRep trim loop (outer: ccw, inner: cw)
        ON_BrepLoop& loop = brep->NewLoop(ON_BrepLoop::inner, brep->m_F[0]);

        // Construct trim
        bool bRev3d = (trim.isMember("reversed")) ? !trim["reversed"].asBool() : true;
        ON_BrepTrim& trim = brep->NewTrim(edge, bRev3d, loop, t2i);
        trim.m_type = ON_BrepTrim::boundary;

        // Set trim tolerance
        trim.m_tolerance[0] = RW3DM_VAR_TOLERANCE;
        trim.m_tolerance[1] = RW3DM_VAR_TOLERANCE;
    }
}


void constructFreeformTrimCurve(Json::Value& trim, Config& cfg, ON_Brep*& brep)
{
    // TO-DO
}


void constructContainerTrimCurve(Json::Value& trim, Config& cfg, ON_Brep*& brep)
{
    // TO-DO
}


bool checkLinearBoundaryTrim(ON_NurbsCurve *trimCurve)
{
    unsigned int trimValidateCount = 0;
    if (trimCurve->Degree() == 1)
    {
        double *cp1 = trimCurve->CV(0);
        double *cp2 = trimCurve->CV(1);
        for (int i = 0; i < 2; i++)
        {
            if (cp1[i] == 0.0 || cp1[i] == 1.0)
                trimValidateCount++;
            if (cp2[i] == 0.0 || cp2[i] == 1.0)
                trimValidateCount++;
        }
    }
    if (trimValidateCount == 4)
        return true;
    return false;
}


int surfaceCvIndex(int u, int v, int size_u, int size_v)
{
    return v + (u * size_v);
}

int volumeCvIndex(int u, int v, int w, int size_u, int size_v, int size_w)
{
    return v + (size_v * (u + (size_u * w)));
}
