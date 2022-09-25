//
// Created by Andrew Astakhov on 25.09.2022.
//
#include "iostream"
#include "vector"
#include <opennurbs_public.h>
#include <opennurbs_point.h>

double dist(const ON_3dPoint& point_a,const ON_3dPoint& point_b){

    return point_a.DistanceTo(point_b);
}



std::vector<double> SplitDoubles(std::string& text) {
    std::vector<double> words;
    std::string word;

    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                // convert string to double
                double num_double = std::stod(word);
                words.push_back(num_double);
                word.clear();
            }
        } else {

            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(0.0);
    }

    return words;
}
ON_3dPoint VectorToON_3dPoint(const std::vector<double>& point){
    return ON_3dPoint(point[0], point[1], point[2]);
}
std::vector<ON_3dPoint> ReadPointIo(const int& count) {


    std::vector<ON_3dPoint> points;



    for (int i = 0; i < count; ++i) {
        std::string input;
        std::getline(std::cin, input);

        points.push_back(VectorToON_3dPoint(SplitDoubles(input)));

    }

    return points;
}