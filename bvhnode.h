#ifndef BVHNODE_H
#define BVHNODE_H

#include "boundingbox.h"
#include "ray.h"
#include "sphere.h"
#include "triangle.h"
#include "cylinder.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <limits>

class BVHNode {
public:
    struct Primitive {
        enum class PrimitiveType { Sphere, Triangle, Cylinder };
        PrimitiveType type;
        BoundingBox bbox;
        void* object;

        Primitive(const BoundingBox& bbox, void* object, PrimitiveType type)
            : type(type), bbox(bbox), object(object) {}

        BoundingBox getBoundingBox() const {
            return bbox;
        }

        bool doesIntersect(const Ray& ray) const {
            switch (type) {
                case PrimitiveType::Sphere:
                    return static_cast<Sphere*>(object)->doesIntersect(ray);
                case PrimitiveType::Triangle:
                    return static_cast<Triangle*>(object)->doesIntersect(ray);
                case PrimitiveType::Cylinder:
                    return static_cast<Cylinder*>(object)->doesIntersect(ray);
                default:
                    return false;
            }
        }

        float getIntersectionDistance(const Ray& ray) const {
            switch (type) {
                case PrimitiveType::Sphere:
                    return static_cast<Sphere*>(object)->getIntersectionDistance(ray);
                case PrimitiveType::Triangle:
                    return static_cast<Triangle*>(object)->getIntersectionDistance(ray);
                case PrimitiveType::Cylinder:
                    return static_cast<Cylinder*>(object)->getIntersectionDistance(ray);
                default:
                    return -1.0f;
            }
        }
    };

    BoundingBox bbox;
    std::vector<Primitive> primitives;
    std::unique_ptr<BVHNode> left;
    std::unique_ptr<BVHNode> right;

    BVHNode() = default;

    // Build the BVH tree
    static std::unique_ptr<BVHNode> build(std::vector<Primitive>& primitives, int depth = 0) {
        const int maxDepth = 16;
        const int minPrimitives = 2;

        // Base case: Leaf node
        if (primitives.size() <= minPrimitives || depth >= maxDepth) {
            auto node = std::make_unique<BVHNode>();
            node->primitives = std::move(primitives);

            // Compute bounding box for all primitives
            for (const auto& primitive : node->primitives) {
                node->bbox = BoundingBox::merge(node->bbox, primitive.getBoundingBox());
            }
            return node;
        }

        // Compute the overall bounding box
        BoundingBox globalBox;
        for (const auto& primitive : primitives) {
            globalBox = BoundingBox::merge(globalBox, primitive.getBoundingBox());
        }

        // Find the axis with the largest extent
        Vector3 extents = globalBox.max - globalBox.min;
        int axis = 0; // Default to X-axis
        if (extents.y > extents.x) axis = 1; // Y-axis
        if (extents.z > (axis == 0 ? extents.x : extents.y)) axis = 2; // Z-axis

        // Sort primitives along the chosen axis
        std::sort(primitives.begin(), primitives.end(), [axis](const Primitive& a, const Primitive& b) {
            if (axis == 0) return a.bbox.min.x < b.bbox.min.x; // X-axis
            if (axis == 1) return a.bbox.min.y < b.bbox.min.y; // Y-axis
            return a.bbox.min.z < b.bbox.min.z; // Z-axis
        });

        // Split primitives into two halves
        size_t mid = primitives.size() / 2;
        std::vector<Primitive> leftPrimitives(primitives.begin(), primitives.begin() + mid);
        std::vector<Primitive> rightPrimitives(primitives.begin() + mid, primitives.end());

        // Create a new node and recursively build children
        auto node = std::make_unique<BVHNode>();
        node->bbox = globalBox;
        node->left = build(leftPrimitives, depth + 1);
        node->right = build(rightPrimitives, depth + 1);

        return node;
    }

    // Check for ray intersection
    bool doesIntersect(const Ray& ray) const {
        if (!bbox.doesIntersect(ray)) return false;

        for (const auto& primitive : primitives) {
            if (primitive.doesIntersect(ray)) return true;
        }

        bool leftHit = left ? left->doesIntersect(ray) : false;
        bool rightHit = right ? right->doesIntersect(ray) : false;

        return leftHit || rightHit;
    }

    // Get closest intersection distance
    float getIntersectionDistance(const Ray& ray) const {
        if (!bbox.doesIntersect(ray)) return -1.0f;

        float closest = std::numeric_limits<float>::max();

        for (const auto& primitive : primitives) {
            float dist = primitive.getIntersectionDistance(ray);
            if (dist > 0 && dist < closest) closest = dist;
        }

        if (left) {
            float leftDist = left->getIntersectionDistance(ray);
            if (leftDist > 0 && leftDist < closest) closest = leftDist;
        }

        if (right) {
            float rightDist = right->getIntersectionDistance(ray);
            if (rightDist > 0 && rightDist < closest) closest = rightDist;
        }

        return closest == std::numeric_limits<float>::max() ? -1.0f : closest;
    }

    bool trace(const Ray& ray, float& closestDistance, Vector3& hitPoint, Vector3& normal, Color& objectColor, float& reflectivity, float& transparency, float& refractiveIndex) const {
        if (!bbox.doesIntersect(ray)) return false;

        bool hit = false;

        for (const auto& primitive : primitives) {
            float distance = primitive.getIntersectionDistance(ray);
            if (distance > 0 && distance < closestDistance) {
                closestDistance = distance;
                hitPoint = ray.origin + ray.direction * distance;
                hit = true;

                // Retrieve data for shading based on the primitive type
                switch (primitive.type) {
                    case Primitive::PrimitiveType::Sphere: {
                        auto sphere = static_cast<Sphere*>(primitive.object);
                        normal = (hitPoint - sphere->getCenter()).normalize();
                        objectColor = sphere->getColor(hitPoint);
                        reflectivity = sphere->getReflectivity();
                        transparency = sphere->getTransparency();
                        refractiveIndex = sphere->getRefractiveIndex();
                        break;
                    }
                    case Primitive::PrimitiveType::Triangle: {
                        auto triangle = static_cast<Triangle*>(primitive.object);
                        normal = triangle->getNormal(hitPoint);
                        objectColor = triangle->getColor(hitPoint);
                        reflectivity = triangle->getReflectivity();
                        transparency = triangle->getTransparency();
                        refractiveIndex = triangle->getRefractiveIndex();
                        break;
                    }
                    case Primitive::PrimitiveType::Cylinder: {
                        auto cylinder = static_cast<Cylinder*>(primitive.object);
                        normal = cylinder->getNormal(hitPoint);
                        objectColor = cylinder->getColor(hitPoint);
                        reflectivity = cylinder->getReflectivity();
                        transparency = cylinder->getTransparency();
                        refractiveIndex = cylinder->getRefractiveIndex();
                        break;
                    }
                }
            }
        }

        if (left) hit |= left->trace(ray, closestDistance, hitPoint, normal, objectColor, reflectivity, transparency, refractiveIndex);
        if (right) hit |= right->trace(ray, closestDistance, hitPoint, normal, objectColor, reflectivity, transparency, refractiveIndex);

        return hit;
    }
};

#endif
