#include "light.h"
#include "glm/fwd.hpp"
#include "raytracer/IntersectInfo.h"
#include "raytracer/TextureInfo.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "utils/scenedata.h"
#include "utils/sceneparser.h"

Light::Light(){}


glm::vec3 Light::phong(glm::vec4 intersection, glm::vec3 normal, glm::vec4 directionToCamera, SceneMaterial &material,
                       std::vector<SceneLightData> &lights, glm::vec3 textureColor, float blend) {

    glm::vec3 illumination(0.f, 0.f, 0.f);

    //add the ambient term
    illumination += glm::vec3(material.cAmbient) * globalData.ka;

    for (SceneLightData &light : lights) {

        LightType type = light.type;

        switch(type){

        case LightType::LIGHT_DIRECTIONAL:
        {

            glm::vec4 intersectToLight = -light.dir;
            intersectToLight = glm::normalize(intersectToLight);
            if(getIsShadowed(intersection + (0.001f * glm::vec4(normal, 0.f)), intersectToLight, LightType::LIGHT_DIRECTIONAL)){
                continue;
            }

            illumination += getLighting(intersectToLight, false, intersection, normal, directionToCamera, material, light, textureColor, blend);

            break;
        }

        case LightType::LIGHT_POINT:
        {
            glm::vec4 intersectToLight = light.pos - intersection;
            intersectToLight = glm::normalize(intersectToLight);
            if(getIsShadowed(intersection + (0.001f * glm::vec4(normal, 0.f)), intersectToLight, LightType::LIGHT_POINT)){
                continue;
            }

            illumination += getLighting(intersectToLight, true, intersection, normal, directionToCamera, material, light, textureColor, blend);
            break;
        }

        case LightType::LIGHT_SPOT:{

            glm::vec4 intersectToLight = light.pos - intersection;
            intersectToLight = glm::normalize(intersectToLight);
            if(getIsShadowed(intersection + (0.001f * glm::vec4(normal, 0.f)), intersectToLight, LightType::LIGHT_SPOT)){
                continue;
            }

            glm::vec3 spotLight = getLighting(intersectToLight, true, intersection, normal, directionToCamera, material, light, textureColor, blend);
            illumination += addFalloff(light, intersection, spotLight);
            break;
        }
        }
    }

    return illumination;
}


glm::vec3 Light::getLighting(glm::vec3 intersectToLight, bool hasAttenuation, glm::vec3 intersection, glm::vec3 normal,
                             glm::vec4 bigDirectionToCamera, SceneMaterial &material, SceneLightData light, glm::vec3 textureColor, float blend){

    glm::vec3 directionToCamera = glm::vec3(bigDirectionToCamera);
    glm::vec3 illumination(0.f, 0.f, 0.f);

    //attenuation factor
    float attenuation = 1.f;
    if(hasAttenuation){
        float distance = glm::length(glm::vec3(light.pos) - intersection);
        attenuation = fmin(1.f, 1.f/(light.function.x + (light.function.y * distance) + (light.function.z * distance * distance)));
    }

    //diffuse term
    float angle = glm::dot(normal, intersectToLight);

    if(angle > 0){
        textureColor.x = (textureColor.x / 255.f) * blend;
        textureColor.y = (textureColor.y / 255.f) * blend;
        textureColor.z = (textureColor.z / 255.f) * blend;

        illumination.x += ((globalData.kd * material.cDiffuse.x * (1.f - blend)) + textureColor.x) * attenuation * light.color.x * angle;
        illumination.y += ((globalData.kd * material.cDiffuse.y * (1.f - blend)) + textureColor.y) * attenuation * light.color.y * angle;
        illumination.z += ((globalData.kd * material.cDiffuse.z * (1.f - blend)) + textureColor.z) * attenuation * light.color.z * angle;
    }

    //specular term
    glm::vec3 reflectedLight = glm::reflect(-intersectToLight, normal);
    reflectedLight = glm::normalize(reflectedLight);
    float new_angle = glm::dot(reflectedLight, directionToCamera);
    float exp = glm::pow(new_angle, material.shininess);

    if(new_angle > 0){
        illumination.x += attenuation * light.color.x * globalData.ks * material.cSpecular.x * exp;
        illumination.y += attenuation * light.color.y * globalData.ks * material.cSpecular.y * exp;
        illumination.z += attenuation * light.color.z * globalData.ks * material.cSpecular.z * exp;
    }

    return illumination;
}


