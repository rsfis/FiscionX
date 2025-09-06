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
#include <unordered_map>
#include <random>
#include <queue>
#include <mutex>
#include <thread>

#define GLM_ENABLE_EXPERIMENTAL
#include "dependencies/glad/glad.h"
#include "dependencies/GLFW/glfw3.h"
#include "dependencies/glm/glm.hpp"
#include "dependencies/glm/gtc/type_ptr.hpp"
#include "dependencies/glm/gtc/matrix_transform.hpp"
#include "dependencies/glm/gtc/type_ptr.hpp"
#include "dependencies/glm/gtx/quaternion.hpp"
#include "dependencies/glm/gtx/euler_angles.hpp"

#include "dependencies/fmod/inc/fmod.hpp"
#include "dependencies/fmod/inc/fmod_errors.h"

#include "dependencies/stb/stb_image.h"
#include "dependencies/stb/stb_image_write.h"

#include "dependencies/ft2build.h"
#include "dependencies/freetype/freetype.h"

#include "dependencies/tinygltf/tiny_gltf.h"

#include "dependencies/vlc/vlc.h"
#include "dependencies/vlc/libvlc.h"

#include "dependencies/bullet/btBulletDynamicsCommon.h"
#include "dependencies/bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include "dependencies/bullet/BulletCollision/Gimpact/btGImpactShape.h"
#include "dependencies/bullet/BulletDynamics/Vehicle/btRaycastVehicle.h"

#define     PI 3.14159265358979323846f
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

#define     FISCIONX_MOUSE_BUTTON_1   0
#define     FISCIONX_MOUSE_BUTTON_2   1
#define     FISCIONX_MOUSE_BUTTON_3   2
#define	    FISCIONX_MOUSE_BUTTON_4   3
#define     FISCIONX_MOUSE_BUTTON_5   4
#define     FISCIONX_MOUSE_BUTTON_6   5
#define     FISCIONX_MOUSE_BUTTON_7   6
#define     FISCIONX_MOUSE_BUTTON_8   7
#define     FISCIONX_MOUSE_BUTTON_LAST   FISCIONX_MOUSE_BUTTON_8
#define     FISCIONX_MOUSE_BUTTON_LEFT   FISCIONX_MOUSE_BUTTON_1
#define     FISCIONX_MOUSE_BUTTON_RIGHT  FISCIONX_MOUSE_BUTTON_2
#define     FISCIONX_MOUSE_BUTTON_MIDDLE FISCIONX_MOUSE_BUTTON_3

#define     FISCIONX_CURSOR_DISABLED 0x00034003
#define     FISCIONX_CURSOR_HIDDEN 0x00034002
#define     FISCIONX_CURSOR_NORMAL 0x00034001
#define     FISCIONX_CURSOR_CAPTURED 0x00034004
#define     FISCIONX_CURSOR_LOCKED 0x00034005

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

struct Glyph {
	glm::vec2 uv0, uv1;    // UV min/max atlas
	glm::ivec2 sizePx;     // gliph in pixels in atlas
	glm::ivec2 bearingPx;  // gliph offset starting from the base line
	int advancePx;         // horizontal advance to the next gliph based on font size
};

namespace FiscionX {
	struct Vector2 {
		float x, y;
		Vector2(float _x = 0, float _y = 0) : x(_x), y(_y) {}

		Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
		Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }

		Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
		Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

		Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
		Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
		Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
		Vector2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

		float dot(const Vector2& o) const {
			return x * o.x + y * o.y;
		}

		float cross(const Vector2& o) const {
			return x * o.y - y * o.x;
		}

		float length() const {
			return std::sqrt(x * x + y * y);
		}

		float lengthSquared() const {
			return x * x + y * y;
		}

		Vector2 normalized() const {
			float len = length();
			return (len > 0) ? (*this / len) : Vector2();
		}

		float distance(const Vector2& o) const {
			return (*this - o).length();
		}

		static Vector2 lerp(const Vector2& a, const Vector2& b, float t) {
			return a + (b - a) * t;
		}

		Vector2 perpendicular() const {
			return Vector2(-y, x);
		}

