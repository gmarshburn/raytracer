#ifndef LIGHT_H
#define LIGHT_H


#include "glm/fwd.hpp"
#include "raytracer/IntersectInfo.h"
#include "raytracer/TextureInfo.h"
#include "utils/sceneparser.h"
class Light
{
public:

    //constructor
    Light();

    //member variables
    std::unordered_map <std::string, TextureInfo> filenameToTextureInfo;
    RenderData renderData;
    SceneGlobalData globalData;

    //methods
    glm::vec3 phong(glm::vec4 intersection, glm::vec3  normal, glm::vec4 directionToCamera, SceneMaterial &material,
                    std::vector<SceneLightData> &lights, glm::vec3 textureColor, float blend);
    glm::vec3 getLighting(glm::vec3 intersectToLight, bool hasAttenuation, glm::vec3 intersection, glm::vec3 normal,
                          glm::vec4 directionToCamera, SceneMaterial &material, SceneLightData light, glm::vec3 textureColor, float blend);
    glm::vec3 addFalloff(SceneLightData &light, glm::vec4 intersection, glm::vec3 illumination);
    bool getIsShadowed(glm::vec4 intersection, glm::vec4 direction, LightType type);
    glm::vec3 getTextureColor(glm::vec4 intersection, IntersectInfo infoFinal, std::unordered_map <std::string, TextureInfo> map,
                              RenderData newRenderData, SceneGlobalData newGlobalData);
    glm::vec2 getCubeCR(glm::vec4 intersection, IntersectInfo info, float m, float n);
    glm::vec2 getCylinderConeCR(glm::vec4 intersection, IntersectInfo info, float m, float n);
    glm::vec2 getSphereCR(glm::vec4 intersection, IntersectInfo info, float m, float n);
};

#endif // LIGHT_H
