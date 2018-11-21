#ifndef RW3DM_H
#define RW3DM_H

#include <iostream>
#include <string>

#include <opennurbs_public.h>

#include "rw3dm_export.h"


bool RW3DM_EXPORT read_3dm(std::string);
bool RW3DM_EXPORT write_3dm(std::string);

#endif /* RW3DM_H */
