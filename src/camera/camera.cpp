//#include <ostream>
#include <stdexcept>
#include "camera.h"
#include <iostream>

void Camera::setup(glm::vec3 posToSet, glm::vec3 lookToSet, glm::vec3 upToSet, float heightToSet, float apetureToSet, float focalToSet){
    pos = posToSet;
    look = lookToSet;
    up = upToSet;

    heightAngle = heightToSet;
    aperture = apetureToSet;
    focalLength = focalToSet;

    //calculate uvw
    glm::vec3 w = (-look)/glm::length(look);
    glm::vec3 v = (up - glm::dot(up, w)*w)/glm::length((up - glm::dot(up, w)*w));
    glm::vec3 u = glm::cross(v, w);

    //get rotation and translation from uvw
    glm::mat4 rotation = {u.x, v.x, w.x, 0.f,
                          u.y, v.y, w.y, 0.f,
                          u.z, v.z, w.z, 0.f,
                          0.f, 0.f, 0.f, 1.f};

    glm::mat4 translation = {1.f, 0.f, 0.f, 0.f,
                             0.f, 1.f, 0.f, 0.f,
                             0.f, 0.f, 1.f, 0.f,
                             -pos.x, -pos.y, -pos.z, 1.f};

    viewMatrix = rotation * translation; //camera to world
    inverseViewMatrix = glm::inverse(viewMatrix); //world to camera
}

float Camera::getHeightAngle() const {
    return heightAngle;
}

float Camera::getFocalLength() const {
    return focalLength;
}

float Camera::getAperture() const {
    return aperture;
}
