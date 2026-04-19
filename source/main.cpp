#include "FiscionCore.h"
//#include <Windows.h>
//#include <psapi.h>
#define PROJECT_VERSION "1.0.0"

FiscionX::Light* dirLight;
FiscionX::Light* spotLight;

FiscionX::Model* staticModel;
FiscionX::Model* boxModel;
FiscionX::Model* skinnedModel;
FiscionX::Model* kratosStaticModel;

/*
void PrintRAMUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;

    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {

        SIZE_T ramBytes = pmc.WorkingSetSize; // RAM real usada
        SIZE_T ramMB = ramBytes / (1024 * 1024);

        std::cout << "RAM usada: " << ramMB << " MB\n";
    }
}
*/

void update() {
    FiscionX::Core::ClockTick();

    FiscionX::Core::AudioSystem.listenerPos = { FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z };
    FiscionX::Core::AudioSystem.forward = { -FiscionX::Core::Camera.front.x, FiscionX::Core::Camera.front.y , -FiscionX::Core::Camera.front.z };
    FiscionX::Core::AudioSystem.up = { -FiscionX::Core::Camera.up.x, FiscionX::Core::Camera.up.y, -FiscionX::Core::Camera.up.z };

    if (skinnedModel) {
        skinnedModel->update(FiscionX::Core::deltaTime);
    }

    // CAMERA
    float camVel = FiscionX::Core::Camera.speed * FiscionX::Core::deltaTime;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_W)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_S)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_A)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.right * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_D)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.right * camVel;

    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_I)) dirLight->yaw += 0.04f;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_K)) dirLight->yaw -= 0.04f;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_J)) dirLight->pitch += 0.04f;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_L)) dirLight->pitch -= 0.04f;
    if (skinnedModel) {
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_M)) {
            skinnedModel->destroy();
            delete skinnedModel;
            skinnedModel = nullptr;
        }
    }
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_ESCAPE)) FiscionX::Core::Terminate();
    //if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_Z)) PrintRAMUsage();

    if (skinnedModel) {
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_SPACE)) skinnedModel->position.y += 0.004f;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_B)) skinnedModel->position.z += 0.004f;
    }
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

    staticModel->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
    boxModel->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
    kratosStaticModel->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
    if (skinnedModel) {
        skinnedModel->draw(FiscionX::Core::shaderSkinned, glm::mat4(1.0f), 0, false, view, projection);
    }

    FiscionX::Core::Draw::PostProcessing(viewProj, dirLight);

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

    FiscionX::Core::AMBIENT_LIGHT_INTENSITY = 2.0f;

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

    spotLight = new FiscionX::Light();
    spotLight->type = FiscionX::LIGHT_POINT;
    spotLight->position = FiscionX::Vector3(0.0f, 1.0f, -4.0f);
    spotLight->direction = FiscionX::Vector3(0.0f, 0.0f, 0.0f);
    spotLight->color = FiscionX::Vector3(1.0f, 0, 0);
    spotLight->intensity = 15.0f;
    spotLight->maxDistance = 30.0f;
    spotLight->cutOff = glm::cos(glm::radians(25.0f));
    spotLight->outerCutOff = glm::cos(glm::radians(30.0f));
    spotLight->constant = 1.0f;
    spotLight->linear = 0.09f;
    spotLight->quadratic = 0.032f;
    spotLight->hasGlow = true;
    spotLight->enableShadows = true;

    FiscionX::Core::CreateAllShadowMaps();

    // Draw halo and glow also for point lights and spot lights; Point lights are traversing walls/solid objects (Criação da textura está correta. Problema: Shader ou Computando); Soft Shadows; Sombras e Luz devem passar por malhas com transparência; Sliders; Viewports; UI Masks; Model Cache; Particles; Fog; Ambient Occlusion; Terrains; Water

    staticModel = new FiscionX::Model(
        "assets/models/car_scene.glb",
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0.01f, 0.01f, 0.01f)
    );
    kratosStaticModel = new FiscionX::Model(
        "assets/models/kratos.glb",
        FiscionX::Vector3(0, 0, 7.0f),
        FiscionX::Vector3(0),
        FiscionX::Vector3(0.1f, 0.1f, 0.1f)
    );
    boxModel = new FiscionX::Model(
        "assets/models/wall.glb",
        FiscionX::Vector3(0, 0.09f, 4.2f),
        FiscionX::Vector3(2, 0, 0),
        FiscionX::Vector3(0.5f, 0.5f, 0.5f)
    );
    skinnedModel = new FiscionX::Model(
        "assets/models/camel.glb",
        FiscionX::Vector3(0.4f, 0, 1.5f),
        FiscionX::Vector3(1, 0, 0),
        FiscionX::Vector3(1, 1, 1)
    );

    skinnedModel->playAnim("Armature|Idle_01", true);

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    system("pause");
    return 0;
}