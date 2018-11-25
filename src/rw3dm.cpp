#include "rw3dm.h"

void startON()
{
    ON::Begin();
}

void stopON()
{
    ON::End();
}

bool readONFile(std::string file_name, py::list &data)
{
    // Open file
    FILE* fp = ON::OpenFile(file_name.c_str(), "rb");
    if (!fp)
    {
        py::print("Cannot open file: " + file_name);
        return false;
    }

    // Create achive object from file pointer
    ON_BinaryFile archive(ON::archive_mode::read3dm, fp);

    ONX_Model model;

    // Start reading the model archive
    unsigned int tableFilter = ON::curve_object | ON::surface_object;
    if (!model.IncrementalReadBegin(archive, true, tableFilter, (ON_TextLog *)nullptr))
    {
        py::print("Problem in start reading the model archive");
        return false;
    }

    // Read models
    ON_ModelComponentReference mCompRef;
    while (model.IncrementalReadModelGeometry(archive, true, true, true, 0, mCompRef))
    {
        // Check if there are any models to read
        if (!mCompRef.IsEmpty())
        {
            const ON_ModelGeometryComponent &geometryComp = model.ModelGeometryComponentFromId(mCompRef.ModelComponentId());
            const ON_Geometry *geometry = geometryComp.Geometry((ON_Geometry *)nullptr);
            if (geometry != nullptr)
            {
                switch (geometry->ObjectType())
                {
                case ON::curve_object:
                    _readCurve(geometry, data);
                    break;
                case ON::surface_object:
                    _readSurface(geometry, data);
                    break;
                case ON::brep_object:
                    _readBrep(geometry, data);
                    break;
                default:
                    break;
                }
            }
        }
    }

    // Finish reading the model archive
    if (!model.IncrementalReadFinish(archive, true, tableFilter, (ON_TextLog *)nullptr))
    {
        py::print("Problem in finish reading the model archive");
        return false;
    }

    // Close file
    ON::CloseFile(fp);

    return true;
}

bool writeONFile(py::list &data, std::string file_name, py::kwargs &kwargs)
{
    // Get keyword arguments
    int version = 50;
    if (kwargs.contains("version"))
        version = py::cast<int>(kwargs["version"]);

    // Create model
    ONX_Model model;

    // Read shape data
    for (auto d : data)
    {
        py::dict dd = py::cast<py::dict>(d);
        if (dd.contains("shape_type"))
        {
            std::string shape_type = py::cast<std::string>(dd["shape_type"]);
            if (shape_type == "curve")
                if (!_constructCurve(model, dd))
                    return false;
            if (shape_type == "surface")
                if (!_constructSurface(model, dd))
                    return false;
        }
    }

    // Write model to the file
    model.Write(file_name.c_str(), version);

    return true;
}

void _readCurve(const ON_Geometry* geometry, py::list &data)
{
    // We know that "geometry" is a curve object
    const ON_Curve *curve = (ON_Curve *)geometry;

    // Construct the dictionary
    py::dict dataDict;
    _constructDict(curve, dataDict);
    data.append(dataDict);
}

void _readSurface(const ON_Geometry* geometry, py::list &data)
{
    // We know that "geometry" is a surface object
    const ON_Surface *surface = (ON_Surface *)geometry;

    // Construct the dictionary
    py::dict dataDict;
    _constructDict(surface, dataDict);
    data.append(dataDict);
}

void _readBrep(const ON_Geometry* geometry, py::list &data)
{
    ON_Brep *geo = (ON_Brep *)geometry;
    unsigned int brepFaceIndex = 0;
    ON_BrepFace *face;
    while (face = geo->Face(brepFaceIndex))
    {
        const ON_Surface *faceSurf = face->SurfaceOf();
        if (faceSurf)
        {
            py::dict dataDict;
            _constructDict(faceSurf, dataDict);
            data.append(dataDict);
        }
        brepFaceIndex++;
    }
}

