#include "FiscionCore.h"
#define ENGINE_VERSION "1.0.0"

// GLOBALS
GLFWwindow* FiscionX::Core::Window;
int FiscionX::Core::SCREEN_WIDTH, FiscionX::Core::SCREEN_HEIGHT;

GLuint FiscionX::Core::depthShaderStatic;
GLuint FiscionX::Core::depthShaderSkinned;
GLuint FiscionX::Core::depthShaderCubeStatic;
GLuint FiscionX::Core::depthShaderCubeSkinned;
GLuint FiscionX::Core::shaderStatic;
GLuint FiscionX::Core::shaderSkinned;

unsigned int FiscionX::Core::SHADOW_WIDTH;
unsigned int FiscionX::Core::SHADOW_HEIGHT;
unsigned int FiscionX::Core::SHADOW_CUBE_SIZE;
float        FiscionX::Core::NEAR_PLANE;
float         FiscionX::Core::FAR_PLANE;
float         FiscionX::Core::SHADOW_VIEW_RADIUS;
float         FiscionX::Core::AMBIENT_LIGHT_INTENSITY = 1.0f;
glm::vec3     FiscionX::Core::AMBIENT_LIGHT_SKYCOLOR = { 0.3f, 0.3f, 0.35f };
glm::vec3     FiscionX::Core::AMBIENT_LIGHT_GROUNDCOLOR = { 0.05f, 0.05f, 0.07f };
GLuint FiscionX::Core::depthMapFBO;
GLuint FiscionX::Core::depthMap;

FiscionX::AudioSystem FiscionX::Core::AudioSystem;

std::vector<FiscionX::Sound> FiscionX::Core::AllSounds;
std::vector<FiscionX::Model*> FiscionX::Core::AllModels;
std::vector<FiscionX::Light*> FiscionX::Core::AllLights;
std::vector<FiscionX::ShadowMap> FiscionX::Core::AllShadowMaps;

float FiscionX::Core::lastFrame;
float FiscionX::Core::deltaTime;
int FiscionX::Core::FPS;
float FiscionX::Core::lastFPSTime;

float lastX = 640, lastY = 360;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0;

// =================== Shader Loader ===================
GLuint LoadShader(const char* vertexSrc, const char* fragmentSrc);

// ====================== Math ========================
float FiscionX::Math::getDistance3D(glm::vec3 pos1, glm::vec3 pos2) {
    return std::sqrt(std::pow(pos2.x - pos1.x, 2) + std::pow(pos2.y - pos1.y, 2) + std::pow(pos2.z - pos1.z, 2));
}

// =================== File System ====================
    File::File(std::string _path) {
        path = _path;

        if (std::filesystem::exists(path)) {
            readFile();
        }
        else {
            std::ofstream novoArquivo(path);
            if (novoArquivo.is_open()) {
                readFile();
                std::cout << "File created at " << path << std::endl;
            }
            else {
                std::cerr << "ERR 0x008 - Couldn't create new file at " << path << std::endl;
                glfwTerminate();
                system("pause");
                std::exit(-8);
            }
        }
    }

    void File::readFile() {
        std::fstream rawfile(path);

        if (rawfile.is_open()) {
            for (std::string line; std::getline(rawfile, line);) {
                file += line + "\n";
            }
        }
        else {
            std::cerr << "ERR 0x009 - Couldn't read file at " << path << std::endl;
            glfwTerminate();
            system("pause");
            std::exit(-9);
        }
    }

    void File::clearFile() {
        rawfile.clear();
        file.clear();
    }

    void File::saveFile(std::string outputPath) {
        std::ofstream outFile(outputPath, std::ios::out | std::ios::trunc); // flags to indicate it is an output file and will be truncated (clear) before the fileStr being paste
        outFile << file;
        outFile.close();
    }

    /* HOW TO USE:
        File* newFileCreation = new File("savefile.giromba");
        newFileCreation->file += "Creating File Example\nHello!";
        newFileCreation->saveFile("savefile.giromba");
        newFileCreation->rawfile.close();
        free(newFileCreation);

        File* readingFile = new File("savefile.giromba");
        std::cout << readingFile->file;
        readingFile->rawfile.close();
        free(readingFile);
    */

// =================== UI ====================
// ================= IMAGES ==================

    FiscionX::UI::Image::Image(const char* path, float sx, float sy) {
        int w, h, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path, &w, &h, &channels, STBI_rgb_alpha);
        if (!data) {
            std::cerr << "ERR 0x007 - Cannot create raw image texture: " << stbi_failure_reason() << std::endl;
            glfwTerminate();
            system("pause");
            std::exit(-7);
        }

        w_ = w;
        h_ = h;
        aspect_ratio = (float)w / (float)h;

        scale = glm::vec2(sx, sy);

        // Cria textura
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GLfloat maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

        stbi_image_free(data);

        float _w = 1.0f;
        float _h = 1.0f / aspect_ratio;

        float quad[] = {
            -_w, -_h, 0.0f, 0.0f,
             _w, -_h, 1.0f, 0.0f,
             _w,  _h, 1.0f, 1.0f,
            -_w,  _h, 0.0f, 1.0f
        };

        GLuint indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        GLuint EBO;
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void FiscionX::UI::Image::flip(bool flipx, bool flipy) {
        float u0 = flipx ? 1.0f : 0.0f;
        float u1 = flipx ? 0.0f : 1.0f;
        float v0 = flipy ? 1.0f : 0.0f;
        float v1 = flipy ? 0.0f : 1.0f;

        float _w = 1.0f;
        float _h = 1.0f / aspect_ratio;

        float quad[] = {
            -_w, -_h, u0, v0,
             _w, -_h, u1, v0,
             _w,  _h, u1, v1,
            -_w,  _h, u0, v1
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
    }

    void FiscionX::UI::Image::draw(float x, float y) {
        glDisable(GL_DEPTH_TEST);
        glUseProgram(shader);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);

        glUniform2f(glGetUniformLocation(shader, "position"), x, y);
        float sx = scale.x;
        float sy = scale.y;
        glUniform2f(glGetUniformLocation(shader, "scale"), sx, sy);
        glUniform1f(glGetUniformLocation(shader, "aspect_ratio"), aspect_ratio);
        glUniform1f(glGetUniformLocation(shader, "rotation"), rotation);
        glUniform1f(glGetUniformLocation(shader, "alpha"), alpha);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

GLuint FiscionX::UI::Image::shader = 0; // Define global IMAGE shader variable

// =================== Camera ===================
    FiscionX::Camera::Camera() {
        updateVectors();
    }

    glm::mat4 FiscionX::Camera::GetView() {
        return glm::lookAt(position, position + front, up);
    }

    void FiscionX::Camera::ProcessMouse(float xoffset, float yoffset) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch -= yoffset;

        if (pitch > 89.0f)  pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateVectors();
    }

    void FiscionX::Camera::updateVectors() {
        if (canLook) {
            glm::vec3 dir;
            dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            dir.y = sin(glm::radians(pitch));
            dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            front = glm::normalize(dir);
            right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
            up = glm::normalize(glm::cross(right, front));
        }
    }

// =================== Audio ====================
FMOD_RESULT SYS;
FMOD::System* FMOD_SYS;

    void FiscionX::AudioSystem::init() {
        SYS = FMOD::System_Create(&FMOD_SYS);
        if (SYS != FMOD_OK) {
            std::cerr << "ERR 0x011 - FMOD Audio System couldn't be created: " << std::endl;
            glfwTerminate();
            system("pause");
            std::exit(-11);
        }

        SYS = FMOD_SYS->init(512, FMOD_INIT_NORMAL, nullptr);
        if (SYS != FMOD_OK) {
            std::cerr << "ERR 0x012 - FMOD Audio System couldn't be initialized" << std::endl;
            glfwTerminate();
            system("pause");
            std::exit(-12);
        }

        SYS = FMOD_SYS->set3DSettings(1.0, 1.0f, 1.0f);
    }

    void FiscionX::AudioSystem::update() {
        FMOD_SYS->update();
        listenerPos = { FiscionX::Core::Camera.position[0], FiscionX::Core::Camera.position[1], FiscionX::Core::Camera.position[2]};
        forward = { -FiscionX::Core::Camera.front[0], FiscionX::Core::Camera.front[1], -FiscionX::Core::Camera.front[2] };
        up = { -FiscionX::Core::Camera.up[0], FiscionX::Core::Camera.up[1], -FiscionX::Core::Camera.up[2] };
        FMOD_SYS->set3DListenerAttributes(0, &listenerPos, &velocity, &forward, &up);
    }

    FiscionX::Sound::Sound(const char* path, bool loop, bool threedimensional, glm::vec3 position,
    float minDistance, float maxDistance, float vol) {
        int mode = threedimensional ? FMOD_3D_LINEARROLLOFF : FMOD_2D;
        FMOD_SYS->createSound(path, mode, nullptr, &audiofont);

        pos = position;
        minDist = minDistance;
        maxDist = maxDistance;
        looping = loop;
        volume = vol;

        FiscionX::Core::AllSounds.push_back(*this);
    }
    
    void FiscionX::Sound::useEffect(FMOD_DSP_TYPE type) {
        FMOD_SYS->createDSPByType(type, &dsp);
        curr_channel->addDSP(0, dsp);
    }

    void FiscionX::Sound::updateValues() {
        audiofont->set3DMinMaxDistance(minDist, maxDist);
        if (looping) { audiofont->setMode(FMOD_LOOP_NORMAL); }
        if (!looping) { audiofont->setMode(FMOD_LOOP_OFF); }

        FMOD_VECTOR position = { pos.x, pos.y, pos.z };
        FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
        curr_channel->set3DAttributes(&position, &vel);
        curr_channel->setPaused(paused);
        curr_channel->setVolume(volume);
    }

    void FiscionX::Sound::play() {
        updateValues();
        FMOD_SYS->playSound(audiofont, nullptr, false, &curr_channel);
    }

