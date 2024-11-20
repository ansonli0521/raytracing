
#include "scene.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./raytracer <render_mode> (binary or phong)\n";
        return 1;
    }

    std::string renderMode = argv[1];

    if (renderMode != "binary" && renderMode != "phong") {
        std::cerr << "Invalid render mode. Use 'binary' or 'phong'.\n";
        return 1;
    }

    Scene scene;
    scene.loadFromJson("scene.json");

    Camera* camera = scene.getCamera();
    if (camera) {
        camera->renderScene(scene, "output.ppm", renderMode);
    }

    return 0;
}