void _constructDict(const ON_Curve *curve, py::dict &data)
{
    // Try to get the NURBS form of the curve object
    ON_NurbsCurve nurbsCurve;
    if (curve->NurbsCurve(&nurbsCurve))
    {
        // Set shape type
        data["shape_type"] = "curve";

        // Set dimension
        data["dimension"] = nurbsCurve.Dimension();

        // Rational or non-rational curve
        data["rational"] = nurbsCurve.IsRational();

        // Get degree
        data["degree"] = nurbsCurve.Degree();

        // Get knot vector
        py::list knotVector;
        for (int idx = 0; idx < nurbsCurve.KnotCount(); idx++)
            knotVector.append(nurbsCurve.Knot(idx));
        data["knotvector"] = knotVector;

        py::dict controlPoints;

        // Get control points
        int numCoords = nurbsCurve.IsRational() ? nurbsCurve.CVSize() - 1 : nurbsCurve.CVSize();
        py::list points;
        for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
        {
            double *vertex = nurbsCurve.CV(idx);
            py::list point;
            for (int c = 0; c < numCoords; c++)
                point.append(vertex[c]);
            points.append(point);
        }
        controlPoints["points"] = points;

        // Check if the NURBS curve is rational
        if (nurbsCurve.IsRational())
        {
            // Get weights
            py::list weights;
            for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
            {
                double *vertex = nurbsCurve.CV(idx);
                weights.append(vertex[numCoords - 1]);
            }
            controlPoints["weights"] = weights;
        }
        data["control_points"] = controlPoints;
    }
}

void _constructDict(const ON_Surface *surf, py::dict &data)
{
    // Try to get the NURBS form of the surface object
    ON_NurbsSurface nurbsSurface;
    if (surf->NurbsSurface(&nurbsSurface))
    {
        // Set shape type
        data["shape_type"] = "surface";

        // Set dimension
        data["dimension"] = nurbsSurface.Dimension();

        // Rational or non-rational surface
        data["rational"] = nurbsSurface.IsRational();

        // Get degrees
        data["degree_u"] = nurbsSurface.Degree(0);
        data["degree_v"] = nurbsSurface.Degree(1);

        // Get knot vectors
        py::list knotVectorU;
        for (int idx = 0; idx < nurbsSurface.KnotCount(0); idx++)
            knotVectorU.append(nurbsSurface.Knot(0, idx));
        data["knotvector_u"] = knotVectorU;

        py::list knotVectorV;
        for (int idx = 0; idx < nurbsSurface.KnotCount(1); idx++)
            knotVectorV.append(nurbsSurface.Knot(1, idx));
        data["knotvector_v"] = knotVectorV;

        py::dict controlPoints;

        // Get control points
        int numCoords = nurbsSurface.IsRational() ? nurbsSurface.CVSize() - 1 : nurbsSurface.CVSize();
        int sizeU = nurbsSurface.CVCount(0);
        int sizeV = nurbsSurface.CVCount(1);
        py::list points;
        for (int idxU = 0; idxU < sizeU; idxU++)
        {
            for (int idxV = 0; idxV < sizeV; idxV++)
            {
                double *vertex = nurbsSurface.CV(idxU, idxV);
                py::list point;
                for (int c = 0; c < numCoords; c++)
                    point.append(vertex[c]);
                points.append(point);
            }
        }
        controlPoints["points"] = points;

        // Check if the NURBS surface is rational
        if (nurbsSurface.IsRational())
        {
            // Get weights
            py::list weights;
            for (int idxU = 0; idxU < sizeU; idxU++)
            {
                for (int idxV = 0; idxV < sizeV; idxV++)
                {
                    double *vertex = nurbsSurface.CV(idxU, idxV);
                    weights.append(vertex[numCoords - 1]);
                }
            }
            controlPoints["weights"] = weights;
        }
        data["size_u"] = sizeU;
        data["size_v"] = sizeV;
        data["control_points"] = controlPoints;
    }
}


