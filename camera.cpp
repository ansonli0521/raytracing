#include "camera.h"
#include "scene.h"
#include "color.h"
#include <fstream>
#include <cmath>
#include <random>

Camera::Camera(Vector3 pos, Vector3 dir, Vector3 up, float fov, int w, int h, float aperture, float focusDistance)
    : position(pos), forward(dir.normalize()), up(up.normalize()), fov(fov), width(w), height(h), aperture(aperture), focusDistance(focusDistance) {}

void Camera::renderScene(const Scene& scene, const std::string& filename, const std::string& renderMode, int samplesPerPixel) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    outFile << "P3\n" << width << " " << height << "\n255\n";

    Vector3 horizontal = right() * 2.0f * std::tan(fov / 2.0f) * focusDistance;
    Vector3 vertical = up * 2.0f * std::tan(fov / 2.0f * height / width) * focusDistance;
    Vector3 lowerLeftCorner = position + forward * focusDistance - horizontal / 2.0f - vertical / 2.0f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color accumulatedColor = {0.0f, 0.0f, 0.0f};

            for (int sample = 0; sample < samplesPerPixel; ++sample) {
                float u = (x + static_cast<float>(rand()) / RAND_MAX) / (width - 1);
                float v = (y + static_cast<float>(rand()) / RAND_MAX) / (height - 1);

                Vector3 rayDirection = lowerLeftCorner + u * horizontal + v * vertical - position;
                rayDirection = rayDirection.normalize();

                // Lens Sampling
                Vector3 lensPoint = randomInUnitDisk() * (aperture / 2.0f);
                Vector3 lensOffset = lensPoint.x * right() + lensPoint.y * up;

                Vector3 focalPoint = position + rayDirection * focusDistance;
                Ray ray(position + lensOffset, (focalPoint - (position + lensOffset)).normalize());

                Color hdrColor;
                if (renderMode == "binary") {
                    hdrColor = scene.traceRay(ray) ? Color(1, 0, 0) : Color(0, 0, 0);
                } else if (renderMode == "phong") {
                    hdrColor = scene.traceRayWithShading(ray);
                } else if (renderMode == "pathtracer") {
                    hdrColor = scene.traceRayWithBRDF(ray, 5); // Depth of 5 for pathtracer
                }

                accumulatedColor = accumulatedColor + hdrColor;
            }

            Color averagedColor = accumulatedColor * (1.0f / samplesPerPixel);
            Color mappedColor = toneMap(averagedColor);

            outFile << static_cast<int>(std::clamp(mappedColor.r * 255.0f, 0.0f, 255.0f)) << " "
                    << static_cast<int>(std::clamp(mappedColor.g * 255.0f, 0.0f, 255.0f)) << " "
                    << static_cast<int>(std::clamp(mappedColor.b * 255.0f, 0.0f, 255.0f)) << " ";
        }
        outFile << "\n";
    }

    outFile.close();
}

Vector3 Camera::right() const {
    return forward.cross(up).normalize();
}

Vector3 Camera::randomInUnitDisk() const {
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    while (true) {
        float x = dist(generator);
        float y = dist(generator);
        if (x * x + y * y <= 1.0f) {
            return {x, y, 0.0f};
        }
    }
}

Color Camera::toneMap(const Color& hdrColor) const {
    float maxComponent = std::max({hdrColor.r, hdrColor.g, hdrColor.b});
    if (maxComponent > 1.0f) {
        return hdrColor * (1.0f / maxComponent);
    }
    return hdrColor;
}
