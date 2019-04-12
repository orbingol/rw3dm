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


bool Config::show_config()
{
    return bool(std::atoi(params.at("show_config").first.c_str()));
}


bool Config::warnings() {
    return bool(std::atoi(params.at("warnings").first.c_str()));
}

bool Config::normalize()
{
    return bool(std::atoi(params.at("normalize").first.c_str()));
}

bool Config::trims()
{
    return bool(std::atoi(params.at("trims").first.c_str()));
}

bool Config::extract_curves()
{
    return bool(std::atoi(params.at("extract_curves").first.c_str()));
}

// Parse configuration from a string
void parseConfig(char *conf_str, Config & cfg)
{
    // Define delimiters
    std::string delimiter = ";";
    std::string cfg_delimiter = "=";

    // Parse user config string
    std::string s(conf_str);
    while (true)
    {
        std::size_t pos = s.find(delimiter);
        std::string cfg_directive = s.substr(0, pos);
        std::size_t cfg_pos = cfg_directive.find(cfg_delimiter);
        if (cfg_pos != std::string::npos)
        {
            std::string key = cfg_directive.substr(0, cfg_pos);
            std::string value = cfg_directive.substr(cfg_pos + 1);
            updateConfig(key, value, cfg);
        }
        if (pos == std::string::npos)
            break;
        else
            s = s.substr(++pos);
    }
}

// Update application configuration
void updateConfig(std::string &key, std::string &value, Config &cfg)
{
    auto search = cfg.params.find(key);
    if (search != cfg.params.end())
    {
        std::string val;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "false" || value == "0")
            val = "0";
        else if (value == "true" || value == "0")
            val = "1";
        else
            val = std::string(value);
        cfg.params[key].first = val;
    }
}