bool _constructCurve(ONX_Model &model, py::dict &data)
{
    // Check for required variables
    std::vector<std::string> requiredVariables = {
        "dimension",
        "rational",
        "degree",
        "knotvector",
        "control_points"
    };
    for (auto reqVar : requiredVariables)
    {
        if (!data.contains(reqVar.c_str()))
            return false;
    }

    // Get dimension
    int dimension = py::cast<int>(data["dimension"]);
    // Can only work with 2- and 3-dimensional curves
    if (dimension > 3 || dimension < 2)
        return false;

    // Get knot vector
    py::list knotVector = py::cast<py::list>(data["knotvector"]);

    // Get control points and weights
    py::dict controlPoints = py::cast<py::dict>(data["control_points"]);

    if (!controlPoints.contains("points"))
        return false;

    py::list points = py::cast<py::list>(controlPoints["points"]);
    py::list weights;
    if (controlPoints.contains("weights"))
    {
        weights = py::cast<py::list>(controlPoints["weights"]);
    }
    else
    {
        for (int i = 0; i < points.size(); i++)
            weights.append(1.0);
    }

    // Create OpenNURBS curve instance
    ON_NurbsCurve nurbsCurve(dimension,
        true,
        py::cast<int>(data["degree"]) + 1,
        (int)points.size()
    );

    // Set knot vector
    for (int idx = 0; idx < nurbsCurve.KnotCount(); idx++)
        nurbsCurve.SetKnot(idx, py::cast<double>(knotVector[idx]));

    // Set control points
    for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
    {
        py::list cptDict = py::cast<py::list>(points[idx]);
        ON_4dPoint cpt(
            py::cast<double>(cptDict[0]),
            py::cast<double>(cptDict[1]),
            (dimension == 2) ? 0.0 : py::cast<double>(cptDict[2]),
            py::cast<double>(weights[idx])
        );
        nurbsCurve.SetCV(idx, cpt);
    }

    // Add curve to the model
    model.AddModelGeometryComponent(&nurbsCurve, nullptr);

    return true;
}

bool _constructSurface(ONX_Model &model, py::dict &data)
{
    // Check for required variables
    std::vector<std::string> requiredVariables = {
        "dimension",
        "rational",
        "degree_u", "degree_v",
        "knotvector_u", "knotvector_v",
        "size_u", "size_v", "control_points"
    };
    for (auto reqVar : requiredVariables)
    {
        if (!data.contains(reqVar.c_str()))
            return false;
    }

    // Get dimension
    int dimension = py::cast<int>(data["dimension"]);
    // Can only work with 3-dimensional surfaces
    if (dimension != 3)
        return false;

    // Get knot vectors
    py::list knotVectorU = py::cast<py::list>(data["knotvector_u"]);
    py::list knotVectorV = py::cast<py::list>(data["knotvector_v"]);

    // Get control points and weights
    py::dict controlPoints = py::cast<py::dict>(data["control_points"]);

    if (!controlPoints.contains("points"))
        return false;

    int sizeU = py::cast<int>(data["size_u"]);
    int sizeV = py::cast<int>(data["size_v"]);
    py::list points = py::cast<py::list>(controlPoints["points"]);
    py::list weights;
    if (controlPoints.contains("weights"))
    {
        weights = py::cast<py::list>(controlPoints["weights"]);
    }
    else
    {
        for (int i = 0; i < points.size(); i++)
            weights.append(1.0);
    }

    // Create OpenNURBS surface instance
    ON_NurbsSurface nurbsSurface(
        dimension,
        true,
        py::cast<int>(data["degree_u"]) + 1,
        py::cast<int>(data["degree_v"]) + 1,
        sizeU,
        sizeV
    );

    // Set knot vectors
    for (int idx = 0; idx < nurbsSurface.KnotCount(0); idx++)
        nurbsSurface.SetKnot(0, idx, py::cast<double>(knotVectorU[idx]));
    for (int idx = 0; idx < nurbsSurface.KnotCount(1); idx++)
        nurbsSurface.SetKnot(1, idx, py::cast<double>(knotVectorV[idx]));

    // Set control points
    for (int idxU = 0; idxU < nurbsSurface.CVCount(0); idxU++)
    {
        for (int idxV = 0; idxV < nurbsSurface.CVCount(1); idxV++)
        {
            int idx = idxV + (sizeV * idxU);
            py::list cptDict = py::cast<py::list>(points[idx]);
            ON_4dPoint cpt(
                py::cast<double>(cptDict[0]),
                py::cast<double>(cptDict[1]),
                py::cast<double>(cptDict[2]),
                py::cast<double>(weights[idx])
            );
            nurbsSurface.SetCV(idxU, idxV, cpt);
        }
    }

    // Add surface to the model
    model.AddModelGeometryComponent(&nurbsSurface, nullptr);

    return true;
}
