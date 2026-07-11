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
#include <chrono>
#include <algorithm>
#include <cmath>
#include <math.h>
#include <climits>
#include <set>
#include <bitset>
#include <array>
#include <unordered_map>
#include <random>
#include <queue>
#include <mutex>
#include <numeric>
#include <thread>
#include <Windows.h>

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
#include "dependencies/bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"

#include "meshoptimizer.h"

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

		Vector2(const glm::vec2& g) : x(g.x), y(g.y) {}

		Vector2& operator=(const glm::vec2& g) {
			x = g.x; y = g.y;
			return *this;
		}

		operator glm::vec2() const {
			return glm::vec2(x, y);
		}

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

		Vector3(const glm::vec3& g) : x(g.x), y(g.y), z(g.z) {}

		Vector3& operator=(const glm::vec3& g) {
			x = g.x; y = g.y; z = g.z;
			return *this;
		}

		operator glm::vec3() const {
			return glm::vec3(x, y, z);
		}

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

		Vector4(const glm::vec4& g) : x(g.x), y(g.y), z(g.z), w(g.w) {}

		Vector4& operator=(const glm::vec4& g) {
			x = g.x; y = g.y; z = g.z; w = g.w;
			return *this;
		}

		operator glm::vec4() const {
			return glm::vec4(x, y, z, w);
		}

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

		Mat4(const glm::mat4& g) {
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					m[c][r] = g[c][r];
		}

		Mat4& operator=(const glm::mat4& g) {
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					m[c][r] = g[c][r];
			return *this;
		}

		operator glm::mat4() const {
			glm::mat4 result;
			for (int c = 0; c < 4; c++)
				for (int r = 0; r < 4; r++)
					result[c][r] = m[c][r];
			return result;
		}

		Mat4(float diag = 1.0f) {
			for (int col = 0; col < 4; col++)
				for (int row = 0; row < 4; row++)
					m[col][row] = (col == row ? diag : 0.0f);
		}

		static Mat4 identity() {
			return Mat4(1.0f);
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
			result(0, 0) = r.x;  result(0, 1) = r.y;  result(0, 2) = r.z;
			result(1, 0) = u.x;  result(1, 1) = u.y;  result(1, 2) = u.z;
			result(2, 0) = -f.x; result(2, 1) = -f.y; result(2, 2) = -f.z;
			result(0, 3) = -r.dot(eye);
			result(1, 3) = -u.dot(eye);
			result(2, 3) = f.dot(eye);
			return result;
		}
	};

	struct Math {
		static float getDistance3D(FiscionX::Vector3 pos1, FiscionX::Vector3 pos2);
		// OPTIM: squared-distance version — skips the sqrt entirely. Use this for
		// any "is X farther than N units" check (culling, LOD, etc.), which only
		// needs the comparison, never the actual distance value. Compare against
		// N*N instead of N.
		static float getDistanceSq3D(FiscionX::Vector3 pos1, FiscionX::Vector3 pos2);
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
		static Vector3 lookAt3D(const Vector3& position, const Vector3& target);
		static Vector3 toEulerAngles(
			const FiscionX::Vector3& lookAtForward,
			FiscionX::Vector2 axisLock
		);
	};

	struct UI {
		struct Image {
			GLuint texture;
			GLuint VAO, VBO;
			static GLuint shader;

			float alpha = 1.0f;
			float rotation = 0.0f; // radians
			FiscionX::Vector2 scale = FiscionX::Vector2(1.0f);

			float aspect_ratio = 1.0f;
			int w_, h_;

			Image(const char* path);
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

			libvlc_instance_t* vlcInstance = nullptr;
			libvlc_media_t* media = nullptr;
			libvlc_media_player_t* mediaPlayer = nullptr;

			std::vector<unsigned char> pixels;
			std::mutex pixelMutex;
			bool hasNewFrame = false;

			GLuint texture = 0;
			GLuint VAO = 0, VBO = 0, EBO = 0;

			float alpha = 1.0f;
			float rotation = 0.0f; // radians
			FiscionX::Vector2 scale = FiscionX::Vector2(1, 1);
			float aspect_ratio = 1.0f;
			int width = 0, height = 0;

			Video(const char* path, int desiredWidth = 640, int desiredHeight = 360);
			~Video();

			void play();
			void pause();
			void stop();

			void update();
			void draw(FiscionX::Vector2 position);

			void createTextureIfNeeded();
			static void* lockCallback(void* opaque, void** planes);
			static void unlockCallback(void* opaque, void* const picture, void* const* planes);
			static void displayCallback(void* opaque, void* picture);
		};

		struct Button {
			FiscionX::Vector2 position;
			FiscionX::Vector2 size;
			FiscionX::UI::Image* image = nullptr;
			FiscionX::UI::Font* font = nullptr;
			std::string text = "";
			FiscionX::Vector4 textColor = 0;
			FiscionX::Vector2 textOffset = 0;
			bool textCentered = false;
			FiscionX::Vector4 normalColor = 1;
			FiscionX::Vector4 hoverColor = 1;
			FiscionX::Vector4 pressColor = 1;
			bool isHovering = false;
			bool isPressed = false;
			bool canBePressed = true;
			float cooldownToNextPress = 0.2f; // Measured in seconds!
			float timerToNextPress = 0;
			void (*PressCallback)() = nullptr;

			Button(FiscionX::Vector2 _position = 0, FiscionX::Vector2 _size = 0, FiscionX::UI::Image* _image = nullptr,
				FiscionX::UI::Font* _font = nullptr, std::string _text = "", FiscionX::Vector4 _textColor = 0, bool _textCentered = false,
				FiscionX::Vector2 _textOffset = 0, FiscionX::Vector4 _normalColor = 1, FiscionX::Vector4 _hoverColor = 1,
				FiscionX::Vector4 _pressColor = 1, float cooldownBetweenPresses = 0.5f, void (*_PressCallback)() = nullptr);
			void update(float deltaTime);
			void draw();
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

	struct Image3D {
		GLuint texture;
		GLuint VAO, VBO;
		static GLuint shader;

		Vector3 position = Vector3(0.0f);

		float alpha = 1.0f;
		Vector3 rotation = FiscionX::Vector3(0.0f); // radians
		FiscionX::Vector3 scale = FiscionX::Vector3(1.0f);

		float aspect_ratio = 1.0f;
		int w_, h_;

		Image3D(const char* path);
		void draw(glm::mat4 view, glm::mat4 projection);
	};

	struct ShadowMap {
		GLuint fbo = 0;
		GLuint depthMap = 0;
		glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
		std::vector<glm::mat4> cascadeLightSpaceMatrices;
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
		float yaw, pitch;
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
		float shadowUpdatePeriod = 0.01f;           // seconds between updates (~100 Hz default)
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

		struct DebugTracer {
			Vector3 a, b, color;
			float life;
		};
		static std::vector<DebugTracer> activeTracers;
		static float TRACER_LIFETIME;
		static void PushDebugLine(Vector3 a, Vector3 b, Vector3 color);
		static void PushImpactMarker(Vector3 point, Vector3 color);
		static void AddTracerLine(Vector3 a, Vector3 b, Vector3 color);
		static void AddImpactMarkerTracer(Vector3 point, Vector3 color);
		static void UpdateAndPushTracers(float dt);


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
			Vector3 getLinearVelocity();
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
			void setCenterOfMass(const btVector3& newCOMOffset);
		};

		static std::vector<Physics::Rigidbody*> worldBodies;

		enum class JointType {
			POINT2POINT,
			HINGE,
			CONETWIST,
			DOF6,
			DOF6_SPRING,
			FIXED
		};

		struct Joint {
			JointType type;

			btRigidBody* bodyA = nullptr;
			btRigidBody* bodyB = nullptr;

			btTransform frameA = btTransform::getIdentity();
			btTransform frameB = btTransform::getIdentity();

			bool collideConnected = false;

			btVector3 linearLower = btVector3(0, 0, 0);
			btVector3 linearUpper = btVector3(0, 0, 0);
			btVector3 angularLower = btVector3(0, 0, 0);
			btVector3 angularUpper = btVector3(0, 0, 0);

			bool useSpring[6] = { false,false,false,false,false,false };
			btScalar stiffness[6] = { 0,0,0,0,0,0 };
			btScalar damping[6] = { 0,0,0,0,0,0 };

			btScalar swing1 = SIMD_PI;
			btScalar swing2 = SIMD_PI;
			btScalar twist = SIMD_PI;

			btScalar hingeLower = 0;
			btScalar hingeUpper = 0;

			btTypedConstraint* constraint = nullptr;
		};

		static btTypedConstraint* CreateJoint(FiscionX::Physics::Joint& desc);

		struct Vehicle {
			btRaycastVehicle::btVehicleTuning* tuning;
			btVehicleRaycaster* raycaster;
			btRaycastVehicle* vehicle;
			Rigidbody* chassi;

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
			glm::vec3 getPosition();
			glm::vec3 getForwardVec();
		};

		static struct Raycast {
			static bool CheckCollisionWithBody(FiscionX::Physics::Rigidbody* body, Vector3 origin, Vector3 end);
			static const btRigidBody* GetFirstBodyCollided(Vector3 origin, Vector3 end);
			static const btRigidBody* GetFirstBodyCollided(Vector3 origin, Vector3 end, Vector3& outHitPoint, Vector3& outHitNormal);
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

		// FEATURE: per-instance stream buffer (model matrix + normal matrix) used
		// only by the instanced draw path (static models, main color pass). Lazily
		// created/attached to `vao` the first time this submesh is drawn instanced;
		// re-filled (glBufferData orphaning) every call after that. Never touched
		// by the regular per-instance drawSubMesh path, so nothing about the
		// existing non-instanced rendering changes.
		mutable GLuint instanceVBO = 0;

		// Per-submesh axis-aligned bounding box (local space, computed at load time)
		glm::vec3 aabbMin = glm::vec3(1e30f);
		glm::vec3 aabbMax = glm::vec3(-1e30f);

		// OPTIM (per-submesh LOD): centro local do AABB, usado para medir a distância
		// câmera→submesh individualmente, em vez da posição única do Instance inteiro.
		glm::vec3 aabbCenter() const { return (aabbMin + aabbMax) * 0.5f; }

		// CPU-side copies of pos+indices+uvs+skinning kept for LOD generation (cleared after buildLODs)
		std::vector<glm::vec3>          cpuPositions;  // one per vertex
		std::vector<uint32_t>           cpuIndices;    // triangle list
		std::vector<glm::vec2>          cpuUVs;        // one per vertex (matches cpuPositions)
		std::vector<glm::u16vec4>       cpuJoints;     // one per vertex (0 if not skinned)
		std::vector<glm::vec4>          cpuWeights;    // one per vertex (0 if not skinned)

		// LOD variants for this submesh (indices match Model::lodDistances)
		struct LODLevel {
			GLuint vao = 0, vbo = 0, ebo = 0;
			GLuint jbo = 0, wbo = 0;          // skinning buffers (0 if not skinned)
			size_t indexCount = 0;
			GLenum indexType = GL_UNSIGNED_INT;
			// FEATURE: same purpose as SubMesh::instanceVBO, but for this LOD tier's
			// own VAO (each LOD has its own vertex/index buffers, so it needs its
			// own instance stream buffer too).
			mutable GLuint instanceVBO = 0;
		};
		std::vector<LODLevel> lodLevels;

		GLuint baseColorTex = 0;
		GLuint normalMapTex = 0;
		GLuint aoTex = 0;          // Ambient Occlusion Map (glTF occlusionTexture, canal R)
		GLuint transmissionTex = 0;
		GLuint glossinessTex = 0;
		GLuint specularF0Tex = 0;
		GLuint metallicTex = 0;
		bool useMetalRoughness = false;
		float metallicFactor = 1.0f; // glTF spec default = 1.0
		float roughnessFactor = 1.0f; // glTF spec default = 1.0

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
		bool isSkinned = false;

		tinygltf::Model gltfModel;
		std::vector<tinygltf::Node> nodes;
		std::vector<tinygltf::Skin> skins;

		std::map<std::string, tinygltf::Animation> animations;

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

		// OPTIM: texture cache — imgIndex -> GL texture ID
		// Prevents re-uploading the same image when multiple materials reference it
		std::unordered_map<int, GLuint> textureCache;

		// Camera extracted from the GLB
		int  cameraNodeIndex = -1;   // index of the camera node found in the glTF (-1 = none)
		bool drivesCamera = false; // true while this model is driving Core::Camera
		bool cameraAnimFinished = false; // true once a non-repeating anim has reached its end

		float alpha = 1.0f;

		// OPTIM: calculado uma única vez em init(), depois que meshes é montado.
		// Antes, DrawTransparentPass calculava `base` (translate*eulerAngleXYZ*scale)
		// para TODA instância de TODO modelo, todo frame, e só depois de montar a
		// matriz é que descobria (dentro do loop de submeshes) que o modelo nem tinha
		// nenhuma submesh BLEND. Com a flag, modelos 100% opacos são pulados de cara.
		bool hasBlendSubMesh = false;
		float maxViewDistance = 120;

		// OPTIM: grama (e qualquer instância "achatada": só posição, rotação em
		// Y e escala uniforme, sem physics sync, sem stretch em X/Z) manda 5
		// floats/instância (posição.xyz + rotY + escala) em vez dos 25 floats
		// (mat4 model + mat3 normalMatrix) do caminho genérico — 5x menos bytes
		// por instância no glBufferData de todo frame, e a montagem da matriz
		// (translate*eulerAngleXYZ*scale + normalMatrix) deixa de rodar na CPU
		// pra 200k+ instâncias: o vertex shader remonta a matriz a partir dos
		// 5 floats (ver vertexGrassInstanced em FiscionShaders.h). Habilite
		// manualmente só em Models cujas instâncias respeitam essas restrições
		// (ex.: grassModel->useCompactInstancing = true;) — trees/props com
		// rotação em X/Z, escala não-uniforme ou physics sync devem continuar
		// no caminho genérico (drawSubMeshInstanced).
		bool useCompactInstancing = false;

		struct Instance {
			FiscionX::Vector3 position;
			FiscionX::Vector3 rotation;
			FiscionX::Vector3 scale;
			bool visible = true;

			std::vector<tinygltf::Node> nodes;
			std::vector<tinygltf::Skin> skins;

			bool isAffectedByLight = true;
			bool castsShadows = true;
			bool acceptsShadows = true;

			// FIX: estava "false" por padrão e nada no código jamais setava essa flag
			// como true (nem no loader, nem no main.cpp) — ou seja, o frustum culling
			// por instância nunca era de fato aplicado, mesmo com o teste
			// (isSubMeshInFrustum) pronto e sendo chamado condicionalmente. Toda
			// instância de todo modelo era sempre desenhada por inteiro, mesmo
			// completamente fora da câmera. Se algum modelo específico precisar
			// nunca ser cortado (ex: view model da arma preso na câmera), desligue
			// essa flag manualmente só naquela instância.
			bool enableFrustumCulling = true;

			// FIX: occlusion query custa ~9 chamadas ao driver por submesh (glBeginQuery/
			// glBindVertexArray/glUseProgram/glDrawElements/glEndQuery + os state changes em
			// volta), e isso só compensa quando o objeto em si é caro de desenhar (prédios,
			// veículos grandes) — a query serve pra evitar um draw call caro. Em objetos
			// baratos (grama, folhagem, detritos), a query custa mais caro do que o desenho
			// que ela tentaria evitar. Default true (mantém o comportamento atual pra tudo
			// que já usava occlusion), mas instâncias como grama devem setar isso pra false
			// (ex.: grass.modelInst->enableOcclusionQuery = false;).
			bool enableOcclusionQuery = true;

			int  cameraNodeIndex = -1;   // index of the camera node found in the glTF (-1 = none)
			bool drivesCamera = false; // true while this model is driving Core::Camera
			bool cameraAnimFinished = false; // true once a non-repeating anim has reached its end

			glm::mat4 physicsSyncTransformMatrix = glm::mat4(1.0f);

			// OPTIM: cache of the last-built world matrix (translate * eulerAngleXYZ *
			// scale, or the physics-synced equivalent) plus the position/rotation/
			// scale/physicsSyncTransformMatrix values it was built from. getInstBase()
			// below reuses this instead of rebuilding the matrix (eulerAngleXYZ is
			// trig-heavy) every single frame for every instance — critical when a
			// Model has hundreds of thousands of instances (grass) that never move.
			// Comparison is plain float equality: safe because if nothing wrote to
			// these fields since last frame, the bits are identical; any real change
			// (movement, wind sway, physics) naturally invalidates the cache.
			mutable glm::mat4 cachedInstBase = glm::mat4(1.0f);
			mutable FiscionX::Vector3 cachedPosition;
			mutable FiscionX::Vector3 cachedRotation;
			mutable FiscionX::Vector3 cachedScale;
			mutable glm::mat4 cachedPhysicsSync = glm::mat4(1.0f);
			mutable bool hasCachedInstBase = false;

			// Returns this instance's world matrix, rebuilding it only if position/
			// rotation/scale/physicsSyncTransformMatrix changed since the cached copy.
			inline glm::mat4 getInstBase() const {
				if (hasCachedInstBase &&
					position.x == cachedPosition.x && position.y == cachedPosition.y && position.z == cachedPosition.z &&
					rotation.x == cachedRotation.x && rotation.y == cachedRotation.y && rotation.z == cachedRotation.z &&
					scale.x == cachedScale.x && scale.y == cachedScale.y && scale.z == cachedScale.z &&
					physicsSyncTransformMatrix == cachedPhysicsSync) {
					return cachedInstBase;
				}

				glm::mat4 instBase = (physicsSyncTransformMatrix != glm::mat4(1.0f))
					? glm::scale(physicsSyncTransformMatrix, glm::vec3(scale.x, scale.y, scale.z))
					: glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z))
					* glm::eulerAngleXYZ(rotation.y, rotation.x, rotation.z)
					* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

				cachedInstBase = instBase;
				cachedPosition = position;
				cachedRotation = rotation;
				cachedScale = scale;
				cachedPhysicsSync = physicsSyncTransformMatrix;
				hasCachedInstBase = true;
				return instBase;
			}

			// OPTIM: true when scale.x == scale.y == scale.z (within a small epsilon).
			// For a uniformly-scaled instance, the normal matrix is just the rotation
			// part of the model matrix (orthonormal) — no need for the expensive
			// glm::transpose(glm::inverse(modelMatrix)). Used by both the instanced
			// and non-instanced draw paths to skip the mat4 inverse for the common
			// case (grass, trees, props placed without non-uniform stretching).
			inline bool hasUniformScale() const {
				const float eps = 1e-5f;
				return std::abs(scale.x - scale.y) < eps && std::abs(scale.y - scale.z) < eps;
			}

			float alpha = 1.0f;

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

			Model* model;

			static bool isSubMeshInFrustum(const SubMesh& sub,
				const glm::mat4& modelMatrix,
				const glm::vec4 planes[6]);
			void computeSubMeshVisibility(const glm::mat4& viewProj,
				std::vector<bool>& outVisible) const;
			void updateOcclusion(const glm::mat4& viewProj);
			void syncTransformWithBody(Physics::Rigidbody* body, Vector3 positionOffset, Vector3 rotationOffset);
			void playAnim(const std::string& name, bool repeat, const std::string& next = "");
			void update(float deltaTime, bool isSkinned);
		};
		std::vector<Instance> instances;

		glm::vec3 boundingCenter = glm::vec3(0.0f);
		float boundingRadius = 1.1f;
		bool boundsComputed = false;

		// FIX: calcula boundingCenter/boundingRadius (esfera que envolve a união de
		// TODAS as submeshes, em espaço "de instância") uma única vez por Model.
		// Existiam DUAS cópias quase idênticas deste cálculo (no passe opaco de
		// Model::draw e em Core::DrawTransparentPass), e as duas cometiam o mesmo
		// erro: usavam sub.aabbMin/aabbMax crus, sem passar por sub.transform (o
		// transform do node do glTF — posição/rotação/ESCALA — aplicado em cima dos
		// vértices na hora de desenhar, ver processNode() no loader). Pra modelos
		// exportados sem "aplicar a escala" no node (caso da grama: node com escala
		// pequena, geometria só vira o tamanho final através desse transform), isso
		// dava uma esfera praticamente do tamanho do mesh CRU (ex: boundingRadius
		// ~0.01 pra uma folha de grama que na verdade mede dezenas de cm depois do
		// transform) — ou seja, a esfera não correspondia nem de longe ao volume
		// realmente desenhado, tanto errando a posição do centro quanto o raio.
		// Agora os 8 cantos do AABB de cada submesh são transformados por
		// sub.transform ANTES de entrar na união, então o resultado já está no
		// mesmo espaço que instBase/base usam (posição/rotação/escala da própria
		// instância é aplicada depois, em cima disso).
		void computeBoundsIfNeeded() {
			if (boundsComputed || meshes.empty()) return;
			glm::vec3 unionMin(1e30f), unionMax(-1e30f);
			for (const auto& sub : meshes) {
				const glm::vec3& lo = sub.aabbMin;
				const glm::vec3& hi = sub.aabbMax;
				glm::vec3 corners[8] = {
					glm::vec3(sub.transform * glm::vec4(lo.x, lo.y, lo.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(hi.x, lo.y, lo.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(lo.x, hi.y, lo.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(hi.x, hi.y, lo.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(lo.x, lo.y, hi.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(hi.x, lo.y, hi.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(lo.x, hi.y, hi.z, 1.0f)),
					glm::vec3(sub.transform * glm::vec4(hi.x, hi.y, hi.z, 1.0f))
				};
				for (const auto& c : corners) {
					unionMin = glm::min(unionMin, c);
					unionMax = glm::max(unionMax, c);
				}
			}
			boundingCenter = (unionMin + unionMax) * 0.5f;
			boundingRadius = glm::length(unionMax - boundingCenter);
			boundsComputed = true;
		}

		std::vector<float> lodDistances;

		void buildLODs(const std::vector<float>& ratios);

		struct UniformCache {
			// Uniforms simples
			GLint model = -1, lightSpaceMatrix = -1, alphaMode = -1, alphaCutoff = -1;
			GLint baseColorTex = -1, normalMapTex = -1, shadowMap = -1;
			GLint aoTex = -1, hasAOMap = -1;     // Ambient Occlusion Map
			GLint glossinessTex = -1, hasGlossinessMap = -1, glossinessInAlphaOfSpecular = -1;
			GLint specularF0Tex = -1, hasSpecularF0Map = -1;
			GLint metallicTex = -1, useMetalRoughness = -1;
			GLint metallicFactor = -1, roughnessFactor = -1;
			GLint environmentStrength = -1, environmentSkyColor = -1, environmentGroundColor = -1;
			GLint reflectionsStrength = -1;
			GLint isAffectedByLight = -1, acceptsShadows = -1;
			GLint alpha = -1, numLights = -1;
			GLint hasNormalMap = -1;
			GLint hdrExposure = -1;
			// Fog uniforms
			GLint fogColor = -1, fogDensity = -1, fogStart = -1, fogEnd = -1, fogType = -1;
			// IBL uniforms
			GLint irradianceMap = -1, prefilterMap = -1, brdfLUT = -1, hasIBL = -1;
			// Uniforms de luz (até 10)
			struct LightUniforms {
				GLint type = -1, pos = -1, dir = -1, color = -1, intensity = -1;
				GLint maxDist = -1, cutOff = -1, outerCutOff = -1;
				GLint constant = -1, linear = -1, quadratic = -1;
				GLint hasGlow = -1, glowColor = -1, glowRadius = -1;
			} lights[10];
			GLuint cachedShader = 0;
		};
		mutable UniformCache uniformCache;

		// OPTIM: cache de uniforms usados uma vez por frame em Model::draw()
		// (view/projection/viewPos/numLights/reflectionsStrength/sombra direcional/sombras por luz).
		// Evita glGetUniformLocation + std::string/std::to_string a cada frame por modelo.
		struct FrameUniformCache {
			GLuint cachedShader = 0;
			GLint view = -1, projection = -1, viewPos = -1, numLights = -1, reflectionsStrength = -1;
			GLint shadowMapDir = -1, cascadeCount = -1;
			static constexpr int MAX_CASCADES = 16;
			GLint cascadePlaneDistances[MAX_CASCADES];
			GLint cascadeLightSpaceMatrices[MAX_CASCADES];
			// Locations para amostras de sombra por luz (até 10 luzes, igual UniformCache::lights)
			GLint pointShadowMap[10];  // samplerCube, luzes do tipo POINT
			GLint spotShadowMap[10];   // sampler2D,   luzes do tipo SPOT
		};
		mutable FrameUniformCache frameUniformCache;

		// OPTIM: mesma ideia do FrameUniformCache, só que para bindShaderForTransparency().
		// Antes: até 14 glGetUniformLocation com std::to_string()+concatenação POR LUZ
		// (até 10 luzes = até 140 lookups ao driver), refeitos sempre que
		// `shader != lastShader || model != lastModel` na passagem transparente —
		// ou seja, potencialmente todo objeto transparente, todo frame.
		struct TransparencyUniformCache {
			GLuint cachedShader = 0;
			GLint view = -1, projection = -1, viewPos = -1, numLights = -1, reflectionsStrength = -1;
			GLint shadowMapDir = -1, cascadeCount = -1;
			static constexpr int MAX_CASCADES = 16;
			GLint cascadePlaneDistances[MAX_CASCADES];
			GLint cascadeLightSpaceMatrices[MAX_CASCADES];
			struct LightUniforms {
				GLint type = -1, pos = -1, dir = -1, color = -1, intensity = -1;
				GLint maxDist = -1, cutOff = -1, outerCutOff = -1;
				GLint constant = -1, linear = -1, quadratic = -1;
				GLint hasGlow = -1, glowColor = -1, glowRadius = -1;
				GLint shadowCubeMap = -1;     // POINT
				GLint shadowMap2D = -1;       // SPOT
				GLint lightSpaceMatrix = -1;  // SPOT
			} lights[10];
		};
		mutable TransparencyUniformCache transparencyUniformCache;

		// OPTIM: uniforms de luzes/ambiente/fog/IBL/shadowMap/alpha eram reenviados
		// (glUniform*) DENTRO de drawSubMesh — ou seja, uma vez por SUBMESH, por
		// INSTÂNCIA (até ~130 chamadas só pro array de luzes, por draw call). Nenhum
		// desses valores muda dentro de uma mesma chamada de Model::draw(): luzes,
		// ambiente, fog, IBL e o shadowMap da câmera são os mesmos pra TODAS as
		// instâncias/submeshes desenhados por essa chamada. Agora são enviados uma
		// única vez em Model::draw(), antes do loop de instâncias, usando as mesmas
		// locations de uniformCache (populadas por ensureUniformCache).
		void ensureUniformCache(GLuint shader);
		void uploadPerDrawUniforms(GLuint shader, GLuint depthMap);

		// OPTIM: cache de estado de GL escopado por chamada de Model::draw(), pra
		// evitar glUseProgram / glBindVertexArray / glActiveTexture+glBindTexture
		// redundantes quando o submesh seguinte (próxima instância, ou mesmo
		// mesh dentro da mesma instância) usa exatamente o mesmo shader/VAO/textura
		// do submesh anterior — muito comum em cenas com várias instâncias do MESMO
		// Model (grama, folhagem, veículos repetidos etc.), que sempre compartilham
		// material. É resetado no início de cada Model::draw() (ver lá), então nunca
		// fica "stale" em relação a outras passagens (shadow, postprocessing) que
		// rodam entre uma chamada de draw() e outra.
		mutable GLuint lastBoundVAO = 0xFFFFFFFFu;
		mutable GLuint lastUsedProgram = 0xFFFFFFFFu;
		mutable GLuint texUnitCache[10] = {
			0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,
			0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu,0xFFFFFFFFu
		};
		inline void bindTexIfChanged(int unit, GLuint tex) const {
			if (texUnitCache[unit] == tex) return; // já é o handle certo nesse slot: pula glActiveTexture+glBindTexture
			texUnitCache[unit] = tex;
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, tex);
		}
		inline void useProgramIfChanged(GLuint shader) const {
			if (lastUsedProgram == shader) return;
			lastUsedProgram = shader;
			glUseProgram(shader);
		}
		inline void bindVAOIfChanged(GLuint vao) const {
			if (lastBoundVAO == vao) return;
			lastBoundVAO = vao;
			glBindVertexArray(vao);
		}

		const std::vector<glm::mat4>& getBoneTransforms() const;
		Model(const std::string& path);
		void playAnim(const std::string& name, bool repeat, const std::string& next = "");
		void update(float deltaTime);
		GLuint getBaseColorTexture(const tinygltf::Model& model, int materialIndex);
		GLuint getDiffuseTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex);
		GLuint getGlossinessTextureFromSpecGloss(const tinygltf::Model& model, int materialIndex);
		GLuint getNormalMapTexture(const tinygltf::Model& model, int materialIndex);
		void init(const std::string& path);

		static void extractFrustumPlanes(const glm::mat4& viewProj, glm::vec4 planes[6]);

		// FIX: teste barato de esfera-vs-frustum, usado para cortar a instância
		// INTEIRA de uma vez (antes de occlusion query e do teste por submesh),
		// em vez de só existir o teste caro por AABB de cada submesh individual.
		static bool isSphereInFrustum(const glm::vec3& center, float radius, const glm::vec4 planes[6]);

		int selectLOD(float distanceSq) const;
		void drawSubMesh(
			const SubMesh& mesh,
			GLuint shader,
			const glm::mat4& modelMatrix,
			const glm::mat4& lightSpaceMatrix,
			GLuint depthMap,
			bool depthPass,
			Instance* inst,
			GLuint overrideVAO = 0,
			GLuint overrideEBO = 0,
			GLsizei overrideIndexCount = 0,
			GLenum overrideIndexType = GL_UNSIGNED_INT
		);

		// FEATURE: instanced counterpart of drawSubMesh, used ONLY for the main
		// camera color pass (never depthPass) of STATIC (non-skinned) models.
		// `instanceData` is interleaved per instance as 16 floats (model, column-
		// major) + 9 floats (normalMatrix, column-major) = 25 floats/instance.
		// isAffectedByLight/acceptsShadows are still plain uniforms (unchanged
		// fragment shader), so callers must only batch together instances that
		// share both flags — see the grouping in Model::draw().
		// NOTE: `instanceVBORef` must be the instance buffer handle that actually
		// belongs to `vao` — SubMesh::instanceVBO when `vao == mesh.vao`, or the
		// matching SubMesh::LODLevel::instanceVBO when `vao` is a LOD tier's VAO.
		// Each VAO owns its own vertex attribute bindings (locations 6..12), so
		// passing the wrong instanceVBO here (or reusing the same one across
		// different VAOs) leaves those locations unconfigured on every VAO except
		// the first one that ever triggered their setup — instances rendered
		// through any other VAO then read garbage/default attribute data.
		void drawSubMeshInstanced(
			const SubMesh& mesh,
			GLuint shader,
			GLuint vao,
			GLuint& instanceVBORef,
			GLenum indexType,
			GLsizei indexCount,
			const glm::mat4& lightSpaceMatrix,
			const std::vector<float>& instanceData,
			GLsizei instanceCount,
			bool isAffectedByLight,
			bool acceptsShadows
		);

		// FEATURE: (re)uploads `data` into the instance stream buffer owned by
		// `vao` (SubMesh::instanceVBO or SubMesh::LODLevel::instanceVBO), creating
		// the buffer and wiring up attribute locations 6..12 (divisor = 1) the
		// first time it's called for that VAO. `instanceVBO` is passed by
		// reference so the caller's stored handle gets updated on first use.
		static void uploadInstanceStream(GLuint vao, GLuint& instanceVBO, const std::vector<float>& data);

		// FEATURE: compact counterpart of drawSubMeshInstanced/uploadInstanceStream,
		// used ONLY when useCompactInstancing == true (grass). `instanceData` is
		// interleaved per instance as 5 floats: position.xyz, rotationY, scale —
		// 5x smaller than the 25-float mat4+mat3 format, and built directly from
		// Instance::position/rotation.y/scale.x with no matrix math on the CPU
		// side at all (the vertex shader rebuilds the matrix — see
		// vertexGrassInstanced). Only valid for instances with no pitch/roll, no
		// non-uniform scale, no physicsSyncTransformMatrix, and an identity
		// mesh.transform; callers must not mix these into a batch with instances
		// that violate any of those (Model::draw() decides this once per Model
		// via useCompactInstancing, so it never mixes formats within one draw).
		void drawSubMeshInstancedCompact(
			const SubMesh& mesh,
			GLuint shader,
			GLuint vao,
			GLuint& instanceVBORef,
			GLenum indexType,
			GLsizei indexCount,
			const glm::mat4& lightSpaceMatrix,
			const std::vector<float>& instanceData,
			GLsizei instanceCount,
			bool isAffectedByLight,
			bool acceptsShadows
		);

		// FEATURE: (re)uploads `data` (5 floats/instance: position.xyz, rotationY,
		// scale) into the instance stream buffer owned by `vao`, wiring up
		// attribute locations 6 (vec4: pos+rotY) and 7 (float: scale) the first
		// time it's called for that VAO — same orphaning/STREAM_DRAW behavior as
		// uploadInstanceStream, just a 5-float stride instead of 25.
		static void uploadInstanceStreamCompact(GLuint vao, GLuint& instanceVBO, const std::vector<float>& data);

		// FEATURE: instanced counterpart of the depthPass branch of drawSubMesh,
		// used ONLY for the 2D shadow passes (cascade + spot) of STATIC
		// (non-skinned) Models — see comment on Core::depthShaderStaticInstanced.
		// Reuses whichever instance buffer format the color pass already builds
		// for this Model (25-float generic via uploadInstanceStream, or 5-float
		// compact via uploadInstanceStreamCompact, chosen by `compact`) — pass
		// `shader` as Core::depthShaderStaticInstanced or
		// Core::depthShaderGrassInstanced to match. Unlike the color-pass
		// instanced draw, BLEND submeshes are included here too (shadows don't
		// need back-to-front sorting, just the stochastic dithered discard
		// already in depth_fragment), and there's no isAffectedByLight/
		// acceptsShadows split (those don't affect a depth-only write).
		void drawSubMeshInstancedDepth(
			const SubMesh& mesh,
			GLuint shader,
			GLuint vao,
			GLuint& instanceVBORef,
			GLenum indexType,
			GLsizei indexCount,
			const glm::mat4& lightSpaceMatrix,
			const std::vector<float>& instanceData,
			GLsizei instanceCount,
			bool compact
		);

		void unload();
		// FiscionCore.h — dentro de Model
		inline Instance* addInstance(FiscionX::Vector3 position, FiscionX::Vector3 rotation, FiscionX::Vector3 scale) {
			Instance inst;
			inst.position = position;
			inst.rotation = rotation;
			inst.scale = scale;
			inst.model = this;
			inst.nodes = nodes;
			inst.skins = skins;
			inst.physicsSyncTransformMatrix = glm::mat4(1.0f);
			inst.animTranslations = animTranslations;
			inst.animRotations = animRotations;
			inst.animScales = animScales;
			inst.nodeGlobalTransforms = nodeGlobalTransforms;
			inst.nodeParents = nodeParents;
			inst.finalBoneMatrices = finalBoneMatrices;
			inst.animations = animations;   // cada instância tem sua própria cópia
			inst.boneTransforms = boneTransforms;
			inst.occlusionQueries = occlusionQueries;
			inst.isVisible = isVisible;
			inst.cameraNodeIndex = cameraNodeIndex;   // FIX: propaga o nó de câmera para a instância

			// Cada instância precisa de seu próprio UBO de skinning independente.
			// Compartilhar o uboSkin do Model faz com que todas as instâncias
			// usem os ossos da última a atualizar, quebrando a animação individual.
			if (isSkinned) {
				glGenBuffers(1, &inst.uboSkin);
				glBindBuffer(GL_UNIFORM_BUFFER, inst.uboSkin);
				std::vector<glm::mat4> identityMats(100, glm::mat4(1.0f));
				glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 100, identityMats.data(), GL_DYNAMIC_DRAW);
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, inst.uboSkin);
			}
			else {
				inst.uboSkin = 0;
			}

			instances.push_back(std::move(inst));
			// FIX (bug crítico): antes retornava "&inst", o endereço da variável
			// LOCAL da função — um ponteiro pra pilha que já é lixo assim que
			// addInstance() retorna (push_back move o CONTEÚDO pro vetor, mas
			// não muda o que "&inst" aponta). Todo Instance* devolvido por esta
			// função (de qualquer prop: árvore, pedra, grama, etc.) nascia
			// dangling. "Funcionava" só por sorte de a memória da pilha ainda
			// não ter sido reescrita — até algo fazer aritmética de ponteiro
			// com ele (ex: removeInstance), quando o resultado vira lixo.
			return &instances.back();
		}

		// Remove "inst" do vetor com swap-and-pop (troca com o último elemento
		// e dá pop_back) em vez de erase() no meio: erase() desloca TODOS os
		// elementos depois do removido, invalidando os ponteiros que qualquer
		// outra instância guarde pra eles. Com swap-and-pop, no máximo UMA outra
		// instância muda de endereço — a que estava em back().
		//
		// Retorna o Instance* que foi deslocado pro slot que "inst" ocupava
		// (ou nullptr se "inst" já era o último, ou se não foi encontrado).
		// Quem guarda Instance* de longa duração pro MESMO Model (é o caso da
		// grama, com várias chunks compartilhando os mesmos 2 modelos) PRECISA
		// usar esse retorno pra atualizar o ponteiro que apontava pro antigo
		// back() — ver TerrainSystem::RemoveGrassForChunk.
		inline Instance* removeInstance(Instance* inst) {
			if (!inst) return nullptr;
			size_t idx = inst - instances.data();
			if (idx >= instances.size()) return nullptr;

			size_t lastIdx = instances.size() - 1;
			bool displaced = (idx != lastIdx);
			if (displaced) {
				instances[idx] = std::move(instances[lastIdx]);
			}
			instances.pop_back();

			return displaced ? &instances[idx] : nullptr;
		}

		void draw(GLuint shader, const glm::mat4& lightSpaceMatrix, GLuint depthMap, bool depthPass, FiscionX::Mat4 view, FiscionX::Mat4 projection,
			bool skipOcclusionAndCulling = false);
		void bindShaderForTransparency(GLuint shader, FiscionX::Mat4 view, FiscionX::Mat4 projection);
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
		// FEATURE: instanced counterparts of depthShaderStatic, used ONLY for the
		// 2D shadow passes (cascade + spot — NOT the point-light cube pass) of
		// STATIC (non-skinned) Models, same 25-float/5-float per-instance buffers
		// as the color pass (shaderStaticInstanced/shaderGrassInstanced). See
		// Model::drawSubMeshInstancedDepth.
		static GLuint depthShaderStaticInstanced;
		static GLuint depthShaderGrassInstanced;
		static GLuint shaderStatic;
		static GLuint shaderSkinned;
		// FEATURE: linked against the SAME `fragment` shader as shaderStatic/shaderSkinned
		// (fragment.glsl is untouched) — only the vertex stage differs, reading
		// model/normalMatrix from per-instance attributes instead of uniforms. Used
		// exclusively by Model::draw() for the main camera color pass of static
		// (non-skinned) models; see Model::drawSubMeshInstanced.
		static GLuint shaderStaticInstanced;
		// FEATURE: compact counterpart of shaderStaticInstanced, used only for
		// Model instances with useCompactInstancing == true (grass). Same
		// fragment shader; the vertex stage rebuilds model/normalMatrix from
		// 5 floats/instance instead of reading a pre-built mat4+mat3 — see
		// vertexGrassInstanced and Model::drawSubMeshInstancedCompact.
		static GLuint shaderGrassInstanced;
		static GLuint shaderUI;
		//static GLuint shaderGeometry;

		// --- POST PROCESSING ---
		static GLuint mainFBO;
		static GLuint mainColorBuffer;
		static GLuint mainNormalRoughBuffer; // RGBA16F: rgb = view-space normal, a = roughness (opaque pass only)
		static GLuint mainMetallicBuffer;    // R16F: metallic (opaque pass only) — usado pelo SSR composite p/ F0 físico
		static GLuint mainDepthBuffer;
		static GLuint screenQuadVAO, screenQuadVBO;
		static GLuint godRaysShader;

		static GLuint ssaoFBO;
		static GLuint ssaoColorBuffer;
		static GLuint ssaoBlurFBO;
		static GLuint ssaoBlurColorBuffer;
		static GLuint ssaoShader;
		static GLuint ssaoBlurShader;
		static GLuint ssaoNoiseTex;
		static std::vector<glm::vec3> ssaoKernel;

		// --- SSAO config ---
		static bool  SSAO_ENABLED;      // liga/desliga o passe inteiro (incl. blur); quando false, ssaoTexture fica neutro (sem AO/GI)
		static float SSAO_RADIUS;       // raio de amostragem em unidades de view-space
		static float SSAO_BIAS;         // bias contra acne/self-occlusion
		static float SSAO_INTENSITY;    // multiplicador de força da oclusão
		static float SSAO_GI_STRENGTH;  // contribuição de GI aproximado embutido no mesmo passe

		// --- SSR (Screen Space Reflections) ---
		static GLuint ssrFBO;
		static GLuint ssrColorBuffer;       // rgb = reflection color, a = hit confidence/mask
		static GLuint ssrBlurFBO;
		static GLuint ssrBlurColorBuffer;   // ssrColorBuffer borrado por um kernel cujo raio escala com a roughness do pixel
		static GLuint ssrCompositeFBO;
		static GLuint ssrCompositeColorBuffer; // composited result (scene + SSR); blitted back into mainColorBuffer
		static GLuint ssrShader;
		static GLuint ssrBlurShader;        // borra ssrColorBuffer (raio ~ roughness) antes do composite — aproxima reflexo "glossy"
		static GLuint ssrCompositeShader;   // adds ssrBlurColorBuffer on top of mainColorBuffer (writes into ssrCompositeColorBuffer)
		static bool   SSR_ENABLED;
		static float  SSR_MAX_DISTANCE;     // view-space units the ray is allowed to travel
		static float  SSR_THICKNESS;        // depth thickness tolerance multiplier (scaled by scene distance per-sample, not an absolute value)
		static int    SSR_MAX_STEPS;        // linear march steps before binary refinement
		static int    SSR_BINARY_STEPS;     // binary-search refinement steps
		static float  SSR_FADE_SCREEN_EDGE; // 0..1, how much of the screen border fades the reflection out
		static float  SSR_STRIDE;           // march step multiplier (scaled by view distance from camera, not an absolute value)
		static float  SSR_MAX_BLUR_RADIUS;  // raio do blur (em pixels de tela) atingido em roughness = 1.0; 0 desliga o blur

		// --- IBL ---
		static GLuint iblIrradianceMap;    // samplerCube — diffuse irradiance
		static GLuint iblPrefilterMap;     // samplerCube — specular prefiltered (mip chain)
		static GLuint iblBrdfLUT;          // sampler2D   — BRDF integration LUT
		static bool   iblReady;            // true after successful pre-computation
		// -----------

		//---- FOG ----
		static Vector3 fogColor;
		static float fogDensity;
		static float fogStart;
		static float fogEnd;
		static int fogType;

		static float sunDiskSize;
		static float sunHaloSize;
		static Vector3 sunColor;

		static float godRaysDensity;
		static float godRaysWeight;
		static float godRaysDecay;
		static float godRaysExposure;
		static int godRaysNumOfSamples;

		static Vector3 colorCorrection;

		//static unsigned int SHADOW_WIDTH;
		//static unsigned int SHADOW_HEIGHT;
		//static unsigned int SHADOW_CUBE_SIZE;

		static int DIR_SHADOW_SIZE;
		static int SPOT_SHADOW_SIZE;
		static int POINT_SHADOW_SIZE;

		static float        NEAR_PLANE;
		static float         FAR_PLANE;
		static float         AMBIENT_LIGHT_INTENSITY;
		static FiscionX::Vector3 AMBIENT_LIGHT_SKYCOLOR;
		static FiscionX::Vector3 AMBIENT_LIGHT_GROUNDCOLOR;
		static float SHADOW_VIEW_RADIUS;
		static GLuint depthMapFBO;
		static GLuint depthMap;
		static std::vector<float> shadowCascadeLevels;
		static float REFLECTIONS_STRENGTH;
		static float HDR_EXPOSURE;

		static float IBL_HDR_SCALE;

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

		// DEBUG: contadores pra verificar se o frustum culling por instância está
		// realmente cortando alguma coisa. Zerados/lidos uma vez por segundo junto
		// com o FPS (ver ClockTick). Só contam a passagem da câmera principal
		// (depthPass == false) — shadow passes não usam esse culling de propósito.
		static int DEBUG_InstancesTotal;
		static int DEBUG_InstancesCulled;

		static bool enableShaderCache;
		static bool enableModelCache;
		static bool compressTexturesAutomatically;

		static void CreateShadowMap(ShadowMap& sm, int LIGHT_TYPE);
		static void CreateAllShadowMaps();
		static std::vector<glm::vec4> getFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);
		static glm::mat4 getLightSpaceMatrix(FiscionX::Light& L, const float nearPlane, const float farPlane);
		static glm::mat4 ComputeLightSpaceMatrix(Light& L);
		static void RenderAllShadowPasses(FiscionX::Mat4 view, FiscionX::Mat4 projection, FiscionX::Mat4 viewProj);

		static void SetCursorMode(int mode);
		static void SetCacheSettings(bool _enableShaderCache, bool _enableModelCache);

		static void NewWindow(int width, int height, const char* window_label);
		static void Set3DSettings(const int _DIRECTIONAL_LIGHT_SHADOW_SIZE, const int _SPOT_LIGHT_SHADOW_SIZE,
			const int _POINT_LIGHT_SHADOW_SIZE, const std::vector<float> SHADOW_CASCADE_LEVELS, const float _NEAR_PLANE,
			const float _FAR_PLANE, const bool _COMPRESS_TEXTURES_AUTOMATICALLY);
		static void ClockTick();
		static bool LoadHDR(const char* path);
		static void SetWindowSize(int width, int height);
		static void SetWindowIcon(const char* path);
		static void SetWindowFullscreen(bool fullscreen, int monitorIndex);
		static void SortModels();
		static void DrawTransparentPass(FiscionX::Mat4 view, FiscionX::Mat4 projection);
		static void Terminate();

		static struct Draw {
			static void HDR(FiscionX::Mat4 view, FiscionX::Mat4 projection);
			static void SwapBuffers();
			static void PostProcessing(FiscionX::Mat4 viewProj, FiscionX::Light* dirLight);
			static void ClearBackground(float r, float g, float b, float a);
			static void DrawLine(Vector2 startPosition, Vector2 endPosition, float thickness, Vector4 color);
			static void DrawRect(Vector2 position, Vector2 size, Vector4 color, float borderThickness = 0, float borderRadius = 0, int segments = 16);
			static void DrawCircle(Vector2 position, float radius, Vector4 color, float borderThickness = 0, int segments = 64);
			static void DrawArc(Vector2 position, Vector4 color, float radius, float start_angle, float end_angle, float thickness = 0, int segments = 64);
			static void DrawPolygon(const std::vector<Vector2> vertices, Vector4 color, float borderThickness = 0);
			static void DrawEllipse(Vector2 position, Vector2 size, Vector4 color, float borderThickness = 0, int segments = 64);
		};
	};
};

#endif