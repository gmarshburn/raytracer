#ifndef INTERSECTINFO_H
#define INTERSECTINFO_H

#include "utils/scenedata.h"
#include <glm/glm.hpp>

// A Struct that contains all of the info that a shape needs to return
// into RayTracer when there's an intersection with the shpae and the ray

struct IntersectInfo{
    glm::mat4 ctm;
    glm::mat4 inverseCTM;
    glm::vec4 normal;
    float t;
    SceneMaterial material;
    PrimitiveType type;
    int shapeListIndex;
};

#endif // INTERSECTINFO_H