glm::vec3 Light::addFalloff(SceneLightData &light, glm::vec4 intersection, glm::vec3 illumination){

    float mainAngle = light.angle;
    float innerAngle = mainAngle - light.penumbra;
    glm::vec3 lightToIntersection = glm::vec3(intersection) - glm::vec3(light.pos);
    lightToIntersection = glm::normalize(lightToIntersection);
    glm::vec3 lightDirection = glm::normalize(light.dir);
    float currAngle = glm::acos(glm::dot(lightDirection, lightToIntersection)); //angle between main vector of light and vector from light to object

    if(currAngle > innerAngle && currAngle <= mainAngle){
        float innerTerm = (currAngle - innerAngle) / (mainAngle - innerAngle);
        float falloff = (-2.f * innerTerm * innerTerm * innerTerm) + (3.f * innerTerm * innerTerm);
        illumination.x *= 1.f - falloff;
        illumination.y *= 1.f - falloff;
        illumination.z *= 1.f - falloff;
    } else if(currAngle > mainAngle){
        illumination.x = 0.f;
        illumination.y = 0.f;
        illumination.z = 0.f;
    }

    return illumination;
}


bool Light::getIsShadowed(glm::vec4 intersection, glm::vec4 direction, LightType type){

    float smallestT = FLT_MAX;
    IntersectInfo intersectInfo;

    //loop over each object in the scene
    for(RenderShapeData shapeData : renderData.shapes){

        PrimitiveType type = shapeData.primitive.type;

        //based on what type of shape we're looking at, check if the ray intersects it
        switch(type){
        case PrimitiveType::PRIMITIVE_CONE:
        {
            Cone cone = Cone{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = cone.traceRay(intersection, direction);
            break;
        }

        case PrimitiveType::PRIMITIVE_CUBE:
        {
            Cube cube = Cube{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = cube.traceRay(intersection, direction);
            break;
        }

        case PrimitiveType::PRIMITIVE_CYLINDER:
        {
            Cylinder cylinder = Cylinder{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = cylinder.traceRay(intersection, direction);
            break;
        }

        case PrimitiveType::PRIMITIVE_SPHERE:
        {
            Sphere sphere = Sphere{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = sphere.traceRay(intersection, direction);
            break;
        }

        case PrimitiveType::PRIMITIVE_MESH:
            break;
        }

        //if the t that was just calculated is the smallest seen
        //yet, save that and its intersectInfo as our smallest
        if(intersectInfo.t < smallestT){
            smallestT = intersectInfo.t;
        }
    }

    if(type == LightType::LIGHT_DIRECTIONAL && smallestT < FLT_MAX){
        return true;
    } else if(smallestT < FLT_MAX && smallestT < glm::length(direction)){
        return true;
    } else return false;
}


glm::vec3 Light::getTextureColor(glm::vec4 intersection, IntersectInfo infoFinal, std::unordered_map <std::string, TextureInfo> map, RenderData newRenderData, SceneGlobalData newGlobalData){

    renderData = newRenderData;
    globalData = newGlobalData;
    filenameToTextureInfo = map;

    glm::vec3 textureColor = {255, 255, 255};
    if(infoFinal.material.blend != 0){

        int c;
        int r;
        switch(infoFinal.type){

        case PrimitiveType::PRIMITIVE_CONE:
        {
            glm::vec2 coneCR = getCylinderConeCR(intersection, infoFinal, infoFinal.material.textureMap.repeatU,
                                                 infoFinal.material.textureMap.repeatV);
            c = coneCR.x;
            r = coneCR.y;
            break;
        }

        case PrimitiveType::PRIMITIVE_CUBE:
        {
            glm::vec2 cubeCR = getCubeCR(intersection, infoFinal, infoFinal.material.textureMap.repeatU,
                                         infoFinal.material.textureMap.repeatV);
            c = cubeCR.x;
            r = cubeCR.y;
            break;
        }

        case PrimitiveType::PRIMITIVE_CYLINDER:
        {
            glm::vec2 cylinderCR = getCylinderConeCR(intersection, infoFinal, infoFinal.material.textureMap.repeatU,
                                                     infoFinal.material.textureMap.repeatV);
            c = cylinderCR.x;
            r = cylinderCR.y;
            break;
        }

        case PrimitiveType::PRIMITIVE_SPHERE:
        {
            glm::vec2 sphereCR = getSphereCR(intersection, infoFinal, infoFinal.material.textureMap.repeatU,
                                             infoFinal.material.textureMap.repeatV);
            c = sphereCR.x;
            r = sphereCR.y;
            break;
        }

        case PrimitiveType::PRIMITIVE_MESH:
        {
            break;
        }

        }

        if(infoFinal.material.textureMap.filename != ""){
            TextureInfo textureInfo = filenameToTextureInfo[infoFinal.material.textureMap.filename];
            RGBA color = textureInfo.rgba[(r * textureInfo.width) + c];
            textureColor.x = color.r;
            textureColor.y = color.g;
            textureColor.z = color.b;
        } else {
            textureColor = {0.f, 0.f, 0.f};
        }

    }
    return textureColor;
}


glm::vec2 Light::getCubeCR(glm::vec4 intersection, IntersectInfo info, float m, float n){

    glm::vec4 intersectionObject = info.inverseCTM * intersection;
    float u;
    float v;

    float epsilon = 0.001f;
    if(intersectionObject.x <= -0.5f + epsilon){
        u = intersectionObject.z + 0.5;
        v = intersectionObject.y + 0.5;
    } else if (intersectionObject.x >= 0.5f - epsilon){
        u = 0.5f - intersectionObject.z;
        v = 0.5f + intersectionObject.y;
    } else if(intersectionObject.y <= -0.5f + epsilon){
        u = intersectionObject.x + 0.5f;
        v = intersectionObject.z + 0.5f;
    } else if(intersectionObject.y >= 0.5f - epsilon){
        u = 0.5f + intersectionObject.x;
        v = 0.5f - intersectionObject.z;
    } else if(intersectionObject.z <= -0.5f + epsilon){
        u = -intersectionObject.x + 0.5f;
        v = intersectionObject.y + 0.5f;
    } else if(intersectionObject.z >= 0.5f - epsilon){
        u = 0.5f + intersectionObject.x;
        v = 0.5f + intersectionObject.y;
    }

    int width = filenameToTextureInfo[info.material.textureMap.filename].width;
    int height = filenameToTextureInfo[info.material.textureMap.filename].height;
    int c = ((int)std::floor(u * m * width) % width);
    int r = (int)std::floor((1 - v) * n * height) % height;
    return glm::vec2{c, r};
}

glm::vec2 Light::getCylinderConeCR(glm::vec4 intersection, IntersectInfo info, float m, float n){

    glm::vec4 intersectionObject = info.inverseCTM * intersection;
    float u;
    float v;
    if(info.normal.y == 1.f){
        u = intersectionObject.x + 0.5f;
        v = -intersectionObject.z + 0.5f;
    } else if(info.normal.y == -1.f){
        u = 0.5f + intersectionObject.x;
        v = 0.5f + intersectionObject.z;
    } else {
        float theta = atan2(intersectionObject.z, intersectionObject.x);
        if(theta < 0){
            u = -theta / (2.f * M_PI);
        } else {
            u = 1.f - (theta / (2.f * M_PI));
        }
        v = intersectionObject.y + 0.5f;
    }

    int width = filenameToTextureInfo[info.material.textureMap.filename].width;
    int height = filenameToTextureInfo[info.material.textureMap.filename].height;
    int c = (int)std::floor(u * m * width);
    if(u == 1.f){
        c = c - 1.f;
    }
    c = c % width;

    int r = (int)std::floor((1 - v) * n * height);
    if(v == 1.f){
        r = r - 1.f;
    }
    r = r  % height;

    return glm::vec2{c, r};

}

glm::vec2 Light::getSphereCR(glm::vec4 intersection, IntersectInfo info, float m, float n){

    glm::vec4 intersectionObject = info.inverseCTM * intersection;
    float u = 0;
    float v = 0;
    if(info.normal.y == 0.5f){
        u = 0.5f;
        v = 1.f;
    } else if(info.normal.y == -0.5f){
        u = 0.5f;
        v = -1.f;
    } else {
        float theta = std::atan2(intersectionObject.z, intersectionObject.x);
        if(theta < 0){
            u = (-theta) / (2.f * M_PI);
        } else {
            u = 1.f - (theta / (2.f * M_PI));
        }

        float phi = std::asin(intersectionObject.y / 0.5f);
        v = (phi / M_PI) + 0.5f;
        if(v == 0.f || v == 1.f){
            v = 0.5f;
        }
    }

    int width = filenameToTextureInfo[info.material.textureMap.filename].width;
    int height = filenameToTextureInfo[info.material.textureMap.filename].height;
    int c = (int)std::floor(u * m * width);
    if(u == 1.f){
        c = c - 1.f;
    }
    c = c % width;

    int r = (int)std::floor((1 - v) * n * height);
    if(v == 1.f){
        r = r - 1.f;
    }
    r = r  % height;
    //std::cout << "c: " << c << ", r: " << r << std::endl;
    return glm::vec2{c, r};
}
