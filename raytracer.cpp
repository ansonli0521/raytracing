
#include "scene.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: ./raytracer <render_mode> (binary or phong) <json_file_name>\n";
        return 1;
    }

    std::string renderMode = argv[1];
    std::string filename = argv[2];

    if (renderMode != "binary" && renderMode != "phong") {
        std::cerr << "Invalid render mode. Use 'binary' or 'phong'.\n";
        return 1;
    }

    Scene scene;
    scene.loadFromJson(filename);
    scene.buildBVH();

    int samplesPerPixel = 100;
    Camera* camera = scene.getCamera();
    if (camera) {
        camera->renderScene(scene, "output.ppm", renderMode, samplesPerPixel);
    }

    return 0;
}
