#ifndef CUBE_H
#define CUBE_H

#include "utils/scenedata.h"
#include "raytracer/IntersectInfo.h"
#include <glm/glm.hpp>

struct Cube
{
public:

    //constructor
    Cube(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &new_material);

    //member variables
    glm::mat4 ctm;
    PrimitiveType type;
    glm::vec4 normal;
    SceneMaterial material;

    //methods
    IntersectInfo traceRay(glm::vec4 p, glm::vec4 d);
    float calculateT(glm::vec4 pObject, glm::vec4 dObject, float smallestT, int currCoord, int firstToCheck, int secondToCheck, bool positive);
};

#endif // CUBE_H
