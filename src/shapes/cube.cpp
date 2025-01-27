#include "cube.h"

// A struct to model a cube and its implicit equation in the 3D world

Cube::Cube(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &new_material) {
    ctm = new_ctm;
    type = new_type;
    material = new_material;
}


IntersectInfo Cube::traceRay(glm::vec4 p, glm::vec4 d){
    
    glm::mat4 inverseCTM = inverse(ctm); //ctm is object to world, inverse is world to object

    //calcualtes p and d in object space
    glm::vec4 pObject = inverseCTM * p;
    glm::vec4 dObject = inverseCTM * d;

    float smallestT = FLT_MAX;

    //intersection with x
    smallestT = calculateT(pObject, dObject, smallestT, 0, 1, 2, true);
    smallestT = calculateT(pObject, dObject, smallestT, 0, 1, 2, false);
    //intersection with y
    smallestT = calculateT(pObject, dObject, smallestT, 1, 2, 0, true);
    smallestT = calculateT(pObject, dObject, smallestT, 1, 2, 0, false);
    //intersection with z
    smallestT = calculateT(pObject, dObject, smallestT, 2, 0, 1, true);
    smallestT = calculateT(pObject, dObject, smallestT, 2, 0, 1, false);

    normal = glm::normalize(normal);

    IntersectInfo intersectInfo = IntersectInfo{ctm, inverseCTM, normal, smallestT, material, type};
    return intersectInfo;
}


float Cube::calculateT(glm::vec4 pObject, glm::vec4 dObject, float smallestT, int currCoord, int firstToCheck, int secondToCheck, bool positive){

    float t;
    int multiplier;

    //if this is a side on the positive or negative side of an axis
    if(positive){
        t = (0.5f - pObject[currCoord]) / dObject[currCoord];
        multiplier = 1;
    } else {
        t = (-0.5f - pObject[currCoord]) / dObject[currCoord];
        multiplier = -1;
    }

    float firstCoord = pObject[firstToCheck] + (t * dObject[firstToCheck]);
    float secondCoord = pObject[secondToCheck] + (t * dObject[secondToCheck]);

    //check if the other two coordinates are in bounds given the calculated t
    if(firstCoord <= 0.5 && firstCoord >= -0.5 && secondCoord <= 0.5 && secondCoord >= -0.5){
        if(t < smallestT && t >= 0.f){
            smallestT = t;
            normal = {0.f, 0.f, 0.f, 0.f};
            normal[currCoord] = 1.f * multiplier;
        }
    }
    return smallestT;
}
