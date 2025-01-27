#ifndef SPHERE_H
#define SPHERE_H

#include "utils/scenedata.h"
#include "raytracer/IntersectInfo.h"
#include <glm/glm.hpp>

struct Sphere
{
public:

    //constructor
    Sphere(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &new_material);

    //member variables
    glm::mat4 ctm;
    PrimitiveType type;
    SceneMaterial material;

    //methods
    IntersectInfo traceRay(glm::vec4 p, glm::vec4 d);
    float calculateT(float a, float b, float c, float discriminant, glm::vec4 pObject, glm::vec4 dObject);
};

#endif // SPHERE_H
