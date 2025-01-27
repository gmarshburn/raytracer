#include "cone.h"

// A struct to model a cone and its implicit equation in the 3D world

Cone::Cone(glm::mat4 new_ctm, PrimitiveType new_type, SceneMaterial &new_material){
    ctm = new_ctm;
    type = new_type;
    material = new_material;
}


IntersectInfo Cone::traceRay(glm::vec4 p, glm::vec4 d){
    
    glm::mat4 inverseCTM = inverse(ctm); //ctm is object to world, inverse is world to object
    
    //calcualtes p and d in object space
    glm::vec4 pObject = inverseCTM * p;
    glm::vec4 dObject = inverseCTM * d;
    
    //bounded cone intersection formula
    float a = (dObject.x * dObject.x) + (dObject.z * dObject.z)  - ((dObject.y * dObject.y) / 4.f);
    float b = (2.f * pObject.x * dObject.x) + (2.f * pObject.z * dObject.z) - ((pObject.y * dObject.y) / 2.f) + (dObject.y / 4.f);
    float c = (pObject.x * pObject.x) + (pObject.z * pObject.z) - ((pObject.y * pObject.y) / 4.f) + (pObject.y / 4.f) - (1.f / 16.f);
    float discriminant = (b * b) - (4.f * a * c);
    float tCone = calculateTBody(a, b, c, discriminant, pObject, dObject);
    float tLid = calculateTLid(pObject, dObject);

    //get the smallest of all possible intersections and its normal
    float t = tCone;
    glm::vec4 normal = {2.f * (pObject.x + (t * dObject.x)),
                        -0.5f * (pObject.y + (t * dObject.y) - 0.5f),
                        2.f * (pObject.z + (t * dObject.z)), 0.f};
    if(tLid < t){
        t = tLid;
        normal = {0.f, -1.f, 0.f, 0.f};
    }

    normal = glm::normalize(normal);

    IntersectInfo intersectInfo = IntersectInfo{ctm, inverseCTM, normal, t, material, type};
    return intersectInfo;
}


float Cone::calculateTBody(float a, float b, float c, float discriminant, glm::vec4 pObject, glm::vec4 dObject){
    
    float t = FLT_MAX;

    //if we have only one or two real roots
    if(discriminant >= 0){
        
        //calculate the t that would get us this intersection
        float tPlus = ((-b + sqrt(discriminant)) / (2.f * a));
        float tMinus = ((-b - sqrt(discriminant)) / (2.f * a));

        //plug each t into the y coordinate
        float yPlus = pObject.y + (tPlus * dObject.y);
        float yMinus = pObject.y + (tMinus * dObject.y);

        //if neither ys are in range, t remains at infinity
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
        } else if((yPlus >= -0.5f && yPlus <= 0.5f) && (yMinus >= -0.5f && yMinus <= 0.5f)){
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


float Cone::calculateTLid(glm::vec4 pObject, glm::vec4 dObject){

    //calculate the intersection with the base plane and make its within the bound of the cone's circle base
    float ty = (-0.5 - pObject.y) / dObject.y;
    float baseIntersection = ((pObject.x + (ty * dObject.x)) * (pObject.x + (ty * dObject.x))) +
                             ((pObject.z + (ty * dObject.z)) * (pObject.z + (ty * dObject.z))) - 0.25f;
    float t = FLT_MAX;

    if(baseIntersection <= 0.f && ty >= 0.f){
        t = ty;
    }

    return t;
}
