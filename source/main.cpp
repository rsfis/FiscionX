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

FiscionX::Physics::Rigidbody* groundBody;
FiscionX::Physics::Rigidbody* capsuleBody;
FiscionX::Physics::Rigidbody* staticCarBody;
FiscionX::Physics::Rigidbody* carChassiBody;
FiscionX::Physics::Vehicle* vehicle;

FiscionX::UI::Image* img;
FiscionX::Image3D* img3D;

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

    FiscionX::Physics::DynamicWorld->stepSimulation(FiscionX::Core::deltaTime, 10);
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
        FiscionX::Core::LoadHDR("assets/environment/ferndale_studio_12_2k.hdr");
        carChassiBody->setTransform(FiscionX::Vector3(3, 10, -4), FiscionX::Vector3(0, 0, 0));
    }

    //FiscionX::Vector3 d = FiscionX::Math::lookAt3D(kratosStaticModel->position, FiscionX::Core::Camera.position - FiscionX::Vector3(0.0f, 1.0f, 0.0f));
    //FiscionX::Vector3 eu = FiscionX::Math::toEulerAngles(d, FiscionX::Vector2(1.0f, 1.0f));
    //kratosStaticModel->rotation = eu;

    // CAPSULE & KRATOS
    capsuleBody->activate();
    //kratosStaticModel->syncTransformWithBody(capsuleBody, FiscionX::Vector3(0, -1.25f, 0), FiscionX::Vector3(0, 0, 0));

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
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_Z)) {
        skinnedModel->playAnim("CameraAction", false);
    }

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
    FiscionX::Core::DrawTransparentPass(view, projection);

    img3D->draw(view, projection);

    img->draw(FiscionX::Vector2(300, 200));

    FiscionX::Core::Draw::PostProcessing(viewProj, dirLight); // No godray; Add Bloom
    //FiscionX::Physics::DrawDebugWorld(projection, view);
    
    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(3048, 1024, 512, { 15.0f, 70.0f, 150.0f }, 0.01f, 1000.0f, true);
    FiscionX::Core::SetCacheSettings(true, true);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");
    //FiscionX::Core::SetWindowFullscreen(true, 0);
    FiscionX::Core::SetWindowIcon("assets/icons/fiscionx_logo_big_512.png");
    FiscionX::Core::SetCursorMode(FISCIONX_CURSOR_DISABLED);
    FiscionX::Physics::CreatePhysicsWorld(FiscionX::Vector3(0, -9.81f, 0), 10);
    FiscionX::Core::LoadHDR("assets/environment/puresky.hdr");

    FiscionX::Core::AMBIENT_LIGHT_INTENSITY = 0.6f;
    FiscionX::Core::HDR_EXPOSURE = 1.0f;

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

    /*
    spotLight = new FiscionX::Light();
    spotLight->type = FiscionX::LIGHT_SPOT;
    spotLight->position = FiscionX::Vector3(0.0f, 1.0f, -4.0f);
    spotLight->direction = FiscionX::Vector3(0.0f, 0.0f, 0.0f);
    spotLight->color = FiscionX::Vector3(1.0f, 0.0f, 0.0f);
    spotLight->intensity = 15.0f;
    spotLight->maxDistance = 30.0f;
    spotLight->cutOff = FiscionX::Math::cos(FiscionX::Math::radians(25.0f));
    spotLight->outerCutOff = FiscionX::Math::cos(FiscionX::Math::radians(30.0f));
    spotLight->constant = 1.0f;
    spotLight->linear = 0.09f;
    spotLight->quadratic = 0.032f;
    spotLight->hasGlow = true;
    spotLight->enableShadows = true;
    */

    FiscionX::Core::CreateAllShadowMaps();

    // Real HDR; Displacement Map & SPOM; Emission Textures; Lights extracted from glb; Contact Shadows; Anim Blending; Draw halo and glow also for point lights and spot lights; Point lights are traversing walls/solid objects (Criação da textura está errada, computar está errado, renderizar está errado); Sombras e Luz devem passar por malhas com transparência; Sliders; Viewports; UI Masks; Model Cache; Particles; Fog; Ambient Occlusion; Terrains; Water

    staticModel = new FiscionX::Model(
        "assets/models/car_scene.glb",
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0, 0, 0),
        FiscionX::Vector3(0.01f, 0.01f, 0.01f)
    );
    kratosStaticModel = new FiscionX::Model(
        "assets/models/kratos.glb",
        FiscionX::Vector3(0, 3, 7.0f),
        FiscionX::Vector3(0),
        FiscionX::Vector3(0.1f, 0.1f, 0.1f)
    );
    boxModel = new FiscionX::Model(
        "assets/models/factory.glb",
        FiscionX::Vector3(0, 0.09f, 4.2f),
        FiscionX::Vector3(2, 0, 0),
        FiscionX::Vector3(0.5f, 0.5f, 0.5f)
    );
    skinnedModel = new FiscionX::Model(
        "assets/models/camel.glb", // camel.glb / camera_test_anim.glb
        FiscionX::Vector3(0.0f, 0.0f, 3.0f), // 0.0f, 0.0f, 3.0f / 0.0f, 0.0f, 0.0f
        FiscionX::Vector3(1, 0, 0), // 1, 0, 0 / 0, 0, 0
        FiscionX::Vector3(1.0f, 1.0f, 1.0f) // 1.0f, 1.0f, 1.0f / 0.6f, 0.6f, 0.6f
    );

    skinnedModel->playAnim("Armature|Idle_01", true); // Armature|Idle_01 / CameraAction


    staticModel->buildLODs({ 0.5f, 0.25f, 0.1f }); // gera 3 LODs
    staticModel->lodDistances = { 10.0f, 40.0f, 250.0f }; // distâncias de transição
    staticModel->enableFrustumCulling = true;

    kratosStaticModel->buildLODs({ 0.5f, 0.25f, 0.1f }); // gera 3 LODs
    kratosStaticModel->lodDistances = { 10.0f, 40.0f, 250.0f }; // distâncias de transição
    kratosStaticModel->enableFrustumCulling = true;

    skinnedModel->buildLODs({ 0.5f, 0.25f, 0.1f }); // gera 3 LODs
    skinnedModel->lodDistances = { 10.0f, 40.0f, 80.0f }; // distâncias de transição
    skinnedModel->enableFrustumCulling = true;

    boxModel->buildLODs({ 0.5f, 0.25f, 0.1f }); // gera 3 LODs
    boxModel->lodDistances = { 10.0f, 40.0f, 80.0f }; // distâncias de transição
    boxModel->enableFrustumCulling = true;

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
        suspensionRestLength, wheelRadius, true); // frontt right

    vehicle->addWheel(FiscionX::Vector3(1.0, 0.00f, -1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, false); // back left

    vehicle->addWheel(FiscionX::Vector3(-1.0, 0.00f, -1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, false); // back right

    FiscionX::Physics::DynamicWorld->addVehicle(vehicle->vehicle);

    for (int i = 0; i < vehicle->getNumWheels(); ++i) {
        FiscionX::Physics::Vehicle::WheelInfo& wheel = vehicle->getWheelInfo(i);

        wheel.info->m_suspensionStiffness = 30.0f;             // holds well the weight
        wheel.info->m_wheelsDampingCompression = 2.0f;         // absorves impacts
        wheel.info->m_wheelsDampingRelaxation = 3.5f;          // relaxes smoothly

        wheel.info->m_maxSuspensionTravelCm = 500.0f;          // vertical spacement for the wheel to move
        wheel.info->m_maxSuspensionForce = 20000.0f;           // max suspension force

        wheel.info->m_frictionSlip = 1500.0f;                  // great traction
        wheel.info->m_rollInfluence = 0.1f;                    // great grip on the ground
        wheel.info->m_bIsFrontWheel = (i < 2);                 // front
    }

    // JOINTS
    /*FiscionX::Physics::Joint j;
    j.type = FiscionX::Physics::JointType::CONETWIST;
    j.bodyA = carChassiBody->body;
    j.bodyB = capsuleBody->body;

    j.frameA.setOrigin(btVector3(0.0, 0.5, 0));
    j.frameB.setOrigin(btVector3(0, -0.75, 0));

    j.collideConnected = false;

    j.swing1 = SIMD_PI * 0.7f;
    j.swing2 = SIMD_PI * 0.7f;
    j.twist = SIMD_PI * 0.5f;

    FiscionX::Physics::CreateJoint(j);
    */

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    return 0;
}