#include "camera.h"
#include "scene.h"
#include "color.h"
#include <fstream>
#include <cmath>

Camera::Camera(Vector3 pos, Vector3 dir, Vector3 up, float fov, int w, int h)
    : position(pos), forward(dir.normalize()), up(up.normalize()), fov(fov), width(w), height(h) {}

void Camera::renderScene(const Scene& scene, const std::string& filename, const std::string& renderMode) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    outFile << "P3\n" << width << " " << height << "\n255\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float u = (2.0f * x / (width - 1)) - 1.0f;
            float v = (2.0f * y / (height - 1)) - 1.0f;

            Vector3 rayDirection = (forward + u * right() + v * up).normalize();
            Ray ray(position, rayDirection);
            
            Color hdrColor;
            if (renderMode == "binary") {
                hdrColor = scene.traceRay(ray) ? Color(1, 0, 0) : Color(0, 0, 0);
            } else if (renderMode == "phong") {
                hdrColor = scene.traceRayWithShading(ray);
            }

            Color mappedColor = toneMap(hdrColor);

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
