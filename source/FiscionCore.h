#ifndef FiscionCore
#define FiscionCore

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <functional>
#include <filesystem>
#include <fstream>
#include <thread>
#include <algorithm>
#include <cmath>
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "dependencies/glad/glad.h"
#include "dependencies/GLFW/glfw3.h"
#include "dependencies/glm/glm.hpp"
#include "dependencies/glm/gtc/type_ptr.hpp"
#include "dependencies/glm/gtc/matrix_transform.hpp"
//#include "dependencies/glm/gtc/type_ptr.hpp"
#include "dependencies/glm/gtx/quaternion.hpp"
#include "dependencies/glm/gtx/euler_angles.hpp"

#include "dependencies/fmod/inc/fmod.hpp"
#include "dependencies/fmod/inc/fmod_errors.h"

#include "dependencies/stb/stb_image.h"
#include "dependencies/stb/stb_image_write.h"
#include "dependencies/tinygltf/tiny_gltf.h"

#define 	FISCIONX_KEY_SPACE   32
#define 	FISCIONX_KEY_APOSTROPHE   39 /* ' */
#define 	FISCIONX_KEY_COMMA   44 /* , */
#define 	FISCIONX_KEY_MINUS   45 /* - */
#define 	FISCIONX_KEY_PERIOD   46 /* . */
#define 	FISCIONX_KEY_SLASH   47 /* / */
#define 	FISCIONX_KEY_0   48
#define 	FISCIONX_KEY_1   49
#define 	FISCIONX_KEY_2   50
#define 	FISCIONX_KEY_3   51
#define 	FISCIONX_KEY_4   52
#define 	FISCIONX_KEY_5   53
#define 	FISCIONX_KEY_6   54
#define 	FISCIONX_KEY_7   55
#define 	FISCIONX_KEY_8   56
#define 	FISCIONX_KEY_9   57
#define 	FISCIONX_KEY_SEMICOLON   59 /* ; */
#define 	FISCIONX_KEY_EQUAL   61 /* = */
#define 	FISCIONX_KEY_A   65
#define 	FISCIONX_KEY_B   66
#define 	FISCIONX_KEY_C   67
#define 	FISCIONX_KEY_D   68
#define 	FISCIONX_KEY_E   69
#define 	FISCIONX_KEY_F   70
#define 	FISCIONX_KEY_G   71
#define 	FISCIONX_KEY_H   72
#define 	FISCIONX_KEY_I   73
#define 	FISCIONX_KEY_J   74
#define 	FISCIONX_KEY_K   75
#define 	FISCIONX_KEY_L   76
#define 	FISCIONX_KEY_M   77
#define 	FISCIONX_KEY_N   78
#define 	FISCIONX_KEY_O   79
#define 	FISCIONX_KEY_P   80
#define 	FISCIONX_KEY_Q   81
#define 	FISCIONX_KEY_R   82
#define 	FISCIONX_KEY_S   83
#define 	FISCIONX_KEY_T   84
#define 	FISCIONX_KEY_U   85
#define 	FISCIONX_KEY_V   86
#define 	FISCIONX_KEY_W   87
#define 	FISCIONX_KEY_X   88
#define 	FISCIONX_KEY_Y   89
#define 	FISCIONX_KEY_Z   90
#define 	FISCIONX_KEY_LEFT_BRACKET   91 /* [ */
#define 	FISCIONX_KEY_BACKSLASH   92 /* \ */
#define 	FISCIONX_KEY_RIGHT_BRACKET   93 /* ] */
#define 	FISCIONX_KEY_GRAVE_ACCENT   96 /* ` */
#define 	FISCIONX_KEY_WORLD_1   161 /* non-US #1 */
#define 	FISCIONX_KEY_WORLD_2   162 /* non-US #2 */
#define 	FISCIONX_KEY_ESCAPE   256
#define 	FISCIONX_KEY_ENTER   257
#define 	FISCIONX_KEY_TAB   258
#define 	FISCIONX_KEY_BACKSPACE   259
#define 	FISCIONX_KEY_INSERT   260
#define 	FISCIONX_KEY_DELETE   261
#define 	FISCIONX_KEY_RIGHT   262
#define 	FISCIONX_KEY_LEFT   263
#define 	FISCIONX_KEY_DOWN   264
#define 	FISCIONX_KEY_UP   265
#define 	FISCIONX_KEY_PAGE_UP   266
#define 	FISCIONX_KEY_PAGE_DOWN   267
#define 	FISCIONX_KEY_HOME   268
#define 	FISCIONX_KEY_END   269
#define 	FISCIONX_KEY_CAPS_LOCK   280
#define 	FISCIONX_KEY_SCROLL_LOCK   281
#define 	FISCIONX_KEY_NUM_LOCK   282
#define 	FISCIONX_KEY_PRINT_SCREEN   283
#define 	FISCIONX_KEY_PAUSE   284
#define 	FISCIONX_KEY_F1   290
#define 	FISCIONX_KEY_F2   291
#define 	FISCIONX_KEY_F3   292
#define 	FISCIONX_KEY_F4   293
#define 	FISCIONX_KEY_F5   294
#define 	FISCIONX_KEY_F6   295
#define 	FISCIONX_KEY_F7   296
#define 	FISCIONX_KEY_F8   297
#define 	FISCIONX_KEY_F9   298
#define 	FISCIONX_KEY_F10   299
#define 	FISCIONX_KEY_F11   300
#define 	FISCIONX_KEY_F12   301
#define 	FISCIONX_KEY_F13   302
#define 	FISCIONX_KEY_F14   303
#define 	FISCIONX_KEY_F15   304
#define 	FISCIONX_KEY_F16   305
#define 	FISCIONX_KEY_F17   306
#define 	FISCIONX_KEY_F18   307
#define 	FISCIONX_KEY_F19   308
#define 	FISCIONX_KEY_F20   309
#define 	FISCIONX_KEY_F21   310
#define 	FISCIONX_KEY_F22   311
#define 	FISCIONX_KEY_F23   312
#define 	FISCIONX_KEY_F24   313
#define 	FISCIONX_KEY_F25   314
#define 	FISCIONX_KEY_KP_0   320
#define 	FISCIONX_KEY_KP_1   321
#define 	FISCIONX_KEY_KP_2   322
#define 	FISCIONX_KEY_KP_3   323
#define 	FISCIONX_KEY_KP_4   324
#define 	FISCIONX_KEY_KP_5   325
#define 	FISCIONX_KEY_KP_6   326
#define 	FISCIONX_KEY_KP_7   327
#define 	FISCIONX_KEY_KP_8   328
#define 	FISCIONX_KEY_KP_9   329
#define 	FISCIONX_KEY_KP_DECIMAL   330
#define 	FISCIONX_KEY_KP_DIVIDE   331
#define 	FISCIONX_KEY_KP_MULTIPLY   332
#define 	FISCIONX_KEY_KP_SUBTRACT   333
#define 	FISCIONX_KEY_KP_ADD   334
#define 	FISCIONX_KEY_KP_ENTER   335
#define 	FISCIONX_KEY_KP_EQUAL   336
#define 	FISCIONX_KEY_LEFT_SHIFT   340
#define 	FISCIONX_KEY_LEFT_CONTROL   341
#define 	FISCIONX_KEY_LEFT_ALT   342
#define 	FISCIONX_KEY_LEFT_SUPER   343
#define 	FISCIONX_KEY_RIGHT_SHIFT   344
#define 	FISCIONX_KEY_RIGHT_CONTROL   345
#define 	FISCIONX_KEY_RIGHT_ALT   346
#define 	FISCIONX_KEY_RIGHT_SUPER   347
#define 	FISCIONX_KEY_MENU   348

