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
#include "bvhnode.h"
#include <random>

struct Light {
    Vector3 position;
    float intensity;
    Color color;
    bool areaLight;
    Vector3 normal;
    float width, height;
    Vector3 u, v;
};

class Scene {
public:
    void addSphere(const Vector3 &center, float radius, const Color &color, float reflectivity = 0.0f, float transparency = 0.0f, float refractiveIndex = 1.0f, Texture* texture = nullptr);
    void addTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color, float reflectivity = 0.0f, float transparency = 0.0f, float refractiveIndex = 1.0f, Texture* texture = nullptr);
    void addCylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Color &color, float reflectivity = 0.0f, float transparency = 0.0f, float refractiveIndex = 1.0f, Texture* texture = nullptr);
    void addLight(const Vector3 &position, float intensity, const Color &color, 
              bool areaLight = false, const Vector3 &normal = {0, -1, 0}, 
              float width = 0.0f, float height = 0.0f);
    void buildBVH();
    bool traceRay(const Ray &ray) const;
    Color traceRayWithShading(const Ray &ray, int depth = 3) const;
    Color traceRayWithBRDF(const Ray &ray, int depth = 3) const;
    Light sampleLight(const Vector3& surfacePoint, Vector3& sampledPoint, float& pdf) const;
    void loadFromJson(const std::string &filename);
    Camera* getCamera() const { return camera; }

private:
    std::vector<Sphere*> spheres;
    std::vector<Triangle*> triangles;
    std::vector<Cylinder*> cylinders;
    std::vector<Light> lights;
    Camera* camera = nullptr;
    std::unique_ptr<BVHNode> bvhRoot = nullptr;
    Vector3 randomHemisphereDirection(const Vector3& normal) const;
};

#endif
