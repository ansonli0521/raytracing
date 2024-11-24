#ifndef CAMERA_H
#define CAMERA_H

#include "vector3.h"
#include "ray.h"
#include "color.h"
#include <string>

class Scene;

class Camera {
public:
    Camera(Vector3 position, Vector3 direction, Vector3 up, float fov, int width, int height);
    void renderScene(const Scene& scene, const std::string& filename, const std::string& renderMode, int samplesPerPixel) const;

private:
    Vector3 position, forward, up;
    float fov;
    int width, height;
    Vector3 right() const;
    Color toneMap(const Color& hdrColor) const;
};

#endif
