#include "FiscionCore.h"
#define PROJECT_VERSION "1.0.0"

using namespace FiscionX;

FiscionX::Light* dirLight;

// ---- Player ----
Model* pistol;
Physics::Rigidbody* playerBody = nullptr;
const float PLAYER_RADIUS = 0.4f;
const float PLAYER_CAPSULE_HEIGHT = 1.0f;
const float PLAYER_TOTAL_HEIGHT = PLAYER_CAPSULE_HEIGHT + 2.0f * PLAYER_RADIUS;
const float EYE_OFFSET = PLAYER_TOTAL_HEIGHT * 0.5f - 0.2f;
const float MOVE_SPEED = 6.0f;
const float SPRINT_MULT = 1.6f;
const float JUMP_FORCE = 6.5f;
bool isGrounded = false;

std::vector<Physics::Rigidbody*> worldBodies;

// ---- Shot ----
float shootCooldown = 0.0f;
const float SHOOT_COOLDOWN_TIME = 0.2f;

struct DebugTracer {
    Vector3 a, b, color;
    float life;
};
std::vector<DebugTracer> activeTracers;
const float TRACER_LIFETIME = 2.0f;

Physics::Rigidbody* CreateStaticBox(Vector3 position, Vector3 halfExtents) {
    Physics::Shape shape = Physics::CreateBoxShape(position, Vector3(0, 0, 0), halfExtents, 0.0f);
    Physics::Rigidbody* body = new Physics::Rigidbody(shape);
    body->setFriction(0.9f);
    Physics::DynamicWorld->addRigidBody(body->body);
    worldBodies.push_back(body);
    return body;
}

void BuildArena() {
    // Floor
    CreateStaticBox(Vector3(0, -0.5f, 0), Vector3(60.0f, 0.5f, 60.0f));

    // Arena WAlls
    CreateStaticBox(Vector3(0, 4.0f, 60.0f), Vector3(60.0f, 4.0f, 1.0f));
    CreateStaticBox(Vector3(0, 4.0f, -60.0f), Vector3(60.0f, 4.0f, 1.0f));
    CreateStaticBox(Vector3(60.0f, 4.0f, 0), Vector3(1.0f, 4.0f, 60.0f));
    CreateStaticBox(Vector3(-60.0f, 4.0f, 0), Vector3(1.0f, 4.0f, 60.0f));

    // Blocks
    CreateStaticBox(Vector3(6, 1.0f, 4), Vector3(1.0f, 1.0f, 1.0f));
    CreateStaticBox(Vector3(-8, 1.5f, 10), Vector3(1.5f, 1.5f, 1.5f));
    CreateStaticBox(Vector3(12, 1.0f, -6), Vector3(1.0f, 1.0f, 3.0f));
    CreateStaticBox(Vector3(-15, 2.0f, -12), Vector3(2.0f, 2.0f, 0.5f));
    CreateStaticBox(Vector3(0, 1.0f, -20), Vector3(4.0f, 1.0f, 1.0f));
    CreateStaticBox(Vector3(20, 3.0f, 0), Vector3(1.0f, 3.0f, 1.0f));
    CreateStaticBox(Vector3(-20, 0.75f, 0), Vector3(3.0f, 0.75f, 3.0f));

    // Small Tower
    CreateStaticBox(Vector3(-6, 0.5f, -6), Vector3(2.0f, 0.5f, 2.0f));
    CreateStaticBox(Vector3(-6, 1.5f, -6), Vector3(1.5f, 0.5f, 1.5f));
    CreateStaticBox(Vector3(-6, 2.5f, -6), Vector3(1.0f, 0.5f, 1.0f));
}

void CreatePlayer() {
    Physics::Shape shape = Physics::CreateCapsuleShape(Vector3(0, 5, 0), Vector3(0, 0, 0),
        PLAYER_RADIUS, PLAYER_CAPSULE_HEIGHT, 80.0f);
    playerBody = new Physics::Rigidbody(shape);
    playerBody->setFriction(0.0f);
    playerBody->setDamping(0.0f);
    playerBody->setBouncingFactor(0.0f);
    playerBody->lockAxis(Vector3(0, 0, 0));
    playerBody->activate();
    Physics::DynamicWorld->addRigidBody(playerBody->body);
}

