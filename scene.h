
#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "sphere.h"
#include "ray.h"
#include "camera.h"
#include "color.h"

class Scene {
public:
    void addSphere(const Vector3 &center, float radius, const Color &color);
    Color traceRay(const Ray &ray) const;
    void loadFromJson(const std::string &filename);
    Camera* getCamera() const { return camera; }

private:
    std::vector<Sphere*> objects;
    Camera* camera = nullptr;
};

#endif
