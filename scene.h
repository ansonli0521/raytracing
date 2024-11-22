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
#include "texture.h"

struct Light {
    Vector3 position;
    float intensity;
    Color color;
};

class Scene {
public:
    void addSphere(const Vector3 &center, float radius, const Color &color, float reflectivity = 0.0f, float transparency = 0.0f, float refractiveIndex = 1.0f, Texture* texture = nullptr);
    void addTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color, float reflectivity = 0.0f, float transparency = 0.0f, float refractiveIndex = 1.0f, Texture* texture = nullptr);
    void addCylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Color &color, float reflectivity = 0.0f, float transparency = 0.0f, float refractiveIndex = 1.0f, Texture* texture = nullptr);
    void addLight(const Vector3 &position, float intensity, const Color &color);
    bool traceRay(const Ray &ray) const;
    Color traceRayWithShading(const Ray& ray, int depth = 3) const;
    void loadFromJson(const std::string &filename);
    Camera* getCamera() const { return camera; }

private:
    std::vector<Sphere*> spheres;
    std::vector<Triangle*> triangles;
    std::vector<Cylinder*> cylinders;
    std::vector<Light> lights;
    Camera* camera = nullptr;
};

#endif