bool CheckGrounded() {
    Vector3 pos = playerBody->getPosition();
    float halfHeight = PLAYER_TOTAL_HEIGHT * 0.5f;

    Vector3 origin(pos.x, pos.y - halfHeight + 0.05f, pos.z);
    Vector3 end(pos.x, pos.y - halfHeight - 0.25f, pos.z);

    const btRigidBody* hit = Physics::Raycast::GetFirstBodyCollided(origin, end);
    return hit != nullptr && hit != playerBody->body;
}

void UpdatePlayerMovement() {
    isGrounded = CheckGrounded();

    Vector3 camFront = Core::Camera.front;
    Vector3 camRight = Core::Camera.right;

    Vector3 forward = Vector3(camFront.x, 0.0f, camFront.z).normalized();
    Vector3 right = Vector3(camRight.x, 0.0f, camRight.z).normalized();

    Vector3 moveDir(0, 0, 0);
    if (Input::GetKeyPressed(FISCIONX_KEY_W)) moveDir += forward;
    if (Input::GetKeyPressed(FISCIONX_KEY_S)) moveDir -= forward;
    if (Input::GetKeyPressed(FISCIONX_KEY_D)) moveDir += right;
    if (Input::GetKeyPressed(FISCIONX_KEY_A)) moveDir -= right;
    if (moveDir.lengthSquared() > 0.0001f) moveDir = moveDir.normalized();

    float speed = MOVE_SPEED;
    if (Input::GetKeyPressed(FISCIONX_KEY_LEFT_SHIFT)) speed *= SPRINT_MULT;

    btVector3 currentVel = playerBody->body->getLinearVelocity();
    float verticalVel = currentVel.y();

    if (isGrounded && Input::GetKeyPressed(FISCIONX_KEY_SPACE)) {
        verticalVel = JUMP_FORCE;
    }

    Vector3 horizontal = moveDir * speed;

    playerBody->activate();
    playerBody->setLinearVelocity(Vector3(horizontal.x, verticalVel, horizontal.z));
}

void SyncCameraToPlayer() {
    Vector3 pos = playerBody->getPosition();
    Core::Camera.position = Vector3(pos.x, pos.y + EYE_OFFSET, pos.z);
}

void PushDebugLine(Vector3 a, Vector3 b, Vector3 color) {
    Physics::debugLines.push_back(a.x); Physics::debugLines.push_back(a.y); Physics::debugLines.push_back(a.z);
    Physics::debugLines.push_back(color.x); Physics::debugLines.push_back(color.y); Physics::debugLines.push_back(color.z);

    Physics::debugLines.push_back(b.x); Physics::debugLines.push_back(b.y); Physics::debugLines.push_back(b.z);
    Physics::debugLines.push_back(color.x); Physics::debugLines.push_back(color.y); Physics::debugLines.push_back(color.z);
}

void PushImpactMarker(Vector3 point, Vector3 color) {
    float s = 0.2f;
    PushDebugLine(point + Vector3(s, 0, 0), point - Vector3(s, 0, 0), color);
    PushDebugLine(point + Vector3(0, s, 0), point - Vector3(0, s, 0), color);
    PushDebugLine(point + Vector3(0, 0, s), point - Vector3(0, 0, s), color);
}

void AddTracerLine(Vector3 a, Vector3 b, Vector3 color) {
    activeTracers.push_back({ a, b, color, TRACER_LIFETIME });
}

void AddImpactMarkerTracer(Vector3 point, Vector3 color) {
    float s = 0.2f;
    AddTracerLine(point + Vector3(s, 0, 0), point - Vector3(s, 0, 0), color);
    AddTracerLine(point + Vector3(0, s, 0), point - Vector3(0, s, 0), color);
    AddTracerLine(point + Vector3(0, 0, s), point - Vector3(0, 0, s), color);
}

