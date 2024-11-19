#include "scene.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

void Scene::addSphere(const Vector3 &center, float radius, const Color &color) {
    objects.emplace_back(new Sphere(center, radius, color));
}

Color Scene::traceRay(const Ray &ray) const {
    for (auto &obj : objects) {
        if (obj->doesIntersect(ray)) {
            return obj->getColor();
        }
    }
    return {0, 0, 0};
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
        }
    }
}
