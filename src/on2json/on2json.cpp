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

#include "on2json.h"


bool on2json(std::string &fileName, Config &cfg, std::string &jsonString)
{
    // Start modeler
    initializeRwExt();

    // Try to open .3DM file
    FILE* fp = ON::OpenFile(fileName.c_str(), "rb");
    if (!fp)
    {
        if (!cfg.silent())
            std::cout << "[ERROR] Cannot open file '" << fileName << "' for reading" << std::endl;
        return false;
    }

    // Create achive object from file pointer
    ON_BinaryFile archive(ON::archive_mode::read3dm, fp);

    // Initialize a model archive object
    ONX_Model model;

    // Start reading the model archive
    unsigned int tableFilter = ON::curve_object | ON::surface_object;
    if (!model.IncrementalReadBegin(archive, true, tableFilter, (ON_TextLog *)nullptr))
    {
        if (!cfg.silent())
            std::cout << "[ERROR] Cannot start reading model archive from the file " << fileName << std::endl;
        return false;
    }

    // Create JSON data object
    Json::Value dataDef;

    // Read models
    unsigned int modelCount = 0;
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
                Json::Value data;
                if (ON::curve_object == geometry->ObjectType() && cfg.extract_curves())
                {
                    extractCurveData(geometry, cfg, data);
                }
                else
                {
                    switch (geometry->ObjectType())
                    {
                    case ON::surface_object:
                        extractSurfaceData(geometry, cfg, data);
                        break;
                    case ON::brep_object:
                        extractBrepData(geometry, cfg, data);
                        break;
                    case ON::extrusion_object:
                        extractExtrusionData(geometry, cfg, data);
                        break;
                    }
                }
                // Only add to the array if JSON output is not empty
                if (!data.empty())
                {
                    if (data.isArray())
                    {
                        for (auto d : data)
                        {
                            dataDef[modelCount] = d;
                            modelCount++;
                        }
                    }
                    else
                    {
                        dataDef[modelCount] = data;
                        modelCount++;
                    }
                }
            }
        }
    }

    // Finish reading the model archive
    if (!model.IncrementalReadFinish(archive, true, tableFilter, (ON_TextLog *)nullptr))
    {
        if (!cfg.silent())
            std::cout << "[ERROR] Cannot complete reading model archive from the file " << fileName << std::endl;
        return false;
    }

    // Close file
    ON::CloseFile(fp);

    // Stop modeler
    finalizeRwExt();

    // If no geometry was extracted, do not continue
    if (dataDef.empty())
        return false;

    // Create shape JSON object
    Json::Value shapeDef;
    shapeDef["type"] = (cfg.extract_curves()) ? "curve" : "surface";
    shapeDef["count"] = modelCount;
    shapeDef["data"] = dataDef;

    // Create root JSON object
    Json::Value root;
    root["shape"] = shapeDef;

    // Convert root JSON object into a string
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "\t";
    jsonString = Json::writeString(wbuilder, root);

    return true;
}

std::string on2json_run(std::string &fileName, Config &cfg)
{
    // Save file name
    std::string fnameSave;

    // Extract geometry data from .3dm file
    std::string jsonString;
    if (on2json(fileName, cfg, jsonString))
    {
        // Try to open a file for writing JSON string
        fnameSave = fileName.substr(0, fileName.find_last_of(".")) + ".json";
        std::ofstream fileSave(fnameSave.c_str(), std::ios::out);
        if (!fileSave)
        {
            if (!cfg.silent())
                std::cout << "[ERROR] Cannot open file '" << fnameSave << "' for writing!" << std::endl;
            fnameSave.clear();
        }
        else
        {
            // Save JSON string to the file
            fileSave << jsonString << std::endl;
            fileSave.close();
        }
    }

    return fnameSave;
}
