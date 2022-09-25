//
// Created by Andrew Astakhov on 25.09.2022.
//

#ifndef RW3DM_CCC_H
#define RW3DM_CCC_H
#include "common.h"
#include "rw3dm.h"
#include <opennurbs_point.h>
double dist(const ON_3dPoint& point_a,const ON_3dPoint& point_b);

std::vector<ON_3dPoint> ReadPointIo(const int& i);

int main();
#endif //RW3DM_CCC_H
