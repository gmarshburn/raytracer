#ifndef CYLINDER_H
#define CYLINDER_H

#include "raytracer/IntersectInfo.h"
#include "utils/scenedata.h"
#include <glm/glm.hpp>

struct Cylinder
{
public:

    //constructor
    Cylinder(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &new_material);

    //member variables
    glm::mat4 ctm;
    PrimitiveType type;
    SceneMaterial material;

    //methods
    IntersectInfo traceRay(glm::vec4 p, glm::vec4 d);
    float calculateTBody(float a, float b, float c, float discriminant, glm::vec4 pObject, glm::vec4 dObject);
    float calculateTLid(glm::vec4 pObject, glm::vec4 dObject, bool positive);
};

#endif // CYLINDER_H
