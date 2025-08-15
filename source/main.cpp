#include "FiscionCore.h"
#define PROJECT_VERSION "0.8.2"

FiscionX::Light* dirLight;
FiscionX::Light* pointLight;
FiscionX::Light* spotLight;

FiscionX::Sound* exSound;

FiscionX::Model* staticModel;
FiscionX::Model* boxModel;
FiscionX::Model* skinnedModel;
FiscionX::Model* kratosStaticModel;

FiscionX::UI::Image* image_didi;

FiscionX::Physics::Rigidbody* groundBody;
FiscionX::Physics::Rigidbody* capsuleBody;
FiscionX::Physics::Rigidbody* staticCarBody;
FiscionX::Physics::Rigidbody* carChassiBody;
FiscionX::Physics::Vehicle* vehicle;

void update() {
    FiscionX::Core::ClockTick();

    FiscionX::Core::AudioSystem.listenerPos = { FiscionX::Core::Camera.position[0], FiscionX::Core::Camera.position[1], FiscionX::Core::Camera.position[2] };
    FiscionX::Core::AudioSystem.forward = { -FiscionX::Core::Camera.front[0], FiscionX::Core::Camera.front[1], -FiscionX::Core::Camera.front[2] };
    FiscionX::Core::AudioSystem.up = { -FiscionX::Core::Camera.up[0], FiscionX::Core::Camera.up[1], -FiscionX::Core::Camera.up[2] };

    skinnedModel->update(FiscionX::Core::deltaTime);

    FiscionX::Physics::DynamicWorld->stepSimulation(FiscionX::Core::deltaTime);

    // VEHICLE
    vehicle->update(deltaTime);
	
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_I)) {
        vehicle->applyEngineForce(800, 0);
        vehicle->applyEngineForce(800, 1);
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
    }
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_J)) { 
        vehicle->setSteeringValue(0.5f, 0);
        vehicle->setSteeringValue(0.5f, 1);
    }
    else if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_L)) {
        vehicle->setSteeringValue(-0.4f, 0);
        vehicle->setSteeringValue(-0.4f, 1);
    }
    else {
        vehicle->setSteeringValue(0, 0);
        vehicle->setSteeringValue(0, 1);
    }
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_T)) {
        carChassiBody->setTransform(FiscionX::Vector3(3, 10, -4), FiscionX::Vector3(0, 0, 0));
    }

    // CAPSULE & KRATOS
    capsuleBody->activate();
    kratosStaticModel->syncTransformWithBody(capsuleBody, FiscionX::Vector3(0, -1.25f, 0), FiscionX::Vector3(0, 0, 0));
	
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_E)) {
        std::cout << "Is capsule colliding with ground: " << FiscionX::Physics::CheckCollisionBetween(groundBody, capsuleBody) << std::endl;
    }
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_F)) {
		capsuleBody->applyCentralForce(FiscionX::Vector3(2, 0, 0));
	}
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_R)) {
        capsuleBody->applyCentralForce(FiscionX::Vector3(0, 0, 2));
    }

    // RAYCAST
	bool rayCameraCollidingWithCapsuleBody = FiscionX::Physics::Raycast::CheckCollisionWithBody(capsuleBody, FiscionX::Vector3(FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z), FiscionX::Vector3(FiscionX::Core::Camera.position.x + FiscionX::Core::Camera.front.x * 5, FiscionX::Core::Camera.position.y + FiscionX::Core::Camera.front.y * 5, FiscionX::Core::Camera.position.z + FiscionX::Core::Camera.front.z * 5));
    const btRigidBody* firstBodyCollidedWithRay = FiscionX::Physics::Raycast::GetFirstBodyCollided(FiscionX::Vector3(FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z), FiscionX::Vector3(FiscionX::Core::Camera.position.x + FiscionX::Core::Camera.front.x * 5, FiscionX::Core::Camera.position.y + FiscionX::Core::Camera.front.y * 5, FiscionX::Core::Camera.position.z + FiscionX::Core::Camera.front.z * 5));
	
    if (FiscionX::Input::GetMouseButtonPressed(FISCIONX_MOUSE_BUTTON_1)) {
		std::cout << "Is ray colliding with capsule body: " << rayCameraCollidingWithCapsuleBody << std::endl;
    }

    // CAMERA
    float camVel = FiscionX::Core::Camera.speed * FiscionX::Core::deltaTime;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_W)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_S)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.front * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_A)) FiscionX::Core::Camera.position -= FiscionX::Core::Camera.right * camVel;
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_D)) FiscionX::Core::Camera.position += FiscionX::Core::Camera.right * camVel;
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

    image_didi->draw(FiscionX::Vector2(- 0.5f, -0.5f));

    //FiscionX::Physics::DrawDebugWorld(projection, view);

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(4096, 1024, 512, 15.0f, 0.01f, 100.0f);
    FiscionX::Core::SetCacheSettings(true, true);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");
    //FiscionX::Core::SetWindowFullscreen(true, 0);
	FiscionX::Core::SetWindowIcon("assets/icons/fiscionx_logo_big_512.png");
	FiscionX::Core::SetCursorMode(FISCIONX_CURSOR_DISABLED);
    FiscionX::Physics::CreatePhysicsWorld(FiscionX::Vector3(0, -9.81f, 0), 10);

    dirLight = new FiscionX::Light();
    dirLight->type = FiscionX::LIGHT_DIRECTIONAL;
    dirLight->direction = FiscionX::Vector3(0.0f, -1.0f, -1.0f);
    dirLight->color = FiscionX::Vector3(1.0f, 1.0f, 1.0f);
    dirLight->intensity = 1.2f;
    dirLight->maxDistance = 0.0f;
    dirLight->cutOff = 0.0f;
    dirLight->outerCutOff = 0.0f;
    dirLight->constant = 1.0f;
    dirLight->linear = 0.0f;
    dirLight->quadratic = 0.0f;
    dirLight->hasGlow = false;
    dirLight->enableShadows = true;

    pointLight = new FiscionX::Light();
    pointLight->type = FiscionX::LIGHT_POINT;
    pointLight->position = FiscionX::Vector3(0.559f, 3.526f, -2.51f);
    pointLight->color = FiscionX::Vector3(1, 1, 1);
    pointLight->intensity = 25;
    pointLight->constant = 1;
    pointLight->linear = 0.003;
    pointLight->quadratic = 0.012;
    pointLight->maxDistance = 10;
    pointLight->hasGlow = false;
    pointLight->enableShadows = true;

    spotLight = new FiscionX::Light();
    spotLight->type = FiscionX::LIGHT_SPOT;
    spotLight->position = FiscionX::Vector3(0.0f, 1.0f, -4.0f);
    spotLight->direction = FiscionX::Vector3(0.0f, 0.0f, 1.0f);
    spotLight->color = FiscionX::Vector3(0.0f, 1, 0);
    spotLight->intensity = 2.0f;
    spotLight->maxDistance = 15.0f;
    spotLight->cutOff = glm::cos(glm::radians(25.0f));
    spotLight->outerCutOff = glm::cos(glm::radians(30.0f));
    spotLight->constant = 1.0f;
    spotLight->linear = 0.09f;
    spotLight->quadratic = 0.032f;
    spotLight->hasGlow = false;
    spotLight->enableShadows = true;
    
    FiscionX::Core::CreateAllShadowMaps();

    // Text; Video; Fullscreen; Buttons; Sliders; Viewports; Model Cache; Anisotropic Filter and TAA; Particles;
    
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
        FiscionX::Vector3(0, 0, 4.2f),
        FiscionX::Vector3(0),
        FiscionX::Vector3(0.5f, 0.5f, 0.5f)
    );
    skinnedModel = new FiscionX::Model(
        "assets/models/camel.glb",
        FiscionX::Vector3(0.4f, 0, 1.5f),
        FiscionX::Vector3(1, 0, 0),
        FiscionX::Vector3(1, 1, 1)
    );

    skinnedModel->playAnim("Armature|Idle_01", true);

    image_didi = new FiscionX::UI::Image("assets/images/didi.png", FiscionX::Vector2(0.5f, 0.5f));

    exSound = new FiscionX::Sound("assets/audio/music/K.mp3", false, true, FiscionX::Vector3(0.5f, 0.3f, 0.0f), 2.0f, 10.0f, 1.0f);
    exSound->play();
    //exSound->useEffect(FMOD_DSP_TYPE_DISTORTION);
    //exSound->dsp->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL, 1.0f);

    // Physics
	FiscionX::Physics::Shape groundShape = FiscionX::Physics::CreateBoxShape(FiscionX::Vector3(0, 0, 0), FiscionX::Vector3(0, 0, 0), FiscionX::Vector3(30.0f, 0.01f, 30.0f), 0.0f);
	groundBody = new FiscionX::Physics::Rigidbody(groundShape);
    groundBody->setBouncingFactor(0.0f);
    FiscionX::Physics::DynamicWorld->addRigidBody(groundBody->body);

    // === Capsule ===
	FiscionX::Physics::Shape capsuleShape = FiscionX::Physics::CreateCapsuleShape(FiscionX::Vector3(0, 8, 0), FiscionX::Vector3(0, 0, 0), 0.5f, 1.5f, 1.0f);
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
        880.0f
	);
	carChassiBody = new FiscionX::Physics::Rigidbody(carShape);
    FiscionX::Physics::DynamicWorld->addRigidBody(carChassiBody->body);

    vehicle = new FiscionX::Physics::Vehicle(carChassiBody);

    FiscionX::Vector3 wheelDirectionCS0(0, -1, 0);
    FiscionX::Vector3 wheelAxleCS(-1, 0, 0);

    float suspensionRestLength = 0.6;
    float wheelRadius = 0.5;
    
    vehicle->addWheel(FiscionX::Vector3(1.0, 0.1f, 1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, true); // front left

    vehicle->addWheel(FiscionX::Vector3(-1.0, 0.1f, 1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, true); // frontt right

    vehicle->addWheel(FiscionX::Vector3(1.0, 0.1f, -1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, false); // back left

    vehicle->addWheel(FiscionX::Vector3(-1.0, 0.1f, -1.5), wheelDirectionCS0, wheelAxleCS,
        suspensionRestLength, wheelRadius, false); // back right

    
    FiscionX::Physics::DynamicWorld->addVehicle(vehicle->vehicle);

    for (int i = 0; i < vehicle->getNumWheels(); ++i) {
        FiscionX::Physics::Vehicle::WheelInfo& wheel = vehicle->getWheelInfo(i);
        
        wheel.info->m_suspensionStiffness = 14.0f;             // holds well the weight
        wheel.info->m_wheelsDampingCompression = 2.0f;         // absorves impacts
        wheel.info->m_wheelsDampingRelaxation = 3.5f;          // relaxes smoothly

		wheel.info->m_maxSuspensionTravelCm = 150.0f;          // vertical spacement for the wheel to move
        wheel.info->m_maxSuspensionForce = 10000.0f;           // max suspension force

        wheel.info->m_frictionSlip = 1500.0f;                  // great traction
        wheel.info->m_rollInfluence = 0.1f;                    // great grip on the ground
		wheel.info->m_bIsFrontWheel = (i < 2);                 // front
    }

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    system("pause");
    return 0;
}