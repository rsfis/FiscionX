#include "FiscionCore.h"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#define PROJECT_VERSION "1.0.0"

FiscionX::Light* dirLight;
FiscionX::Light* spotLight;

FiscionX::Model* staticModel;
FiscionX::Model* boxModel;
FiscionX::Model* skinnedModel;
FiscionX::Model* kratosStaticModel;

FiscionX::Physics::Rigidbody* groundBody;
FiscionX::Physics::Rigidbody* capsuleBody;
FiscionX::Physics::Rigidbody* staticCarBody;
FiscionX::Physics::Rigidbody* carChassiBody;
FiscionX::Physics::Vehicle* vehicle;

FiscionX::UI::Image* img;
FiscionX::Image3D* img3D;

bool showSettingsPanel = true;
bool settingsPanelHasCursor = false;

void InitImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(FiscionX::Core::Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ShutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DrawSettingsPanel() {
    if (!showSettingsPanel) return;

    ImGui::SetNextWindowSize(ImVec2(420, 680), ImGuiCond_FirstUseEver);
    ImGui::Begin("FiscionX :: Core Settings", &showSettingsPanel);

    ImGui::TextDisabled("Project version %s", PROJECT_VERSION);
    ImGui::Text("FPS: %d", FiscionX::Core::FPS);
    ImGui::Separator();

    // ---------------- JANELA ----------------
    if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Resolution: %d x %d", FiscionX::Core::SCREEN_WIDTH, FiscionX::Core::SCREEN_HEIGHT);

        static int newWidth = FiscionX::Core::SCREEN_WIDTH;
        static int newHeight = FiscionX::Core::SCREEN_HEIGHT;
        ImGui::InputInt("Width", &newWidth);
        ImGui::InputInt("Height", &newHeight);
        if (ImGui::Button("Apply Resolution")) {
            FiscionX::Core::SetWindowSize(newWidth, newHeight);
        }

        static int monitorIndex = 0;
        ImGui::InputInt("Selected Monitor", &monitorIndex);
        ImGui::SameLine();
        if (ImGui::Button("Fullscreen")) {
            FiscionX::Core::SetWindowFullscreen(true, monitorIndex);
        }
        ImGui::SameLine();
        if (ImGui::Button("Windowed")) {
            FiscionX::Core::SetWindowFullscreen(false, monitorIndex);
        }

        static char iconPath[256] = "assets/icons/fiscionx_logo_big_512.png";
        ImGui::InputText("Icon (path)", iconPath, IM_ARRAYSIZE(iconPath));
        ImGui::SameLine();
        if (ImGui::Button("Define icon")) {
            FiscionX::Core::SetWindowIcon(iconPath);
        }
    }

    // ---------------- CURSOR / INPUT ----------------
    if (ImGui::CollapsingHeader("Cursor")) {
        static int cursorMode = 0; // 0 normal, 1 hidden, 2 disabled, 3 captured, 4 locked
        const char* cursorLabels[] = { "Normal", "Hidden", "Disabled", "Captured", "Locked" };
        const int cursorValues[] = {
            FISCIONX_CURSOR_NORMAL, FISCIONX_CURSOR_HIDDEN, FISCIONX_CURSOR_DISABLED,
            FISCIONX_CURSOR_CAPTURED, FISCIONX_CURSOR_LOCKED
        };
        if (ImGui::Combo("Cursor Mode", &cursorMode, cursorLabels, IM_ARRAYSIZE(cursorLabels))) {
            FiscionX::Core::SetCursorMode(cursorValues[cursorMode]);
        }
    }

    // ---------------- CACHE ----------------
    if (ImGui::CollapsingHeader("Cache")) {
        static bool shaderCache = FiscionX::Core::enableShaderCache;
        static bool modelCache = FiscionX::Core::enableModelCache;
        bool changed = false;
        changed |= ImGui::Checkbox("Shader cache", &shaderCache);
        changed |= ImGui::Checkbox("Model cache", &modelCache);
        if (changed) {
            FiscionX::Core::SetCacheSettings(shaderCache, modelCache);
        }
        ImGui::Checkbox("Compress textures automatically", &FiscionX::Core::compressTexturesAutomatically);
    }

    // ---------------- CAMERA ----------------
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::DragFloat3("Position", &FiscionX::Core::Camera.position.x, 0.05f);
        ImGui::DragFloat3("Front", &FiscionX::Core::Camera.front.x, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat("Yaw", &FiscionX::Core::Camera.yaw, 0.5f);
        ImGui::DragFloat("Pitch", &FiscionX::Core::Camera.pitch, 0.5f, -89.0f, 89.0f);
        ImGui::DragFloat("Velocity", &FiscionX::Core::Camera.speed, 0.05f, 0.0f, 100.0f);
        ImGui::DragFloat("Sensibility", &FiscionX::Core::Camera.sensitivity, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("FOV", &FiscionX::Core::Camera.fov, 0.5f, 1.0f, 179.0f);
        ImGui::Checkbox("Can Look", &FiscionX::Core::Camera.canLook);
    }

    // ---------------- SSAO ----------------
    if (ImGui::CollapsingHeader("SSAO")) {
        ImGui::Checkbox("SSAO enabled", &FiscionX::Core::SSAO_ENABLED);
        ImGui::BeginDisabled(!FiscionX::Core::SSAO_ENABLED);
        ImGui::DragFloat("Rau", &FiscionX::Core::SSAO_RADIUS, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Bias", &FiscionX::Core::SSAO_BIAS, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Intensity", &FiscionX::Core::SSAO_INTENSITY, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("GI Force", &FiscionX::Core::SSAO_GI_STRENGTH, 0.01f, 0.0f, 5.0f);
        ImGui::EndDisabled();
    }

    // ---------------- SSR ----------------
    if (ImGui::CollapsingHeader("SSR")) {
        ImGui::Checkbox("SSR enabled", &FiscionX::Core::SSR_ENABLED);
        ImGui::BeginDisabled(!FiscionX::Core::SSR_ENABLED);
        ImGui::DragFloat("Max Distance", &FiscionX::Core::SSR_MAX_DISTANCE, 0.5f, 0.0f, 500.0f);
        ImGui::DragFloat("Thickness", &FiscionX::Core::SSR_THICKNESS, 0.01f, 0.0f, 20.0f);
        ImGui::SliderInt("Max steps", &FiscionX::Core::SSR_MAX_STEPS, 1, 256);
        ImGui::SliderInt("Binary steps", &FiscionX::Core::SSR_BINARY_STEPS, 0, 16);
        ImGui::DragFloat("Fade", &FiscionX::Core::SSR_FADE_SCREEN_EDGE, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Stride", &FiscionX::Core::SSR_STRIDE, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Blur (px)", &FiscionX::Core::SSR_MAX_BLUR_RADIUS, 0.1f, 0.0f, 64.0f);
        ImGui::EndDisabled();
    }

    // ---------------- IBL / HDR ----------------
    if (ImGui::CollapsingHeader("IBL / HDR")) {
        ImGui::Text("IBL ready: %s", FiscionX::Core::iblReady ? "yes" : "no");
        ImGui::DragFloat("Exposition HDR", &FiscionX::Core::HDR_EXPOSURE, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Scale HDR IBL", &FiscionX::Core::IBL_HDR_SCALE, 0.001f, 0.0f, 2.0f);
        ImGui::TextWrapped("*All of this will only be applied on the next HDR load.");

        static char hdrPath[256] = "assets/environment/gardens.hdr";
        ImGui::InputText("HDR (path)", hdrPath, IM_ARRAYSIZE(hdrPath));
        if (ImGui::Button("Load HDR")) {
            FiscionX::Core::LoadHDR(hdrPath);
        }

        ImGui::Separator();
        ImGui::DragFloat("Reflections strength", &FiscionX::Core::REFLECTIONS_STRENGTH, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Ambient intensity", &FiscionX::Core::AMBIENT_LIGHT_INTENSITY, 0.01f, 0.0f, 5.0f);
        ImGui::ColorEdit3("Ambient sky color", &FiscionX::Core::AMBIENT_LIGHT_SKYCOLOR.x);
        ImGui::ColorEdit3("Ambient ground color", &FiscionX::Core::AMBIENT_LIGHT_GROUNDCOLOR.x);
    }

    // ---------------- SOMBRAS ----------------
    if (ImGui::CollapsingHeader("Shadows")) {
        ImGui::Text("Defined using Set3DSettings on setup (read-only):");
        ImGui::Text("Directional: %d  |  Spot: %d  |  Point: %d",
            FiscionX::Core::DIR_SHADOW_SIZE, FiscionX::Core::SPOT_SHADOW_SIZE, FiscionX::Core::POINT_SHADOW_SIZE);

        ImGui::DragFloat("Near plane", &FiscionX::Core::NEAR_PLANE, 0.01f, 0.001f, 100.0f);
        ImGui::DragFloat("Far plane", &FiscionX::Core::FAR_PLANE, 1.0f, 1.0f, 10000.0f);
        ImGui::DragFloat("Shadow View Radius", &FiscionX::Core::SHADOW_VIEW_RADIUS, 0.5f, 0.0f, 1000.0f);

        ImGui::Text("Cascade levels:");
        for (size_t i = 0; i < FiscionX::Core::shadowCascadeLevels.size(); i++) {
            ImGui::PushID((int)i);
            ImGui::DragFloat("##cascade", &FiscionX::Core::shadowCascadeLevels[i], 0.5f, 0.0f, 2000.0f);
            ImGui::PopID();
        }

        if (ImGui::Button("Recreate shadow maps")) {
            FiscionX::Core::CreateAllShadowMaps();
        }
    }

    // ---------------- SOL / GOD RAYS ----------------
    if (ImGui::CollapsingHeader("Sun e God Rays")) {
        ImGui::DragFloat("Sun Disk Size", &FiscionX::Core::sunDiskSize, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Sun Halo Size", &FiscionX::Core::sunHaloSize, 0.001f, 0.0f, 1.0f);
        ImGui::ColorEdit3("Sun Color", &FiscionX::Core::sunColor.x);

        ImGui::Separator();
        ImGui::DragFloat("God rays density", &FiscionX::Core::godRaysDensity, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("God rays weight", &FiscionX::Core::godRaysWeight, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("God rays decay", &FiscionX::Core::godRaysDecay, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("God rays exposure", &FiscionX::Core::godRaysExposure, 0.01f, 0.0f, 5.0f);
        ImGui::SliderInt("God rays samples", &FiscionX::Core::godRaysNumOfSamples, 1, 256);
    }

    // ---------------- COLOR CORRECTION ----------------
    if (ImGui::CollapsingHeader("Color Correction")) {
        ImGui::ColorEdit3("Color correction", &FiscionX::Core::colorCorrection.x);
    }

    // ---------------- MODELOS DA CENA (uma aba por instancia) ----------------
    if (ImGui::CollapsingHeader("All Models", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Total de modelos: %d", (int)FiscionX::Core::AllModels.size());

        for (size_t m = 0; m < FiscionX::Core::AllModels.size(); m++) {
            FiscionX::Model* model = FiscionX::Core::AllModels[m];
            if (!model) continue;

            ImGui::PushID((int)m);
            std::string modelLabel = "Model " + std::to_string(m) +
                (model->isSkinned ? " (Skinned)" : " (Static)") +
                " - " + std::to_string(model->instances.size()) + " instancia(s)";

            if (ImGui::TreeNodeEx(modelLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::DragFloat("Alpha do modelo", &model->alpha, 0.01f, 0.0f, 1.0f);

                if (!model->instances.empty()) {
                    std::string tabBarId = "InstanceTabs_" + std::to_string(m);
                    if (ImGui::BeginTabBar(tabBarId.c_str(), ImGuiTabBarFlags_FittingPolicyScroll)) {
                        for (size_t i = 0; i < model->instances.size(); i++) {
                            FiscionX::Model::Instance& inst = model->instances[i];
                            std::string tabLabel = "Instancia " + std::to_string(i);

                            ImGui::PushID((int)i);
                            if (ImGui::BeginTabItem(tabLabel.c_str())) {

                                ImGui::Checkbox("Visible", &inst.visible);
                                ImGui::SameLine();
                                ImGui::Checkbox("Affected by light", &inst.isAffectedByLight);

                                ImGui::DragFloat3("Position", &inst.position.x, 0.05f);
                                ImGui::DragFloat3("Rotation", &inst.rotation.x, 0.5f);
                                ImGui::DragFloat3("Scale", &inst.scale.x, 0.01f, 0.0001f, 1000.0f);
                                ImGui::DragFloat("Alpha", &inst.alpha, 0.01f, 0.0f, 1.0f);

                                ImGui::Separator();
                                ImGui::Checkbox("Cast Shadows", &inst.castsShadows);
                                ImGui::SameLine();
                                ImGui::Checkbox("Accepts Shadows", &inst.acceptsShadows);
                                ImGui::Checkbox("Enable Frustum culling", &inst.enableFrustumCulling);

                                if (model->isSkinned) {
                                    ImGui::Separator();
                                    ImGui::Text("Current Animation: %s",
                                        inst.currentAnim.name.empty() ? "(none)" : inst.currentAnim.name.c_str());
                                    ImGui::Text("Repeats: %s | Time: %.2f",
                                        inst.currentAnim.repeat ? "yes" : "no", inst.currentAnim.time);

                                    if (!inst.animations.empty()) {
                                        static std::map<std::string, int> selectedAnimIndex;
                                        std::string key = std::to_string(m) + "_" + std::to_string(i);

                                        std::vector<std::string> animNames;
                                        for (auto& kv : inst.animations) animNames.push_back(kv.first);

                                        int& selIdx = selectedAnimIndex[key];
                                        if (selIdx >= (int)animNames.size()) selIdx = 0;

                                        if (ImGui::BeginCombo("Animations",
                                            animNames.empty() ? "" : animNames[selIdx].c_str())) {
                                            for (int a = 0; a < (int)animNames.size(); a++) {
                                                bool isSelected = (a == selIdx);
                                                if (ImGui::Selectable(animNames[a].c_str(), isSelected)) {
                                                    selIdx = a;
                                                }
                                                if (isSelected) ImGui::SetItemDefaultFocus();
                                            }
                                            ImGui::EndCombo();
                                        }

                                        static bool repeatAnim = true;
                                        ImGui::Checkbox("Repeat", &repeatAnim);
                                        ImGui::SameLine();
                                        if (ImGui::Button("Play Anim") && !animNames.empty()) {
                                            inst.playAnim(animNames[selIdx], repeatAnim);
                                        }
                                    }
                                }

                                if (model->cameraNodeIndex >= 0) {
                                    ImGui::Separator();
                                    ImGui::Checkbox("Controls Camera (drivesCamera)", &inst.drivesCamera);
                                }

                                ImGui::EndTabItem();
                            }
                            ImGui::PopID();
                        }
                        ImGui::EndTabBar();
                    }
                }
                else {
                    ImGui::TextDisabled("This model doesn't have instances.");
                }

                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    // ---------------- LUZES (se existirem na cena) ----------------
    if (ImGui::CollapsingHeader("All Lights")) {
        ImGui::Text("Lights on Scene: %d", (int)FiscionX::Core::AllLights.size());
        for (size_t i = 0; i < FiscionX::Core::AllLights.size(); i++) {
            FiscionX::Light* L = FiscionX::Core::AllLights[i];
            if (!L) continue;
            ImGui::PushID((int)i);
            std::string label = "Light " + std::to_string(i) +
                (L->type == FiscionX::LIGHT_DIRECTIONAL ? " (Directional)" :
                    L->type == FiscionX::LIGHT_POINT ? " (Point)" : " (Spot)");
            if (ImGui::TreeNode(label.c_str())) {
                ImGui::DragFloat3("Position", &L->position.x, 0.05f);
                ImGui::DragFloat3("Direction", &L->direction.x, 0.01f);
                ImGui::DragFloat("Yaw", &L->yaw, 0.5f);
                ImGui::DragFloat("Pitch", &L->pitch, 0.5f);
                ImGui::ColorEdit3("Color", &L->color.x);
                ImGui::DragFloat("Intensity", &L->intensity, 0.05f, 0.0f, 100.0f);
                ImGui::DragFloat("Max Distance", &L->maxDistance, 0.5f, 0.0f, 2000.0f);
                ImGui::DragFloat("Cut off", &L->cutOff, 0.001f, 0.0f, 1.0f);
                ImGui::DragFloat("Outer cut off", &L->outerCutOff, 0.001f, 0.0f, 1.0f);
                ImGui::DragFloat("Constant", &L->constant, 0.01f, 0.0f, 5.0f);
                ImGui::DragFloat("Linear", &L->linear, 0.001f, 0.0f, 1.0f);
                ImGui::DragFloat("Quadratic", &L->quadratic, 0.001f, 0.0f, 1.0f);
                ImGui::Checkbox("Has glow", &L->hasGlow);
                if (L->hasGlow) {
                    ImGui::ColorEdit3("Glow Color", &L->glowColor.x);
                    ImGui::DragFloat("Glow Ray", &L->glowRadius, 0.05f, 0.0f, 50.0f);
                }
                ImGui::Checkbox("Enable Shadows", &L->enableShadows);
                ImGui::DragFloat("Shadows Update Period", &L->shadowUpdatePeriod, 0.001f, 0.0f, 1.0f);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    ImGui::Separator();
    ImGui::TextDisabled("Press F1 to open/close this panel.");

    ImGui::End();
}

void update() {
    FiscionX::Core::ClockTick();

    FiscionX::Core::AudioSystem.listenerPos = { FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z };
    FiscionX::Core::AudioSystem.forward = { -FiscionX::Core::Camera.front.x, FiscionX::Core::Camera.front.y , -FiscionX::Core::Camera.front.z };
    FiscionX::Core::AudioSystem.up = { -FiscionX::Core::Camera.up.x, FiscionX::Core::Camera.up.y, -FiscionX::Core::Camera.up.z };

    if (skinnedModel) {
        skinnedModel->update(FiscionX::Core::deltaTime);
    }

    FiscionX::Physics::DynamicWorld->stepSimulation(FiscionX::Core::deltaTime, 10);

    // Toggle do painel de configuracoes
    static bool f1WasPressed = false;
    bool f1IsPressed = FiscionX::Input::GetKeyPressed(FISCIONX_KEY_F1);
    if (f1IsPressed && !f1WasPressed) {
        FiscionX::Core::Camera.canLook = !FiscionX::Core::Camera.canLook;
        showSettingsPanel = !showSettingsPanel;
    }
    f1WasPressed = f1IsPressed;

    ImGuiIO& io = ImGui::GetIO();
    bool wantsCursor = showSettingsPanel;
    if (wantsCursor != settingsPanelHasCursor) {
        settingsPanelHasCursor = wantsCursor;
        FiscionX::Core::SetCursorMode(wantsCursor ? FISCIONX_CURSOR_NORMAL : FISCIONX_CURSOR_DISABLED);
    }

    if (!io.WantCaptureKeyboard) {
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_I)) {
            vehicle->applyEngineForce(800, 0);
            vehicle->applyEngineForce(800, 1);
            dirLight->direction.y += 0.01f;
        }
        else {
            vehicle->setBrake(2, 0);
            vehicle->setBrake(2, 1);
            vehicle->applyEngineForce(0, 0);
            vehicle->applyEngineForce(0, 1);
        }
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_K)) {
            vehicle->setBrake(10, 0);
            vehicle->setBrake(10, 1);
            vehicle->applyEngineForce(-1000, 0);
            vehicle->applyEngineForce(-1000, 1);
            dirLight->direction.y -= 0.01f;
        }
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_J)) {
            vehicle->setSteeringValue(0.5f, 0);
            vehicle->setSteeringValue(0.5f, 1);
            dirLight->direction.x -= 0.01f;
        }
        else if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_L)) {
            vehicle->setSteeringValue(-0.4f, 0);
            vehicle->setSteeringValue(-0.4f, 1);
            dirLight->direction.x += 0.01f;
        }
        else {
            vehicle->setSteeringValue(0, 0);
            vehicle->setSteeringValue(0, 1);
        }
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_T)) {
            FiscionX::Core::LoadHDR("assets/environment/parkinglot.hdr");
            carChassiBody->setTransform(FiscionX::Vector3(3, 10, -4), FiscionX::Vector3(0, 0, 0));
        }

        capsuleBody->activate();

        float camVel = FiscionX::Core::Camera.speed * FiscionX::Core::deltaTime;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_W)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.front * camVel;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_S)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.front * camVel;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_A)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.right * camVel;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_D)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.right * camVel;

        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_I)) dirLight->yaw += 0.04f;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_K)) dirLight->yaw -= 0.04f;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_J)) dirLight->pitch += 0.04f;
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_L)) dirLight->pitch -= 0.04f;

        if (boxModel) {
            if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_M)) {
                boxModel->instances.empty();
                boxModel->unload();
            }
        }
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_ESCAPE)) FiscionX::Core::Terminate();
        if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_Z)) {
            skinnedModel->instances[0].playAnim("CameraAction", false);
        }

        if (skinnedModel) {
            if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_SPACE)) skinnedModel->instances[0].position.y += 0.004f;
            if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_B)) skinnedModel->instances[0].position.z += 0.004f;
        }
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
    FiscionX::Core::Draw::HDR(view, projection);

    FiscionX::Core::SortModels();

    for (FiscionX::Model* model : FiscionX::Core::AllModels) {
        if (model->isSkinned == false) {
            model->draw(FiscionX::Core::shaderStatic, glm::mat4(1.0f), 0, false, view, projection);
        }
        if (model->isSkinned == true) {
            model->draw(FiscionX::Core::shaderSkinned, glm::mat4(1.0f), 0, false, view, projection);
        }
    }

    img3D->draw(view, projection);

    FiscionX::Core::Draw::PostProcessing(viewProj, dirLight); // No godray; Add Bloom

    FiscionX::Core::DrawTransparentPass(view, projection);

    img->draw(FiscionX::Vector2(300, 200));

    // === ImGui: desenhado por cima de tudo, antes do SwapBuffers ===
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawSettingsPanel();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(3048, 1024, 512, { 15.0f, 70.0f, 150.0f }, 0.01f, 1000.0f, true);
    FiscionX::Core::SetCacheSettings(true, true);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");
    FiscionX::Core::SetWindowIcon("assets/icons/fiscionx_logo_big_512.png");
    FiscionX::Core::SetCursorMode(FISCIONX_CURSOR_DISABLED);
    FiscionX::Physics::CreatePhysicsWorld(FiscionX::Vector3(0, -9.81f, 0), 10);
    FiscionX::Core::LoadHDR("assets/environment/gardens.hdr");

    // ImGui precisa ser inicializado DEPOIS de NewWindow (contexto OpenGL/GLFW já existe)
    InitImGui();
    showSettingsPanel = false;

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

    dirLight = new FiscionX::Light();
    dirLight->type = FiscionX::LIGHT_DIRECTIONAL;
    dirLight->yaw = 0;
    dirLight->pitch = -138;
    dirLight->color = FiscionX::Vector3(1.0f, 1.0f, 1.0f);
    dirLight->intensity = 5.0f;
    dirLight->maxDistance = 0.0f;
    dirLight->cutOff = 0.0f;
    dirLight->outerCutOff = 0.0f;
    dirLight->constant = 1.0f;
    dirLight->linear = 0.0f;
    dirLight->quadratic = 0.0f;
    dirLight->hasGlow = false;
    dirLight->enableShadows = true;

    FiscionX::Core::CreateAllShadowMaps();

    staticModel = new FiscionX::Model(
        "assets/models/car_scene.glb"
    );
    staticModel->addInstance(
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0.01f, 0.01f, 0.01f)
    );
    kratosStaticModel = new FiscionX::Model(
        "assets/models/ellie.glb"
    );
    kratosStaticModel->addInstance(
        FiscionX::Vector3(0, 3, 7.0f),
        FiscionX::Vector3(0),
        FiscionX::Vector3(2.1f, 2.1f, 2.1f)
    );
    kratosStaticModel->addInstance(
        FiscionX::Vector3(0, 0, 7.0f),
        FiscionX::Vector3(0),
        FiscionX::Vector3(2.1f, 2.1f, 2.1f)
    );
    boxModel = new FiscionX::Model(
        "assets/models/seoul2.glb"
    );
    boxModel->addInstance(
        FiscionX::Vector3(0, 0.09f, 4.2f),
        FiscionX::Vector3(2, 0, 0),
        FiscionX::Vector3(0.5f, 0.5f, 0.5f)
    );
    skinnedModel = new FiscionX::Model(
        "assets/models/camel.glb"
    );
    skinnedModel->addInstance(
        FiscionX::Vector3(0.0f, 0.0f, 3.0f),
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(1.0f, 1.0f, 1.0f)
    );
    skinnedModel->addInstance(
        FiscionX::Vector3(0.0f, 3.0f, 3.0f),
        FiscionX::Vector3(1.5f, 0, 0),
        FiscionX::Vector3(1.0f, 1.0f, 1.0f)
    );

    skinnedModel->instances[0].playAnim("Armature|Idle_01", true);
    skinnedModel->instances[1].playAnim("Armature|WalkCycle", true);

    staticModel->buildLODs({ 0.5f, 0.25f, 0.1f });
    staticModel->lodDistances = { 10.0f, 40.0f, 250.0f };

    kratosStaticModel->buildLODs({ 0.5f, 0.25f, 0.1f });
    kratosStaticModel->lodDistances = { 10.0f, 40.0f, 250.0f };

    skinnedModel->buildLODs({ 0.5f, 0.25f, 0.1f });
    skinnedModel->lodDistances = { 10.0f, 40.0f, 80.0f };

    boxModel->buildLODs({ 0.5f, 0.25f, 0.1f });
    boxModel->lodDistances = { 10.0f, 40.0f, 80.0f };

    img = new FiscionX::UI::Image("assets/images/didi.png");

    img3D = new FiscionX::Image3D("assets/images/didi.png");
    img3D->position = FiscionX::Vector3(7.0f, 1.0f, 0.0f);

    // Physics
    FiscionX::Physics::Shape groundShape = FiscionX::Physics::CreateBoxShape(FiscionX::Vector3(0, 0, 0), FiscionX::Vector3(0, 0, 0), FiscionX::Vector3(30.0f, 0.01f, 30.0f), 0.0f);
    groundBody = new FiscionX::Physics::Rigidbody(groundShape);
    groundBody->setBouncingFactor(0.0f);
    FiscionX::Physics::DynamicWorld->addRigidBody(groundBody->body);

    // === Capsule ===
    FiscionX::Physics::Shape capsuleShape = FiscionX::Physics::CreateCapsuleShape(FiscionX::Vector3(-11, 4, 0), FiscionX::Vector3(0, 0, 0), 0.5f, 1.5f, 1.0f);
    capsuleBody = new FiscionX::Physics::Rigidbody(capsuleShape);
    capsuleBody->setFriction(0.5f);
    capsuleBody->setRollingFriction(0.3f);
    capsuleBody->setDamping(0.05f);
    capsuleBody->lockAxis(FiscionX::Vector3(1, 1, 1));
    capsuleBody->setBouncingFactor(0.0f);
    FiscionX::Physics::DynamicWorld->addRigidBody(capsuleBody->body);

    // == Mesh Collider ==
    FiscionX::Physics::Shape staticCarShape = FiscionX::Physics::CreateMeshShape("assets/models/car_scene.glb", FiscionX::Vector3(0, 0, 0), FiscionX::Vector3(0, 0, 0), FiscionX::Vector3(0.01f, 0.01f, 0.01f), 0.0f);
    staticCarShape.gshape->updateBound();
    staticCarBody = new FiscionX::Physics::Rigidbody(staticCarShape);
    staticCarBody->setBouncingFactor(0.0f);
    FiscionX::Physics::DynamicWorld->addRigidBody(staticCarBody->body);

    // ==== Car ====
    FiscionX::Physics::Shape carShape = FiscionX::Physics::CreateBoxShape(
        FiscionX::Vector3(-10, 4, 0),
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0.8f, 0.5f, 1.7f),
        900.0f
    );
    carChassiBody = new FiscionX::Physics::Rigidbody(carShape);
    FiscionX::Physics::DynamicWorld->addRigidBody(carChassiBody->body);

    vehicle = new FiscionX::Physics::Vehicle(carChassiBody);

    FiscionX::Vector3 wheelDirectionCS0(0, -1, 0);
    FiscionX::Vector3 wheelAxleCS(-1, 0, 0);

    float suspensionRestLength = 0.6;
    float wheelRadius = 0.3;

    vehicle->addWheel(FiscionX::Vector3(1.0, 0.00f, 1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, true); // front left

    vehicle->addWheel(FiscionX::Vector3(-1.0, 0.00f, 1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, true); // front right

    vehicle->addWheel(FiscionX::Vector3(1.0, 0.00f, -1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, false); // back left

    vehicle->addWheel(FiscionX::Vector3(-1.0, 0.00f, -1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, false); // back right

    FiscionX::Physics::DynamicWorld->addVehicle(vehicle->vehicle);

    for (int i = 0; i < vehicle->getNumWheels(); ++i) {
        FiscionX::Physics::Vehicle::WheelInfo& wheel = vehicle->getWheelInfo(i);

        wheel.info->m_suspensionStiffness = 30.0f;
        wheel.info->m_wheelsDampingCompression = 2.0f;
        wheel.info->m_wheelsDampingRelaxation = 3.5f;

        wheel.info->m_maxSuspensionTravelCm = 500.0f;
        wheel.info->m_maxSuspensionForce = 20000.0f;

        wheel.info->m_frictionSlip = 1500.0f;
        wheel.info->m_rollInfluence = 0.1f;
        wheel.info->m_bIsFrontWheel = (i < 2);
    }

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }

    ShutdownImGui();
    FiscionX::Core::Terminate();
    return 0;
}