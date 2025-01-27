#include "raytracer.h"
#include "QtGui/qimage.h"
#include "raytracer/TextureInfo.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include "raytracescene.h"
#include "light.h"
#include <unordered_map>
#include <iostream>
#include <ostream>


RayTracer::RayTracer(Config config) : m_config(config) {}


void RayTracer::render(RGBA *imageData, const RayTraceScene &scene) {

    storeTextures(scene);
    camera = scene.getCamera();    
    renderData = scene.renderData;
    globalData = scene.getGlobalData();

    //depth
    int k = 1;
    bool beginning = true;

    //loop over i,j pixels in view plane
    for(int i = 0; i < scene.m_height; i++){ //rows
        for(int j = 0; j < scene.m_width; j++){ //cols

            //calculate d
            glm::vec4 d = getDistance(i, j, k, scene);

            //transform eye and d into world space
            glm::vec4 p = {0.f, 0.f, 0.f, 1.f};
            glm::vec4 pWorld = camera.inverseViewMatrix * p;
            glm::vec4 dWorld = camera.inverseViewMatrix * d;           

            //cast ray into the scene
            SceneGlobalData globalData = scene.getGlobalData();
            glm::vec3 illumination = castRay(pWorld, dWorld, 4, 0);

            //set the pixel to the returned color
            RGBA color = toRGBA(illumination);
            imageData[(i * scene.m_width) + j].r = color.r;
            imageData[(i * scene.m_width) + j].g = color.g;
            imageData[(i * scene.m_width) + j].b = color.b;
        }
    }
}


