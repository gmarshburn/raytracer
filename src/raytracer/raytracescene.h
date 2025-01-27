#pragma once

#include "utils/scenedata.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"

// A class representing a scene to be ray-traced

// Feel free to make your own design choices for RayTraceScene, the functions below are all optional / for your convenience.
// You can either implement and use these getters, or make your own design.
// If you decide to make your own design, feel free to delete these as TAs won't rely on them to grade your assignments.

class RayTraceScene
{
private:
    Camera camera;
    SceneGlobalData globalData;
public:

    //constructor
    RayTraceScene(int width, int height, const RenderData &metaData);

    //variables
    int m_width;
    int m_height;
    RenderData renderData;

    //ethods
    const Camera& getCamera() const;
    const SceneGlobalData &getGlobalData() const;
};