void UpdateAndPushTracers(float dt) {
    for (int i = (int)activeTracers.size() - 1; i >= 0; --i) {
        activeTracers[i].life -= dt;
        if (activeTracers[i].life <= 0.0f) {
            activeTracers.erase(activeTracers.begin() + i);
        }
    }

    for (const DebugTracer& t : activeTracers) {
        PushDebugLine(t.a, t.b, t.color);
    }
}

void HandleShooting(float dt) {
    shootCooldown -= dt;

    bool mouseDown = Input::GetMouseButtonPressed(FISCIONX_MOUSE_BUTTON_LEFT);

    if (!mouseDown) return;
    if (shootCooldown > 0.0f) return;
    shootCooldown = SHOOT_COOLDOWN_TIME;

    pistol->instances[0].playAnim("Armature|FPS_Pistol_Fire", false, "Armature|FPS_Pistol_Idle");

    Vector3 dir = Core::Camera.front.normalized();

    // Pushes ray origin out of the player collider
    // 0.4 ray + security offset
    const float SELF_CLEAR_DIST = PLAYER_RADIUS + 0.15f;
    Vector3 origin = Core::Camera.position + dir * SELF_CLEAR_DIST;

    const float range = 150.0f;
    Vector3 end = origin + dir * range;

    Vector3 hitPointOnSurface, hitNormal;
    const btRigidBody* hit = Physics::Raycast::GetFirstBodyCollided(origin, end, hitPointOnSurface, hitNormal);

    Vector3 hitPoint = end;
    Vector3 tracerColor(1.0f, 0.85f, 0.2f);

    if (hit != nullptr && hit != playerBody->body) {
        hitPoint = hitPointOnSurface;
        tracerColor = Vector3(1.0f, 0.15f, 0.1f);
        AddImpactMarkerTracer(hitPoint, tracerColor);
    }

    AddTracerLine(origin, hitPoint, tracerColor);
}

void DrawCrosshair() {
    Vector2 center((float)Core::SCREEN_WIDTH * 0.5f, (float)Core::SCREEN_HEIGHT * 0.5f);
    Vector4 color(1.0f, 1.0f, 1.0f, 0.85f);
    float len = 8.0f;
    Core::Draw::DrawLine(center - Vector2(len, 0), center + Vector2(len, 0), 2.0f, color);
    Core::Draw::DrawLine(center - Vector2(0, len), center + Vector2(0, len), 2.0f, color);
}

const glm::vec3 PISTOL_LOCAL_CORRECTION(0.0f, 3.05f, 0.2f);
void SyncPistolToCamera() {
    const Vector3 VIEWMODEL_OFFSET(-0.08f, -0.25f, 0.1f);

    Vector3 front = Core::Camera.front.normalized();
    Vector3 right = Core::Camera.right.normalized();
    Vector3 up = Core::Camera.up.normalized();

    // ---- Posição ----
    Vector3 pos = Core::Camera.position
        + right * VIEWMODEL_OFFSET.x
        + up * VIEWMODEL_OFFSET.y
        + front * VIEWMODEL_OFFSET.z;

    glm::mat3 camBasis(
        right.x, right.y, right.z,
        up.x, up.y, up.z,
        -front.x, -front.y, -front.z
    );

    glm::mat3 correction = glm::mat3(glm::eulerAngleXYZ(
        PISTOL_LOCAL_CORRECTION.x, PISTOL_LOCAL_CORRECTION.y, PISTOL_LOCAL_CORRECTION.z));

    glm::mat4 finalRot = glm::mat4(camBasis) * glm::mat4(correction);

    float angleX, angleY, angleZ;
    glm::extractEulerAngleXYZ(finalRot, angleX, angleY, angleZ);

    pistol->instances[0].position = pos;
    pistol->instances[0].rotation = Vector3(angleY, angleX, angleZ);
}

