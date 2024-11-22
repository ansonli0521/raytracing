#include "scene.h"
#include "json.hpp"
#include <fstream>
#include <limits>
#include <cmath>
#include "texture.h"

using json = nlohmann::json;

void Scene::addSphere(const Vector3 &center, float radius, const Color &color, float reflectivity, float transparency, float refractiveIndex, Texture* texture) {
    spheres.emplace_back(new Sphere(center, radius, color, reflectivity, transparency, refractiveIndex, texture));
}

void Scene::addTriangle(const Vector3 &v0, const Vector3 &v1, const Vector3 &v2, const Color &color, float reflectivity, float transparency, float refractiveIndex, Texture* texture) {
    triangles.emplace_back(new Triangle(v0, v1, v2, color, reflectivity, transparency, refractiveIndex, texture));
}

void Scene::addCylinder(const Vector3 &center, const Vector3 &axis, float radius, float height, const Color &color, float reflectivity, float transparency, float refractiveIndex, Texture* texture) {
    cylinders.emplace_back(new Cylinder(center, axis, radius, height, color, reflectivity, transparency, refractiveIndex, texture));
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

Color Scene::traceRayWithShading(const Ray &ray, int depth) const {
    if (depth <= 0) {
        return {0.0f, 0.0f, 0.0f}; // Stop recursion
    }

    float closestDistance = std::numeric_limits<float>::max();
    const Sphere* closestSphere = nullptr;
    const Triangle* closestTriangle = nullptr;
    const Cylinder* closestCylinder = nullptr;
    Vector3 hitPoint, normal;
    Color objectColor;

    // Find the closest intersection
    for (const auto& sphere : spheres) {
        float t = sphere->getIntersectionDistance(ray);
        if (t > 0 && t < closestDistance) {
            closestDistance = t;
            closestSphere = sphere;
            closestTriangle = nullptr;
            closestCylinder = nullptr;
            hitPoint = ray.origin + ray.direction * t;
            normal = (hitPoint - sphere->getCenter()).normalize();
            objectColor = sphere->getColor(hitPoint);
        }
    }

    for (const auto& triangle : triangles) {
        float t = triangle->getIntersectionDistance(ray);
        if (t > 0 && t < closestDistance) {
            closestDistance = t;
            closestSphere = nullptr;
            closestTriangle = triangle;
            closestCylinder = nullptr;
            hitPoint = ray.origin + ray.direction * t;
            normal = triangle->getNormal(hitPoint);
            objectColor = triangle->getColor(hitPoint);
        }
    }

    for (const auto& cylinder : cylinders) {
        float t = cylinder->getIntersectionDistance(ray);
        if (t > 0 && t < closestDistance) {
            closestDistance = t;
            closestSphere = nullptr;
            closestTriangle = nullptr;
            closestCylinder = cylinder;
            hitPoint = ray.origin + ray.direction * t;
            normal = cylinder->getNormal(hitPoint);
            objectColor = cylinder->getColor(hitPoint);
        }
    }

    if (closestDistance == std::numeric_limits<float>::max()) {
        return {0.0f, 0.0f, 0.0f}; // Background color
    }

    float reflectivity = 0.0f;
    float transparency = 0.0f;
    float refractiveIndex = 1.0f;

    if (closestSphere) {
        reflectivity = closestSphere->getReflectivity();
        transparency = closestSphere->getTransparency();
        refractiveIndex = closestSphere->getRefractiveIndex();
    } else if (closestTriangle) {
        reflectivity = closestTriangle->getReflectivity();
        transparency = closestTriangle->getTransparency();
        refractiveIndex = closestTriangle->getRefractiveIndex();
    } else if (closestCylinder) {
        reflectivity = closestCylinder->getReflectivity();
        transparency = closestCylinder->getTransparency();
        refractiveIndex = closestCylinder->getRefractiveIndex();
    }

    // Compute shading
    Color finalColor = {0.0f, 0.0f, 0.0f};
    Vector3 viewDir = -ray.direction.normalize();

    for (const auto& light : lights) {
        Vector3 lightDir = (light.position - hitPoint).normalize();

        // Shadow ray
        Ray shadowRay(hitPoint + normal * 1e-4, lightDir);
        bool inShadow = false;

        for (const auto& sphere : spheres) {
            if (sphere->doesIntersect(shadowRay)) {
                inShadow = true;
                break;
            }
        }

        for (const auto& triangle : triangles) {
            if (triangle->doesIntersect(shadowRay)) {
                inShadow = true;
                break;
            }
        }

        for (const auto& cylinder : cylinders) {
            if (cylinder->doesIntersect(shadowRay)) {
                inShadow = true;
                break;
            }
        }

        if (!inShadow) {
            float diff = std::max(0.0f, normal.dot(lightDir));
            Vector3 reflectDir = (2 * normal.dot(lightDir) * normal - lightDir).normalize();
            float spec = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), 32);

            Color diffuse = objectColor * diff;
            Color specular = light.color * spec * light.intensity;

            finalColor = finalColor + diffuse * light.intensity + specular;
        }
    }

    // Reflection
    Vector3 reflectionDir = ray.direction - 2 * ray.direction.dot(normal) * normal;
    Ray reflectedRay(hitPoint + reflectionDir * 1e-4, reflectionDir);
    Color reflectionColor = traceRayWithShading(reflectedRay, depth - 1);

    // Refraction
    Color refractionColor = {0.0f, 0.0f, 0.0f};
    if (transparency > 0.0f) {
        float eta = 1.0f / refractiveIndex; // Assume air refractive index = 1
        float cosTheta = -normal.dot(ray.direction);
        float k = 1 - eta * eta * (1 - cosTheta * cosTheta);

        if (k >= 0.0f) {
            Vector3 refractionDir = eta * ray.direction + (eta * cosTheta - std::sqrt(k)) * normal;
            refractionDir = refractionDir.normalize();
            Ray refractedRay(hitPoint + refractionDir * 1e-4, refractionDir);
            refractionColor = traceRayWithShading(refractedRay, depth - 1);
        }
    }

    // Combine reflection, refraction, and shading
    return finalColor * (1.0f - reflectivity - transparency) +
           reflectionColor * reflectivity +
           refractionColor * transparency;
}

