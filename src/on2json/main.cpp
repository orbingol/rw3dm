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

#include "common.h"
#include "on2json.h"


// RW3DM executable
int main(int argc, char **argv)
{
    // Print app information
    std::cout << "ON2JSON: Geometry Extractor for Rhino/OpenNURBS"
        << " (RW3DM v" << RW3DM_VERSION_MAJOR << "."
        << RW3DM_VERSION_MINOR << "."
        << RW3DM_VERSION_PATCH << ")"
        << std::endl;
    std::cout << "OpenNURBS version: "
        << ON::VersionQuartetAsString()
        << std::endl;
    std::cout << std::endl;

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

    // Convert .3dm file to geomdl .json file
    std::string output = on2json_run(filename, cfg);

    // If converter returns an empty string, it means a failure
    if (output.empty())
    {
        std::cout << "[ERROR] Geometry data was NOT extracted successfully" << std::endl;
        return EXIT_FAILURE;
    }

    // Print success message
    std::cout << "[SUCCESS] Geometry data was extracted to file '" << output << "' successfully" << std::endl;
    return EXIT_SUCCESS;
}
