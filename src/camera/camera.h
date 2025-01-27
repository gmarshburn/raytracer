#pragma once

#include <glm/glm.hpp>

// A class representing a virtual camera.

class Camera {
public:

    glm::vec3 pos;
    glm::vec3 look;
    glm::vec3 up;

    float heightAngle; // The height angle of the camera in RADIANS

    float aperture;    // Only applicable for depth of field
    float focalLength;

    glm::mat4 viewMatrix;
    glm::mat4 inverseViewMatrix;

    void setup(glm::vec3 posToSet, glm::vec3 lookToSet, glm::vec3 upToSet, float heightToSet, float apetureToSet, float focalToSet);

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    //returns the width angle of the camera in RADIANS
    float getWidthAngle() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getFocalLength() const;

    // Returns the focal length of this camera.
    // This is for the depth of field extra-credit feature only;
    // You can ignore if you are not attempting to implement depth of field.
    float getAperture() const;

};
