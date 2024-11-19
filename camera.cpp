#include "camera.h"
#include "scene.h"
#include "color.h"
#include <fstream>
#include <cmath>

Camera::Camera(Vector3 pos, Vector3 dir, Vector3 up, float fov, int w, int h)
    : position(pos), forward(dir.normalize()), up(up.normalize()), fov(fov), width(w), height(h) {}

void Camera::renderScene(const Scene &scene, const std::string &file) const {
    std::ofstream ofs(file, std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    float aspectRatio = static_cast<float>(width) / height;
    float scale = std::tan(fov * 0.5 * M_PI / 180.0);

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            float x = (2 * (i + 0.5) / width - 1) * aspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / height) * scale;

            Vector3 dir = (forward + (right() * x) + (up * y)).normalize();
            Ray ray(position, dir);
            Color col = scene.traceRay(ray);

            ofs.put(static_cast<unsigned char>(std::clamp(col.r * 255.0f, 0.0f, 255.0f)));
            ofs.put(static_cast<unsigned char>(std::clamp(col.g * 255.0f, 0.0f, 255.0f)));
            ofs.put(static_cast<unsigned char>(std::clamp(col.b * 255.0f, 0.0f, 255.0f)));
        }
    }
    ofs.close();
}

Vector3 Camera::right() const {
    return forward.cross(up).normalize();
}
