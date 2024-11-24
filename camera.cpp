#include "camera.h"
#include "scene.h"
#include "color.h"
#include <fstream>
#include <cmath>
#include <random>

Camera::Camera(Vector3 pos, Vector3 dir, Vector3 up, float fov, int w, int h)
    : position(pos), forward(dir.normalize()), up(up.normalize()), fov(fov), width(w), height(h) {}

void Camera::renderScene(const Scene& scene, const std::string& filename, const std::string& renderMode, int samplesPerPixel) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    outFile << "P3\n" << width << " " << height << "\n255\n";

    // Random number generator for pixel sampling
    std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Color finalColor = {0.0f, 0.0f, 0.0f};

            // Accumulate colors for each sample
            for (int i = 0; i < samplesPerPixel; ++i) {
                // Random offsets within the pixel
                float u = (x + distribution(generator)) / (width - 1);
                float v = (y + distribution(generator)) / (height - 1);

                Vector3 rayDirection = (forward + (2.0f * u - 1.0f) * right() + (2.0f * v - 1.0f) * up).normalize();
                Ray ray(position, rayDirection);

                Color hdrColor;
                if (renderMode == "binary") {
                    hdrColor = scene.traceRay(ray) ? Color(1, 0, 0) : Color(0, 0, 0);
                } else if (renderMode == "phong") {
                    hdrColor = scene.traceRayWithShading(ray);
                }

                finalColor = finalColor + hdrColor; // Accumulate the sampled color
            }

            // Average the accumulated color and apply tone mapping
            finalColor = finalColor * (1.0f / samplesPerPixel);
            Color mappedColor = toneMap(finalColor);

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

Color Camera::toneMap(const Color& hdrColor) const {
    float maxComponent = std::max({hdrColor.r, hdrColor.g, hdrColor.b});
    if (maxComponent > 1.0f) {
        return hdrColor * (1.0f / maxComponent);
    }
    return hdrColor;
}
