/*
Copyright (c) 2019 Onur Rauf Bingol

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

#include "common.h"
#include "rw3dm.h"


// RW3DM executable
int main(int argc, char **argv)
{
    // Print app information
    std::cout << "ON2JSON: Spline Geometry Extractor for Rhino/OpenNURBS" << std::endl;
    std::cout << "Copyright (c) 2019 IDEA Lab at Iowa State University." << std::endl;
    std::cout << "Licensed under the terms of the MIT License.\n" << std::endl;

    // File name to read
    std::string filename;

    // Initialize configuration
    Config cfg;

    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: " << argv[0] << " FILENAME OPTIONS\n" << std::endl;
        std::cout << "Available options:" << std::endl;
        for (auto p : cfg.params)
            std::cout << "  - " << p.first << ": " << p.second.second << std::endl;
        std::cout << "\nExample: " << argv[0] << " my_file.3dm normalize=false;trims=true" << std::endl;
        return EXIT_FAILURE;
    }
    else
        filename = std::string(argv[1]);

    // Update configuration
    if (argc == 3)
        parseConfig(argv[2], cfg);

    // Print configuration
    if (cfg.show_config())
    {
        std::cout << "Using configuration:" << std::endl;
        for (auto p : cfg.params)
            std::cout << "  - " << p.first << ": " << p.second.first << std::endl;
    }

    // Start OpenNURBS
    initializeRwExt();

    // Try to open .3DM file
    FILE* fp = ON::OpenFile(filename.c_str(), "rb");
    if (!fp)
    {
        std::cout << "[ERROR]: Cannot open file '" << filename << "' for reading" << std::endl;
        return EXIT_FAILURE;
    }

    // Create achive object from file pointer
    ON_BinaryFile archive(ON::archive_mode::read3dm, fp);

    // Initialize a model archive object
    ONX_Model model;

    // Start reading the model archive
    unsigned int tableFilter = ON::curve_object | ON::surface_object;
    if (!model.IncrementalReadBegin(archive, true, tableFilter, (ON_TextLog *)nullptr))
    {
        std::cout << "[ERROR]: Cannot start reading model archive from the file " << filename << std::endl;
        return EXIT_FAILURE;
    }

    // Create JSON root object
    Json::Value root;
    
    // Create JSON shape object
    Json::Value shapeDef;
    if (cfg.extract_curves())
        shapeDef["type"] = "curve";
    else
        shapeDef["type"] = "surface";

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
                    }
                }
                dataDef[modelCount] = data;
                modelCount++;
            }
        }
    }

    // Finish reading the model archive
    if (!model.IncrementalReadFinish(archive, true, tableFilter, (ON_TextLog *)nullptr))
    {
        std::cout << "ERROR: Cannot complete reading model archive from the file " << filename << std::endl;
        return EXIT_FAILURE;
    }

    // Close file
    ON::CloseFile(fp);

    // Stop OpenNURBS
    finalizeRwExt();

    // Update root JSON object
    shapeDef["count"] = modelCount;
    shapeDef["data"] = dataDef;
    root["shape"] = shapeDef;

    // Try to open JSON file for writing
    std::string fnameSave = filename.substr(0, filename.find_last_of(".")) + ".json";
    std::ofstream fileSave(fnameSave.c_str(), std::ios::out);
    if (!fileSave)
    {
        std::cerr << "[ERROR] Cannot open file '" << fnameSave << "' for writing!" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        // Convert JSON data structure into a string
        Json::StreamWriterBuilder wbuilder;
        wbuilder["indentation"] = "\t";
        std::string jsonDocument = Json::writeString(wbuilder, root);

        // Write JSON string to a file
        fileSave << jsonDocument << std::endl;
        fileSave.close();

        // Print success message
        std::cout << "[SUCCESS] Geometry data was extracted to file '" << fnameSave << "' successfully" << std::endl;
    }

    // Exit successfully
    return EXIT_SUCCESS;
}
