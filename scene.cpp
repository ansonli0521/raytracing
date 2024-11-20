#include "scene.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

void Scene::addSphere(const Vector3 &center, float radius, const Color &color) {
    spheres.emplace_back(new Sphere(center, radius, color));
}

void Scene::addTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color) {
    triangles.emplace_back(new Triangle(v0, v1, v2, color));
}

void Scene::addCylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Color &color) {
    cylinders.emplace_back(new Cylinder(center, axis, radius, height, color));
}

bool Scene::traceRay(const Ray &ray) const {
    for (const auto &sphere : spheres) {
        if (sphere->doesIntersect(ray)) {
            return true;
        }
    }

    for (const auto &triangle : triangles) {
        if (triangle->doesIntersect(ray)) {
            return true;
        }
    }

    for (const auto &cylinder : cylinders) {
        if (cylinder->doesIntersect(ray)) {
            return true;
        }
    }

    return false;
}

void Scene::loadFromJson(const std::string &filename) {
    std::ifstream file(filename);
    json sceneJson;
    file >> sceneJson;

    auto cameraConfig = sceneJson["camera"];
    Vector3 position = {cameraConfig["position"][0], cameraConfig["position"][1], cameraConfig["position"][2]};
    Vector3 lookAt = {cameraConfig["look_at"][0], cameraConfig["look_at"][1], cameraConfig["look_at"][2]};
    Vector3 up = {cameraConfig["up"][0], cameraConfig["up"][1], cameraConfig["up"][2]};
    float fov = cameraConfig["fov"];
    int width = cameraConfig["width"];
    int height = cameraConfig["height"];

    camera = new Camera(position, lookAt, up, fov, width, height);

    for (const auto &object : sceneJson["objects"]) {
        std::string type = object["type"];
        if (type == "sphere") {
            Vector3 center = {object["center"][0], object["center"][1], object["center"][2]};
            float radius = object["radius"];
            Color color = {object["color"][0], object["color"][1], object["color"][2]};
            addSphere(center, radius, color);
        } else if (type == "triangle") {
            Vector3 v0 = {object["v0"][0], object["v0"][1], object["v0"][2]};
            Vector3 v1 = {object["v1"][0], object["v1"][1], object["v1"][2]};
            Vector3 v2 = {object["v2"][0], object["v2"][1], object["v2"][2]};
            Color color = {object["color"][0], object["color"][1], object["color"][2]};
            addTriangle(v0, v1, v2, color);
        } else if (type == "cylinder") {
            Vector3 center = {object["center"][0], object["center"][1], object["center"][2]};
            Vector3 axis = {object["axis"][0], object["axis"][1], object["axis"][2]};
            float radius = object["radius"];
            float height = object["height"];
            Color color = {object["color"][0], object["color"][1], object["color"][2]};
            addCylinder(center, axis, radius, height, color);
        }
    }
}
