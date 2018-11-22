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
                    switch (geometry->ObjectType())
                    {
                    case ON::curve_object:
                        _readCurve(geometry);
                        break;
                    case ON::surface_object:
                        _readSurface(geometry);
                        break;
                    case ON::brep_object:
                        _readBrep(geometry);
                        break;
                    default:
                        break;
                    }                        
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

void _readCurve(const ON_Geometry* geometry)
{
    py::print("Succesfully read curve object!");
}

void _readSurface(const ON_Geometry* geometry)
{
    py::print("Succesfully read surface object!");
}

void _readBrep(const ON_Geometry* geometry)
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
