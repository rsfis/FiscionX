#include "FiscionCore.h"
#define PROJECT_VERSION "0.6.5"
//testezeira
FiscionX::Light* dirLight;
FiscionX::Light* pointLight;
FiscionX::Light* spotLight;

FiscionX::Sound* exSound;

FiscionX::Model* staticModel;
FiscionX::Model* boxModel;
FiscionX::Model* skinnedModel;
FiscionX::Model* kratosStaticModel;

FiscionX::UI::Image* image_didi;

FiscionX::Camera FiscionX::Core::Camera;

void update() {
    FiscionX::Core::ClockTick();

    skinnedModel->update(FiscionX::Core::deltaTime);

    FiscionX::Core::Camera.update(FiscionX::Core::Window, FiscionX::Core::deltaTime);
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

    staticModel->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
    boxModel->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
    kratosStaticModel->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
    skinnedModel->draw(FiscionX::Core::shaderSkinned, glm::mat4(1.0f), 0, false, view, projection);

    image_didi->draw(-0.5f, -0.5f);

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(4096, 4096, 4096, 15.0f, 0.01f, 100.0f);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");

    dirLight = new FiscionX::Light();
    dirLight->type = FiscionX::LIGHT_DIRECTIONAL;
    dirLight->direction = glm::normalize(glm::vec3(0.0f, -1.0f, -1.0f));
    dirLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
    dirLight->intensity = 1.2f;
    dirLight->maxDistance = 0.0f;
    dirLight->cutOff = 0.0f;
    dirLight->outerCutOff = 0.0f;
    dirLight->constant = 1.0f;
    dirLight->linear = 0.0f;
    dirLight->quadratic = 0.0f;
    dirLight->hasGlow = false;

    pointLight = new FiscionX::Light();
    pointLight->type = FiscionX::LIGHT_POINT;
    pointLight->position = glm::vec3(0.559f, 3.526f, -2.51f);
    pointLight->color = glm::vec3(1, 1, 1);
    pointLight->intensity = 25;
    pointLight->constant = 1;
    pointLight->linear = 0.003;
    pointLight->quadratic = 0.012;
    pointLight->maxDistance = 10;
    pointLight->hasGlow = false;

    spotLight = new FiscionX::Light();
    spotLight->type = FiscionX::LIGHT_SPOT;
    spotLight->position = glm::vec3(0.0f, 1.0f, -4.0f);
    spotLight->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    spotLight->color = glm::vec3(0.0f, 1, 0);
    spotLight->intensity = 2.0f;
    spotLight->maxDistance = 15.0f;
    spotLight->cutOff = glm::cos(glm::radians(25.0f));
    spotLight->outerCutOff = glm::cos(glm::radians(30.0f));
    spotLight->constant = 1.0f;
    spotLight->linear = 0.09f;
    spotLight->quadratic = 0.032f;
    spotLight->hasGlow = false;

    FiscionX::Core::CreateAllShadowMaps();

    // Texto; Video; Cache para Modelos; Filtro Anisotropico e TAA; Optimização; GUI; Colisões Capsule e Box; Detecção de Colisões por Malha (Mesh Colision); Separação; Particulas; Física;

    staticModel = new FiscionX::Model(
        "assets/models/car_scene.glb",
        glm::vec3(0, 0, 0),
        glm::vec3(0),
        glm::vec3(0.01f)
    );
    kratosStaticModel = new FiscionX::Model(
        "assets/models/kratos.glb",
        glm::vec3(0, 0, 7.0f),
        glm::vec3(0),
        glm::vec3(0.1f)
    );
    boxModel = new FiscionX::Model(
        "assets/models/wall.glb",
        glm::vec3(0, 0, 4.2f),
        glm::vec3(0),
        glm::vec3(0.5f)
    );
    skinnedModel = new FiscionX::Model(
        "assets/models/camel.glb",
        glm::vec3(0.4f, 0, 1.5f),
        glm::vec3(1, 0, 0),
        glm::vec3(1)
    );

    skinnedModel->playAnim("Armature|Idle_01", true);

    image_didi = new FiscionX::UI::Image("assets/images/didi.png", 0.5f, 0.5f);

    exSound = new FiscionX::Sound("assets/audio/music/K.mp3", false, true, glm::vec3(0.5f, 0.3f, 0.0f), 2.0f, 10.0f, 1.0f);
    exSound->play();
    // exSound->useEffect(FMOD_DSP_TYPE_);
    // exSound->dsp->setParameterFloat(TYPE, AMOUNT);

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    system("pause");
    return 0;
}