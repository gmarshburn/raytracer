#include "sphere.h"

// A struct to model a sphere and its implicit equation in the 3D world

Sphere::Sphere(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &new_material){
    ctm = new_ctm;
    type = new_type;
    material = new_material;
}

IntersectInfo Sphere::traceRay(glm::vec4 p, glm::vec4 d){

    glm::mat4 inverseCTM = inverse(ctm); //ctm is object to world, inverse is world to object

    //calcualtes p and d in object space
    glm::vec4 pObject = inverseCTM * p;
    glm::vec4 dObject = inverseCTM * d;

    //bounded cone intersection formula
    float a = (dObject.x * dObject.x) + (dObject.y * dObject.y) + (dObject.z * dObject.z);
    float b = (2 * pObject.x * dObject.x) + (2 * pObject.y * dObject.y) + (2 * pObject.z * dObject.z);
    float c = (pObject.x * pObject.x) + (pObject.y * pObject.y) + (pObject.z * pObject.z) - 0.25f;
    float discriminant = (b * b) - (4 * a * c);
    float t =  calculateT(a, b, c, discriminant, pObject, dObject);

    glm::vec3 normalBefore = {2 * (pObject.x + (t * dObject.x)), 2 * (pObject.y + (t * dObject.y)), 2 * (pObject.z + (t * dObject.z))};
    glm::vec4 normal = glm::vec4(glm::normalize(normalBefore), 0.f);

    IntersectInfo intersectInfo = IntersectInfo{ctm, inverseCTM, normal, t, material, type};
    return intersectInfo;
}


float Sphere::calculateT(float a, float b, float c, float discriminant, glm::vec4 pObject, glm::vec4 dObject){

    float t = FLT_MAX;
    // if we have one or two real roots
    if(discriminant >= 0){

        //calculate the t that would get us this intersection
        float tPlus = ((-b + sqrt(discriminant)) / (2.f*a));
        float tMinus = ((-b - sqrt(discriminant)) / (2.f*a));

        //plug each t into the y coordinate
        float yPlus = pObject.y + (tPlus * dObject.y);
        float yMinus = pObject.y + (tMinus * dObject.y);

        //if neither are in range, t remains at infinity
        if(!(yPlus >= -0.5f && yPlus <= 0.5f) && !(yMinus >= -0.5f && yMinus <= 0.5f)){
            t = FLT_MAX;

        //if yPlus is in range and yMinus isn't, set t to yPlus
        } else if((yPlus >= -0.5f && yPlus <= 0.5f) && !(yMinus >= -0.5f && yMinus <= 0.5f)){
            if(tPlus >= 0){
                t = tPlus;
            }

        //if yMinus is in range and yPlus isn't, set t to yMinus
        } else if(!(yPlus >= -0.5f && yPlus <= 0.5f) && (yMinus >= -0.5f && yMinus <= 0.5f)){
            if(tMinus >= 0){
                t = tMinus;
            }

        //if both are in range, set t to the minimum of both
        } else {
            if(tPlus >= 0 && tMinus >= 0){
                t = std::fmin(tPlus, tMinus);
            } else if (tPlus >= 0 && tMinus < 0){
                t = tPlus;
            } else if(tPlus < 0 && tMinus >= 0){
                t = tMinus;
            } else {
                t = FLT_MAX;
            }
        }
    }

    return t;
}