extern float lastX, lastY;
extern bool firstMouse;
extern float deltaTime, lastFrame;
extern FMOD_RESULT SYS;
extern FMOD::System* FMOD_SYS;

GLuint LoadShader(const char* vertexSrc, const char* fragmentSrc);

struct File {
	std::fstream rawfile;
	std::string path;
	std::string file;

	File(std::string _path);
	void readFile();
	void clearFile();
	void saveFile(std::string outputPath);
};

struct FiscionX {
	struct Math {
		static float getDistance3D(glm::vec3 pos1, glm::vec3 pos2);
	};
	struct UI{
		struct Image {
			GLuint texture;
			GLuint VAO, VBO;
			static GLuint shader;

			float alpha = 1.0f;
			float rotation = 0.0f; // degrees
			glm::vec2 scale = glm::vec2(1.0f);

			float aspect_ratio = 1.0f;
			int w_, h_;

			Image(const char* path, float sx = 1.0f, float sy = 1.0f);
			void flip(bool flipx, bool flipy);
			void draw(float x = 0.0f, float y = 0.0f);
		};
	};

	struct Camera {
		glm::vec3 position = glm::vec3(0, 2, 3);
		glm::vec3 front = glm::vec3(0, 0, -1);
		glm::vec3 up = glm::vec3(0, 1, 0);
		glm::vec3 right;
		float     yaw = -90.0f;
		float     pitch = 0.0f;
		float     speed = 2.0f;
		float     sensitivity = 0.1f;
		float     fov = 60.0f;

