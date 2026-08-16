# FiscionX
**FiscionX** is a modern and lightweight C++ framework built on OpenGL for game development designed for Windows x64 and x86 architectures. It offers out-of-the-box support for skinned and static 3D models, spatial audio, embedded GLB model loading, physics, and a powerful shader system — all designed for fast prototyping and efficient runtime performance.

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
#define PROJECT_VERSION "1.0.0"

FiscionX::Light* dirLight;

void update() {
    FiscionX::Core::ClockTick();

    FiscionX::Core::AudioSystem.listenerPos = { FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z };
    FiscionX::Core::AudioSystem.forward = { -FiscionX::Core::Camera.front.x, FiscionX::Core::Camera.front.y , -FiscionX::Core::Camera.front.z };
    FiscionX::Core::AudioSystem.up = { -FiscionX::Core::Camera.up.x, FiscionX::Core::Camera.up.y, -FiscionX::Core::Camera.up.z };

    // CAMERA
    float camVel = FiscionX::Core::Camera.speed * FiscionX::Core::deltaTime;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_W)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_S)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_A)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.right * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_D)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.right * camVel;
}

void draw() {
    // DRAW NORMAL
    FiscionX::Mat4 view = FiscionX::Core::Camera.GetView();
    FiscionX::Mat4 projection = FiscionX::Mat4(0).perspective(
        glm::radians(FiscionX::Core::Camera.fov),
        (float)FiscionX::Core::SCREEN_WIDTH / FiscionX::Core::SCREEN_HEIGHT,
        FiscionX::Core::NEAR_PLANE,
        FiscionX::Core::FAR_PLANE
    );
    FiscionX::Mat4 viewProj = projection * view;

    FiscionX::Core::RenderAllShadowPasses(view, projection, viewProj);

    FiscionX::Core::Draw::ClearBackground(0.2f, 0.2f, 1.0f, 1.0f);

    // DRAW NORMAL
    FiscionX::Core::SortModels();

    FiscionX::Core::Draw::PostProcessing(viewProj, dirLight); // Sun is beign drawn in front of every object.

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(5120, 1024, 512, { 15.0f, 70.0f, 150.0f }, 0.01f, 3000.0f, false);
    FiscionX::Core::SetCacheSettings(true, true);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");
    //FiscionX::Core::SetWindowFullscreen(true, 0);
    FiscionX::Core::SetWindowIcon("assets/icons/fiscionx_logo_big_512.png");
    FiscionX::Core::SetCursorMode(FISCIONX_CURSOR_DISABLED);
    FiscionX::Physics::CreatePhysicsWorld(FiscionX::Vector3(0, -9.81f, 0), 10);
    FiscionX::Core::LoadHDR("assets/environment/gardens.hdr");

    FiscionX::Core::AMBIENT_LIGHT_INTENSITY = 0.4f;
    FiscionX::Core::HDR_EXPOSURE = 1.0f;
    
    FiscionX::Core::SSR_ENABLED = true;
    FiscionX::Core::SSR_MAX_DISTANCE = 30.0f;
    FiscionX::Core::SSR_THICKNESS = 1.5f;
    FiscionX::Core::SSR_MAX_STEPS = 48;
    FiscionX::Core::SSR_BINARY_STEPS = 6;
    FiscionX::Core::SSR_STRIDE = 0.35f;
    FiscionX::Core::SSR_FADE_SCREEN_EDGE = 0.0f;
    FiscionX::Core::SSR_MAX_BLUR_RADIUS = 10.0f;
    
    FiscionX::Core::SSAO_ENABLED = true;
    FiscionX::Core::SSAO_RADIUS = 0.5f;
    FiscionX::Core::SSAO_BIAS = 0.025f;
    FiscionX::Core::SSAO_INTENSITY = 1.5f;
    FiscionX::Core::SSAO_GI_STRENGTH = 0.6f;
    
    FiscionX::Core::fogColor = FiscionX::Vector3(0.4f, 0.4f, 0.45f);
    FiscionX::Core::fogDensity = 0.015f;
    FiscionX::Core::fogStart = 30.0f;
    FiscionX::Core::fogEnd = 120.0f;
    FiscionX::Core::fogType = 2;

    FiscionX::Core::VOLUMETRIC_FOG_ENABLED = true;
    FiscionX::Core::VOLUMETRIC_FOG_COLOR = FiscionX::Vector3(0.92f, 1.0f, 1.0f);
    FiscionX::Core::VOLUMETRIC_FOG_DENSITY = 0.005f;
    FiscionX::Core::VOLUMETRIC_FOG_HEIGHT_START = 2.9f;
    FiscionX::Core::VOLUMETRIC_FOG_HEIGHT_FALLOFF = 0.29f;
    FiscionX::Core::VOLUMETRIC_FOG_ANISOTROPY = 0.081f;
    FiscionX::Core::VOLUMETRIC_FOG_SCATTERING = 1.87f;
    FiscionX::Core::VOLUMETRIC_FOG_AMBIENT = 0.015f;
    FiscionX::Core::VOLUMETRIC_FOG_MAX_DISTANCE = 180.0f;
    FiscionX::Core::VOLUMETRIC_FOG_STEPS = 40;
    FiscionX::Core::VOLUMETRIC_FOG_NOISE_SCALE = 0.05f;
    FiscionX::Core::VOLUMETRIC_FOG_NOISE_SPEED = 0.4f;
    FiscionX::Core::VOLUMETRIC_FOG_NOISE_INTENSITY = 0.35f;
    FiscionX::Core::VOLUMETRIC_FOG_BLUR_RADIUS = 3.0f;

    dirLight = new FiscionX::Light();
    dirLight->type = FiscionX::LIGHT_DIRECTIONAL;
    dirLight->yaw = 0;
    dirLight->pitch = -138;
    dirLight->color = FiscionX::Vector3(1.0f, 1.0f, 1.0f);
    dirLight->intensity = 4.0f;
    dirLight->maxDistance = 0.0f;
    dirLight->cutOff = 0.0f;
    dirLight->outerCutOff = 0.0f;
    dirLight->constant = 1.0f;
    dirLight->linear = 0.0f;
    dirLight->quadratic = 0.0f;
    dirLight->hasGlow = false;
    dirLight->enableShadows = true;

    FiscionX::Core::CreateAllShadowMaps();

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    system("pause");
    return 0;
}
```

## 🔧 Build & Setup
- Download the newest release
- Unpack the files
- Paste the .cpp, .h and the "dependencies" directory into your source project folder.
- Paste .dll files from redist/<your_cpu_architecture> into the release or debug folder of your project, or exacly where the output executable is located.
- Paste .lib files from redist/<your_cpu_architecture> into the source code folder. If necessary, open FiscionCore.h and alter the directory of these .lib files since the solution directory.
- Paste "redist/plugins" into your release folders
- Link the static libraries (.lib) and the FiscionX cpp (core.cpp, tiny_gltf.cpp, stb_image.cpp and stb_image_write.cpp) and headers (FiscionCore.h and FiscionShaders.h) files to your compiler.

Or:

- Download the FiscionX Hub at Releases and create your project automatically with it.

## Links
Discord - https://discord.gg/42ScmJA2t6.
Wiki - https://github.com/rsfis/fiscionx/wiki.

<img width="885" height="717" alt="image" src="https://github.com/user-attachments/assets/c761c5e5-0229-4de1-87a1-d46a3ef849a3" />
<img width="520" height="714" alt="image" src="https://github.com/user-attachments/assets/29d0ca4f-d690-4ff6-af05-4d40165cfc3a" />
