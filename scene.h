#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"
#include "ray.h"
#include "camera.h"
#include "color.h"

class Scene {
public:
    void addSphere(const Vector3 &center, float radius, const Color &color);
    void addTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color);
    void addCylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Color &color);
    Color traceRay(const Ray &ray) const;
    void loadFromJson(const std::string &filename);
    Camera* getCamera() const { return camera; }

private:
    std::vector<Sphere*> spheres;
    std::vector<Triangle*> triangles;
    std::vector<Cylinder*> cylinders;
    Camera* camera = nullptr;
};

#endif
