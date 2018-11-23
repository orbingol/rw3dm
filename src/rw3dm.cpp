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

bool writeONFile(py::list &data, std::string file_name)
{
    py::print("Write to .3DM has not been implemented yet!");
    return false;
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
                controlPoints["weights"] = weights;
            }
        }
        data["size_u"] = sizeU;
        data["size_v"] = sizeV;
        data["control_points"] = controlPoints;
    }
}