void Scene::addLight(const Vector3 &position, float intensity, const Color &color) {
    lights.push_back({position, intensity, color});
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
        Texture* texture = nullptr;

        if (object.contains("texture")) {
            std::string textureFile = object["texture"];
            texture = new Texture(textureFile);
        }

        if (type == "sphere") {
            Vector3 center = {object["center"][0], object["center"][1], object["center"][2]};
            float radius = object["radius"];
            Color color = {object["color"][0], object["color"][1], object["color"][2]};
            float reflectivity = object["reflectivity"];
            float transparency = object["transparency"];
            float refractiveIndex = object["refractive_index"];
            addSphere(center, radius, color, reflectivity, transparency, refractiveIndex, texture);
        } else if (type == "triangle") {
            Vector3 v0 = {object["v0"][0], object["v0"][1], object["v0"][2]};
            Vector3 v1 = {object["v1"][0], object["v1"][1], object["v1"][2]};
            Vector3 v2 = {object["v2"][0], object["v2"][1], object["v2"][2]};
            Color color = {object["color"][0], object["color"][1], object["color"][2]};
            float reflectivity = object["reflectivity"];
            float transparency = object["transparency"];
            float refractiveIndex = object["refractive_index"];
            addTriangle(v0, v1, v2, color, reflectivity, transparency, refractiveIndex, texture);
        } else if (type == "cylinder") {
            Vector3 center = {object["center"][0], object["center"][1], object["center"][2]};
            Vector3 axis = {object["axis"][0], object["axis"][1], object["axis"][2]};
            float radius = object["radius"];
            float height = object["height"];
            Color color = {object["color"][0], object["color"][1], object["color"][2]};
            float reflectivity = object["reflectivity"];
            float transparency = object["transparency"];
            float refractiveIndex = object["refractive_index"];
            addCylinder(center, axis, radius, height, color, reflectivity, transparency, refractiveIndex, texture);
        }
    }

    for (const auto &light : sceneJson["lights"]) {
        Vector3 position = {light["position"][0], light["position"][1], light["position"][2]};
        float intensity = light["intensity"];
        Color color = {light["color"][0], light["color"][1], light["color"][2]};
        addLight(position, intensity, color);
    }
}
