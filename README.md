# FiscionX

**FiscionX** is a modern and lightweight C++ framework built on OpenGL for game development. It offers out-of-the-box support for skinned and static 3D models, spatial audio, embedded GLB model loading, physics, and a powerful shader system — all designed for fast prototyping and efficient runtime performance.

## ✨ Features

- **Static and Skinned Models**
  - Load and render both static meshes and animated characters with skeletal animation.
  
- **GLB 3D Model Loading**
  - Fully supports `.glb` files with embedded textures, animations, and materials.
  
- **Custom & Built-in Shaders**
  - Easily create your own GLSL shaders or use pre-made shaders included with the engine.
  
- **Spatial Audio with FMOD**
  - Immersive 3D sound support using FMOD for realistic audio positioning and effects.
  
- **Binary Caching System**
  - Models and shaders are cached in binary format for ultra-fast loading and reloading.

- **Multimedia Support**
  - Render **images**, **text**, and **videos** as part of your scenes with high flexibility.
  
- **Physics**
  - Simulate realistic physics with bullet3 library.
  
- **Antialiasing**
  - Built-in antialiasing for clean, high-quality visuals.

## 🛠 Dependencies
No dependencies needed! Everything is included. Just one single header.

## Code example for a simple window
```cpp
#include "FiscionCore.h"

void update() {
    FiscionX::Core::ClockTick();

    float camVel = FiscionX::Core::Camera.speed * FiscionX::Core::deltaTime;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_W) == true) FiscionX::Core::Camera.position += FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_S) == true) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_A) == true) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.right * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_D) == true) FiscionX::Core::Camera.position += FiscionX::Core::Camera.right * camVel;
}

void draw() {
    glm::mat4 view = FiscionX::Core::Camera.GetView();
    glm::mat4 projection = glm::perspective(
        glm::radians(FiscionX::Core::Camera.fov),
        (float)FiscionX::Core::SCREEN_WIDTH / FiscionX::Core::SCREEN_HEIGHT,
        FiscionX::Core::NEAR_PLANE,
        FiscionX::Core::FAR_PLANE
    );
    glm::mat4 viewProj = projection * view;

    FiscionX::Core::RenderAllShadowPasses(view, projection, viewProj);
    FiscionX::Core::Draw::ClearBackground(0.1f, 0.1f, 0.1f, 1.0f);
    FiscionX::Core::SortModels();

    //Your draw function

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(4096, 4096, 4096, 15.0f, 0.01f, 100.0f);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");

    FiscionX::Core::CreateAllShadowMaps();

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    return 0;
}
```

## 🔧 Build & Setup
- Download the newest release
- Unpack the files
- Paste the .cpp, .h and the "dependencies" directory into your source project folder.
- Paste .dll and .lib files into the release or debug folder of your project, or exacly where the output executable is located.
- Link the static libraries (.lib) and the FiscionX cpp (core.cpp, tiny_gltf.cpp, stb_image.cpp and stb_image_write.cpp) and headers (FiscionCore.h and FiscionShaders.h) files to your compiler.