		Vector2 reflect(const Vector2& normal) const {
			return *this - normal * (2.0f * this->dot(normal));
		}

		Vector2 project(const Vector2& onNormal) const {
			Vector2 n = onNormal.normalized();
			return n * (this->dot(n));
		}

		Vector2 reject(const Vector2& onNormal) const {
			return *this - project(onNormal);
		}
	};

	struct Vector3 {
		float x, y, z;
		Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

		Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
		Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }

		Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
		Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

		Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
		Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
		Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
		Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

		float dot(const Vector3& o) const {
			return x * o.x + y * o.y + z * o.z;
		}

		Vector3 cross(const Vector3& o) const {
			return Vector3(
				y * o.z - z * o.y,
				z * o.x - x * o.z,
				x * o.y - y * o.x
			);
		}

		float length() const {
			return std::sqrt(x * x + y * y + z * z);
		}

		float lengthSquared() const {
			return x * x + y * y + z * z;
		}

		Vector3 normalized() const {
			float len = length();
			return (len > 0) ? (*this / len) : Vector3();
		}

		float distance(const Vector3& o) const {
			return (*this - o).length();
		}

		static Vector3 lerp(const Vector3& a, const Vector3& b, float t) {
			return a + (b - a) * t;
		}

		Vector3 reflect(const Vector3& normal) const {
			return *this - normal * (2.0f * this->dot(normal));
		}

		Vector3 project(const Vector3& onNormal) const {
			Vector3 n = onNormal.normalized();
			return n * (this->dot(n));
		}

		Vector3 reject(const Vector3& onNormal) const {
			return *this - project(onNormal);
		}
	};

	struct Vector4 {
		float x, y, z, w;
		Vector4(float _x = 0, float _y = 0, float _z = 0, float _w = 0)
			: x(_x), y(_y), z(_z), w(_w) {
		}

		// ---- Operadores básicos ----
		Vector4 operator+(const Vector4& o) const { return Vector4(x + o.x, y + o.y, z + o.z, w + o.w); }
		Vector4 operator-(const Vector4& o) const { return Vector4(x - o.x, y - o.y, z - o.z, w - o.w); }
		Vector4 operator*(float s) const { return Vector4(x * s, y * s, z * s, w * s); }
		Vector4 operator/(float s) const { return Vector4(x / s, y / s, z / s, w / s); }

		Vector4& operator+=(const Vector4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
		Vector4& operator-=(const Vector4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
		Vector4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
		Vector4& operator/=(float s) { x /= s; y /= s; z /= s; w /= s; return *this; }

		bool operator==(const Vector4& o) const { return x == o.x && y == o.y && z == o.z && w == o.w; }
		bool operator!=(const Vector4& o) const { return !(*this == o); }
		
		float dot(const Vector4& o) const {
			return x * o.x + y * o.y + z * o.z + w * o.w;
		}

		float length() const {
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		float lengthSquared() const {
			return x * x + y * y + z * z + w * w;
		}

		Vector4 normalized() const {
			float len = length();
			return (len > 0) ? (*this / len) : Vector4();
		}

		float distance(const Vector4& o) const {
			return (*this - o).length();
		}

		static Vector4 lerp(const Vector4& a, const Vector4& b, float t) {
			return a + (b - a) * t;
		}

		Vector4 homogenized() const {
			return (w != 0.0f) ? Vector4(x / w, y / w, z / w, 1.0f) : *this;
		}
	};

	struct Mat4 {
		float m[4][4]; // column-major (m[col][row])

		Mat4(float diag = 1.0f) {
			for (int col = 0; col < 4; col++)
				for (int row = 0; row < 4; row++)
					m[col][row] = (col == row ? diag : 0.0f);
		}

		static Mat4 identity() {
			return Mat4(1.0f);
		}

		operator glm::mat4() const {
			glm::mat4 result(1.0f);
			for (int col = 0; col < 4; col++)
				for (int row = 0; row < 4; row++)
					result[col][row] = m[col][row];
			return result;
		}

		float& operator()(int row, int col) { return m[col][row]; }
		const float& operator()(int row, int col) const { return m[col][row]; }

		Mat4 operator*(const Mat4& o) const {
			Mat4 result(0.0f);
			for (int col = 0; col < 4; col++) {
				for (int row = 0; row < 4; row++) {
					for (int i = 0; i < 4; i++) {
						result(row, col) += (*this)(row, i) * o(i, col);
					}
				}
			}
			return result;
		}

		Vector4 operator*(const Vector4& v) const {
			return Vector4(
				m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
				m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
				m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
				m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
			);
		}

		static Mat4 translate(const Vector3& v) {
			Mat4 result = Mat4::identity();
			result(0, 3) = v.x;
			result(1, 3) = v.y;
			result(2, 3) = v.z;
			return result;
		}

		static Mat4 scale(const Vector3& v) {
			Mat4 result(1.0f);
			result(0, 0) = v.x;
			result(1, 1) = v.y;
			result(2, 2) = v.z;
			return result;
		}

		static Mat4 rotate(float angleRadians, const Vector3& axis) {
			Vector3 a = axis.normalized();
			float c = std::cos(angleRadians);
			float s = std::sin(angleRadians);
			float ic = 1.0f - c;

			Mat4 result(1.0f);
			result(0, 0) = c + a.x * a.x * ic;
			result(0, 1) = a.x * a.y * ic - a.z * s;
			result(0, 2) = a.x * a.z * ic + a.y * s;

			result(1, 0) = a.y * a.x * ic + a.z * s;
			result(1, 1) = c + a.y * a.y * ic;
			result(1, 2) = a.y * a.z * ic - a.x * s;

			result(2, 0) = a.z * a.x * ic - a.y * s;
			result(2, 1) = a.z * a.y * ic + a.x * s;
			result(2, 2) = c + a.z * a.z * ic;
			return result;
		}

		static Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane) {
			Mat4 result(0.0f);
			float tanHalfFov = std::tan(fovRadians / 2.0f);

			result(0, 0) = 1.0f / (aspect * tanHalfFov);
			result(1, 1) = 1.0f / tanHalfFov;
			result(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
			result(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
			result(3, 2) = -1.0f;
			return result;
		}

		static Mat4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
			Vector3 f = (center - eye).normalized();
			Vector3 r = f.cross(up).normalized();
			Vector3 u = r.cross(f);

			Mat4 result(1.0f);
			result(0, 0) = r.x; result(0, 1) = r.y; result(0, 2) = r.z;
			result(1, 0) = u.x; result(1, 1) = u.y; result(1, 2) = u.z;
			result(2, 0) = -f.x; result(2, 1) = -f.y; result(2, 2) = -f.z;
			result(0, 3) = -r.dot(eye);
			result(1, 3) = -u.dot(eye);
			result(2, 3) = f.dot(eye);
			return result;
		}
	};

	struct Math {
		static float getDistance3D(FiscionX::Vector3 pos1, FiscionX::Vector3 pos2);
		static float radians(float degrees);
		static float degrees(float radians);
		static float clamp(float value, float min, float max);
		static float min(float a, float b);
		static float max(float a, float b);
		static float lerp(float a, float b, float t);
		static float sin(float angle);
		static float cos(float angle);
		static float tan(float angle);
		static float asin(float value);
		static float acos(float value);
		static float atan(float value);
		static float atan2(float y, float x);
		static int   sign(float value);
		static int   randInt(int min, int max);
		static float randFloat(float min, float max);
		static float sqrt(float value);
		static float pow(float base, float exponent);
		static float abs(float value);
		static int   abs(int value);
		static float floor(float value);
		static float ceil(float value);
		static float round(float value);
		static float log(float value);
		static float angleBetween(Vector3 a, Vector3 b);
	};

	struct UI{
		struct Image {
			GLuint texture;
			GLuint VAO, VBO;
			static GLuint shader;

			float alpha = 1.0f;
			float rotation = 0.0f; // radians
			FiscionX::Vector2 scale = FiscionX::Vector2(1.0f);

			float aspect_ratio = 1.0f;
			int w_, h_;

			Image(const char* path, FiscionX::Vector2 scl);
			void flip(bool flipx, bool flipy);
			void draw(FiscionX::Vector2 position);
		};

		struct Font {
			std::map<char, Glyph> Characters;
			GLuint textureAtlas;
			int atlasWidth, atlasHeight;
			GLuint textVAO, textVBO;

			Font(const char* fontPath, int pixelSize = 48);
			~Font();
		};

		static void DrawText(Font* font, const char* text, FiscionX::Vector2 position, float size, FiscionX::Vector4 color, float rotation);

		struct Video {
			static GLuint shaderVideo;
			// libVLC handles
			libvlc_instance_t* vlcInstance = nullptr;
			libvlc_media_t* media = nullptr;
			libvlc_media_player_t* mediaPlayer = nullptr;

			// pixel buffer (RV32 = RGBA little-endian)
			std::vector<unsigned char> pixels;
			std::mutex pixelMutex;
			bool hasNewFrame = false;

			// GL objects (quad similar to UI::Image)
			GLuint texture = 0;
			GLuint VAO = 0, VBO = 0, EBO = 0;

			// sizing / layout like UI::Image
			float alpha = 1.0f;
			float rotation = 0.0f; // radians
			FiscionX::Vector2 scale = FiscionX::Vector2(1, 1);
			float aspect_ratio = 1.0f;
			int width = 0, height = 0;

			// ctor/dtor
			Video(const char* path, int desiredWidth = 640, int desiredHeight = 360);
			~Video();

			// controls
			void play();
			void pause();
			void stop();

			// call in main thread each frame: uploads new frame to GPU
			void update();

			// draw using same shader/uniforms as UI::Image
			void draw(FiscionX::Vector2 position);

			void createTextureIfNeeded();
			static void* lockCallback(void* opaque, void** planes);
			static void unlockCallback(void* opaque, void* const picture, void* const* planes);
			static void displayCallback(void* opaque, void* picture);
		};
	};

	struct Camera {
		FiscionX::Vector3 position = FiscionX::Vector3(0, 2, 3);
		FiscionX::Vector3 front = FiscionX::Vector3(0, 0, -1);
		FiscionX::Vector3 up = FiscionX::Vector3(0, 1, 0);
		FiscionX::Vector3 right;
		float     yaw = -90.0f;
		float     pitch = 0.0f;
		float     speed = 2.0f;
		float     sensitivity = 0.1f;
		float     fov = 60.0f;
		bool      canLook = true;

		Camera();
		FiscionX::Mat4 GetView();
		void ProcessMouse(float xoffset, float yoffset);
		void updateVectors();
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
		FiscionX::Vector3 pos;
		float minDist;
		float maxDist;
		bool looping;
		float volume;
		FMOD::Channel* curr_channel;
		FMOD::DSP* dsp;

		Sound(const char* path, bool loop = false, bool threedimensional = false, FiscionX::Vector3 position = FiscionX::Vector3(0, 0, 0),
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
		FiscionX::Vector3 position = FiscionX::Vector3(0, 0, 0);
		FiscionX::Vector3 direction = FiscionX::Vector3(0, 0, 0);
		FiscionX::Vector3 color = FiscionX::Vector3(1.0f, 1.0f, 1.0f);
		float       intensity = 1.0f;

		float       maxDistance = 300.0f;
		float       cutOff = 0;
		float       outerCutOff = 0;
		float       constant = 0;
		float       linear = 0;
		float       quadratic = 0;

		bool        hasGlow = false;
		FiscionX::Vector3   glowColor = FiscionX::Vector3(0, 0, 0);
		float       glowRadius = 0;

		bool enableShadows = true;
		float lastShadowUpdateTime = -9999.0f;      // last time generated shadow maps
		float shadowUpdatePeriod = 0.03f;           // seconds between updates (20 Hz default)
		glm::vec3 lastPosition = glm::vec3(FLT_MAX); // last light position (used to detect movement)

		Light();
	};

	struct Physics {
		struct GLDebugDrawer : public btIDebugDraw {
			int m_debugMode = DBG_DrawWireframe;

			void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

			void setDebugMode(int debugMode) override;

			int getDebugMode() const override;

			// Métodos obrigatórios da classe base
			void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) override;
			void reportErrorWarning(const char* warningString) override;
			void draw3dText(const btVector3&, const char*) override;
		};

		static btBroadphaseInterface* broadphase;
		static btDefaultCollisionConfiguration* collisionConfig;
		static btCollisionDispatcher* dispatcher;
		static btSequentialImpulseConstraintSolver* solver;
		static btDiscreteDynamicsWorld* DynamicWorld;
		static float maxCollisionDistance;

		static GLuint debugVAO, debugVBO;
		static GLuint debugShader;
		static std::vector<float> debugLines;
		static GLDebugDrawer* debugDrawer;

		struct Shape {
			btCollisionShape* shape = nullptr;
			btGImpactMeshShape* gshape = nullptr;
			btRigidBody::btRigidBodyConstructionInfo info;
			btDefaultMotionState motion;
			Shape(btCollisionShape* _shape, btRigidBody::btRigidBodyConstructionInfo _info, btDefaultMotionState _motion, btGImpactMeshShape* _gshape);
		};

		struct Rigidbody {
			btRigidBody* body;
			Shape shape;
			Rigidbody(Shape _shape);

			void activate();
			void applyForce(Vector3 force, Vector3 relPos);
			void applyImpulse(Vector3 impulse, Vector3 relPos);
			void applyTorque(Vector3 torque);
			void applyCentralForce(Vector3 force);
			void applyCentralImpulse(Vector3 impulse);
			void applyTorqueImpulse(Vector3 torqueImpulse);
			void setLinearVelocity(Vector3 velocity);
			void setAngularVelocity(Vector3 velocity);
			void setTransform(Vector3 position, Vector3 rotation);
			void setLinearFactor(Vector3 factor);
			void setAngularFactor(Vector3 factor);
			void setCollisionShape(Shape* newShape);
			void clearForces();
			Vector3 getPosition();
			Vector3 getRotation();
			void setMass(float mass, Vector3 inertia);
			void setFriction(float friction);
			void setRollingFriction(float friction);
			void setDamping(float damping);
			void lockAxis(Vector3 axis);
			void setBouncingFactor(float factor);
		};

		struct Vehicle {
			btRaycastVehicle::btVehicleTuning* tuning;
			btVehicleRaycaster* raycaster;
			btRaycastVehicle* vehicle;

			struct WheelInfo {
				btWheelInfo* info;
			};

			Vehicle(Rigidbody* chassiBody);
			void addWheel(FiscionX::Vector3 relativePosition, FiscionX::Vector3 wheelDirectionCS0, FiscionX::Vector3 wheelAxleCS,
				float suspensionRestLength, float wheelRadius, bool);
			int getNumWheels();
			WheelInfo& getWheelInfo(int index);

			void update(float deltaTime);

			void applyEngineForce(float force, int wheelIndex);
			void setSteeringValue(float value, int wheelIndex);
			void setBrake(float brake, int wheelIndex);
			FiscionX::Vector3 getWheelWorldPosition(int wheelIndex);
			FiscionX::Vector3 getWheelRotation(int wheelIndex);
			float getCurrentSpeedKmh();
		};

		static struct Raycast {
			static bool CheckCollisionWithBody(FiscionX::Physics::Rigidbody* body, Vector3 origin, Vector3 end);
			static const btRigidBody* GetFirstBodyCollided(Vector3 origin, Vector3 end);
		};

		static void DrawDebugWorld(glm::mat4 projection, glm::mat4 view);

		static void CreatePhysicsWorld(Vector3 gravity, int maxIterations);
		static Shape CreateCapsuleShape(Vector3 position, Vector3 rotation, float radius, float height, float mass);
		static Shape CreateBoxShape(Vector3 position, Vector3 rotation, Vector3 scale, float mass);
		static Shape CreateCyllinderShape(Vector3 position, Vector3 rotation, float radius, float height, float mass);
		static Shape CreateSphereShape(Vector3 position, Vector3 rotation, float radius, float mass);
		static Shape CreateConeShape(Vector3 position, Vector3 rotation, float radius, float height, float mass);

		static btTriangleMesh* LoadMeshFromFile(const char* path, FiscionX::Vector3 scale);
		static Shape CreateMeshShape(const char* path, FiscionX::Vector3 position, FiscionX::Vector3 rotation, FiscionX::Vector3 scale, float mass);

		static bool CheckCollisionBetween(Rigidbody* bodyA, Rigidbody* bodyB);
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
		FiscionX::Vector3 position, rotation, scale;
		glm::mat4 physicsSyncTransformMatrix = glm::mat4(1.0f);
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

		bool isAffectedByLight = true;
		bool castsShadows = true;
		bool acceptsShadows = true;

		const std::vector<glm::mat4>& getBoneTransforms() const;
		Model(const std::string& path, Vector3 pos, Vector3 rot, Vector3 scl);
		void playAnim(const std::string& name, bool repeat, const std::string& next = "");
		void update(float deltaTime);
		GLuint getBaseColorTexture(const tinygltf::Model& model, int materialIndex);
		GLuint getDiffuseTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex);
		GLuint getGlossinessTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex);
		GLuint getNormalMapTexture(const tinygltf::Model& model, int materialIndex);
		void init(const std::string& path);
		void updateOcclusion(const glm::mat4& viewProj);
		void syncTransformWithBody(Physics::Rigidbody* body, Vector3 positionOffset, Vector3 rotationOffset);
		void drawSubMesh(
			const SubMesh& mesh,
			GLuint shader,
			const glm::mat4& modelMatrix,
			const glm::mat4& lightSpaceMatrix,
			GLuint depthMap,
			bool depthPass
		);
		void draw(GLuint shader, const glm::mat4& lightSpaceMatrix, GLuint depthMap, bool depthPass, FiscionX::Mat4 view, FiscionX::Mat4 projection);
	};

	struct Input {
		static FiscionX::Vector2 mousePosition;
		static FiscionX::Vector2 mouseDelta;
		static FiscionX::Vector2 scrollOffset;

		static bool GetKeyPressed(int key);
		static bool GetKeyReleased(int key);
		static FiscionX::Vector2 GetMousePosition();
		static FiscionX::Vector2 GetMouseDelta();
		static FiscionX::Vector2 GetScrollOffset();
		static bool GetMouseButtonPressed(int button);
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

		//static unsigned int SHADOW_WIDTH;
		//static unsigned int SHADOW_HEIGHT;
		//static unsigned int SHADOW_CUBE_SIZE;

		static int DIR_SHADOW_SIZE;
		static int SPOT_SHADOW_SIZE;
		static int POINT_SHADOW_SIZE;

		static float        NEAR_PLANE;
		static float         FAR_PLANE;
		static float         AMBIENT_LIGHT_INTENSITY;
		static glm::vec3     AMBIENT_LIGHT_SKYCOLOR;
		static glm::vec3     AMBIENT_LIGHT_GROUNDCOLOR;
		static float SHADOW_VIEW_RADIUS;
		static GLuint depthMapFBO;
		static GLuint depthMap;

		static GLuint textShader;

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

		static bool enableShaderCache;
		static bool enableModelCache;

		static void CreateShadowMap(ShadowMap& sm, int LIGHT_TYPE);
		static void CreateAllShadowMaps();
		static glm::mat4 ComputeLightSpaceMatrix(const Light& L);
		static void RenderAllShadowPasses(FiscionX::Mat4 view, FiscionX::Mat4 projection, FiscionX::Mat4 viewProj);

		static void SetCursorMode(int mode);
		static void SetCacheSettings(bool _enableShaderCache, bool _enableModelCache);

		static void NewWindow(int width, int height, const char* window_label);
		static void Set3DSettings(const int _DIRECTIONAL_LIGHT_SHADOW_SIZE, const int _SPOT_LIGHT_SHADOW_SIZE,
			const int _POINT_LIGHT_SHADOW_SIZE, const float _SHADOW_VIEW_RADIUS, const float _NEAR_PLANE, const float _FAR_PLANE);
		static void ClockTick();
		static void SetWindowSize(int width, int height);
		static void SetWindowIcon(const char* path);
		static void SetWindowFullscreen(bool fullscreen, int monitorIndex);
		static void SortModels();
		static void Terminate();

		static struct Draw {
			static void SwapBuffers();
			static void ClearBackground(float r, float g, float b, float a);
		};
	};
};

#endif