// =================== Mouse Callback ===================
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    FiscionX::Core::Camera.ProcessMouse(xoffset, yoffset);
}

// ==================== LIGHTS ======================
FiscionX::Light::Light() {
    FiscionX::Core::AllLights.push_back(this);
}

// ==================== MODELS ======================
void FiscionX::generateTangents(
    std::vector<VertexData>& vertices,
    const std::vector<uint32_t>& indices
) {
    for (size_t i = 0; i < indices.size(); i += 3) {
        VertexData& v0 = vertices[indices[i + 0]];
        VertexData& v1 = vertices[indices[i + 1]];
        VertexData& v2 = vertices[indices[i + 2]];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.uv - v0.uv;
        glm::vec2 deltaUV2 = v2.uv - v0.uv;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    for (auto& v : vertices) {
        // Ortogonaliza com Gram-Schmidt
        v.tangent = glm::normalize(v.tangent - v.normal * glm::dot(v.normal, v.tangent));
        // Determine handedness (bitangent cross)
        float handedness = (glm::dot(glm::cross(v.normal, v.tangent), v.bitangent) < 0.0f) ? -1.0f : 1.0f;
        v.tangent = glm::vec4(v.tangent, handedness);
    }
}

    const std::vector<glm::mat4>& FiscionX::Model::getBoneTransforms() const {
        return boneTransforms;
    }

    FiscionX::Model::Model(const std::string& path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
        : position(pos), rotation(rot), scale(scl)
    {
        init(path);
    }

    void FiscionX::Model::playAnim(const std::string& name, bool repeat, const std::string& next) {
        currentAnim.name = name;
        currentAnim.repeat = repeat;
        currentAnim.nextAnim = next;
        currentAnim.time = 0.0f;
    }

    void FiscionX::Model::update(float deltaTime) {
        for (size_t i = 0; i < occlusionQueries.size(); ++i) {
            GLuint result;
            glGetQueryObjectuiv(occlusionQueries[i], GL_QUERY_RESULT, &result);
            isVisible[i] = result != 0;
        }

        if (!isSkinned) return;

        auto itAnim = animations.find(currentAnim.name);
        if (itAnim == animations.end()) return;
        const tinygltf::Animation& anim = itAnim->second;

        currentAnim.time += deltaTime;
        float t = currentAnim.time;

        float maxTime = 0.0f;
        for (const auto& channel : anim.channels) {
            const tinygltf::AnimationSampler& samp = anim.samplers[channel.sampler];
            const tinygltf::Accessor& inputAcc = gltfModel.accessors[samp.input];
            if (inputAcc.count == 0) continue;
            const tinygltf::BufferView& inputView = gltfModel.bufferViews[inputAcc.bufferView];
            const tinygltf::Buffer& inputBuffer = gltfModel.buffers[inputView.buffer];
            const float* times = reinterpret_cast<const float*>(
                &inputBuffer.data[inputView.byteOffset + inputAcc.byteOffset]
                );
            float lastKey = times[inputAcc.count - 1];
            maxTime = std::fmax(maxTime, lastKey);
        }

        if (maxTime > 0.0f && t > maxTime) {
            if (currentAnim.repeat) {
                t = fmodf(t, maxTime);
                currentAnim.time = t;
            }
            else if (!currentAnim.nextAnim.empty()) {
                playAnim(currentAnim.nextAnim, true);
                return;
            }
            else {
                t = maxTime;
            }
        }

        animTranslations.clear();
        animRotations.clear();
        animScales.clear();

        for (const auto& channel : anim.channels) {
            const tinygltf::AnimationSampler& samp = anim.samplers[channel.sampler];
            int nodeIndex = channel.target_node;

            const tinygltf::Accessor& inputAcc = gltfModel.accessors[samp.input];
            const tinygltf::BufferView& inputView = gltfModel.bufferViews[inputAcc.bufferView];
            const tinygltf::Buffer& inputBuffer = gltfModel.buffers[inputView.buffer];
            const float* times = reinterpret_cast<const float*>(
                &inputBuffer.data[inputView.byteOffset + inputAcc.byteOffset]
                );

            const tinygltf::Accessor& outputAcc = gltfModel.accessors[samp.output];
            const tinygltf::BufferView& outputView = gltfModel.bufferViews[outputAcc.bufferView];
            const tinygltf::Buffer& outputBuffer = gltfModel.buffers[outputView.buffer];
            const float* values = reinterpret_cast<const float*>(
                &outputBuffer.data[outputView.byteOffset + outputAcc.byteOffset]
                );

            int keyCount = static_cast<int>(inputAcc.count);
            if (keyCount == 0) continue;

            int key = 0;
            while (key + 1 < keyCount && t > times[key + 1]) key++;
            int nextKey = (key + 1 < keyCount) ? (key + 1) : key;

            float t0 = times[key];
            float t1 = times[nextKey];
            float factor = 0.0f;
            if (t1 - t0 > 0.0f) factor = (t - t0) / (t1 - t0);

            if (channel.target_path == "translation") {
                glm::vec3 A(values[key * 3 + 0],
                    values[key * 3 + 1],
                    values[key * 3 + 2]);
                glm::vec3 B(values[nextKey * 3 + 0],
                    values[nextKey * 3 + 1],
                    values[nextKey * 3 + 2]);
                glm::vec3 tr = glm::mix(A, B, factor);
                animTranslations[nodeIndex] = tr;
            }
            else if (channel.target_path == "rotation") {
                glm::quat A(values[key * 4 + 3],
                    values[key * 4 + 0],
                    values[key * 4 + 1],
                    values[key * 4 + 2]);
                glm::quat B(values[nextKey * 4 + 3],
                    values[nextKey * 4 + 0],
                    values[nextKey * 4 + 1],
                    values[nextKey * 4 + 2]);
                glm::quat R = glm::slerp(A, B, factor);
                animRotations[nodeIndex] = R;
            }
            else if (channel.target_path == "scale") {
                glm::vec3 A(values[key * 3 + 0],
                    values[key * 3 + 1],
                    values[key * 3 + 2]);
                glm::vec3 B(values[nextKey * 3 + 0],
                    values[nextKey * 3 + 1],
                    values[nextKey * 3 + 2]);
                glm::vec3 sc = glm::mix(A, B, factor);
                animScales[nodeIndex] = sc;
            }
            
            boneTransforms = finalBoneMatrices;
        }

        nodeGlobalTransforms.clear();
        std::function<void(int)> recurseGlobal = [&](int idx) {
            const tinygltf::Node& node = nodes[idx];
            int parent = nodeParents[idx];
            glm::mat4 parentMat = (parent >= 0 && nodeGlobalTransforms.count(parent))
                ? nodeGlobalTransforms[parent]
                : glm::mat4(1.0f);

                glm::vec3 T_def(0.0f);
                glm::quat R_def(1, 0, 0, 0);
                glm::vec3 S_def(1.0f);
                if (!node.matrix.empty()) {
                    glm::mat4 M = glm::make_mat4(node.matrix.data());
                    nodeGlobalTransforms[idx] = parentMat * M;
                    for (int c : node.children) recurseGlobal(c);
                    return;
                }
                else {
                    if (!node.translation.empty()) {
                        T_def = glm::make_vec3(node.translation.data());
                    }
                    if (!node.rotation.empty()) {
                        R_def = glm::make_quat(node.rotation.data());
                    }
                    if (!node.scale.empty()) {
                        S_def = glm::make_vec3(node.scale.data());
                    }
                }

                glm::vec3 T_use = (animTranslations.count(idx) > 0)
                    ? animTranslations[idx]
                    : T_def;
                    glm::quat R_use = (animRotations.count(idx) > 0)
                        ? animRotations[idx]
                        : R_def;
                        glm::vec3 S_use = (animScales.count(idx) > 0)
                            ? animScales[idx]
                            : S_def;

                            glm::mat4 local = glm::translate(glm::mat4(1.0f), T_use)
                                * glm::mat4_cast(R_use)
                                * glm::scale(glm::mat4(1.0f), S_use);

                            nodeGlobalTransforms[idx] = parentMat * local;

                            for (int c : node.children) {
                                recurseGlobal(c);
                            }
            };

        for (int root : gltfModel.scenes[gltfModel.defaultScene].nodes) {
            recurseGlobal(root);
        }

        if (!skins.empty()) {
            const tinygltf::Skin& skin = skins[0];
            finalBoneMatrices.resize(skin.joints.size());

            const tinygltf::Accessor& invBindAcc = gltfModel.accessors[skin.inverseBindMatrices];
            const tinygltf::BufferView& invBindView = gltfModel.bufferViews[invBindAcc.bufferView];
            const tinygltf::Buffer& invBindBuf = gltfModel.buffers[invBindView.buffer];

            for (size_t i = 0; i < skin.joints.size(); ++i) {
                int jointIdx = skin.joints[i];
                glm::mat4 boneGlobal = nodeGlobalTransforms[jointIdx];

                const float* matData = reinterpret_cast<const float*>(
                    &invBindBuf.data[invBindView.byteOffset + invBindAcc.byteOffset + sizeof(float) * 16 * i]
                    );
                glm::mat4 invBind = glm::make_mat4(matData);
                finalBoneMatrices[i] = boneGlobal * invBind;
            }

            glBindBuffer(GL_UNIFORM_BUFFER, uboSkin);
            glBufferData(
                GL_UNIFORM_BUFFER,
                sizeof(glm::mat4) * finalBoneMatrices.size(),
                finalBoneMatrices.data(),
                GL_DYNAMIC_DRAW
            );
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboSkin);
        }
    }

    GLuint FiscionX::Model::getBaseColorTexture(const tinygltf::Model& model, int materialIndex) {
        if (materialIndex < 0 || materialIndex >= (int)model.materials.size()) return 0;
        const auto& mat = model.materials[materialIndex];

        static const std::vector<std::string> keys = {
            "baseColorTexture",
            "albedo",
            "diffuse"
        };

        int texIndex = -1;
        for (const std::string& key : keys) {
            auto it = mat.values.find(key);
            if (it != mat.values.end()) {
                texIndex = it->second.TextureIndex();
                break;
            }
        }

        if (texIndex < 0 || texIndex >= (int)model.textures.size()) return 0;
        int imgIndex = model.textures[texIndex].source;
        if (imgIndex < 0 || imgIndex >= (int)model.images.size()) return 0;

        const auto& img = model.images[imgIndex];
        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
            img.width, img.height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE,
            img.image.data());

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GLfloat maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

        return texID;
    }

    GLuint FiscionX::Model::getDiffuseTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex) {
        if (materialIndex < 0 || materialIndex >= (int)model.materials.size()) return 0;
        const auto& mat = model.materials[materialIndex];

        auto extIt = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
        if (extIt != mat.extensions.end()) {
            const auto& ext = extIt->second;
            if (ext.Has("diffuseTexture")) {
                int texIndex = ext.Get("diffuseTexture").Get("index").Get<int>();
                if (texIndex >= 0 && texIndex < (int)model.textures.size()) {
                    int imgIndex = model.textures[texIndex].source;
                    if (imgIndex >= 0 && imgIndex < (int)model.images.size()) {
                        const auto& img = model.images[imgIndex];
                        GLuint texID;
                        glGenTextures(1, &texID);
                        glBindTexture(GL_TEXTURE_2D, texID);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0,
                            GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
                        glGenerateMipmap(GL_TEXTURE_2D);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        GLfloat maxAniso = 0.0f;
                        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
                        return texID;
                    }
                }
            }
        }

        return 0;
    }

    GLuint FiscionX::Model::getGlossinessTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex) {
        if (materialIndex < 0 || materialIndex >= (int)model.materials.size()) return 0;
        const auto& mat = model.materials[materialIndex];

        auto extIt = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
        if (extIt != mat.extensions.end()) {
            const auto& ext = extIt->second;
            if (ext.Has("specularGlossinessTexture")) {
                int texIndex = ext.Get("specularGlossinessTexture").Get("index").Get<int>();
                if (texIndex >= 0 && texIndex < (int)model.textures.size()) {
                    int imgIndex = model.textures[texIndex].source;
                    if (imgIndex >= 0 && imgIndex < (int)model.images.size()) {
                        const auto& img = model.images[imgIndex];

                        GLenum format = GL_RGBA;
                        if (img.component == 1) format = GL_RED;
                        else if (img.component == 3) format = GL_RGB;
                        else if (img.component == 4) format = GL_RGBA;

                        GLuint texID;
                        glGenTextures(1, &texID);
                        glBindTexture(GL_TEXTURE_2D, texID);
                        glTexImage2D(GL_TEXTURE_2D, 0, format, img.width, img.height, 0,
                            format, GL_UNSIGNED_BYTE, img.image.data());
                        glGenerateMipmap(GL_TEXTURE_2D);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        GLfloat maxAniso = 0.0f;
                        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
                        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
                        return texID;
                    }
                }
            }
        }

        return 0;
    }

    GLuint FiscionX::Model::getNormalMapTexture(const tinygltf::Model& model, int materialIndex) {
        if (materialIndex < 0 || materialIndex >= (int)model.materials.size()) return 0;
        const auto& mat = model.materials[materialIndex];
        if (mat.additionalValues.find("normalTexture") == mat.additionalValues.end()) return 0;

        int texIndex = mat.additionalValues.at("normalTexture").TextureIndex();
        if (texIndex < 0 || texIndex >= (int)model.textures.size()) return 0;
        int imgIndex = model.textures[texIndex].source;
        const auto& img = model.images[imgIndex];

        GLuint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GLfloat maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
        return texID;
    }

    void FiscionX::Model::init(const std::string& path) {
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        bool ret = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);
        if (!warn.empty()) std::cout << "GLTF Warn: " << warn << std::endl;
        if (!err.empty())  std::cerr << "GLTF Error: " << err << std::endl;
        if (!ret) {
            throw std::runtime_error("Failed to load GLB: " + path);
            glfwTerminate();
            system("pause");
            std::exit(0);
        }

        nodes = gltfModel.nodes;
        skins = gltfModel.skins;
        isSkinned = !skins.empty();

        nodeParents.clear();
        for (int i = 0; i < (int)nodes.size(); ++i) nodeParents[i] = -1;
        for (int i = 0; i < (int)nodes.size(); ++i) {
            for (int c : nodes[i].children) {
                nodeParents[c] = i;
            }
        }

        for (size_t i = 0; i < gltfModel.animations.size(); ++i) {
            const auto& a = gltfModel.animations[i];
            animations[a.name] = a;
        }

        if (isSkinned) {
            glGenBuffers(1, &uboSkin);
            glBindBuffer(GL_UNIFORM_BUFFER, uboSkin);
            std::vector<glm::mat4> identityMats(100, glm::mat4(1.0f));
            glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 100, identityMats.data(), GL_DYNAMIC_DRAW);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboSkin);
        }

        std::function<void(int, glm::mat4)> processNode =
            [&](int nodeIndex, glm::mat4 parentTransform)
            {
                const tinygltf::Node& node = nodes[nodeIndex];
                glm::mat4 local(1.0f);

                if (!node.matrix.empty()) {
                    local = glm::make_mat4(node.matrix.data());
                }
                else {
                    glm::vec3 T(0.0f), S(1.0f);
                    glm::quat R(1, 0, 0, 0);
                    if (!node.translation.empty()) T = glm::make_vec3(node.translation.data());
                    if (!node.rotation.empty())    R = glm::make_quat(node.rotation.data());
                    if (!node.scale.empty())       S = glm::make_vec3(node.scale.data());
                    local = glm::translate(glm::mat4(1.0f), T)
                        * glm::mat4_cast(R)
                        * glm::scale(glm::mat4(1.0f), S);
                }

                glm::mat4 globalTransform = parentTransform * local;

                if (node.mesh >= 0) {
                    const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];
                    for (size_t p = 0; p < mesh.primitives.size(); ++p) {
                        const tinygltf::Primitive& prim = mesh.primitives[p];
                        SubMesh sub;
                        sub.transform = globalTransform;

                        bool hasPosition = prim.attributes.find("POSITION") != prim.attributes.end();
                        bool hasNormal = prim.attributes.find("NORMAL") != prim.attributes.end();
                        bool hasTangent = prim.attributes.find("TANGENT") != prim.attributes.end();
                        bool hasTexCoord = prim.attributes.find("TEXCOORD_0") != prim.attributes.end();
                        if (!hasPosition) continue;

                        const tinygltf::Accessor& posAcc = gltfModel.accessors.at(prim.attributes.at("POSITION"));
                        const tinygltf::BufferView& posView = gltfModel.bufferViews.at(posAcc.bufferView);
                        const tinygltf::Buffer& posBuf = gltfModel.buffers.at(posView.buffer);

                        const tinygltf::Accessor* jointsAccPtr = nullptr;
                        if (isSkinned && prim.attributes.find("JOINTS_0") != prim.attributes.end()) {
                            jointsAccPtr = &gltfModel.accessors.at(prim.attributes.at("JOINTS_0"));
                        }
                        const tinygltf::Accessor* weightsAccPtr = nullptr;
                        if (isSkinned && prim.attributes.find("WEIGHTS_0") != prim.attributes.end()) {
                            weightsAccPtr = &gltfModel.accessors.at(prim.attributes.at("WEIGHTS_0"));
                        }

                        std::vector<unsigned short> jointsData;
                        std::vector<float>           weightsData;
                        if (jointsAccPtr) {
                            const tinygltf::BufferView& jView = gltfModel.bufferViews.at(jointsAccPtr->bufferView);
                            const tinygltf::Buffer& jBuf = gltfModel.buffers.at(jView.buffer);
                            bool isShort = (jointsAccPtr->componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);
                            for (size_t vi = 0; vi < jointsAccPtr->count; ++vi) {
                                if (isShort) {
                                    const unsigned short* src = reinterpret_cast<const unsigned short*>(
                                        &jBuf.data[jView.byteOffset + jointsAccPtr->byteOffset + vi * sizeof(unsigned short) * 4]
                                        );
                                    jointsData.push_back(src[0]);
                                    jointsData.push_back(src[1]);
                                    jointsData.push_back(src[2]);
                                    jointsData.push_back(src[3]);
                                }
                                else {
                                    const unsigned char* src = reinterpret_cast<const unsigned char*>(
                                        &jBuf.data[jView.byteOffset + jointsAccPtr->byteOffset + vi * sizeof(unsigned char) * 4]
                                        );
                                    jointsData.push_back((unsigned short)src[0]);
                                    jointsData.push_back((unsigned short)src[1]);
                                    jointsData.push_back((unsigned short)src[2]);
                                    jointsData.push_back((unsigned short)src[3]);
                                }
                            }
                        }
                        else {
                            jointsData.resize(posAcc.count * 4, 0u);
                        }

                        if (weightsAccPtr) {
                            const tinygltf::BufferView& wView = gltfModel.bufferViews.at(weightsAccPtr->bufferView);
                            const tinygltf::Buffer& wBuf = gltfModel.buffers.at(wView.buffer);
                            for (size_t vi = 0; vi < weightsAccPtr->count; ++vi) {
                                const float* src = reinterpret_cast<const float*>(
                                    &wBuf.data[wView.byteOffset + weightsAccPtr->byteOffset + vi * sizeof(float) * 4]
                                    );
                                weightsData.push_back(src[0]);
                                weightsData.push_back(src[1]);
                                weightsData.push_back(src[2]);
                                weightsData.push_back(src[3]);
                            }
                        }
                        else {
                            for (size_t vi = 0; vi < posAcc.count; ++vi) {
                                weightsData.push_back(1.0f);
                                weightsData.push_back(0.0f);
                                weightsData.push_back(0.0f);
                                weightsData.push_back(0.0f);
                            }
                        }

                        const tinygltf::Accessor* normAccPtr = nullptr;
                        if (hasNormal) {
                            normAccPtr = &gltfModel.accessors.at(prim.attributes.at("NORMAL"));
                        }
                        const tinygltf::BufferView* normViewPtr = nullptr;
                        const tinygltf::Buffer* normBufPtr = nullptr;
                        if (normAccPtr) {
                            normViewPtr = &gltfModel.bufferViews.at(normAccPtr->bufferView);
                            normBufPtr = &gltfModel.buffers.at(normViewPtr->buffer);
                        }

                        const tinygltf::Accessor* tanAccPtr = nullptr;
                        if (hasTangent) {
                            tanAccPtr = &gltfModel.accessors.at(prim.attributes.at("TANGENT"));
                        }
                        const tinygltf::BufferView* tanViewPtr = nullptr;
                        const tinygltf::Buffer* tanBufPtr = nullptr;
                        if (tanAccPtr) {
                            tanViewPtr = &gltfModel.bufferViews.at(tanAccPtr->bufferView);
                            tanBufPtr = &gltfModel.buffers.at(tanViewPtr->buffer);
                        }

                        const tinygltf::Accessor* texAccPtr = nullptr;
                        if (hasTexCoord) {
                            texAccPtr = &gltfModel.accessors.at(prim.attributes.at("TEXCOORD_0"));
                        }
                        const tinygltf::BufferView* texViewPtr = nullptr;
                        const tinygltf::Buffer* texBufPtr = nullptr;
                        if (texAccPtr) {
                            texViewPtr = &gltfModel.bufferViews.at(texAccPtr->bufferView);
                            texBufPtr = &gltfModel.buffers.at(texViewPtr->buffer);
                        }

                        std::vector<float> vertices;
                        vertices.resize(posAcc.count * (3 + 3 + 4 + 2)); // pos + normal + tangent + uv
                        bool needsTangentGen = !hasTangent && hasNormal && hasTexCoord;

                        std::vector<VertexData> tangentVerts;
                        if (needsTangentGen) {
                            tangentVerts.resize(posAcc.count);
                        }

                        for (size_t vi = 0; vi < posAcc.count; ++vi) {
                            size_t base = vi * 12;

                            // POSITION
                            const float* pData = reinterpret_cast<const float*>(
                                &posBuf.data[posView.byteOffset + posAcc.byteOffset + vi * sizeof(float) * 3]
                                );
                            glm::vec3 pos(pData[0], pData[1], pData[2]);
                            vertices[base + 0] = pos.x;
                            vertices[base + 1] = pos.y;
                            vertices[base + 2] = pos.z;

                            // NORMAL
                            glm::vec3 normal(0.0f, 0.0f, 1.0f);
                            if (normAccPtr) {
                                const float* nData = reinterpret_cast<const float*>(
                                    &normBufPtr->data[normViewPtr->byteOffset + normAccPtr->byteOffset + vi * sizeof(float) * 3]
                                    );
                                normal = glm::vec3(nData[0], nData[1], nData[2]);
                            }
                            vertices[base + 3] = normal.x;
                            vertices[base + 4] = normal.y;
                            vertices[base + 5] = normal.z;

                            // TANGENT (do modelo ou zerado para fallback)
                            if (tanAccPtr) {
                                const float* tData = reinterpret_cast<const float*>(
                                    &tanBufPtr->data[tanViewPtr->byteOffset + tanAccPtr->byteOffset + vi * sizeof(float) * 4]
                                    );
                                vertices[base + 6] = tData[0];
                                vertices[base + 7] = tData[1];
                                vertices[base + 8] = tData[2];
                                vertices[base + 9] = tData[3];
                            }
                            else {
                                vertices[base + 6] = 1.0f;
                                vertices[base + 7] = 0.0f;
                                vertices[base + 8] = 0.0f;
                                vertices[base + 9] = 1.0f;
                            }

                            // UV
                            glm::vec2 uv(0.0f);
                            if (texAccPtr) {
                                const float* uvData = reinterpret_cast<const float*>(
                                    &texBufPtr->data[texViewPtr->byteOffset + texAccPtr->byteOffset + vi * sizeof(float) * 2]
                                    );
                                uv = glm::vec2(uvData[0], uvData[1]);
                            }
                            vertices[base + 10] = uv.x;
                            vertices[base + 11] = uv.y;

                            // Preenche struct auxiliar se for gerar tangente
                            if (needsTangentGen) {
                                tangentVerts[vi].position = pos;
                                tangentVerts[vi].normal = normal;
                                tangentVerts[vi].uv = uv;
                            }
                        }

                        // ÍNDICES
                        const tinygltf::Accessor& idxAcc = gltfModel.accessors.at(prim.indices);
                        const tinygltf::BufferView& idxView = gltfModel.bufferViews.at(idxAcc.bufferView);
                        const tinygltf::Buffer& idxBuf = gltfModel.buffers.at(idxView.buffer);
                        const uint8_t* idxDataPtr = idxBuf.data.data() + idxView.byteOffset + idxAcc.byteOffset;

                        std::vector<uint32_t> indices;
                        if (idxAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                            sub.indexType = GL_UNSIGNED_SHORT;
                            const uint16_t* src = reinterpret_cast<const uint16_t*>(idxDataPtr);
                            indices.assign(src, src + idxAcc.count);
                        }
                        else {
                            sub.indexType = GL_UNSIGNED_INT;
                            const uint32_t* src = reinterpret_cast<const uint32_t*>(idxDataPtr);
                            indices.assign(src, src + idxAcc.count);
                        }

                        // GERA TANGENTES se necessário
                        /*if (needsTangentGen) {
                            generateTangents(tangentVerts, indices);
                            for (size_t vi = 0; vi < posAcc.count; ++vi) {
                                size_t base = vi * 12;
                                glm::vec3 t = glm::normalize(tangentVerts[vi].tangent);
                                glm::vec3 b = tangentVerts[vi].bitangent;
                                float handedness = (glm::dot(glm::cross(tangentVerts[vi].normal, t), b) < 0.0f) ? -1.0f : 1.0f;
                                vertices[base + 6] = t.x;
                                vertices[base + 7] = t.y;
                                vertices[base + 8] = t.z;
                                vertices[base + 9] = handedness;
                            }
                        }*/

                        glGenVertexArrays(1, &sub.vao);
                        glGenBuffers(1, &sub.vbo);
                        glGenBuffers(1, &sub.ebo);
                        if (isSkinned) {
                            glGenBuffers(1, &sub.jbo);
                            glGenBuffers(1, &sub.wbo);
                        }

                        glBindVertexArray(sub.vao);

                        glBindBuffer(GL_ARRAY_BUFFER, sub.vbo);
                        glBufferData(
                            GL_ARRAY_BUFFER,
                            vertices.size() * sizeof(float),
                            vertices.data(),
                            GL_STATIC_DRAW
                        );
                        GLsizei stride = (3 + 3 + 4 + 2) * sizeof(float);
                        // posição (loc = 0)
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
                        glEnableVertexAttribArray(0);
                        // normal (loc = 1)
                        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
                        glEnableVertexAttribArray(1);
                        // tangente (loc = 2)
                        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
                        glEnableVertexAttribArray(2);
                        // uv (loc = 3)
                        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(10 * sizeof(float)));
                        glEnableVertexAttribArray(3);

                        if (isSkinned) {
                            // JBO (loc = 4) → ivec4 unsigned short
                            glBindBuffer(GL_ARRAY_BUFFER, sub.jbo);
                            glBufferData(
                                GL_ARRAY_BUFFER,
                                jointsData.size() * sizeof(unsigned short),
                                jointsData.data(),
                                GL_STATIC_DRAW
                            );
                            glVertexAttribIPointer(
                                4,
                                4,
                                GL_UNSIGNED_SHORT,
                                4 * sizeof(unsigned short),
                                (void*)0
                            );
                            glEnableVertexAttribArray(4);

                            // WBO (loc = 5) → vec4 float
                            glBindBuffer(GL_ARRAY_BUFFER, sub.wbo);
                            glBufferData(
                                GL_ARRAY_BUFFER,
                                weightsData.size() * sizeof(float),
                                weightsData.data(),
                                GL_STATIC_DRAW
                            );
                            glVertexAttribPointer(
                                5,
                                4,
                                GL_FLOAT,
                                GL_FALSE,
                                4 * sizeof(float),
                                (void*)0
                            );
                            glEnableVertexAttribArray(5);
                        }

                        // EBO
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub.ebo);
                        glBufferData(
                            GL_ELEMENT_ARRAY_BUFFER,
                            indices.size() * sizeof(uint32_t),
                            indices.data(),
                            GL_STATIC_DRAW
                        );

                        glBindVertexArray(0);

                        sub.indexCount = indices.size();
                        sub.baseColorTex = getBaseColorTexture(gltfModel, prim.material);
                        if (sub.baseColorTex == 0) {
                            sub.baseColorTex = getDiffuseTextureFromSpecGloss(gltfModel, prim.material);
                        }
                        sub.normalMapTex = getNormalMapTexture(gltfModel, prim.material);

                        if (prim.material >= 0 && prim.material < (int)gltfModel.materials.size()) {
                            const auto& mat = gltfModel.materials[prim.material];

                            sub.baseColorTex = getBaseColorTexture(gltfModel, prim.material);
                            if (sub.baseColorTex == 0) {
                                sub.baseColorTex = getDiffuseTextureFromSpecGloss(gltfModel, prim.material);
                            }
                            sub.normalMapTex = getNormalMapTexture(gltfModel, prim.material);

                            if (mat.alphaMode == "MASK") {
                                sub.alphaMode = "MASK";
                                sub.originalAlphaMode = "MASK";
                            }
                            else if (mat.alphaMode == "BLEND") {
                                sub.alphaMode = "BLEND";
                                sub.originalAlphaMode = "BLEND";
                            }

                            if (mat.doubleSided) {
                                sub.doubleSided = true;
                            }

                            if (mat.additionalValues.count("alphaCutoff")) {
                                sub.alphaCutoff = static_cast<float>(mat.additionalValues.at("alphaCutoff").Factor());
                            }

                            // GLOSSINESS
                            if (mat.additionalValues.count("glossinessTexture")) {
                                int texIndex = mat.additionalValues.at("glossinessTexture").TextureIndex();
                                int imgIndex = gltfModel.textures[texIndex].source;
                                const auto& img = gltfModel.images[imgIndex];

                                glGenTextures(1, &sub.glossinessTex);
                                glBindTexture(GL_TEXTURE_2D, sub.glossinessTex);
                                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0,
                                    GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
                                glGenerateMipmap(GL_TEXTURE_2D);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                GLfloat maxAniso = 0.0f;
                                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
                                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
                            }

                            auto extIt = mat.extensions.find("KHR_materials_pbrSpecularGlossiness");
                            if (extIt != mat.extensions.end()) {
                                const auto& ext = extIt->second;
                                bool hasGlossiness = ext.Has("glossinessTexture");
                                bool hasSpecGloss = ext.Has("specularGlossinessTexture");

                                if (hasSpecGloss && !hasGlossiness && sub.glossinessTex == 0) {
                                    sub.glossinessInAlphaOfSpecular = true;
                                }
                            }

                            // SPECULAR F0
                            if (mat.additionalValues.count("specularTexture")) {
                                int texIndex = mat.additionalValues.at("specularTexture").TextureIndex();
                                int imgIndex = gltfModel.textures[texIndex].source;
                                const auto& img = gltfModel.images[imgIndex];

                                glGenTextures(1, &sub.specularF0Tex);
                                glBindTexture(GL_TEXTURE_2D, sub.specularF0Tex);
                                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0,
                                    GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
                                glGenerateMipmap(GL_TEXTURE_2D);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                GLfloat maxAniso = 0.0f;
                                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
                                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
                            }

                            if (sub.specularF0Tex == 0) {
                                sub.specularF0Tex = getGlossinessTextureFromSpecGloss(gltfModel, prim.material);
                            }

                            if (sub.glossinessTex == 0) {
                                sub.glossinessTex = getGlossinessTextureFromSpecGloss(gltfModel, prim.material);
                            }

                            if (mat.extensions.find("KHR_materials_transmission") != mat.extensions.end()) {
                                const auto& ext = mat.extensions.at("KHR_materials_transmission");
                                if (ext.Has("transmissionFactor"))
                                    sub.transmissionFactor = static_cast<float>(ext.Get("transmissionFactor").GetNumberAsDouble());

                                if (ext.Has("transmissionTexture")) {
                                    int texIndex = ext.Get("transmissionTexture").Get("index").Get<int>();
                                    int imgIndex = gltfModel.textures[texIndex].source;
                                    const auto& img = gltfModel.images[imgIndex];

                                    glGenTextures(1, &sub.transmissionTex);
                                    glBindTexture(GL_TEXTURE_2D, sub.transmissionTex);
                                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0,
                                        GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
                                    glGenerateMipmap(GL_TEXTURE_2D);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                    GLfloat maxAniso = 0.0f;
                                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
                                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
                                }
                            }
                        }

                        meshes.push_back(sub);
                    }
                }

                for (int child : node.children) {
                    processNode(child, globalTransform);
                }
            };

        for (int root : gltfModel.scenes.at(gltfModel.defaultScene).nodes) {
            processNode(root, glm::mat4(1.0f));
        }

        occlusionQueries.resize(meshes.size());
        isVisible.resize(meshes.size(), true);
        glGenQueries(meshes.size(), occlusionQueries.data());

        FiscionX::Core::AllModels.push_back(this);
    }

    void FiscionX::Model::updateOcclusion(const glm::mat4& viewProj) {
        glm::mat4 baseMatrix = glm::translate(glm::mat4(1.0f), position)
            * glm::eulerAngleXYZ(rotation.y, rotation.x, rotation.z)
            * glm::scale(glm::mat4(1.0f), scale);

        for (size_t i = 0; i < meshes.size(); ++i) {
            const auto& mesh = meshes[i];
            glm::mat4 modelMatrix = baseMatrix * (isSkinned ? glm::mat4(1.0f) : mesh.transform);
            glm::mat4 mvp = viewProj * modelMatrix;

            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
            glDepthMask(GL_FALSE);
            glDisable(GL_BLEND);

            glBeginQuery(GL_ANY_SAMPLES_PASSED, occlusionQueries[i]);
            // Aqui você pode usar um shader simples e desenhar apenas bounding boxes
            glBindVertexArray(mesh.vao);
            glUseProgram(0); // desenha sem shader (mínimo)
            glDrawElements(GL_TRIANGLES, mesh.indexCount, mesh.indexType, 0);
            glEndQuery(GL_ANY_SAMPLES_PASSED);

            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
        }
    }

    void FiscionX::Model::drawSubMesh(
        const SubMesh& mesh,
        GLuint shader,
        const glm::mat4& modelMatrix,
        const glm::mat4& lightSpaceMatrix,
        GLuint depthMap,
        bool depthPass
    ) {
        glUseProgram(shader);
        glBindVertexArray(mesh.vao);

        if (depthPass) {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            if (mesh.doubleSided)
                glDisable(GL_CULL_FACE);
            else {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT); // para shadow acne
            }

            glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glDrawElements(GL_TRIANGLES, mesh.indexCount, mesh.indexType, 0);

            glBindVertexArray(0);
            glUseProgram(0);
            return;
        }

        int mode = 0;
        if (mesh.alphaMode == "MASK") mode = 1;
        else if (mesh.alphaMode == "BLEND") mode = 2;

        if (mode == 2) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            if (mesh.doubleSided)
                glDisable(GL_CULL_FACE);
            else {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
        }
        else {
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            if (mesh.doubleSided)
                glDisable(GL_CULL_FACE);
            else {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
        }

        if (alpha < 1.0f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);

            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        glUniform1i(glGetUniformLocation(shader, "alphaMode"), mode);
        glUniform1f(glGetUniformLocation(shader, "alphaCutoff"), mesh.alphaCutoff);

        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Base color
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.baseColorTex);
        glUniform1i(glGetUniformLocation(shader, "baseColorTex"), 0);

        // Normal map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mesh.normalMapTex);
        glUniform1i(glGetUniformLocation(shader, "normalMapTex"), 1);
        glUniform1i(glGetUniformLocation(shader, "hasNormalMap"), mesh.normalMapTex != 0);

        // Shadow map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(shader, "shadowMap"), 2);

        // Glossiness texture
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, mesh.glossinessTex);
        glUniform1i(glGetUniformLocation(shader, "glossinessTex"), 3);
        glUniform1i(glGetUniformLocation(shader, "hasGlossinessMap"), mesh.glossinessTex != 0);
        glUniform1i(glGetUniformLocation(shader, "glossinessInAlphaOfSpecular"), mesh.glossinessInAlphaOfSpecular ? 1 : 0);

        // Specular F0 texture
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, mesh.specularF0Tex);
        glUniform1i(glGetUniformLocation(shader, "specularF0Tex"), 4);
        glUniform1i(glGetUniformLocation(shader, "hasSpecularF0Map"), mesh.specularF0Tex != 0);

        // ENVIRONMENT SETTINGS
        glUniform1f(glGetUniformLocation(shader, "environmentStrength"), 1.0f);
        glUniform3f(glGetUniformLocation(shader, "environmentSkyColor"), 0.3f, 0.3f, 0.35f);
        glUniform3f(glGetUniformLocation(shader, "environmentGroundColor"), 0.05f, 0.05f, 0.07f);

        // Alpha Settings
        glUniform1f(glGetUniformLocation(shader, "alpha"), alpha);

        // Lights
        int numLights = std::fmin((int)FiscionX::Core::AllLights.size(), 10);
        glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);
        for (int i = 0; i < numLights; ++i) {
            const Light& L = *FiscionX::Core::AllLights[i];
            std::string idx = std::to_string(i);
            glUniform1i(glGetUniformLocation(shader, ("lightType[" + idx + "]").c_str()), L.type);
            glUniform3fv(glGetUniformLocation(shader, ("lightPos[" + idx + "]").c_str()), 1, glm::value_ptr(L.position));
            glUniform3fv(glGetUniformLocation(shader, ("lightDir[" + idx + "]").c_str()), 1, glm::value_ptr(L.direction));
            glUniform3fv(glGetUniformLocation(shader, ("lightColor[" + idx + "]").c_str()), 1, glm::value_ptr(L.color));
            glUniform1f(glGetUniformLocation(shader, ("lightIntensity[" + idx + "]").c_str()), L.intensity);
            glUniform1f(glGetUniformLocation(shader, ("lightMaxDistance[" + idx + "]").c_str()), L.maxDistance);
            glUniform1f(glGetUniformLocation(shader, ("lightCutOff[" + idx + "]").c_str()), L.cutOff);
            glUniform1f(glGetUniformLocation(shader, ("lightOuterCutOff[" + idx + "]").c_str()), L.outerCutOff);
            glUniform1f(glGetUniformLocation(shader, ("lightConstant[" + idx + "]").c_str()), L.constant);
            glUniform1f(glGetUniformLocation(shader, ("lightLinear[" + idx + "]").c_str()), L.linear);
            glUniform1f(glGetUniformLocation(shader, ("lightQuadratic[" + idx + "]").c_str()), L.quadratic);
            glUniform1i(glGetUniformLocation(shader, ("lightHasGlow[" + idx + "]").c_str()), L.hasGlow);
            glUniform3fv(glGetUniformLocation(shader, ("lightGlowColor[" + idx + "]").c_str()), 1, glm::value_ptr(L.glowColor));
            glUniform1f(glGetUniformLocation(shader, ("lightGlowRadius[" + idx + "]").c_str()), L.glowRadius);
        }

        // Draw
        glDrawElements(GL_TRIANGLES, mesh.indexCount, mesh.indexType, 0);

        glBindVertexArray(0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void FiscionX::Model::draw(GLuint shader, const glm::mat4& lightSpaceMatrix, GLuint depthMap, bool depthPass, glm::mat4 view, glm::mat4 projection) {
        int numLights = static_cast<int>(FiscionX::Core::AllLights.size());
        
        glUseProgram(shader);

        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(FiscionX::Core::Camera.position));
        glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);

        for (int i = 0; i < numLights; ++i) {
            const Light& L = *FiscionX::Core::AllLights[i];
            const ShadowMap& sm = FiscionX::Core::AllShadowMaps[i];
            std::string idx = std::to_string(i);

            glUniform1i(glGetUniformLocation(shader, ("lightType[" + idx + "]").c_str()), L.type);
            glUniform3fv(glGetUniformLocation(shader, ("lightPos[" + idx + "]").c_str()), 1, glm::value_ptr(L.position));
            glUniform3fv(glGetUniformLocation(shader, ("lightDir[" + idx + "]").c_str()), 1, glm::value_ptr(L.direction));
            glUniform3fv(glGetUniformLocation(shader, ("lightColor[" + idx + "]").c_str()), 1, glm::value_ptr(L.color));
            glUniform1f(glGetUniformLocation(shader, ("lightIntensity[" + idx + "]").c_str()), L.intensity);
            glUniform1f(glGetUniformLocation(shader, ("lightMaxDistance[" + idx + "]").c_str()), L.maxDistance);
            glUniform1f(glGetUniformLocation(shader, ("lightCutOff[" + idx + "]").c_str()), L.cutOff);
            glUniform1f(glGetUniformLocation(shader, ("lightOuterCutOff[" + idx + "]").c_str()), L.outerCutOff);
            glUniform1f(glGetUniformLocation(shader, ("lightConstant[" + idx + "]").c_str()), L.constant);
            glUniform1f(glGetUniformLocation(shader, ("lightLinear[" + idx + "]").c_str()), L.linear);
            glUniform1f(glGetUniformLocation(shader, ("lightQuadratic[" + idx + "]").c_str()), L.quadratic);
            glUniform1i(glGetUniformLocation(shader, ("lightHasGlow[" + idx + "]").c_str()), L.hasGlow);
            glUniform3fv(glGetUniformLocation(shader, ("lightGlowColor[" + idx + "]").c_str()), 1, glm::value_ptr(L.glowColor));
            glUniform1f(glGetUniformLocation(shader, ("lightGlowRadius[" + idx + "]").c_str()), L.glowRadius);

            if (L.type == LIGHT_POINT) {
                glActiveTexture(GL_TEXTURE20 + i);
                glBindTexture(GL_TEXTURE_CUBE_MAP, sm.depthMap);
                glUniform1i(glGetUniformLocation(shader, ("shadowCubeMaps[" + idx + "]").c_str()), 20 + i);
            }
            else {
                glActiveTexture(GL_TEXTURE10 + i);
                glBindTexture(GL_TEXTURE_2D, sm.depthMap);
                glUniform1i(glGetUniformLocation(shader, ("shadowMaps[" + idx + "]").c_str()), 10 + i);
                glUniformMatrix4fv(glGetUniformLocation(shader, ("lightSpaceMatrices[" + idx + "]").c_str()), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));
            }
        }
        
        updateOcclusion(view * projection);
        glm::mat4 baseMatrix =
            glm::translate(glm::mat4(1.0f), position)
            * glm::eulerAngleXYZ(rotation.y, rotation.x, rotation.z)
            * glm::scale(glm::mat4(1.0f), scale);

        std::vector<std::pair<float, const SubMesh*>> transparentMeshes;

        for (int i = 0; i < meshes.size(); i++) {
            const auto& mesh = meshes[i];
            glm::mat4 modelMatrix = baseMatrix * (isSkinned ? glm::mat4(1.0f) : mesh.transform);

            bool isTransparent = (mesh.alphaMode == "BLEND");

            if (!isTransparent && !isVisible[i]) continue;

            if (!depthPass && isTransparent) {
                glm::vec3 worldPos = glm::vec3(modelMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0));
                float dist = glm::length(worldPos - FiscionX::Core::Camera.position);
                transparentMeshes.emplace_back(dist, &mesh);
            }
            else {
                drawSubMesh(mesh, shader, modelMatrix, lightSpaceMatrix, depthMap, depthPass);
            }
        }

        if (!depthPass && !transparentMeshes.empty()) {
            std::sort(transparentMeshes.begin(), transparentMeshes.end(),
                [](const std::pair<float, const SubMesh*>& a, const std::pair<float, const SubMesh*>& b) {
                    return a.first > b.first; // distância decrescente (mais longe primeiro)
                });

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            for (const auto& pair : transparentMeshes) {
                glm::mat4 modelMatrix = baseMatrix * (isSkinned ? glm::mat4(1.0f) : pair.second->transform);
                drawSubMesh(*pair.second, shader, modelMatrix, lightSpaceMatrix, depthMap, false);
            }

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }

