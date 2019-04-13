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


// JSON2ON executable
int main(int argc, char **argv)
{
    // Print app information
    std::cout << "JSON2ON: Spline Geometry Converter for Rhino/OpenNURBS" << std::endl;
    std::cout << "Copyright (c) 2019 IDEA Lab at Iowa State University. " << "Licensed under the terms of the MIT License.\n" << std::endl;

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
        std::cout << "\nExample: " << argv[0] << " my_file.json warnings=true" << std::endl;
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

    // Open JSON file
    std::ifstream fp(filename);
    if (!fp)
    {
        std::cout << "[ERROR]: Cannot open file '" << filename << "' for reading" << std::endl;
        return EXIT_FAILURE;
    }

    // Fill the JSON object
    Json::Value root;
    fp >> root;

    // Start OpenNURBS
    initializeRwExt();

    // Create model
    ONX_Model model;

    // Read shape data from JSON
    for (auto d : root["shape"]["data"])
    {
        if (root["shape"]["type"].asString() == "curve")
            constructCurveData(d, cfg, model);
        if (root["shape"]["type"].asString() == "surface")
            constructSurfaceData(d, cfg, model);
    }

    // Write model to the file (version = 50)
    std::string fnameSave = filename.substr(0, filename.find_last_of(".")) + ".3dm";
    if (model.Write(fnameSave.c_str(), 50))
    {
        // Print success message
        std::cout << "[SUCCESS] Geometry data was saved to file '" << fnameSave << "' successfully" << std::endl;
    }

    // Stop OpenNURBS
    finalizeRwExt();

    // Exit successfully
    return EXIT_SUCCESS;
}
