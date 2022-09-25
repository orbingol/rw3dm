//
// Created by Andrew Astakhov on 25.09.2022.
//

#include "iostream"
#include "ccc.h"



int main(){
    auto points= ReadPointIo(2);


    std::cout << std::to_string(dist(points[0], points[1])) <<std::endl;

}