// ================= INPUT ===================
bool FiscionX::Input::GetKeyPressed(int key) {
    if (glfwGetKey(FiscionX::Core::Window, key) == GLFW_PRESS) {
        return true;
    }
    return false;
}

// =================== CORE ===================
    void FiscionX::Core::CreateShadowMap(ShadowMap& sm) {
        glGenFramebuffers(1, &sm.fbo);
        glGenTextures(1, &sm.depthMap);
        glBindTexture(GL_TEXTURE_2D, sm.depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        GLfloat maxAniso = 0.0f;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

        glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sm.depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FiscionX::Core::CreateAllShadowMaps() {
        // ======= Create and Compute Shadow Maps =======
        for (const FiscionX::Light* L : FiscionX::Core::AllLights) {
            FiscionX::ShadowMap sm;
            FiscionX::Core::CreateShadowMap(sm);
            sm.lightSpaceMatrix = FiscionX::Core::ComputeLightSpaceMatrix(*L);
            FiscionX::Core::AllShadowMaps.push_back(sm);
        }
    }

    glm::mat4 FiscionX::Core::ComputeLightSpaceMatrix(const Light& L) {
        if (L.type == LIGHT_DIRECTIONAL) {
            float orthoSize = FiscionX::Core::SHADOW_VIEW_RADIUS;
            glm::vec3 dir = glm::normalize(L.direction);

            glm::vec3 center = Camera.position + Camera.front * glm::vec3(orthoSize);
            glm::vec3 lightPos = center - dir * 30.0f;

            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
            if (abs(glm::dot(dir, up)) > 0.99f)
                up = glm::vec3(0.0f, 0.0f, 1.0f);

            glm::mat4 lightView = glm::lookAt(lightPos, center, up);
            glm::mat4 lightProj = glm::ortho(
                -orthoSize, orthoSize,
                -orthoSize, orthoSize,
                1.0f, 100.0f
            );

            return lightProj * lightView;
        }

        if (L.type == LIGHT_SPOT) {
            float fov = glm::degrees(acos(L.outerCutOff)) * 2.4f;
            glm::mat4 proj = glm::perspective(glm::radians(fov), 1.0f, 0.1f, L.maxDistance);
            glm::vec3 up = glm::abs(L.direction.y) > 0.99f ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
            glm::mat4 view = glm::lookAt(L.position, L.position + glm::normalize(L.direction), up);
            return proj * view;
        }

        if (L.type == LIGHT_POINT) {
            glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, L.maxDistance);
            glm::mat4 view = glm::lookAt(L.position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            return proj * view;
        }

        return glm::mat4(1.0f);
    }

    void FiscionX::Core::RenderAllShadowPasses(glm::mat4 view, glm::mat4 projection, glm::mat4 viewProj) {
        // Atualizar UBO de todos os modelos skinned apenas uma vez
        for (auto& model : AllModels) {
            if (model->isSkinned) {
                const std::vector<glm::mat4>& bones = model->getBoneTransforms();
                glBindBuffer(GL_UNIFORM_BUFFER, model->uboSkin);
                glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * bones.size(), bones.data(), GL_DYNAMIC_DRAW);
                glBindBufferBase(GL_UNIFORM_BUFFER, 0, model->uboSkin);
            }
        }

        for (size_t i = 0; i < AllLights.size(); ++i) {
            const Light& L = *AllLights[i];
            ShadowMap& sm = AllShadowMaps[i];

            if (L.type == LIGHT_POINT) {
                float zFar = L.maxDistance;
                glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, NEAR_PLANE, zFar);
                glm::vec3 pos = L.position;

                std::array<glm::mat4, 6> shadowMatrices = {
                    proj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
                    proj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
                    proj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
                    proj * glm::lookAt(pos, pos + glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
                    proj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
                    proj * glm::lookAt(pos, pos + glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
                };

                glViewport(0, 0, SHADOW_CUBE_SIZE, SHADOW_CUBE_SIZE);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);

                for (int face = 0; face < 6; ++face) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, sm.depthMap, 0);
                    glClear(GL_DEPTH_BUFFER_BIT);

                    // STATIC
                    glUseProgram(depthShaderCubeStatic);
                    glUniform1f(glGetUniformLocation(depthShaderCubeStatic, "farPlane"), zFar);
                    glUniform3fv(glGetUniformLocation(depthShaderCubeStatic, "lightPos"), 1, glm::value_ptr(pos));
                    glUniformMatrix4fv(glGetUniformLocation(depthShaderCubeStatic, ("shadowMatrices[" + std::to_string(face) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowMatrices[face]));

                    for (auto& model : AllModels)
                        if (!model->isSkinned)
                            model->draw(depthShaderCubeStatic, glm::mat4(1.0f), 0, true, view, projection);

                    // SKINNED
                    glUseProgram(depthShaderCubeSkinned);
                    glUniform1f(glGetUniformLocation(depthShaderCubeSkinned, "farPlane"), zFar);
                    glUniform3fv(glGetUniformLocation(depthShaderCubeSkinned, "lightPos"), 1, glm::value_ptr(pos));
                    glUniformMatrix4fv(glGetUniformLocation(depthShaderCubeSkinned, ("shadowMatrices[" + std::to_string(face) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowMatrices[face]));

                    for (auto& model : AllModels)
                        if (model->isSkinned)
                            model->draw(depthShaderCubeSkinned, glm::mat4(1.0f), 0, true, view, projection);
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else {
                sm.lightSpaceMatrix = ComputeLightSpaceMatrix(L);

                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
                glClear(GL_DEPTH_BUFFER_BIT);
                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);

                glUseProgram(depthShaderStatic);
                glUniformMatrix4fv(glGetUniformLocation(depthShaderStatic, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));

                glUseProgram(depthShaderSkinned);
                glUniformMatrix4fv(glGetUniformLocation(depthShaderSkinned, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));

                for (auto& model : AllModels) {
                    if (!model->isSkinned) {
                        glUseProgram(depthShaderStatic);
                        model->draw(depthShaderStatic, glm::mat4(1.0f), 0, true, view, projection);
                    }
                    else {
                        glUseProgram(depthShaderSkinned);
                        model->draw(depthShaderSkinned, glm::mat4(1.0f), 0, true, view, projection);
                    }
                }

                glCullFace(GL_BACK);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
        }
    }

void FiscionX::Core::Set3DSettings(const unsigned int _SHADOW_WIDTH, const unsigned int _SHADOW_HEIGHT,
    const unsigned int _SHADOW_CUBE_SIZE, const float _SHADOW_VIEW_RADIUS, const float _NEAR_PLANE, const float _FAR_PLANE) {

    SHADOW_WIDTH = _SHADOW_WIDTH;
    SHADOW_HEIGHT = _SHADOW_HEIGHT;
    SHADOW_CUBE_SIZE = _SHADOW_CUBE_SIZE;
    NEAR_PLANE = _NEAR_PLANE;
    FAR_PLANE = _FAR_PLANE;
    SHADOW_VIEW_RADIUS = _SHADOW_VIEW_RADIUS;
}

void FiscionX::Core::NewWindow(int width, int height, const char* window_label) {
    std::cout << "FiscionX - " << ENGINE_VERSION << std::endl;

    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(width, height, window_label, NULL, NULL);
    if (!Window) {
        std::cerr << "ERR 0x001 - Failed to initialize GLFW Window\n";
        glfwTerminate();
        system("pause");
        std::exit(-1);
    }
    glfwMakeContextCurrent(Window);
    glfwSwapInterval(0);
    glfwSetCursorPosCallback(Window, mouse_callback);
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "ERR 0x002 - Failed to initialize GLAD\n";
        glfwTerminate();
        system("pause");
        std::exit(-2);
    }

    glEnable(GL_DEPTH_TEST);

    // ====== SHADERS ======
    File* depthFragFile = new File("shaders/depth_fragment.glsl");
    const char* depthFrag = depthFragFile->file.c_str();
    File* depth2DStaticVertFile = new File("shaders/depth_2d_static_vert.glsl");
    const char* depth2DStaticVert = depth2DStaticVertFile->file.c_str();
    File* depth2DSkinnedVertFile = new File("shaders/depth_2d_skinned_vert.glsl");
    const char* depth2DSkinnedVert = depth2DSkinnedVertFile->file.c_str();
    File* depthCubeStaticVertFile = new File("shaders/depth_cube_static_vert.glsl");
    const char* depthCubeStaticVert = depthCubeStaticVertFile->file.c_str();
    File* depthCubeSkinnedVertFile = new File("shaders/depth_cube_skinned_vert.glsl");
    const char* depthCubeSkinnedVert = depthCubeSkinnedVertFile->file.c_str();
    File* vertexStaticFile = new File("shaders/vertex_static.glsl");
    const char* vertexStatic = vertexStaticFile->file.c_str();
    File* vertexSkinnedFile = new File("shaders/vertex_skinned.glsl");
    const char* vertexSkinned = vertexSkinnedFile->file.c_str();
    File* fragmentShaderFile = new File("shaders/fragment.glsl");
    const char* fragmentShader = fragmentShaderFile->file.c_str();
    File* imageVertexShaderFile = new File("shaders/image_vertex.glsl");
    const char* imageVertexShader = imageVertexShaderFile->file.c_str();
    File* imageFragmentShaderFile = new File("shaders/image_fragment.glsl");
    const char* imageFragmentShader = imageFragmentShaderFile->file.c_str();

    depthShaderStatic = LoadShader(depth2DStaticVert, depthFrag);
    depthShaderSkinned = LoadShader(depth2DSkinnedVert, depthFrag);
    depthShaderCubeStatic = LoadShader(depthCubeStaticVert, depthFrag);
    depthShaderCubeSkinned = LoadShader(depthCubeSkinnedVert, depthFrag);
    shaderStatic = LoadShader(vertexStatic, fragmentShader);
    shaderSkinned = LoadShader(vertexSkinned, fragmentShader);
    UI::Image::shader = LoadShader(imageVertexShader, imageFragmentShader);

    // Configure Shadow Mapping
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0,1.0,1.0,1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    GLfloat maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERR 0x003 - Shadow FBO non complete\n";
        glfwTerminate();
        system("pause");
        std::exit(-3);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    AudioSystem = FiscionX::AudioSystem();
    AudioSystem.init();
}

void FiscionX::Core::ClockTick() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    FPS++;

    AudioSystem.update();
    for (Sound sound : FiscionX::Core::AllSounds) {
        sound.updateValues();
    }

    if (currentFrame - lastFPSTime >= 1.0f) {
        std::cout << "FPS: " << FPS << std::endl;
        FPS = 0;
        lastFPSTime = currentFrame;
    }

    glfwPollEvents();
}

void FiscionX::Core::SortModels() {
    // Sort every model
    std::sort(AllModels.begin(), AllModels.end(), [](const Model* a, const Model* b) {
        auto hasBlend = [](const Model* model) {
            if (model->alpha < 1.0f)
                return true;

            return std::any_of(model->meshes.begin(), model->meshes.end(),
                [](const SubMesh& m) {
                    return m.alphaMode == "BLEND";
                });
            };

        bool aBlend = hasBlend(a);
        bool bBlend = hasBlend(b);

        if (aBlend != bBlend)
            return !aBlend && bBlend;

        if (aBlend && bBlend) {
            float da = glm::length2(a->position - FiscionX::Core::Camera.position);
            float db = glm::length2(b->position - FiscionX::Core::Camera.position);
            return da > db;
        }

        return false;
        });
}

void FiscionX::Core::Terminate() {
    glfwTerminate();
}

// ===================== DRAW ========================
void FiscionX::Core::Draw::ClearBackground(float r, float g, float b, float alpha) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, FiscionX::Core::SCREEN_WIDTH, FiscionX::Core::SCREEN_HEIGHT);
    glClearColor(r, g, b, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FiscionX::Core::Draw::SwapBuffers() {
    glfwSwapBuffers(FiscionX::Core::Window);
}

// =================== Shader Loader ===================
GLuint LoadShader(const char* vertexSrc, const char* fragmentSrc) {
    std::hash<std::string> hasher;
    size_t vertexHash = hasher(std::string(vertexSrc));
    size_t fragHash = hasher(std::string(fragmentSrc));
    std::string binaryPath = "cache/shaders/" + std::to_string(vertexHash + fragHash) + ".bin";

    // ───── Tentativa de carregar shader binário ─────
    if (std::filesystem::exists(binaryPath)) {
        std::ifstream in(binaryPath, std::ios::binary);
        if (in) {
            GLenum format;
            in.read(reinterpret_cast<char*>(&format), sizeof(format));

            in.seekg(0, std::ios::end);
            size_t fileSize = static_cast<size_t>(in.tellg());
            size_t size = fileSize - sizeof(format);
            in.seekg(sizeof(format), std::ios::beg);

            std::vector<char> binary(size);
            in.read(binary.data(), size);
            in.close();

            GLuint program = glCreateProgram();
            glProgramBinary(program, format, binary.data(), static_cast<GLint>(size));

            GLint success = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (success) {
                return program;
            }
            else {
                glDeleteProgram(program);
                std::cerr << "ERR 0x010: Shader program couldn't be loaded from " << binaryPath << std::endl;
                std::cerr << "SLN 0x001: Deleting binary file and recompiling\n";

                // Cleaning
                std::filesystem::remove(binaryPath);
            }
        }
    }

    // ───── Compilar do zero ─────
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexSrc, nullptr);
    glCompileShader(vs);
    GLint success;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vs, 512, nullptr, infoLog);
        std::cerr << "ERR 0x004::VERTEX_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
        glfwTerminate();
        system("pause");
        std::exit(-4);
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentSrc, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fs, 512, nullptr, infoLog);
        std::cerr << "ERR 0x005::FRAGMENT_SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
        glfwTerminate();
        system("pause");
        std::exit(-5);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERR 0x006::SHADER_PROGRAM_LINKING_FAILED\n" << infoLog << std::endl;
        glfwTerminate();
        system("pause");
        std::exit(-6);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    // ───── Tentar salvar o binário ─────
    GLint numFormats = 0;
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &numFormats);
    if (numFormats > 0) {
        GLint length = 0;
        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH, &length);

        if (length > 0) {
            std::vector<char> binary(length);
            GLenum format = 0;
            glGetProgramBinary(program, length, nullptr, &format, binary.data());

            std::ofstream out(binaryPath, std::ios::binary);
            out.write(reinterpret_cast<const char*>(&format), sizeof(format));
            out.write(binary.data(), binary.size());
            out.close();
        }
    }

    return program;
}