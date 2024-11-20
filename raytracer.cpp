
#include "scene.h"

int main() {
    Scene scene;
    scene.loadFromJson("scene.json");

    Camera *camera = scene.getCamera();
    if (camera) {
        camera->renderScene(scene, "output_binary.ppm");
    }

    return 0;
}
