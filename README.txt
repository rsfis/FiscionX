# FiscionX

**FiscionX** is a modern and lightweight C++ framework built on OpenGL for game development. It offers out-of-the-box support for skinned and static 3D models, spatial audio, embedded GLB model loading, and a powerful shader system — all designed for fast prototyping and efficient runtime performance.

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
  
- **Antialiasing**
  - Built-in antialiasing for clean, high-quality visuals.

## 🛠 Dependencies

- OpenGL 4
- FMOD
- TinyGLTF
- stb_image

## Code example for a simple window
'''cpp
#include <FiscionX.h>

FiscionX::Camera FiscionX::Core::Camera;

void update() {
    FiscionX::Core::ClockTick(); FiscionX::Core::Camera.update(FiscionX::Core::Window, FiscionX::Core::deltaTime);
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
    system("pause");
    return 0;
}
'''

## 🔧 Build & Setup
Download a library release and copy FiscionX.h, core.cpp, glfw3 dll, fmod dll and .lib files into your project and include FiscionCore.h on your main file.
After, link the .lib files with your project and run.