		Camera();
		glm::mat4 GetView();
		void ProcessMouse(float xoffset, float yoffset);
		void updateVectors();
		void update(GLFWwindow* window, float dt);
	};

	struct AudioSystem {
		FMOD_VECTOR listenerPos = { 0, 0, 0 };
		FMOD_VECTOR forward = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR up = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };

		void init();
		void update();
	};

	struct Sound {
		FMOD::Sound* audiofont;
		bool paused = false;
		glm::vec3 pos;
		float minDist;
		float maxDist;
		bool looping;
		float volume;
		FMOD::Channel* curr_channel;
		FMOD::DSP* dsp;

		Sound(const char* path, bool loop = false, bool threedimensional = false, glm::vec3 position = glm::vec3(0, 0, 0),
			float minDistance = 1.0f, float maxDistance = 1000.0f, float vol = 1.0f);
		void updateValues();
		void useEffect(FMOD_DSP_TYPE type);
		void play();
	};

	struct ShadowMap {
		GLuint fbo = 0;
		GLuint depthMap = 0;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
	};

	enum LightType {
		LIGHT_DIRECTIONAL = 0,
		LIGHT_POINT = 1,
		LIGHT_SPOT = 2
	};

	struct Light {
		int         type = LIGHT_DIRECTIONAL;
		glm::vec3   position = glm::vec3(0);
		glm::vec3   direction = glm::vec3(0);
		glm::vec3   color = glm::vec3(1.0f);
		float       intensity = 1.0f;

		float       maxDistance = 300.0f;
		float       cutOff = 0;
		float       outerCutOff = 0;
		float       constant = 0;
		float       linear = 0;
		float       quadratic = 0;

		bool        hasGlow = false;
		glm::vec3   glowColor = glm::vec3(0);
		float       glowRadius = 0;

		Light();
	};

	struct SubMesh {
		GLuint vao = 0, vbo = 0, ebo = 0;
		GLuint jbo = 0, wbo = 0;
		size_t indexCount = 0;
		GLenum indexType = GL_UNSIGNED_INT;
		glm::mat4 transform = glm::mat4(1.0f);

		GLuint baseColorTex = 0;
		GLuint normalMapTex = 0;
		GLuint transmissionTex = 0;
		GLuint glossinessTex = 0;
		GLuint specularF0Tex = 0;

		std::string alphaMode = "OPAQUE";
		std::string originalAlphaMode = "OPAQUE";
		float alphaCutoff = 0.2f;
		float transmissionFactor = 0.0f;

		bool glossinessInAlphaOfSpecular = false;

		bool doubleSided = false;
	};

	struct VertexData {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent = glm::vec3(0.0f);
		glm::vec3 bitangent = glm::vec3(0.0f);
	};

	void generateTangents(std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

	struct Model {
		std::vector<SubMesh> meshes;
		glm::vec3 position, rotation, scale;
		bool isSkinned = false;

		tinygltf::Model gltfModel;
		std::vector<tinygltf::Node> nodes;
		std::vector<tinygltf::Skin> skins;

		std::map<std::string, tinygltf::Animation> animations;
		struct AnimationState {
			std::string name;
			bool        repeat;
			std::string nextAnim;
			float       time = 0.0f;
		} currentAnim;

		std::map<int, glm::vec3> animTranslations;
		std::map<int, glm::quat> animRotations;
		std::map<int, glm::vec3> animScales;

		std::map<int, glm::mat4> nodeGlobalTransforms;
		std::map<int, int>        nodeParents;

		std::vector<glm::mat4> finalBoneMatrices;
		GLuint uboSkin = 0;

		std::vector<glm::mat4> boneTransforms;

		std::vector<GLuint> occlusionQueries;
		std::vector<bool> isVisible;

		float alpha = 1.0f;

		glm::vec3 boundingCenter = glm::vec3(0.0f);
		float boundingRadius = 1.0f;

		const std::vector<glm::mat4>& getBoneTransforms() const;
		Model(const std::string& path, glm::vec3 pos, glm::vec3 rot, glm::vec3 scl);
		void playAnim(const std::string& name, bool repeat, const std::string& next = "");
		void update(float deltaTime);
		GLuint getBaseColorTexture(const tinygltf::Model& model, int materialIndex);
		GLuint getDiffuseTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex);
		GLuint getGlossinessTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex);
		GLuint getNormalMapTexture(const tinygltf::Model& model, int materialIndex);
		void init(const std::string& path);
		void updateOcclusion(const glm::mat4& viewProj);
		void drawSubMesh(
			const SubMesh& mesh,
			GLuint shader,
			const glm::mat4& modelMatrix,
			const glm::mat4& lightSpaceMatrix,
			GLuint depthMap,
			bool depthPass
		);
		void draw(GLuint shader, const glm::mat4& lightSpaceMatrix, GLuint depthMap, bool depthPass, glm::mat4 view, glm::mat4 projection);
	};

	struct Input {
		static bool GetKeyPressed(int key);
	};

	struct Core {
		static GLFWwindow* Window;
		static int SCREEN_WIDTH, SCREEN_HEIGHT;

		static GLuint depthShaderStatic;
		static GLuint depthShaderSkinned;
		static GLuint depthShaderCubeStatic;
		static GLuint depthShaderCubeSkinned;
		static GLuint shaderStatic;
		static GLuint shaderSkinned;

		static unsigned int SHADOW_WIDTH;
		static unsigned int SHADOW_HEIGHT;
		static unsigned int SHADOW_CUBE_SIZE;
		static float        NEAR_PLANE;
		static float         FAR_PLANE;
		static float         AMBIENT_LIGHT_INTENSITY;
		static glm::vec3     AMBIENT_LIGHT_SKYCOLOR;
		static glm::vec3     AMBIENT_LIGHT_GROUNDCOLOR;
		static float SHADOW_VIEW_RADIUS;
		static GLuint depthMapFBO;
		static GLuint depthMap;

		static Camera Camera;

		static AudioSystem AudioSystem;
		static std::vector<Sound> AllSounds;

		static std::vector<Model*> AllModels;
		static std::vector<Light*> AllLights;
		static std::vector<ShadowMap> AllShadowMaps;

		static float lastFrame;
		static float deltaTime;
		static int FPS;
		static float lastFPSTime;

		static void CreateShadowMap(ShadowMap& sm);
		static void CreateAllShadowMaps();
		static glm::mat4 ComputeLightSpaceMatrix(const Light& L);
		static void RenderAllShadowPasses(glm::mat4 view, glm::mat4 projection, glm::mat4 viewProj);

		static void NewWindow(int width, int height, const char* window_label);
		static void Set3DSettings(const unsigned int _SHADOW_WIDTH, const unsigned int _SHADOW_HEIGHT,
			const unsigned int _SHADOW_CUBE_SIZE, const float _SHADOW_VIEW_RADIUS, const float _NEAR_PLANE, const float _FAR_PLANE);
		static void ClockTick();
		static void SortModels();
		static void Terminate();

		static struct Draw {
			static void SwapBuffers();
			static void ClearBackground(float r, float g, float b, float a);
		};
	};
};

#endif