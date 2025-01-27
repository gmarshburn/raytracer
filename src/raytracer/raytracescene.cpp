#include <stdexcept>
#include "raytracescene.h"
#include "utils/sceneparser.h"
#include <iostream>

RayTraceScene::RayTraceScene(int width, int height, const RenderData &metaData) {
    m_width = width;
    m_height = height;
    renderData = metaData;
    this->globalData = metaData.globalData;

    //sets up the camera for the scene
    SceneCameraData cameraData = metaData.cameraData;
    camera.setup(cameraData.pos, cameraData.look, cameraData.up, cameraData.heightAngle, cameraData.aperture, cameraData.focalLength);

}

//returns the camera object
const Camera& RayTraceScene::getCamera() const {
    return camera;
}

//returns the globalData field of the scene
const SceneGlobalData& RayTraceScene::getGlobalData() const{
    return this->globalData;
}
