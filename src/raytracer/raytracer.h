#pragma once

#include <glm/glm.hpp>
#include "QtCore/qstring.h"
#include "camera/camera.h"
#include "raytracer/TextureInfo.h"
#include "raytracer/raytracescene.h"
#include "utils/rgba.h"
#include "utils/scenedata.h"
#include "utils/sceneparser.h"

// A forward declaration for the RaytraceScene class

class RayTraceScene;

// A class representing a ray-tracer

class RayTracer
{
public:
    struct Config {
        bool enableShadow        = false;
        bool enableReflection    = false;
        bool enableRefraction    = false;
        bool enableTextureMap    = false;
        bool enableTextureFilter = false;
        bool enableParallelism   = false;
        bool enableSuperSample   = false;
        bool enableAcceleration  = false;
        bool enableDepthOfField  = false;
        int maxRecursiveDepth    = 4;
        bool onlyRenderNormals   = false;
    };

public:

    //constructor
    RayTracer(Config config);

    //member variables
    Camera camera;
    std::unordered_map <std::string, TextureInfo> filenameToTextureInfo;
    RenderData renderData;
    SceneGlobalData globalData;

    //methods
    void render(RGBA *imageData, const RayTraceScene &scene);
    glm::vec4 getDistance(int i, int j, int k, const RayTraceScene &scene);
    glm::vec3 castRay(glm::vec4 pWorld, glm::vec4 dWorld, int max_depth, int curr_depth);
    TextureInfo loadTextureFromFile(const QString &file);
    void storeTextures(const RayTraceScene &scene);
    RGBA toRGBA(glm::vec3 &illumination);


private:
    const Config m_config;
};

