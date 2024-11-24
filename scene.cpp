#include "scene.h"
#include "json.hpp"
#include <fstream>
#include <limits>
#include <cmath>
#include <algorithm>
#include "bvhnode.h"
#include "boundingbox.h"
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

void Scene::addLight(const Vector3 &position, float intensity, const Color &color) {
    lights.push_back({position, intensity, color});
}

// Build BVH for the scene
void Scene::buildBVH() {
    std::vector<BVHNode::Primitive> primitives;

    // Add spheres to primitives
    for (size_t i = 0; i < spheres.size(); ++i) {
        BoundingBox bbox = spheres[i]->getBoundingBox();
        primitives.push_back(BVHNode::Primitive(bbox, static_cast<void*>(spheres[i]), BVHNode::Primitive::PrimitiveType::Sphere));
    }

    // Add triangles to primitives
    for (size_t i = 0; i < triangles.size(); ++i) {
        BoundingBox bbox = triangles[i]->getBoundingBox();
        primitives.push_back(BVHNode::Primitive(bbox, static_cast<void*>(triangles[i]), BVHNode::Primitive::PrimitiveType::Triangle));
    }

    // Add cylinders to primitives
    for (size_t i = 0; i < cylinders.size(); ++i) {
        BoundingBox bbox = cylinders[i]->getBoundingBox();
        primitives.push_back(BVHNode::Primitive(bbox, static_cast<void*>(cylinders[i]), BVHNode::Primitive::PrimitiveType::Cylinder));
    }

    // Build the BVH tree
    bvhRoot = BVHNode::build(primitives, 0);
}

// Trace a ray against the BVH
bool Scene::traceRay(const Ray &ray) const {
    if (bvhRoot) {
        return bvhRoot->doesIntersect(ray);
    }
    return false;
}

// Ray tracing with shading
Color Scene::traceRayWithShading(const Ray &ray, int depth) const {
    if (depth <= 0) {
        return {0.0f, 0.0f, 0.0f}; // Stop recursion
    }

    float closestDistance = std::numeric_limits<float>::max();
    Vector3 hitPoint, normal;
    Color objectColor;
    float reflectivity = 0.0f, transparency = 0.0f, refractiveIndex = 1.0f;

    if (bvhRoot && bvhRoot->trace(ray, closestDistance, hitPoint, normal, objectColor, reflectivity, transparency, refractiveIndex)) {
        Color finalColor = {0.0f, 0.0f, 0.0f};
        Vector3 viewDir = -ray.direction.normalize();

        // Shadow ray and light contribution
        for (const auto &light : lights) {
            Vector3 lightDir = (light.position - hitPoint).normalize();
            Ray shadowRay(hitPoint + normal * 1e-4, lightDir); // Avoid self-intersection
            Color lightTransmission = {1.0f, 1.0f, 1.0f};
            float lightDistance = (light.position - hitPoint).length();
            bool inShadow = false;

            // Trace shadow ray through the BVH
            float shadowClosest = lightDistance; // Limit shadow ray to light distance
            while (bvhRoot->trace(shadowRay, shadowClosest, hitPoint, normal, objectColor, reflectivity, transparency, refractiveIndex)) {
                if (transparency > 0.0f) {
                    lightTransmission = lightTransmission * objectColor * transparency;
                    shadowRay = Ray(hitPoint + shadowRay.direction * 1e-4, shadowRay.direction);
                    shadowClosest = lightDistance; // Reset for subsequent intersections
                } else {
                    lightTransmission = {0.0f, 0.0f, 0.0f};
                    inShadow = true;
                    break;
                }
            }

            // Apply shading if the light is not completely blocked
            if (!inShadow) {
                float diff = std::max(0.0f, normal.dot(lightDir));
                Vector3 reflectDir = (2 * normal.dot(lightDir) * normal - lightDir).normalize();
                float spec = std::pow(std::max(0.0f, viewDir.dot(reflectDir)), 32);

                Color diffuse = objectColor * diff * lightTransmission;
                Color specular = light.color * spec * light.intensity * lightTransmission;

                finalColor = finalColor + diffuse * light.intensity + specular;
            }
        }

        // Reflection
        Vector3 reflectionDir = ray.direction - 2 * ray.direction.dot(normal) * normal;
        Ray reflectedRay(hitPoint + reflectionDir * 1e-4, reflectionDir);
        Color reflectionColor = traceRayWithShading(reflectedRay, depth - 1) * reflectivity;

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
                refractionColor = traceRayWithShading(refractedRay, depth - 1) * transparency;
            }
        }

        // Combine reflection, refraction, and shading
        return finalColor * (1.0f - reflectivity - transparency) +
               reflectionColor +
               refractionColor;
    }

    return {0.0f, 0.0f, 0.0f}; // Background color
}

// Load scene from JSON
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
    int aperture = cameraConfig["aperture"];
    int focusDistance = cameraConfig["focus_distance"];

    camera = new Camera(position, lookAt, up, fov, width, height, aperture, focusDistance);

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