glm::vec3 RayTracer::castRay(glm::vec4 pWorld, glm::vec4 dWorld, int max_depth, int curr_depth){

    //store variables for looping over all the shapes
    float smallestT = FLT_MAX;
    IntersectInfo intersectInfo;
    IntersectInfo infoFinal;

    //loop over each object in the scene
    for(RenderShapeData shapeData : renderData.shapes){

        PrimitiveType type = shapeData.primitive.type;

        //based on what type of shape we're looking at, check if the ray intersects it
        switch(type){
        case PrimitiveType::PRIMITIVE_CONE:
        {
            Cone cone = Cone{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = cone.traceRay(pWorld, dWorld);
            break;
        }

        case PrimitiveType::PRIMITIVE_CUBE:
        {
            Cube cube = Cube{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = cube.traceRay(pWorld, dWorld);
            break;
        }

        case PrimitiveType::PRIMITIVE_CYLINDER:
        {
            Cylinder cylinder = Cylinder{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = cylinder.traceRay(pWorld, dWorld);
            break;
        }

        case PrimitiveType::PRIMITIVE_SPHERE:
        {
            Sphere sphere = Sphere{shapeData.ctm, type, shapeData.primitive.material};
            intersectInfo = sphere.traceRay(pWorld, dWorld);
            break;
        }

        case PrimitiveType::PRIMITIVE_MESH:
            break;
        }

        //if the t that was just calculated is the smallest seen
        //yet, save that and its intersectInfo as our smallest
        if(intersectInfo.t < smallestT){
            smallestT = intersectInfo.t;
            infoFinal  = intersectInfo;
        }
    }

    //get the lighting for this square
    glm::vec3 illumination = {0.f, 0.f, 0.f};

    if(smallestT < FLT_MAX){

        //intersection point of the ray on the object in world space
        glm::vec4 intersection = {pWorld.x + (smallestT * dWorld.x), pWorld.y + (smallestT * dWorld.y),
                                  pWorld.z + (smallestT * dWorld.z), 1.0f};

        //vector pointing form the intersection point to the camera in world space
        glm::vec4 directionToCamera = -dWorld;
        directionToCamera = glm::normalize(directionToCamera);
        glm::vec3 L = -directionToCamera;
        glm::vec4 reflectDirection = glm::vec4(L - 2.f*glm::vec3(infoFinal.normal) *
                                     glm::dot(glm::vec3(infoFinal.normal), glm::vec3(L)), 0.f);

        //normal in world space
        glm::vec3 normal = glm::inverse(glm::transpose(infoFinal.ctm)) * infoFinal.normal;
        normal = glm::normalize(normal);

        //get the lights to iterate over to calculate lighting
        std::vector<SceneLightData> lights = renderData.lights;

        //get the illumination for this pixel
        Light lighting = Light{};
        glm::vec3 textureColor = lighting.getTextureColor(intersection, infoFinal, filenameToTextureInfo, renderData, globalData);
        illumination += lighting.phong(intersection, normal, directionToCamera, infoFinal.material,
                                       lights, textureColor, infoFinal.material.blend);
        if(curr_depth < max_depth && glm::length(infoFinal.material.cReflective) != 0.f){
            glm::vec3 reflection = castRay(intersection + (0.001f * glm::vec4(normal, 0.f)),
                                           reflectDirection, max_depth, curr_depth + 1);
            illumination.x += reflection.x * infoFinal.material.cReflective.x * globalData.ks;
            illumination.y += reflection.y * infoFinal.material.cReflective.y * globalData.ks;
            illumination.z += reflection.z * infoFinal.material.cReflective.z * globalData.ks;
        }
    }
    return illumination;
}


glm::vec4 RayTracer::getDistance(int i, int j, int k, const RayTraceScene &scene){

    //calculate the normalized image space coordinate x, y
    float x = ((j + 0.5f) / scene.m_width) - 0.5f;
    float y = ((scene.m_height - 1 - i + 0.5f) / scene.m_height) - 0.5f;

    //calculate the scaling factors U, V
    float vScalar = 2.f * k * (tan(camera.getHeightAngle()/2.f));
    float uScalar = ((float)scene.m_width/(float)scene.m_height) * vScalar;

    //scale x, y with U, V to uv coordinate grid
    glm::vec4 uvk = {uScalar * x, vScalar * y, -k, 1.f};

    glm::vec4 eye = {0.f, 0.f, 0.f, 1.f};

    return uvk - eye;
}


TextureInfo RayTracer::loadTextureFromFile(const QString &file) {

    //load the texture info
    QImage myTexture;
    TextureInfo textureInfo;

    int width; int height;
    if (!myTexture.load(file)) {
        std::cout<<"Failed to load in image: " << file.toStdString() << std::endl;
        TextureInfo empty = {0, 0, nullptr, "null"};
        return empty;
    }
    myTexture = myTexture.convertToFormat(QImage::Format_RGBX8888);
    width = myTexture.width();
    height = myTexture.height();

    RGBA* texture = new RGBA[width*height];
    QByteArray arr = QByteArray::fromRawData((const char*) myTexture.bits(), myTexture.sizeInBytes());

    for (int i = 0; i < arr.size() / 4.f; i++){
        texture[i] = RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]};
    }

    textureInfo.width = width;
    textureInfo.height = height;
    textureInfo.rgba = texture;

    return textureInfo;
}

void RayTracer::storeTextures(const RayTraceScene &scene){

    //store each shape's filename and textureinfo struct in a dictionary
    RenderData renderData = scene.renderData;
    for(RenderShapeData shapeData : renderData.shapes){
        const QString QFilename = QString::fromStdString(shapeData.primitive.material.textureMap.filename);

        //if the shape has a texture
        if(shapeData.primitive.material.textureMap.filename != ""){
            TextureInfo textureInfo = loadTextureFromFile(QFilename);
            std::string filename = shapeData.primitive.material.textureMap.filename;
            filenameToTextureInfo[filename] = textureInfo;
        } else {
            filenameToTextureInfo["null"] = {0, 0, nullptr, "null"};
        }

    }
}


RGBA RayTracer::toRGBA(glm::vec3 &illumination) {

    //converts each component of illumination to a value 0-255
    uint8_t r = 255 * fmin(fmax(illumination.x, 0), 1);
    uint8_t g = 255 * fmin(fmax(illumination.y, 0), 1);
    uint8_t b = 255 * fmin(fmax(illumination.z, 0), 1);
    return RGBA{r, g, b, 255};
}
