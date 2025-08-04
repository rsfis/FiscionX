#include "FiscionCore.h"
#define PROJECT_VERSION "0.6.5"

FiscionX::Light* dirLight;
FiscionX::Light* pointLight;
FiscionX::Light* spotLight;

FiscionX::Sound* exSound;

FiscionX::Model* staticModel;
FiscionX::Model* boxModel;
FiscionX::Model* skinnedModel;
FiscionX::Model* kratosStaticModel;

FiscionX::UI::Image* image_didi;

btRigidBody* groundBody;
btRigidBody* capsuleBody;

GLuint debugVAO = 0, debugVBO = 0;
GLuint debugShader = 0;
std::vector<float> debugLines;

const char* vertexDebug = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fragColor;

uniform mat4 viewProj;

void main() {
    fragColor = aColor;
    gl_Position = viewProj * vec4(aPos, 1.0);
}
)";

const char* fragmentDebug = R"(
#version 330 core
in vec3 fragColor;
out vec4 color;

void main() {
    color = vec4(fragColor, 1.0);
}
)";

struct GLDebugDrawer : public btIDebugDraw {
    int m_debugMode = DBG_DrawWireframe;

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override {
        debugLines.push_back(from.getX());
        debugLines.push_back(from.getY());
        debugLines.push_back(from.getZ());
        debugLines.push_back(color.getX());
        debugLines.push_back(color.getY());
        debugLines.push_back(color.getZ());

        debugLines.push_back(to.getX());
        debugLines.push_back(to.getY());
        debugLines.push_back(to.getZ());
        debugLines.push_back(color.getX());
        debugLines.push_back(color.getY());
        debugLines.push_back(color.getZ());
    }

    void setDebugMode(int debugMode) override {
        m_debugMode = debugMode;
    }

    int getDebugMode() const override {
        return m_debugMode;
    }

    // Métodos obrigatórios da classe base
    void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) override {}
    void reportErrorWarning(const char* warningString) override {
        std::cerr << "Bullet Warning: " << warningString << std::endl;
    }
    void draw3dText(const btVector3&, const char*) override {}
};

void update() {
    FiscionX::Core::ClockTick();
    skinnedModel->update(FiscionX::Core::deltaTime);

    capsuleBody->activate();
    FiscionX::Physics::DynamicWorld->stepSimulation(FiscionX::Core::deltaTime);

    // SYNC KRATOS POSITION AND ROTATION TO CAPSULE'S BODY
    btTransform trans;
    capsuleBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    //kratosStaticModel->position = glm::vec3(pos.getX(), pos.getY(), pos.getZ());

    btScalar matrix[16];
    trans.getOpenGLMatrix(matrix);
    glm::mat4 modelMatrix = glm::make_mat4(matrix);

    float capsuleHeight = 2.0f;
    float verticalOffset = -capsuleHeight / 2.0f;

    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, verticalOffset, 0.0f));

    kratosStaticModel->physicsSyncTransformMatrix = modelMatrix;

    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_F)) {
		capsuleBody->applyCentralForce(btVector3(1, 0, 0));
	}
    if (FiscionX::Input::GetKeyPressed(FISCIONX_KEY_R)) {
        capsuleBody->applyCentralForce(btVector3(0, 0, 1));
    }

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

    image_didi->draw(-0.5f, -0.5f);

    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view));
    FiscionX::Physics::DynamicWorld->debugDrawWorld();
    glUseProgram(debugShader);
    if (!debugLines.empty()) {
        if (debugVAO == 0) {
            glGenVertexArrays(1, &debugVAO);
            glGenBuffers(1, &debugVBO);
        }

        glBindVertexArray(debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
        glBufferData(GL_ARRAY_BUFFER, debugLines.size() * sizeof(float), debugLines.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); // color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        glUseProgram(debugShader);
        glm::mat4 vp = projection * view;
        glUniformMatrix4fv(glGetUniformLocation(debugShader, "viewProj"), 1, GL_FALSE, glm::value_ptr(vp));

        glDrawArrays(GL_LINES, 0, debugLines.size() / 6);

        glBindVertexArray(0);
        glUseProgram(0);
        debugLines.clear();
    }
    glEnable(GL_DEPTH_TEST);

    FiscionX::Core::Draw::SwapBuffers();
}

int main() {
    FiscionX::Core::Set3DSettings(4096, 4096, 4096, 15.0f, 0.01f, 100.0f);
    FiscionX::Physics::CreatePhysicsWorld(btVector3(0, -9.81f, 0));
    FiscionX::Core::NewWindow(1280, 720, "FiscionX");

    debugShader = LoadShader(vertexDebug, fragmentDebug);

    if (debugShader == 0) {
        std::cerr << "[FATAL] Falha ao compilar/linkar o shader de debug!" << std::endl;
        exit(EXIT_FAILURE); // evita crash depois
    }

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

    // === Corpo do chão ===
    btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btDefaultMotionState* groundMotion = new btDefaultMotionState(btTransform::getIdentity());
    btRigidBody::btRigidBodyConstructionInfo groundInfo(0, groundMotion, groundShape);
    groundBody = new btRigidBody(groundInfo);
    FiscionX::Physics::DynamicWorld->addRigidBody(groundBody);

    // === Cápsula ===
    btCollisionShape* capsuleShape = new btCapsuleShape(0.5f, 1.5f);
    btTransform capsuleStart;
    capsuleStart.setIdentity();
	capsuleStart.setRotation(btQuaternion(0, 0, 0));
    capsuleStart.setOrigin(btVector3(0, 5, 0));
    btScalar mass = 1.0f;
    btVector3 inertia(0, 0, 0);
	btScalar friction(0.3f);
    capsuleShape->calculateLocalInertia(mass, inertia);
    btDefaultMotionState* capsuleMotion = new btDefaultMotionState(capsuleStart);
    btRigidBody::btRigidBodyConstructionInfo capsuleInfo(mass, capsuleMotion, capsuleShape, inertia);
    capsuleBody = new btRigidBody(capsuleInfo);
	capsuleBody->setFriction(friction);
	capsuleBody->setRollingFriction(friction/5);
    capsuleBody->setDamping(0.05f, 0.05f); // resistencia do ar
    capsuleBody->setCcdMotionThreshold(0.001f);
    capsuleBody->setCcdSweptSphereRadius(0.3f);
    FiscionX::Physics::DynamicWorld->addRigidBody(capsuleBody);

    GLDebugDrawer* debugDrawer = new GLDebugDrawer();
    debugDrawer->setDebugMode(
        btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb
    );
    FiscionX::Physics::DynamicWorld->setDebugDrawer(debugDrawer);

    while (!glfwWindowShouldClose(FiscionX::Core::Window)) {
        update();
        draw();
    }
    FiscionX::Core::Terminate();
    system("pause");
    return 0;
}