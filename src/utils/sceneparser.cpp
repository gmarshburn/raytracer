#include "sceneparser.h"
#include "QtGui/qimage.h"
#include "raytracer/TextureInfo.h"
#include "scenefilereader.h"
#include "utils/rgba.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }


    renderData.cameraData = fileReader.getCameraData();
    renderData.globalData = fileReader.getGlobalData();
    renderData.shapes.clear();
    renderData.lights.clear();
    glm::mat4 ctm(1.0f);
    SceneNode *root = fileReader.getRootNode();
    traverseSceneDfs(renderData, root, ctm);


    return true;
}


void SceneParser::traverseSceneDfs(RenderData &renderData, SceneNode *currNode, glm::mat4 ctm){

    //gets all of the transformations for this current node
    for(SceneTransformation *transformation : currNode->transformations){

        TransformationType currType = transformation->type;
        switch(currType){

        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm *= glm::translate(transformation->translate);
            break;

        case TransformationType::TRANSFORMATION_SCALE:
            ctm *= glm::scale(transformation->scale);
            break;

        case TransformationType::TRANSFORMATION_ROTATE:
            ctm *= glm::rotate(transformation->angle, transformation->rotate);
            break;

        case TransformationType::TRANSFORMATION_MATRIX:
            ctm *= transformation->matrix;
            break;
        }

    }

    //gets all of the primitives for this current node
    for(ScenePrimitive *primitive : currNode->primitives){
        RenderShapeData newShape;
        newShape.primitive = *primitive;
        newShape.ctm = ctm;
        renderData.shapes.push_back(newShape);
    }

    //gets all of the lights for this current node
    for(SceneLight *light : currNode->lights){
        SceneLightData lightData;
        lightData.id = light->id;
        lightData.type = light->type;
        lightData.color = light->color;
        lightData.function = light->function;
        lightData.pos = ctm * glm::vec4{0, 0, 0, 1};
        lightData.dir = ctm * light->dir;
        lightData.penumbra = light->penumbra;
        lightData.angle = light->angle;
        lightData.width = light->width;
        lightData.height = light->height;
        renderData.lights.push_back(lightData);
    }

    //recursively calls the function for the next children
    for(SceneNode *child : currNode->children){
        traverseSceneDfs(renderData, child, ctm);
    }

}

