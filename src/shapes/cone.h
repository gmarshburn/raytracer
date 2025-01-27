#ifndef CONE_H
#define CONE_H

#include "utils/scenedata.h"
#include "raytracer/IntersectInfo.h"
#include <glm/glm.hpp>

struct Cone
{
public:

    //constructor
    Cone(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &material);

    //member variables
    glm::mat4 ctm;
    PrimitiveType type;
    SceneMaterial material;

    //methods
    IntersectInfo traceRay(glm::vec4 p, glm::vec4 d);
    float calculateTBody(float a, float b, float c, float discriminant, glm::vec4 pObject, glm::vec4 dObject);
    float calculateTLid(glm::vec4 pObject, glm::vec4 dObject);
};

#endif // CONE_H