void update() {
    FiscionX::Core::ClockTick();

    FiscionX::Core::AudioSystem.listenerPos = { FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z };
    FiscionX::Core::AudioSystem.forward = { -FiscionX::Core::Camera.front.x, FiscionX::Core::Camera.front.y , -FiscionX::Core::Camera.front.z };
    FiscionX::Core::AudioSystem.up = { -FiscionX::Core::Camera.up.x, FiscionX::Core::Camera.up.y, -FiscionX::Core::Camera.up.z };

    if (Input::GetKeyPressed(FISCIONX_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(FiscionX::Core::Window, true);
    }

    if (Input::GetKeyPressed(FISCIONX_KEY_R)) {
        pistol->instances[0].playAnim("Armature|FPS_Pistol_Reload_full", false, "Armature|FPS_Pistol_Idle");
    }

    pistol->update(Core::deltaTime);
    SyncPistolToCamera();

    UpdatePlayerMovement();
    FiscionX::Physics::DynamicWorld->stepSimulation(FiscionX::Core::deltaTime, 10);
    SyncCameraToPlayer();
    HandleShooting(FiscionX::Core::deltaTime);
}

void draw() {
    FiscionX::Mat4 view = FiscionX::Core::Camera.GetView();
    FiscionX::Mat4 projection = FiscionX::Mat4(0).perspective(
        glm::radians(FiscionX::Core::Camera.fov),
        (float)FiscionX::Core::SCREEN_WIDTH / FiscionX::Core::SCREEN_HEIGHT,
        FiscionX::Core::NEAR_PLANE,
        FiscionX::Core::FAR_PLANE
    );
    FiscionX::Mat4 viewProj = projection * view;
    FiscionX::Core::RenderAllShadowPasses(view, projection, viewProj);
    FiscionX::Core::Draw::ClearBackground(0.15f, 0.16f, 0.22f, 1.0f);
    FiscionX::Core::SortModels();
    FiscionX::Core::Draw::PostProcessing(viewProj, dirLight);

    pistol->draw(Core::shaderSkinned, 0, 0, false, view, projection, false);

    UpdateAndPushTracers(FiscionX::Core::deltaTime);
    FiscionX::Physics::DrawDebugWorld((glm::mat4)projection, (glm::mat4)view);
    DrawCrosshair();

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(5120, 1024, 512, { 15.0f, 70.0f, 150.0f }, 0.01f, 3000.0f, false);
    FiscionX::Core::SetCacheSettings(true, true);
    FiscionX::Core::NewWindow(1280, 720, "FiscionX - FPS Controller (Debug World)");
    FiscionX::Core::SetWindowIcon("assets/icons/fiscionx_logo_big_512.png");
    FiscionX::Core::SetCursorMode(FISCIONX_CURSOR_DISABLED);
    FiscionX::Core::LoadHDR("gardens.hdr");

    FiscionX::Physics::CreatePhysicsWorld(FiscionX::Vector3(0, -20.0f, 0), 10);

    FiscionX::Core::AMBIENT_LIGHT_INTENSITY = 2.0f;
    FiscionX::Core::HDR_EXPOSURE = 1.0f;

    FiscionX::Core::SSR_ENABLED = false;
    FiscionX::Core::SSAO_ENABLED = false;

    dirLight = new FiscionX::Light();
    dirLight->type = FiscionX::LIGHT_DIRECTIONAL;
    dirLight->yaw = 0;
    dirLight->pitch = -138;
    dirLight->color = FiscionX::Vector3(1.0f, 1.0f, 1.0f);
    dirLight->intensity = 3.0f;
    dirLight->constant = 1.0f;
    dirLight->hasGlow = false;
    dirLight->enableShadows = true;

    pistol = new Model("pistol.glb");
    pistol->addInstance(
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(1.0f, 1.0f, 1.0f)
    );
    pistol->instances[0].playAnim("Armature|FPS_Pistol_Idle", true, ""); // Idle, Walk, Fire, Reload_full

    FiscionX::Core::CreateAllShadowMaps();

    CreatePlayer();
    BuildArena();

    // Camera starts on player's eye position
    FiscionX::Core::Camera.position = FiscionX::Vector3(0, 5 + EYE_OFFSET, 0);
    FiscionX::Core::Camera.speed = MOVE_SPEED; // not used

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }

    FiscionX::Core::Terminate();
    system("pause");
    return 0;
}