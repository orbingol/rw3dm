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
    // Create log object
    ON_TextLog error_log;

    // Read file
    FILE* fp = ON::OpenFile(file_name.c_str(), "rb");
    if (!fp)
    {
        py::print("Cannot open file");
        return false;
    }

    // Create achive object from file pointer
    ON_BinaryFile archive(ON::archive_mode::read3dm, fp);

    ONX_Model model;
    unsigned int tableFilter = ON::curve_object | ON::surface_object;
    bool isModelReadStart = model.IncrementalReadBegin(archive, true, tableFilter, &error_log);

    // TO-DO: Print error log here

    if (isModelReadStart)
    {
        ON_ModelComponentReference mCompRef;
        while (model.IncrementalReadModelGeometry(archive, true, true, true, 0, mCompRef))
        {
            // Check if there are any models to read
            if (mCompRef.IsEmpty())
                break;
            else
            {
                const ON_ModelGeometryComponent &geometryComp = model.ModelGeometryComponentFromId(mCompRef.ModelComponentId());
                const ON_Geometry *geometry = geometryComp.Geometry((ON_Geometry *)nullptr);
                if (geometry != nullptr)
                {
                    py::dict contentDict;
                    switch (geometry->ObjectType())
                    {
                    case ON::curve_object:
                        _readCurve(geometry, contentDict);
                        break;
                    case ON::surface_object:
                        _readSurface(geometry, contentDict);
                        break;
                    case ON::brep_object:
                        _readBrep(geometry, data);
                        break;
                    default:
                        break;
                    }
                    if (contentDict.size() > 0)
                        data.append(contentDict);
                }
            }
        }
    }

    bool isModelReadFinish = model.IncrementalReadFinish(archive, true, tableFilter, &error_log);

    // TO-DO: Print error log here

    // close the file
    ON::CloseFile(fp);

    return true;
}

bool writeONFile(py::list &data, std::string file_name)
{
    return true;
}

void _readCurve(const ON_Geometry* geometry, py::dict &data)
{
    // We know that "geometry" is a curve object
    ON_Curve *geoCurve = (ON_Curve *)geometry;

    // Try to get the NURBS form of the curve object
    ON_NurbsCurve nurbsCurve;
    if (geoCurve->NurbsCurve(&nurbsCurve))
    {
        // Get degree
        data["degree"] = nurbsCurve.Degree();

        // Get knot vector
        py::list knotVector;
        for (int idx = 0; idx < nurbsCurve.KnotCount(); idx++)
        {
            knotVector.append(nurbsCurve.Knot(idx));
        }
        data["knotvector"] = knotVector;

        py::dict controlPoints;

        // Get control points
        int num_coords = nurbsCurve.IsRational() ? nurbsCurve.CVSize() - 1 : nurbsCurve.CVSize();
        py::list points;
        for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
        {
            double *vertex = nurbsCurve.CV(idx);
            py::list point;
            for (int c = 0; c < num_coords; c++)
            {
                point.append(vertex[c]);
            }
            points.append(point);
        }
        controlPoints["points"] = points;

        // Check if the NURBS curve is rational, i.e. weights exist as the last row of control points
        if (nurbsCurve.IsRational())
        {
            // Get weights
            py::list weights;
            for (int idx = 0; idx < nurbsCurve.CVCount(); idx++)
            {
                double *vertex = nurbsCurve.CV(idx);
                weights.append(vertex[num_coords - 1]);
            }
            controlPoints["weights"] = weights;
        }
        data["control_points"] = controlPoints;
    }
    //py::print("Succesfully read curve object!");
}

void _readSurface(const ON_Geometry* geometry, py::dict &data)
{
    py::print("Succesfully read surface object!");
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
            py::print("Succesfully read brep face surface!");
        }
        brepFaceIndex++;
    }
}
