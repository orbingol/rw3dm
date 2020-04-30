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

#ifndef COMMON_H
#define COMMON_H

// C++ includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <exception>
#include <cstddef>
#include <cstdio>
#include <cmath>

// rw3dm configuration
#include "rw3dmConfig.h"


// Application configuration
struct Config {
    // Config parameters
    std::map< std::string, std::pair<std::string, std::string> > params = {
        { "show_config", { "0", "Print the configuration" } },
        { "silent", { "0", "Disable all printed messages" } },
        { "normalize", { "1", "Normalize knot vectors and scale trim curves to [0,1] domain" } },
        { "trims", { "1", "Extract trim curves" } },
        { "sense", { "1", "Extract surface and trim curve direction w.r.t. the face" } },
        { "extract_curves", { "0", "Extract curves (Default is extract surfaces)" } }
    };

    // Methods
    bool show_config() {
        return bool(std::atoi(params.at("show_config").first.c_str()));
    };
    bool silent() {
        return bool(std::atoi(params.at("silent").first.c_str()));
    };
    bool normalize() {
        return bool(std::atoi(params.at("normalize").first.c_str()));
    };
    bool trims() {
        return bool(std::atoi(params.at("trims").first.c_str()));
    };
    bool sense() {
        return bool(std::atoi(params.at("sense").first.c_str()));
    };
    bool extract_curves() {
        return bool(std::atoi(params.at("extract_curves").first.c_str()));
    };
};

// Function prototypes
void parseConfig(char *, Config &);
void updateConfig(std::string &, std::string &, Config &);

#endif /* COMMON_H */
