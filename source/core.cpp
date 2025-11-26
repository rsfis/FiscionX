#include "FiscionCore.h"
#include "FiscionShaders.h"
#define ENGINE_VERSION "1.0.0"

// last error number: 17

// GLOBALS
GLFWwindow * FiscionX::Core::Window;
int FiscionX::Core::SCREEN_WIDTH, FiscionX::Core::SCREEN_HEIGHT;

GLuint FiscionX::Core::depthShaderStatic;
GLuint FiscionX::Core::depthShaderSkinned;
GLuint FiscionX::Core::depthShaderCubeStatic;
GLuint FiscionX::Core::depthShaderCubeSkinned;
GLuint FiscionX::Core::shaderStatic;
GLuint FiscionX::Core::shaderSkinned;
GLuint FiscionX::Core::shaderUI;

int FiscionX::Core::DIR_SHADOW_SIZE = 4096;
int FiscionX::Core::SPOT_SHADOW_SIZE = 1024;
int FiscionX::Core::POINT_SHADOW_SIZE = 512;
float        FiscionX::Core::NEAR_PLANE;
float         FiscionX::Core::FAR_PLANE;
float         FiscionX::Core::SHADOW_VIEW_RADIUS;
float         FiscionX::Core::AMBIENT_LIGHT_INTENSITY = 1.0f;
glm::vec3     FiscionX::Core::AMBIENT_LIGHT_SKYCOLOR = { 0.3f, 0.3f, 0.35f };
glm::vec3     FiscionX::Core::AMBIENT_LIGHT_GROUNDCOLOR = { 0.05f, 0.05f, 0.07f };
GLuint FiscionX::Core::depthMapFBO;
GLuint FiscionX::Core::depthMap;

GLuint FiscionX::Core::textShader;

FiscionX::Camera FiscionX::Core::Camera;

btBroadphaseInterface* FiscionX::Physics::broadphase;
btDefaultCollisionConfiguration* FiscionX::Physics::collisionConfig;
btCollisionDispatcher* FiscionX::Physics::dispatcher;
btSequentialImpulseConstraintSolver* FiscionX::Physics::solver;
btDiscreteDynamicsWorld* FiscionX::Physics::DynamicWorld;
float FiscionX::Physics::maxCollisionDistance = 50.0f;
GLuint FiscionX::Physics::debugVAO = 0, FiscionX::Physics::debugVBO = 0;
GLuint FiscionX::Physics::debugShader = 0;
std::vector<float> FiscionX::Physics::debugLines;
FiscionX::Physics::GLDebugDrawer* FiscionX::Physics::debugDrawer;

FiscionX::Vector2 FiscionX::Input::mousePosition;
FiscionX::Vector2 FiscionX::Input::mouseDelta;
FiscionX::Vector2 FiscionX::Input::scrollOffset;

FiscionX::AudioSystem FiscionX::Core::AudioSystem;

std::vector<FiscionX::Sound> FiscionX::Core::AllSounds;
std::vector<FiscionX::Model*> FiscionX::Core::AllModels;
std::vector<FiscionX::Light*> FiscionX::Core::AllLights;
std::vector<FiscionX::ShadowMap> FiscionX::Core::AllShadowMaps;

float FiscionX::Core::lastFrame;
float FiscionX::Core::deltaTime;
int FiscionX::Core::FPS;
float FiscionX::Core::lastFPSTime;

bool FiscionX::Core::enableShaderCache = true;
bool FiscionX::Core::enableModelCache = true;

float lastX = 640, lastY = 360;
bool firstMouse = true;
float deltaTime = 0.0f, lastFrame = 0;

// =================== Shader Loader ===================
GLuint LoadShader(const char* vertexSrc, const char* fragmentSrc);

// ====================== Math ========================
float FiscionX::Math::getDistance3D(FiscionX::Vector3 pos1, FiscionX::Vector3 pos2) {
	return std::sqrt(std::pow(pos2.x - pos1.x, 2) + std::pow(pos2.y - pos1.y, 2) + std::pow(pos2.z - pos1.z, 2));
}

float FiscionX::Math::clamp(float value, float min, float max) {
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

float FiscionX::Math::radians(float degrees) {
	return degrees * (3.14159265359f / 180.0f);
}

float FiscionX::Math::degrees(float radians) {
	return radians * (180.0f / 3.14159265359f);
}

float FiscionX::Math::lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

float FiscionX::Math::min(float a, float b) {
	return (a < b) ? a : b;
}

float FiscionX::Math::max(float a, float b) {
	return (a > b) ? a : b;
}

float FiscionX::Math::sin(float angle) {
	return std::sin(angle);
}

float FiscionX::Math::cos(float angle) {
	return std::cos(angle);
}

float FiscionX::Math::tan(float angle) {
	return std::tan(angle);
}

float FiscionX::Math::asin(float value) {
	return std::asin(value);
}

float FiscionX::Math::acos(float value) {
	return std::acos(value);
}

float FiscionX::Math::atan(float value) {
	return std::atan(value);
}

int FiscionX::Math::sign(float value) {
	return (value > 0) - (value < 0);
}

int FiscionX::Math::randInt(int min, int max) {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(mt);
}

float FiscionX::Math::randFloat(float min, float max) {
	static std::random_device rd;
	static std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}

float FiscionX::Math::atan2(float y, float x) {
	return std::atan2(y, x);
}

float FiscionX::Math::sqrt(float value) {
	return std::sqrt(value);
}

float FiscionX::Math::pow(float base, float exponent) {
	return std::pow(base, exponent);
}

int FiscionX::Math::abs(int value) {
	return std::abs(value);
}

float FiscionX::Math::abs(float value) {
	return std::fabs(value);
}

float FiscionX::Math::floor(float value) {
	return std::floor(value);
}

float FiscionX::Math::ceil(float value) {
	return std::ceil(value);
}

float FiscionX::Math::round(float value) {
	return std::round(value);
}

float FiscionX::Math::log(float value) {
	return std::log(value);
}

float FiscionX::Math::angleBetween(Vector3 a, Vector3 b) {
	a = a.normalized();
	b = b.normalized();
	return std::acos(Math::clamp(a.dot(b), -1.0f, 1.0f));
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

// =================== Audio ====================
FMOD_RESULT SYS;
FMOD::System* FMOD_SYS;

void FiscionX::AudioSystem::init() {
	SYS = FMOD::System_Create(&FMOD_SYS);
	if (SYS != FMOD_OK) {
		std::cerr << "ERR 0x011 - FMOD Audio System couldn't be created" << std::endl;
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

	FMOD_SYS->set3DListenerAttributes(0, &listenerPos, &velocity, &forward, &up);
}

FiscionX::Sound::Sound(const char* path, bool loop, bool threedimensional, FiscionX::Vector3 position,
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

// =================== UI ====================
// ================= IMAGES ==================

FiscionX::UI::Image::Image(const char* path, FiscionX::Vector2 scl) {
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

	scale = scl;

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

void FiscionX::UI::Image::draw(FiscionX::Vector2 position) {
	glm::mat4 projection = glm::ortho(
		0.0f, (float)FiscionX::Core::SCREEN_WIDTH,
		0.0f, (float)FiscionX::Core::SCREEN_HEIGHT
	);

	glDisable(GL_DEPTH_TEST);
	glUseProgram(shader);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader, "tex"), 0);

	glUniform1i(glGetUniformLocation(shader, "posMode"), 1);
	glUniform2f(glGetUniformLocation(shader, "position"), position.x, position.y);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform2f(glGetUniformLocation(shader, "scale"), scale.x, scale.y);
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

// ================== Text ====================
FiscionX::UI::Font::Font(const char* fontPath, int pixelSize) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "ERR 0x014 - Could not init FreeType" << std::endl;
		std::exit(-20);
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath, 0, &face)) {
		std::cerr << "ERR 0x015 - Failed to load font: " << fontPath << std::endl;
		std::exit(-21);
	}

	FT_Set_Pixel_Sizes(face, 0, pixelSize);

	atlasWidth = 0;
	atlasHeight = 0;
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
		atlasWidth += face->glyph->bitmap.width;
		atlasHeight = std::max(atlasHeight, (int)face->glyph->bitmap.rows);
	}

	glGenTextures(1, &textureAtlas);
	glBindTexture(GL_TEXTURE_2D, textureAtlas);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	int xOffset = 0;
	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;

		FT_Bitmap& bmp = face->glyph->bitmap;

		std::vector<unsigned char> pixels(bmp.width * bmp.rows);
		for (int y = 0; y < bmp.rows; y++) {
			memcpy(&pixels[y * bmp.width],
				&bmp.buffer[y * bmp.pitch],
				bmp.width);
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0,
			xOffset, 0,
			bmp.width, bmp.rows,
			GL_RED, GL_UNSIGNED_BYTE,
			pixels.data());

		Glyph glyph;
		glyph.uv0 = glm::vec2((float)xOffset / atlasWidth, 0.0f);
		glyph.uv1 = glm::vec2((float)(xOffset + bmp.width) / atlasWidth,
			(float)bmp.rows / atlasHeight);
		glyph.sizePx = glm::ivec2(bmp.width, bmp.rows);
		glyph.bearingPx = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		glyph.advancePx = (face->glyph->advance.x >> 6);

		Characters.insert(std::pair<char, Glyph>(c, glyph));

		xOffset += bmp.width;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &textVAO);
	glGenBuffers(1, &textVBO);
	glBindVertexArray(textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

FiscionX::UI::Font::~Font() {
	glDeleteTextures(1, &textureAtlas);
}

void FiscionX::UI::DrawText(Font* font, const char* text, FiscionX::Vector2 position, float scale, FiscionX::Vector4 color, float rotation) {
	glUseProgram(FiscionX::Core::textShader);

	glm::mat4 projection = glm::ortho(0.0f, (float)FiscionX::Core::SCREEN_WIDTH,
		0.0f, (float)FiscionX::Core::SCREEN_HEIGHT);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(FiscionX::Core::textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(FiscionX::Core::textShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform4f(glGetUniformLocation(FiscionX::Core::textShader, "color"), color.x, color.y, color.z, color.w);
	glUniform1f(glGetUniformLocation(FiscionX::Core::textShader, "rotation"), rotation);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(font->textVAO);
	glBindTexture(GL_TEXTURE_2D, font->textureAtlas);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	float x = 0.0f;
	for (const char* p = text; *p; p++) {
		Glyph ch = font->Characters[*p];

		float xpos = x + ch.bearingPx.x * scale;
		float ypos = -(ch.sizePx.y - ch.bearingPx.y) * scale;
		float w = ch.sizePx.x * scale;
		float h = ch.sizePx.y * scale;

		float u0 = ch.uv0.x, v0 = ch.uv0.y;
		float u1 = ch.uv1.x, v1 = ch.uv1.y;

		float vertices[6][4] = {
			{ xpos,     ypos + h, u0, v0 },
			{ xpos,     ypos,     u0, v1 },
			{ xpos + w, ypos,     u1, v1 },

			{ xpos,     ypos + h, u0, v0 },
			{ xpos + w, ypos,     u1, v1 },
			{ xpos + w, ypos + h, u1, v0 }
		};

		glBindBuffer(GL_ARRAY_BUFFER, font->textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.advancePx) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// =================== Video ====================
GLuint FiscionX::UI::Video::shaderVideo = 0;
static const float _videoQuadTemplate[] = {
	-0.5f, -0.5f, 0.0f, 1.0f,
	 0.5f, -0.5f, 1.0f, 1.0f,
	 0.5f,  0.5f, 1.0f, 0.0f,
	-0.5f,  0.5f, 0.0f, 0.0f
};
static const GLuint _videoIdx[] = { 0,1,2, 2,3,0 };

FiscionX::UI::Video::Video(const char* path, int desiredWidth, int desiredHeight) {
	width = desiredWidth > 0 ? desiredWidth : 640;
	height = desiredHeight > 0 ? desiredHeight : 360;
	aspect_ratio = (height != 0) ? ((float)width / (float)height) : 1.0f;

	pixels.resize((size_t)width * height * 4);

	const char* const vlc_args[] = {
		"--no-xlib",
		"--vout=vmem",
		"--no-video-title-show",
		"--quiet"
	};
	vlcInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	if (!vlcInstance) { std::cerr << "ERR 0x016 - Couldn't initialize VLC video instance\n"; return; }

	media = libvlc_media_new_path(vlcInstance, path);
	if (!media) {
		std::cerr << "ERR 0x017 - Couldn't open video instance at " << path << "\n";
		libvlc_release(vlcInstance); vlcInstance = nullptr;
		return;
	}

	mediaPlayer = libvlc_media_player_new_from_media(media);
	libvlc_media_release(media); media = nullptr;

	// registra callbacks
	libvlc_video_set_callbacks(mediaPlayer,
		&Video::lockCallback,
		&Video::unlockCallback,
		&Video::displayCallback,
		this
	);

#ifdef VIDEO_RGBA_FIX
	libvlc_video_set_format(mediaPlayer, "RGBA", width, height, width * 4);
#else
	libvlc_video_set_format(mediaPlayer, "RV32", width, height, width * 4);
#endif

	libvlc_audio_set_mute(mediaPlayer, 0);
	libvlc_audio_set_volume(mediaPlayer, 100);

	// cria quad OpenGL
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_videoQuadTemplate), _videoQuadTemplate, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_videoIdx), _videoIdx, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void* FiscionX::UI::Video::lockCallback(void* opaque, void** planes) {
	Video* self = reinterpret_cast<Video*>(opaque);
	self->pixelMutex.lock();
	*planes = self->pixels.data();
	return self->pixels.data();
}

void FiscionX::UI::Video::unlockCallback(void* opaque, void* picture, void* const* planes) {
	Video* self = reinterpret_cast<Video*>(opaque);
	self->hasNewFrame = true;
	self->pixelMutex.unlock();
}

void FiscionX::UI::Video::displayCallback(void* opaque, void* picture) {
	Video* self = reinterpret_cast<Video*>(opaque);
	self->hasNewFrame = true;
}

void FiscionX::UI::Video::createTextureIfNeeded() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void FiscionX::UI::Video::update() {
	if (!pixelMutex.try_lock()) return;
	if (!hasNewFrame) { pixelMutex.unlock(); return; }

	createTextureIfNeeded();
	if (!texture) return;
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef VIDEO_RGBA_FIX
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
#else
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
		GL_BGRA, GL_UNSIGNED_BYTE, pixels.data());
#endif

	hasNewFrame = false;
	pixelMutex.unlock();
}

void FiscionX::UI::Video::play() {
	if (mediaPlayer) {
		libvlc_media_player_play(mediaPlayer);
	}
}

void FiscionX::UI::Video::pause() {
	if (mediaPlayer) {
		libvlc_media_player_set_pause(mediaPlayer, 1);
	}
}

void FiscionX::UI::Video::stop() {
	if (mediaPlayer) {
		libvlc_media_player_stop(mediaPlayer);
	}
}

FiscionX::UI::Video::~Video() {
	stop();

	if (mediaPlayer) {
		libvlc_media_player_release(mediaPlayer);
		mediaPlayer = nullptr;
	}
	if (media) {
		libvlc_media_release(media);
		media = nullptr;
	}
	if (vlcInstance) {
		libvlc_release(vlcInstance);
		vlcInstance = nullptr;
	}

	if (texture) {
		glDeleteTextures(1, &texture);
		texture = 0;
	}
	if (EBO) {
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}
	if (VBO) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}
	if (VAO) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}

void FiscionX::UI::Video::draw(FiscionX::Vector2 position) {
	if (!texture) return;
	GLuint shaderProgram = shaderVideo;
	if (!shaderProgram) return;

	glDisable(GL_DEPTH_TEST);
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint locTex = glGetUniformLocation(shaderProgram, "videoTex");
	if (locTex >= 0) glUniform1i(locTex, 0);

	float halfW = (float)FiscionX::Core::SCREEN_WIDTH * 0.5f;
	float halfH = (float)FiscionX::Core::SCREEN_HEIGHT * 0.5f;
	glm::mat4 projection = glm::ortho(-halfW, halfW, -halfH, halfH);
	GLint locProj = glGetUniformLocation(shaderProgram, "projection");
	if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

	float sizeX = (float)width * scale.x * 2;
	float sizeY = (float)height * scale.y * 2;
	GLint locScale = glGetUniformLocation(shaderProgram, "scale");
	if (locScale >= 0) glUniform2f(locScale, sizeX, sizeY);

	GLint locPos = glGetUniformLocation(shaderProgram, "position");
	if (locPos >= 0) glUniform2f(locPos, position.x, position.y);

	GLint locRot = glGetUniformLocation(shaderProgram, "rotation");
	if (locRot >= 0) glUniform1f(locRot, rotation);

	GLint locA = glGetUniformLocation(shaderProgram, "alpha");
	if (locA >= 0) glUniform1f(locA, alpha);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

// BUTTONS
FiscionX::UI::Button::Button(FiscionX::Vector2 _position, FiscionX::Vector2 _size, FiscionX::UI::Image* _image, FiscionX::UI::Font* _font,
	std::string _text, FiscionX::Vector4 _textColor, bool _textCentered, FiscionX::Vector2 _textOffset, FiscionX::Vector4 _normalColor, FiscionX::Vector4 _hoverColor, FiscionX::Vector4 _pressColor,
	float cooldownBetweenPresses, void (*_PressCallback)()) {

	position = _position;
	size = _size;
	image = _image;
	font = _font;
	text = _text;
	textColor = _textColor;
	textCentered = _textCentered;
	textOffset = _textOffset;
	normalColor = _normalColor;
	hoverColor = _hoverColor;
	pressColor = _pressColor;
	cooldownToNextPress = cooldownBetweenPresses;
	PressCallback = _PressCallback;
}

void FiscionX::UI::Button::update(float deltaTime) {
	FiscionX::Vector2 mousePosition = FiscionX::Input::GetMousePosition();

	if (mousePosition.x > position.x && mousePosition.x < position.x + size.x && mousePosition.y < position.y + size.y && mousePosition.y > size.y) {
		isHovering = true;
		if (canBePressed == true && FiscionX::Input::GetMouseButtonPressed(FISCIONX_MOUSE_BUTTON_1)) { isPressed = true; canBePressed = false; }
	}
	else {
		isHovering = false;
	}

	if (!canBePressed) {
		if (timerToNextPress < cooldownToNextPress) {
			timerToNextPress += 1 * deltaTime;
		}
		if (timerToNextPress >= cooldownToNextPress) {
			timerToNextPress = 0;
			canBePressed = true;
		}
	}

	if (PressCallback != nullptr) {
		if (isPressed) {
			PressCallback();
			isPressed = false;
		}
	}
}

void FiscionX::UI::Button::draw() {
	FiscionX::Vector4 color;

	if (!isHovering)
		color = normalColor;
	else if (!canBePressed)
		color = pressColor;
	else
		color = hoverColor;

	if (image == nullptr) {
		FiscionX::Core::Draw::DrawRect(position, size, color);
	}
	else {
		image->draw(position);
	}
	if (font != nullptr && !text.empty()) {
		FiscionX::Vector2 textPos;

		FiscionX::Vector2 textSize;

		for (int i = 0; i < text.length(); i++) {
			textSize += font->Characters[text[i]].sizePx.x;
		}
		textSize.y = font->Characters['A'].sizePx.y;

		if (textCentered) {
			textPos.x = position.x + (size.x - textSize.x) / 2;
			textPos.y = position.y + (size.y - textSize.y) * 0.5f - (FiscionX::Core::SCREEN_HEIGHT * 0.0069);
		}
		else {
			textPos.x = position.x + 8.0f;
			textPos.y = position.y + (size.y - textSize.y) * 0.5f;
		}

		textPos += textOffset;
		FiscionX::UI::DrawText(font, text.c_str(), textPos, 1.0f, textColor, 0);
	}
}

// =================== Camera ===================
FiscionX::Camera::Camera() {
	updateVectors();
}

FiscionX::Mat4 FiscionX::Camera::GetView() {
	return FiscionX::Mat4(0).lookAt(FiscionX::Vector3(position.x, position.y, position.z), FiscionX::Vector3(position.x, position.y, position.z) + FiscionX::Vector3(front.x, front.y, front.z), FiscionX::Vector3(up.x, up.y, up.z));
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

		glm::vec3 frontVec = glm::normalize(dir);

		glm::vec3 rightVec = glm::normalize(glm::cross(frontVec, glm::vec3(0.0f, 1.0f, 0.0f)));

		glm::vec3 upVec = glm::normalize(glm::cross(rightVec, frontVec));

		front = FiscionX::Vector3(frontVec.x, frontVec.y, frontVec.z);
		right = FiscionX::Vector3(rightVec.x, rightVec.y, rightVec.z);
		up = FiscionX::Vector3(upVec.x, upVec.y, upVec.z);
	}
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

	float mouseX, mouseY;
	float cartesianMouseX = (float)xpos;
	float cartesianMouseY = (float)(FiscionX::Core::SCREEN_HEIGHT - ypos);
	FiscionX::Input::mousePosition = FiscionX::Vector2(cartesianMouseX, cartesianMouseY);
	FiscionX::Input::mouseDelta = FiscionX::Vector2(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	FiscionX::Input::scrollOffset = FiscionX::Vector2(xoffset, yoffset);
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
		// Orthogonalizes using Gram-Schmidt function
		v.tangent = glm::normalize(v.tangent - v.normal * glm::dot(v.normal, v.tangent));
		// Determines handedness (bitangent cross)
		float handedness = (glm::dot(glm::cross(v.normal, v.tangent), v.bitangent) < 0.0f) ? -1.0f : 1.0f;
		v.tangent = glm::vec4(v.tangent, handedness);
	}
}

const std::vector<glm::mat4>& FiscionX::Model::getBoneTransforms() const {
	return boneTransforms;
}

FiscionX::Model::Model(const std::string& path, FiscionX::Vector3 pos, FiscionX::Vector3 rot, FiscionX::Vector3 scl)
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
		for (int c : nodes[i].children) nodeParents[c] = i;
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
				local = glm::translate(glm::mat4(1.0f), T) * glm::mat4_cast(R) * glm::scale(glm::mat4(1.0f), S);
			}

			glm::mat4 globalTransform = parentTransform * local;

			if (node.mesh >= 0) {
				const tinygltf::Mesh& mesh = gltfModel.meshes[node.mesh];
				for (size_t p = 0; p < mesh.primitives.size(); ++p) {
					const tinygltf::Primitive& prim = mesh.primitives[p];
					SubMesh sub;
					sub.transform = globalTransform;

					bool hasPosition = prim.attributes.find("POSITION") != prim.attributes.end();
					if (!hasPosition) continue;

					bool hasNormal = prim.attributes.find("NORMAL") != prim.attributes.end();
					bool hasTangent = prim.attributes.find("TANGENT") != prim.attributes.end();
					bool hasTexCoord = prim.attributes.find("TEXCOORD_0") != prim.attributes.end();

					// ==== POSITION (must be FLOAT, but treated here generically) ====
					const tinygltf::Accessor& posAcc = gltfModel.accessors.at(prim.attributes.at("POSITION"));
					const tinygltf::BufferView& posBV = gltfModel.bufferViews.at(posAcc.bufferView);
					const tinygltf::Buffer& posBuf = gltfModel.buffers.at(posBV.buffer);
					int posComps = 3; // VEC3
					size_t posCompSize = (posAcc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) ? sizeof(float) :
						(posAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) :
							(posAcc.componentType == TINYGLTF_COMPONENT_TYPE_SHORT ? sizeof(int16_t) :
								(posAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? sizeof(uint8_t) :
									(posAcc.componentType == TINYGLTF_COMPONENT_TYPE_BYTE ? sizeof(int8_t) : sizeof(float)))));
					size_t posStride = posAcc.ByteStride(posBV);
					if (posStride == 0) posStride = posComps * posCompSize;
					const uint8_t* posBase = posBuf.data.data() + posBV.byteOffset + posAcc.byteOffset;

					// ==== NORMAL ====
					const tinygltf::Accessor* normAcc = nullptr;
					const tinygltf::BufferView* normBV = nullptr;
					const tinygltf::Buffer* normBuf = nullptr;
					const uint8_t* normBase = nullptr;
					size_t normStride = 0, normCompSize = 0;
					bool normNorm = false; int normComps = 3;
					int normCT = 0;
					if (hasNormal) {
						normAcc = &gltfModel.accessors.at(prim.attributes.at("NORMAL"));
						normBV = &gltfModel.bufferViews.at(normAcc->bufferView);
						normBuf = &gltfModel.buffers.at(normBV->buffer);
						normCT = normAcc->componentType;
						normNorm = normAcc->normalized;
						normCompSize =
							(normCT == TINYGLTF_COMPONENT_TYPE_FLOAT ? sizeof(float) :
								(normCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) :
									(normCT == TINYGLTF_COMPONENT_TYPE_SHORT ? sizeof(int16_t) :
										(normCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? sizeof(uint8_t) :
											(normCT == TINYGLTF_COMPONENT_TYPE_BYTE ? sizeof(int8_t) : sizeof(float))))));
						normStride = normAcc->ByteStride(*normBV);
						if (normStride == 0) normStride = normComps * normCompSize;
						normBase = normBuf->data.data() + normBV->byteOffset + normAcc->byteOffset;
					}

					// ==== TANGENT ====
					const tinygltf::Accessor* tanAcc = nullptr;
					const tinygltf::BufferView* tanBV = nullptr;
					const tinygltf::Buffer* tanBuf = nullptr;
					const uint8_t* tanBase = nullptr;
					size_t tanStride = 0, tanCompSize = 0;
					bool tanNorm = false; int tanComps = 4; int tanCT = 0;
					if (hasTangent) {
						tanAcc = &gltfModel.accessors.at(prim.attributes.at("TANGENT"));
						tanBV = &gltfModel.bufferViews.at(tanAcc->bufferView);
						tanBuf = &gltfModel.buffers.at(tanBV->buffer);
						tanCT = tanAcc->componentType;
						tanNorm = tanAcc->normalized;
						tanCompSize =
							(tanCT == TINYGLTF_COMPONENT_TYPE_FLOAT ? sizeof(float) :
								(tanCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) :
									(tanCT == TINYGLTF_COMPONENT_TYPE_SHORT ? sizeof(int16_t) :
										(tanCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? sizeof(uint8_t) :
											(tanCT == TINYGLTF_COMPONENT_TYPE_BYTE ? sizeof(int8_t) : sizeof(float))))));
						tanStride = tanAcc->ByteStride(*tanBV);
						if (tanStride == 0) tanStride = tanComps * tanCompSize;
						tanBase = tanBuf->data.data() + tanBV->byteOffset + tanAcc->byteOffset;
					}

					// ==== TEXCOORD_0 (can be UBYTE/USHORT normalized) ====
					const tinygltf::Accessor* uvAcc = nullptr;
					const tinygltf::BufferView* uvBV = nullptr;
					const tinygltf::Buffer* uvBuf = nullptr;
					const uint8_t* uvBase = nullptr;
					size_t uvStride = 0, uvCompSize = 0;
					bool uvNorm = false; int uvComps = 2; int uvCT = 0;
					if (hasTexCoord) {
						uvAcc = &gltfModel.accessors.at(prim.attributes.at("TEXCOORD_0"));
						uvBV = &gltfModel.bufferViews.at(uvAcc->bufferView);
						uvBuf = &gltfModel.buffers.at(uvBV->buffer);
						uvCT = uvAcc->componentType;
						uvNorm = uvAcc->normalized;
						uvCompSize =
							(uvCT == TINYGLTF_COMPONENT_TYPE_FLOAT ? sizeof(float) :
								(uvCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) :
									(uvCT == TINYGLTF_COMPONENT_TYPE_SHORT ? sizeof(int16_t) :
										(uvCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? sizeof(uint8_t) :
											(uvCT == TINYGLTF_COMPONENT_TYPE_BYTE ? sizeof(int8_t) : sizeof(float))))));
						uvStride = uvAcc->ByteStride(*uvBV);
						if (uvStride == 0) uvStride = uvComps * uvCompSize;
						uvBase = uvBuf->data.data() + uvBV->byteOffset + uvAcc->byteOffset;
					}

					// ==== JOINTS_0 (UBYTE/USHORT) ====
					const tinygltf::Accessor* jointsAcc = nullptr;
					const tinygltf::BufferView* jointsBV = nullptr;
					const tinygltf::Buffer* jointsBuf = nullptr;
					const uint8_t* jointsBase = nullptr;
					size_t jointsStride = 0, jointsCompSize = 0;
					int jointsCT = 0; int jointsComps = 4;
					if (isSkinned && prim.attributes.find("JOINTS_0") != prim.attributes.end()) {
						jointsAcc = &gltfModel.accessors.at(prim.attributes.at("JOINTS_0"));
						jointsBV = &gltfModel.bufferViews.at(jointsAcc->bufferView);
						jointsBuf = &gltfModel.buffers.at(jointsBV->buffer);
						jointsCT = jointsAcc->componentType;
						jointsCompSize =
							(jointsCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) : sizeof(uint8_t));
						jointsStride = jointsAcc->ByteStride(*jointsBV);
						if (jointsStride == 0) jointsStride = jointsComps * jointsCompSize;
						jointsBase = jointsBuf->data.data() + jointsBV->byteOffset + jointsAcc->byteOffset;
					}

					// ==== WEIGHTS_0 (FLOAT, or UBYTE/USHORT normalized) ====
					const tinygltf::Accessor* weightsAcc = nullptr;
					const tinygltf::BufferView* weightsBV = nullptr;
					const tinygltf::Buffer* weightsBuf = nullptr;
					const uint8_t* weightsBase = nullptr;
					size_t weightsStride = 0, weightsCompSize = 0;
					bool weightsNorm = false; int weightsCT = 0; int weightsComps = 4;
					if (isSkinned && prim.attributes.find("WEIGHTS_0") != prim.attributes.end()) {
						weightsAcc = &gltfModel.accessors.at(prim.attributes.at("WEIGHTS_0"));
						weightsBV = &gltfModel.bufferViews.at(weightsAcc->bufferView);
						weightsBuf = &gltfModel.buffers.at(weightsBV->buffer);
						weightsCT = weightsAcc->componentType;
						weightsNorm = weightsAcc->normalized;
						weightsCompSize =
							(weightsCT == TINYGLTF_COMPONENT_TYPE_FLOAT ? sizeof(float) :
								(weightsCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? sizeof(uint16_t) : sizeof(uint8_t)));
						weightsStride = weightsAcc->ByteStride(*weightsBV);
						if (weightsStride == 0) weightsStride = weightsComps * weightsCompSize;
						weightsBase = weightsBuf->data.data() + weightsBV->byteOffset + weightsAcc->byteOffset;
					}

					// ==== Assembles vertices buffer: pos(3) + normal(3) + tangent(4) + uv(2) ====
					std::vector<float> vertices;
					vertices.resize(posAcc.count * (3 + 3 + 4 + 2), 0.0f);

					// Buffers auxiliares para skinning
					std::vector<unsigned short> jointsData(posAcc.count * 4, 0);
					std::vector<float> weightsData(posAcc.count * 4, 0.0f);

					for (size_t vi = 0; vi < posAcc.count; ++vi) {
						size_t base = vi * 12;

						// POSITION
						{
							const uint8_t* ptr = posBase + vi * posStride;
							if (posAcc.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
								const float* f = reinterpret_cast<const float*>(ptr);
								vertices[base + 0] = f[0];
								vertices[base + 1] = f[1];
								vertices[base + 2] = f[2];
							}
							else if (posAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
								const uint16_t* s = reinterpret_cast<const uint16_t*>(ptr);
								float scale = posAcc.normalized ? (1.0f / 65535.0f) : 1.0f;
								vertices[base + 0] = s[0] * scale;
								vertices[base + 1] = s[1] * scale;
								vertices[base + 2] = s[2] * scale;
							}
							else if (posAcc.componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
								const int16_t* s = reinterpret_cast<const int16_t*>(ptr);
								float scale = posAcc.normalized ? (1.0f / 32767.0f) : 1.0f;
								vertices[base + 0] = s[0] * scale;
								vertices[base + 1] = s[1] * scale;
								vertices[base + 2] = s[2] * scale;
							}
							else if (posAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
								const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);
								float scale = posAcc.normalized ? (1.0f / 255.0f) : 1.0f;
								vertices[base + 0] = b[0] * scale;
								vertices[base + 1] = b[1] * scale;
								vertices[base + 2] = b[2] * scale;
							}
							else if (posAcc.componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
								const int8_t* b = reinterpret_cast<const int8_t*>(ptr);
								float scale = posAcc.normalized ? (1.0f / 127.0f) : 1.0f;
								vertices[base + 0] = b[0] * scale;
								vertices[base + 1] = b[1] * scale;
								vertices[base + 2] = b[2] * scale;
							}
						}

						// NORMAL -> float, but treat normalized integers if comes instead
						if (hasNormal) {
							const uint8_t* ptr = normBase + vi * normStride;
							if (normCT == TINYGLTF_COMPONENT_TYPE_FLOAT) {
								const float* f = reinterpret_cast<const float*>(ptr);
								vertices[base + 3] = f[0];
								vertices[base + 4] = f[1];
								vertices[base + 5] = f[2];
							}
							else if (normCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
								const uint16_t* s = reinterpret_cast<const uint16_t*>(ptr);
								float scale = normNorm ? (1.0f / 65535.0f) : 1.0f;
								vertices[base + 3] = s[0] * scale * (normNorm ? 2.0f : 1.0f) - (normNorm ? 1.0f : 0.0f);
								vertices[base + 4] = s[1] * scale * (normNorm ? 2.0f : 1.0f) - (normNorm ? 1.0f : 0.0f);
								vertices[base + 5] = s[2] * scale * (normNorm ? 2.0f : 1.0f) - (normNorm ? 1.0f : 0.0f);
							}
							else if (normCT == TINYGLTF_COMPONENT_TYPE_SHORT) {
								const int16_t* s = reinterpret_cast<const int16_t*>(ptr);
								float scale = normNorm ? (1.0f / 32767.0f) : 1.0f;
								vertices[base + 3] = s[0] * scale;
								vertices[base + 4] = s[1] * scale;
								vertices[base + 5] = s[2] * scale;
							}
							else if (normCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
								const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);
								float scale = normNorm ? (1.0f / 255.0f) : 1.0f;
								vertices[base + 3] = b[0] * scale * (normNorm ? 2.0f : 1.0f) - (normNorm ? 1.0f : 0.0f);
								vertices[base + 4] = b[1] * scale * (normNorm ? 2.0f : 1.0f) - (normNorm ? 1.0f : 0.0f);
								vertices[base + 5] = b[2] * scale * (normNorm ? 2.0f : 1.0f) - (normNorm ? 1.0f : 0.0f);
							}
							else if (normCT == TINYGLTF_COMPONENT_TYPE_BYTE) {
								const int8_t* b = reinterpret_cast<const int8_t*>(ptr);
								float scale = normNorm ? (1.0f / 127.0f) : 1.0f;
								vertices[base + 3] = b[0] * scale;
								vertices[base + 4] = b[1] * scale;
								vertices[base + 5] = b[2] * scale;
							}
						}
						else {
							vertices[base + 3] = 0.0f; vertices[base + 4] = 0.0f; vertices[base + 5] = 1.0f;
						}

						// TANGENT -> float/normalized
						if (hasTangent) {
							const uint8_t* ptr = tanBase + vi * tanStride;
							if (tanCT == TINYGLTF_COMPONENT_TYPE_FLOAT) {
								const float* f = reinterpret_cast<const float*>(ptr);
								vertices[base + 6] = f[0];
								vertices[base + 7] = f[1];
								vertices[base + 8] = f[2];
								vertices[base + 9] = f[3];
							}
							else if (tanCT == TINYGLTF_COMPONENT_TYPE_SHORT) {
								const int16_t* s = reinterpret_cast<const int16_t*>(ptr);
								float scale = tanNorm ? (1.0f / 32767.0f) : 1.0f;
								vertices[base + 6] = s[0] * scale;
								vertices[base + 7] = s[1] * scale;
								vertices[base + 8] = s[2] * scale;
								vertices[base + 9] = s[3] * scale;
							}
							else if (tanCT == TINYGLTF_COMPONENT_TYPE_BYTE) {
								const int8_t* b = reinterpret_cast<const int8_t*>(ptr);
								float scale = tanNorm ? (1.0f / 127.0f) : 1.0f;
								vertices[base + 6] = b[0] * scale;
								vertices[base + 7] = b[1] * scale;
								vertices[base + 8] = b[2] * scale;
								vertices[base + 9] = b[3] * scale;
							}
							else if (tanCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
								const uint16_t* s = reinterpret_cast<const uint16_t*>(ptr);
								float scale = tanNorm ? (1.0f / 65535.0f) : 1.0f;
								vertices[base + 6] = s[0] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
								vertices[base + 7] = s[1] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
								vertices[base + 8] = s[2] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
								vertices[base + 9] = s[3] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
							}
							else if (tanCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
								const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);
								float scale = tanNorm ? (1.0f / 255.0f) : 1.0f;
								vertices[base + 6] = b[0] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
								vertices[base + 7] = b[1] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
								vertices[base + 8] = b[2] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
								vertices[base + 9] = b[3] * scale * (tanNorm ? 2.0f : 1.0f) - (tanNorm ? 1.0f : 0.0f);
							}
						}
						else {
							vertices[base + 6] = 1.0f; vertices[base + 7] = 0.0f; vertices[base + 8] = 0.0f; vertices[base + 9] = 1.0f;
						}

						// UV -> float ou normalized 0..1
						if (hasTexCoord) {
							const uint8_t* ptr = uvBase + vi * uvStride;
							if (uvCT == TINYGLTF_COMPONENT_TYPE_FLOAT) {
								const float* f = reinterpret_cast<const float*>(ptr);
								vertices[base + 10] = f[0];
								vertices[base + 11] = f[1];
							}
							else if (uvCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
								const uint16_t* s = reinterpret_cast<const uint16_t*>(ptr);
								float scale = uvNorm ? (1.0f / 65535.0f) : 1.0f;
								vertices[base + 10] = s[0] * scale;
								vertices[base + 11] = s[1] * scale;
							}
							else if (uvCT == TINYGLTF_COMPONENT_TYPE_SHORT) {
								const int16_t* s = reinterpret_cast<const int16_t*>(ptr);
								float scale = uvNorm ? (1.0f / 32767.0f) : 1.0f;
								vertices[base + 10] = s[0] * scale;
								vertices[base + 11] = s[1] * scale;
							}
							else if (uvCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
								const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);
								float scale = uvNorm ? (1.0f / 255.0f) : 1.0f;
								vertices[base + 10] = b[0] * scale;
								vertices[base + 11] = b[1] * scale;
							}
							else if (uvCT == TINYGLTF_COMPONENT_TYPE_BYTE) {
								const int8_t* b = reinterpret_cast<const int8_t*>(ptr);
								float scale = uvNorm ? (1.0f / 127.0f) : 1.0f;
								vertices[base + 10] = b[0] * scale;
								vertices[base + 11] = b[1] * scale;
							}
						}
						else {
							vertices[base + 10] = 0.0f; vertices[base + 11] = 0.0f;
						}

						// JOINTS -> stores as UNSIGNED_SHORT in VBO
						if (jointsAcc) {
							const uint8_t* ptr = jointsBase + vi * jointsStride;
							if (jointsCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
								const uint16_t* s = reinterpret_cast<const uint16_t*>(ptr);
								for (int k = 0; k < 4; ++k) jointsData[vi * 4 + k] = s[k];
							}
							else { // UNSIGNED_BYTE
								const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);
								for (int k = 0; k < 4; ++k) jointsData[vi * 4 + k] = (unsigned short)b[k];
							}
						}

						// WEIGHTS -> converts to float (0..1 if normalized)
						if (weightsAcc) {
							const uint8_t* ptr = weightsBase + vi * weightsStride;
							if (weightsCT == TINYGLTF_COMPONENT_TYPE_FLOAT) {
								const float* f = reinterpret_cast<const float*>(ptr);
								for (int k = 0; k < 4; ++k) weightsData[vi * 4 + k] = f[k];
							}
							else if (weightsCT == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
								const uint16_t* s = reinterpret_cast<const uint16_t*>(ptr);
								float scale = weightsNorm ? (1.0f / 65535.0f) : 1.0f;
								for (int k = 0; k < 4; ++k) weightsData[vi * 4 + k] = s[k] * scale;
							}
							else { // UNSIGNED_BYTE
								const uint8_t* b = reinterpret_cast<const uint8_t*>(ptr);
								float scale = weightsNorm ? (1.0f / 255.0f) : 1.0f;
								for (int k = 0; k < 4; ++k) weightsData[vi * 4 + k] = b[k] * scale;
							}
						}
						else {
							weightsData[vi * 4 + 0] = 1.0f;
						}
					}

					// ==== INDICES ====
					std::vector<uint32_t> indices;
					GLenum indexGLType = GL_UNSIGNED_INT;

					if (prim.indices >= 0) {
						const tinygltf::Accessor& idxAcc = gltfModel.accessors.at(prim.indices);
						const tinygltf::BufferView& idxBV = gltfModel.bufferViews.at(idxAcc.bufferView);
						const tinygltf::Buffer& idxBuf = gltfModel.buffers.at(idxBV.buffer);
						const uint8_t* idxBase = idxBuf.data.data() + idxBV.byteOffset + idxAcc.byteOffset;

						indices.resize(idxAcc.count);
						if (idxAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
							indexGLType = GL_UNSIGNED_BYTE;
							for (size_t i = 0; i < idxAcc.count; ++i) indices[i] = reinterpret_cast<const uint8_t*>(idxBase)[i];
						}
						else if (idxAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
							indexGLType = GL_UNSIGNED_SHORT;
							for (size_t i = 0; i < idxAcc.count; ++i) indices[i] = reinterpret_cast<const uint16_t*>(idxBase)[i];
						}
						else { // UNSIGNED_INT
							indexGLType = GL_UNSIGNED_INT;
							for (size_t i = 0; i < idxAcc.count; ++i) indices[i] = reinterpret_cast<const uint32_t*>(idxBase)[i];
						}
					}
					else {
						// primitive without índices: generates sequential
						// num. vertices == posAcc.count, assuming TRIANGLES
						indices.resize(posAcc.count);
						for (size_t i = 0; i < posAcc.count; ++i) indices[i] = (uint32_t)i;
						indexGLType = (posAcc.count <= 65535) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
					}

					// ==== OpenGL buffers/VAO ====
					glGenVertexArrays(1, &sub.vao);
					glGenBuffers(1, &sub.vbo);
					glGenBuffers(1, &sub.ebo);
					if (isSkinned) {
						glGenBuffers(1, &sub.jbo);
						glGenBuffers(1, &sub.wbo);
					}

					glBindVertexArray(sub.vao);

					glBindBuffer(GL_ARRAY_BUFFER, sub.vbo);
					glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
					GLsizei stride = (3 + 3 + 4 + 2) * sizeof(float);
					// pos (0)
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
					glEnableVertexAttribArray(0);
					// normal (1)
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(1);
					// tangent (2)
					glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
					glEnableVertexAttribArray(2);
					// uv (3)
					glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(10 * sizeof(float)));
					glEnableVertexAttribArray(3);

					if (isSkinned) {
						// JOINTS as UNSIGNED_SHORT (loc 4)
						glBindBuffer(GL_ARRAY_BUFFER, sub.jbo);
						glBufferData(GL_ARRAY_BUFFER, jointsData.size() * sizeof(unsigned short), jointsData.data(), GL_STATIC_DRAW);
						glVertexAttribIPointer(4, 4, GL_UNSIGNED_SHORT, 4 * sizeof(unsigned short), (void*)0);
						glEnableVertexAttribArray(4);

						// WEIGHTS as FLOAT (loc 5)
						glBindBuffer(GL_ARRAY_BUFFER, sub.wbo);
						glBufferData(GL_ARRAY_BUFFER, weightsData.size() * sizeof(float), weightsData.data(), GL_STATIC_DRAW);
						glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
						glEnableVertexAttribArray(5);
					}

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sub.ebo);
					if (indexGLType == GL_UNSIGNED_BYTE) {
						std::vector<uint8_t> idx8(indices.size());
						for (size_t i = 0; i < indices.size(); ++i) idx8[i] = (uint8_t)indices[i];
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx8.size() * sizeof(uint8_t), idx8.data(), GL_STATIC_DRAW);
					}
					else if (indexGLType == GL_UNSIGNED_SHORT) {
						std::vector<uint16_t> idx16(indices.size());
						for (size_t i = 0; i < indices.size(); ++i) idx16[i] = (uint16_t)indices[i];
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx16.size() * sizeof(uint16_t), idx16.data(), GL_STATIC_DRAW);
					}
					else {
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
					}

					glBindVertexArray(0);

					sub.indexCount = indices.size();
					sub.indexType = indexGLType;

					// ==== Textures / Materials ====
					sub.baseColorTex = getBaseColorTexture(gltfModel, prim.material);
					if (sub.baseColorTex == 0) sub.baseColorTex = getDiffuseTextureFromSpecGloss(gltfModel, prim.material);
					sub.normalMapTex = getNormalMapTexture(gltfModel, prim.material);

					if (prim.material >= 0 && prim.material < (int)gltfModel.materials.size()) {
						const auto& mat = gltfModel.materials[prim.material];

						sub.baseColorTex = getBaseColorTexture(gltfModel, prim.material);
						if (sub.baseColorTex == 0) sub.baseColorTex = getDiffuseTextureFromSpecGloss(gltfModel, prim.material);
						sub.normalMapTex = getNormalMapTexture(gltfModel, prim.material);

						if (mat.alphaMode == "MASK") { sub.alphaMode = "MASK"; sub.originalAlphaMode = "MASK"; }
						else if (mat.alphaMode == "BLEND") { sub.alphaMode = "BLEND"; sub.originalAlphaMode = "BLEND"; }

						if (mat.doubleSided) sub.doubleSided = true;
						if (mat.additionalValues.count("alphaCutoff")) sub.alphaCutoff = static_cast<float>(mat.additionalValues.at("alphaCutoff").Factor());

						if (mat.additionalValues.count("glossinessTexture")) {
							int texIndex = mat.additionalValues.at("glossinessTexture").TextureIndex();
							if (texIndex >= 0 && texIndex < (int)gltfModel.textures.size()) {
								int imgIndex = gltfModel.textures[texIndex].source;
								if (imgIndex >= 0 && imgIndex < (int)gltfModel.images.size()) {
									const auto& img = gltfModel.images[imgIndex];
									glGenTextures(1, &sub.glossinessTex);
									glBindTexture(GL_TEXTURE_2D, sub.glossinessTex);
									glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
									glGenerateMipmap(GL_TEXTURE_2D);
									glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
									glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
									GLfloat maxAniso = 0.0f; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
									glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
								}
							}
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

						if (mat.additionalValues.count("specularTexture")) {
							int texIndex = mat.additionalValues.at("specularTexture").TextureIndex();
							if (texIndex >= 0 && texIndex < (int)gltfModel.textures.size()) {
								int imgIndex = gltfModel.textures[texIndex].source;
								if (imgIndex >= 0 && imgIndex < (int)gltfModel.images.size()) {
									const auto& img = gltfModel.images[imgIndex];
									glGenTextures(1, &sub.specularF0Tex);
									glBindTexture(GL_TEXTURE_2D, sub.specularF0Tex);
									glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
									glGenerateMipmap(GL_TEXTURE_2D);
									glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
									glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
									GLfloat maxAniso = 0.0f; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
									glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
								}
							}
						}

						if (sub.specularF0Tex == 0) sub.specularF0Tex = getGlossinessTextureFromSpecGloss(gltfModel, prim.material);
						if (sub.glossinessTex == 0)  sub.glossinessTex = getGlossinessTextureFromSpecGloss(gltfModel, prim.material);

						if (mat.extensions.find("KHR_materials_transmission") != mat.extensions.end()) {
							const auto& ext = mat.extensions.at("KHR_materials_transmission");
							if (ext.Has("transmissionFactor"))
								sub.transmissionFactor = static_cast<float>(ext.Get("transmissionFactor").GetNumberAsDouble());
							if (ext.Has("transmissionTexture")) {
								int texIndex = ext.Get("transmissionTexture").Get("index").Get<int>();
								if (texIndex >= 0 && texIndex < (int)gltfModel.textures.size()) {
									int imgIndex = gltfModel.textures[texIndex].source;
									if (imgIndex >= 0 && imgIndex < (int)gltfModel.images.size()) {
										const auto& img = gltfModel.images[imgIndex];
										glGenTextures(1, &sub.transmissionTex);
										glBindTexture(GL_TEXTURE_2D, sub.transmissionTex);
										glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.image.data());
										glGenerateMipmap(GL_TEXTURE_2D);
										glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
										glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
										GLfloat maxAniso = 0.0f; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
										glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAniso);
									}
								}
							}
						}

						if (mat.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
							sub.useMetalRoughness = true;

							int texIndex = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;
							const tinygltf::Texture& tex = gltfModel.textures[texIndex];
							int imgIndex = tex.source;

							if (imgIndex >= 0 && imgIndex < (int)gltfModel.images.size()) {
								const auto& img = gltfModel.images[imgIndex];

								glGenTextures(1, &sub.metallicTex);
								glBindTexture(GL_TEXTURE_2D, sub.metallicTex);

								// Metal/Roughness e AO (canal R) não são sRGB
								GLenum format = (img.component == 4) ? GL_RGBA : GL_RGB;
								GLenum internalFormat = (img.component == 4) ? GL_RGBA : GL_RGB;

								glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, img.width, img.height, 0, format, GL_UNSIGNED_BYTE, img.image.data());
								glGenerateMipmap(GL_TEXTURE_2D);

								// Configurações de textura
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
								glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
								GLfloat maxAniso = 0.0f; glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
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
	glGenQueries((GLsizei)meshes.size(), occlusionQueries.data());

	FiscionX::Core::AllModels.push_back(this);
}

void FiscionX::Model::updateOcclusion(const glm::mat4& viewProj) {
	glm::mat4 baseMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z))
		* glm::eulerAngleXYZ(rotation.y, rotation.x, rotation.z)
		* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

	for (size_t i = 0; i < meshes.size(); ++i) {
		const auto& mesh = meshes[i];
		glm::mat4 modelMatrix = baseMatrix * (isSkinned ? glm::mat4(1.0f) : mesh.transform);
		glm::mat4 mvp = viewProj * modelMatrix;

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glDisable(GL_BLEND);

		glBeginQuery(GL_ANY_SAMPLES_PASSED, occlusionQueries[i]);
		glBindVertexArray(mesh.vao);
		glUseProgram(0);
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
			glCullFace(GL_FRONT); // to shadow acne
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

	// Metallic/Roughness texture
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mesh.metallicTex);
	glUniform1i(glGetUniformLocation(shader, "metallicTex"), 5);
	glUniform1i(glGetUniformLocation(shader, "useMetalRoughness"), mesh.useMetalRoughness);

	// ENVIRONMENT SETTINGS
	glUniform1f(glGetUniformLocation(shader, "environmentStrength"), FiscionX::Core::AMBIENT_LIGHT_INTENSITY);
	glUniform3f(glGetUniformLocation(shader, "environmentSkyColor"), 0.3f, 0.3f, 0.35f);
	glUniform3f(glGetUniformLocation(shader, "environmentGroundColor"), 0.05f, 0.05f, 0.07f);

	// SHADOW SETTINGS
	glUniform1i(glGetUniformLocation(shader, "isAffectedByLight"), this->isAffectedByLight ? 1 : 0);
	glUniform1i(glGetUniformLocation(shader, "acceptsShadows"), this->acceptsShadows ? 1 : 0);

	// Alpha Settings
	glUniform1f(glGetUniformLocation(shader, "alpha"), alpha);

	// Lights
	int numLights = std::fmin((int)FiscionX::Core::AllLights.size(), 10);
	glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);
	for (int i = 0; i < numLights; ++i) {
		const Light& L = *FiscionX::Core::AllLights[i];
		std::string idx = std::to_string(i);
		glUniform1i(glGetUniformLocation(shader, ("lightType[" + idx + "]").c_str()), L.type);
		glUniform3fv(glGetUniformLocation(shader, ("lightPos[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.position.x, L.position.y, L.position.z)));
		glUniform3fv(glGetUniformLocation(shader, ("lightDir[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.direction.x, L.direction.y, L.direction.z)));
		glUniform3fv(glGetUniformLocation(shader, ("lightColor[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.color.x, L.color.y, L.color.z)));
		glUniform1f(glGetUniformLocation(shader, ("lightIntensity[" + idx + "]").c_str()), L.intensity);
		glUniform1f(glGetUniformLocation(shader, ("lightMaxDistance[" + idx + "]").c_str()), L.maxDistance);
		glUniform1f(glGetUniformLocation(shader, ("lightCutOff[" + idx + "]").c_str()), L.cutOff);
		glUniform1f(glGetUniformLocation(shader, ("lightOuterCutOff[" + idx + "]").c_str()), L.outerCutOff);
		glUniform1f(glGetUniformLocation(shader, ("lightConstant[" + idx + "]").c_str()), L.constant);
		glUniform1f(glGetUniformLocation(shader, ("lightLinear[" + idx + "]").c_str()), L.linear);
		glUniform1f(glGetUniformLocation(shader, ("lightQuadratic[" + idx + "]").c_str()), L.quadratic);
		glUniform1i(glGetUniformLocation(shader, ("lightHasGlow[" + idx + "]").c_str()), L.hasGlow);
		glUniform3fv(glGetUniformLocation(shader, ("lightGlowColor[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.glowColor.x, L.glowColor.y, L.glowColor.z)));
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

void FiscionX::Model::draw(GLuint shader, const glm::mat4& lightSpaceMatrix, GLuint depthMap, bool depthPass, FiscionX::Mat4 view, FiscionX::Mat4 projection) {
	int numLights = static_cast<int>(FiscionX::Core::AllLights.size());

	glUseProgram(shader);

	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(glm::mat4(view)));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(glm::mat4(projection)));
	glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, glm::value_ptr(glm::vec3(FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z)));
	glUniform1i(glGetUniformLocation(shader, "numLights"), numLights);

	for (int i = 0; i < numLights; ++i) {
		const Light& L = *FiscionX::Core::AllLights[i];
		const ShadowMap& sm = FiscionX::Core::AllShadowMaps[i];
		std::string idx = std::to_string(i);

		glUniform1i(glGetUniformLocation(shader, ("lightType[" + idx + "]").c_str()), L.type);
		glUniform3fv(glGetUniformLocation(shader, ("lightPos[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.position.x, L.position.y, L.position.z)));
		glUniform3fv(glGetUniformLocation(shader, ("lightDir[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.direction.x, L.direction.y, L.direction.z)));
		glUniform3fv(glGetUniformLocation(shader, ("lightColor[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.color.x, L.color.y, L.color.z)));
		glUniform1f(glGetUniformLocation(shader, ("lightIntensity[" + idx + "]").c_str()), L.intensity);
		glUniform1f(glGetUniformLocation(shader, ("lightMaxDistance[" + idx + "]").c_str()), L.maxDistance);
		glUniform1f(glGetUniformLocation(shader, ("lightCutOff[" + idx + "]").c_str()), L.cutOff);
		glUniform1f(glGetUniformLocation(shader, ("lightOuterCutOff[" + idx + "]").c_str()), L.outerCutOff);
		glUniform1f(glGetUniformLocation(shader, ("lightConstant[" + idx + "]").c_str()), L.constant);
		glUniform1f(glGetUniformLocation(shader, ("lightLinear[" + idx + "]").c_str()), L.linear);
		glUniform1f(glGetUniformLocation(shader, ("lightQuadratic[" + idx + "]").c_str()), L.quadratic);
		glUniform1i(glGetUniformLocation(shader, ("lightHasGlow[" + idx + "]").c_str()), L.hasGlow);
		glUniform3fv(glGetUniformLocation(shader, ("lightGlowColor[" + idx + "]").c_str()), 1, glm::value_ptr(glm::vec3(L.glowColor.x, L.glowColor.y, L.glowColor.z)));
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
	glm::mat4 baseMatrix =
		glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z))
		* glm::eulerAngleXYZ(rotation.y, rotation.x, rotation.z)
		* glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

	updateOcclusion(view * projection);
	if (physicsSyncTransformMatrix != glm::mat4(1.0f)) {
		baseMatrix = glm::scale(physicsSyncTransformMatrix, glm::vec3(scale.x, scale.y, scale.z));
	}

	std::vector<std::pair<float, const SubMesh*>> transparentMeshes;

	for (int i = 0; i < meshes.size(); i++) {
		const auto& mesh = meshes[i];
		glm::mat4 modelMatrix = baseMatrix * (isSkinned ? glm::mat4(1.0f) : mesh.transform);

		bool isTransparent = (mesh.alphaMode == "BLEND");

		if (!isTransparent && !isVisible[i]) continue;

		if (!depthPass && isTransparent) {
			glm::vec3 worldPos = glm::vec3(modelMatrix * glm::vec4(0.0, 0.0, 0.0, 1.0));
			float dist = glm::length(worldPos - glm::vec3(FiscionX::Core::Camera.position.x, FiscionX::Core::Camera.position.y, FiscionX::Core::Camera.position.z));
			transparentMeshes.emplace_back(dist, &mesh);
		}
		else {
			drawSubMesh(mesh, shader, modelMatrix, lightSpaceMatrix, depthMap, depthPass);
		}
	}

	if (!depthPass && !transparentMeshes.empty()) {
		std::sort(transparentMeshes.begin(), transparentMeshes.end(),
			[](const std::pair<float, const SubMesh*>& a, const std::pair<float, const SubMesh*>& b) {
				return a.first > b.first; // (most far first)
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

void FiscionX::Model::syncTransformWithBody(FiscionX::Physics::Rigidbody* body, FiscionX::Vector3 positionOffset, FiscionX::Vector3 rotationOffset) {
	btTransform trans;
	body->body->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();

	btScalar matrix[16];
	trans.getOpenGLMatrix(matrix);
	glm::mat4 modelMatrix = glm::make_mat4(matrix);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(positionOffset.x, positionOffset.y, positionOffset.z));
	modelMatrix = glm::rotate(modelMatrix, rotationOffset.x, glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotationOffset.y, glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, rotationOffset.z, glm::vec3(0.0f, 0.0f, 1.0f));

	this->physicsSyncTransformMatrix = modelMatrix;
}

// ================= INPUT ===================
bool FiscionX::Input::GetKeyPressed(int key) {
	if (glfwGetKey(FiscionX::Core::Window, key) == GLFW_PRESS) {
		return true;
	}
	return false;
}

bool FiscionX::Input::GetKeyReleased(int key) {
	if (glfwGetKey(FiscionX::Core::Window, key) == GLFW_RELEASE) {
		return true;
	}
	return false;
}

FiscionX::Vector2 FiscionX::Input::GetMousePosition() {
	return mousePosition;
}

FiscionX::Vector2 FiscionX::Input::GetMouseDelta() {
	return mouseDelta;
}

FiscionX::Vector2 FiscionX::Input::GetScrollOffset() {
	return scrollOffset;
}

bool FiscionX::Input::GetMouseButtonPressed(int button) {
	return glfwGetMouseButton(FiscionX::Core::Window, button) == GLFW_PRESS;
}

// ================ Physics ===================
void FiscionX::Physics::CreatePhysicsWorld(FiscionX::Vector3 gravity, int maxIterations) {
	broadphase = new btDbvtBroadphase();
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver();

	DynamicWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	DynamicWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	DynamicWorld->getSolverInfo().m_numIterations = maxIterations;

	debugShader = LoadShader(vertexDebug, fragmentDebug);
	debugDrawer = new GLDebugDrawer();
	debugDrawer->setDebugMode(
		btIDebugDraw::DBG_DrawWireframe
	);
	FiscionX::Physics::DynamicWorld->setDebugDrawer(debugDrawer);
}

void FiscionX::Physics::GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
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

void FiscionX::Physics::GLDebugDrawer::setDebugMode(int debugMode) {
	m_debugMode = debugMode;
}

int FiscionX::Physics::GLDebugDrawer::getDebugMode() const {
	return m_debugMode;
}

void FiscionX::Physics::GLDebugDrawer::drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) {}
void FiscionX::Physics::GLDebugDrawer::reportErrorWarning(const char* warningString) {
	std::cerr << "Bullet Warning: " << warningString << std::endl;
}
void FiscionX::Physics::GLDebugDrawer::draw3dText(const btVector3&, const char*) {}

void FiscionX::Physics::DrawDebugWorld(glm::mat4 projection, glm::mat4 view) {
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
}

FiscionX::Physics::Rigidbody::Rigidbody(FiscionX::Physics::Shape _shape) : shape(_shape) {
	body = new btRigidBody(shape.info);
	body->setCcdMotionThreshold(0.001f);
	body->setCcdSweptSphereRadius(0.3f);
}

void FiscionX::Physics::Rigidbody::activate() {
	if (body) {
		body->activate();
	}
}

void FiscionX::Physics::Rigidbody::setMass(float mass, FiscionX::Vector3 inertia) {
	if (body) {
		btVector3 _inertia(inertia.x, inertia.y, inertia.z);
		if (mass != 0.0f)
			shape.shape->calculateLocalInertia(mass, _inertia);
		body->setMassProps(mass, _inertia);
	}
}

void FiscionX::Physics::Rigidbody::setFriction(float friction) {
	if (body) {
		body->setFriction(friction);
	}
}

void FiscionX::Physics::Rigidbody::setRollingFriction(float friction) {
	if (body) {
		body->setRollingFriction(friction / 5);
	}
}

void FiscionX::Physics::Rigidbody::setDamping(float damping) {
	body->setDamping(damping, damping);
}

void FiscionX::Physics::Rigidbody::applyForce(Vector3 force, Vector3 relPos) {
	if (body) {
		body->applyForce(btVector3(force.x, force.y, force.z), btVector3(relPos.x, relPos.y, relPos.z));
	}
}

void FiscionX::Physics::Rigidbody::applyImpulse(Vector3 impulse, Vector3 relPos) {
	if (body) {
		body->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), btVector3(relPos.x, relPos.y, relPos.z));
	}
}

void FiscionX::Physics::Rigidbody::applyTorque(Vector3 torque) {
	if (body) {
		body->applyTorque(btVector3(torque.x, torque.y, torque.z));
	}
}

void FiscionX::Physics::Rigidbody::applyCentralForce(FiscionX::Vector3 force) {
	if (body) {
		body->applyCentralForce(btVector3(force.x, force.y, force.z));
	}
}

void FiscionX::Physics::Rigidbody::applyCentralImpulse(FiscionX::Vector3 impulse) {
	if (body) {
		body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
	}
}

void FiscionX::Physics::Rigidbody::applyTorqueImpulse(FiscionX::Vector3 torqueImpulse) {
	if (body) {
		body->applyTorqueImpulse(btVector3(torqueImpulse.x, torqueImpulse.y, torqueImpulse.z));
	}
}

void FiscionX::Physics::Rigidbody::setLinearVelocity(FiscionX::Vector3 velocity) {
	if (body) {
		body->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}
}

void FiscionX::Physics::Rigidbody::setAngularVelocity(FiscionX::Vector3 velocity) {
	if (body) {
		body->setAngularVelocity(btVector3(velocity.x, velocity.y, velocity.z));
	}
}

void FiscionX::Physics::Rigidbody::setTransform(FiscionX::Vector3 position, FiscionX::Vector3 rotation) {
	if (body) {
		btTransform transform;
		transform.setOrigin(btVector3(position.x, position.y, position.z));

		float xRad = rotation.x;
		float yRad = rotation.y;
		float zRad = rotation.z;

		btQuaternion rot;
		rot.setEulerZYX(zRad, yRad, xRad);
		transform.setRotation(rot);

		body->setWorldTransform(transform);
	}
}

void FiscionX::Physics::Rigidbody::setLinearFactor(FiscionX::Vector3 factor) {
	if (body) {
		body->setLinearFactor(btVector3(factor.x, factor.y, factor.z));
	}
}

void FiscionX::Physics::Rigidbody::setAngularFactor(FiscionX::Vector3 factor) {
	if (body) {
		body->setAngularFactor(btVector3(factor.x, factor.y, factor.z));
	}
}

void FiscionX::Physics::Rigidbody::setCollisionShape(FiscionX::Physics::Shape* newShape) {
	if (body) {
		body->setCollisionShape(newShape->shape);
	}
}

void FiscionX::Physics::Rigidbody::lockAxis(FiscionX::Vector3 axis) {
	if (body) {
		body->setAngularFactor(btVector3(axis.x, axis.y, axis.z));
	}
}

void FiscionX::Physics::Rigidbody::setBouncingFactor(float factor) {
	if (body) {
		body->setRestitution(factor);
	}
}

void FiscionX::Physics::Rigidbody::setCenterOfMass(const btVector3& newCOMOffset)
{
	if (!body) return;

	btTransform com;
	com.setIdentity();
	com.setOrigin(newCOMOffset);

	body->setCenterOfMassTransform(body->getWorldTransform() * com);
	body->updateInertiaTensor();
}

FiscionX::Vector3 FiscionX::Physics::Rigidbody::getPosition() {
	if (body) {
		btTransform transform;
		body->getMotionState()->getWorldTransform(transform);
		btVector3 pos = transform.getOrigin();
		return FiscionX::Vector3(pos.getX(), pos.getY(), pos.getZ());
	}
}

FiscionX::Vector3 FiscionX::Physics::Rigidbody::getRotation() {
	if (body) {
		btTransform transform;
		body->getMotionState()->getWorldTransform(transform);
		btQuaternion rot = transform.getRotation();
		btScalar roll, pitch, yaw;
		rot.getEulerZYX(yaw, pitch, roll);
		return FiscionX::Vector3(roll, pitch, yaw);
	}
}

// =================== Physics::Shape ===================
FiscionX::Physics::Shape::Shape(btCollisionShape* _shape, btRigidBody::btRigidBodyConstructionInfo _info, btDefaultMotionState _motion, btGImpactMeshShape* _gshape)
	: shape(_shape), gshape(_gshape), info(_info), motion(_motion) {
}

FiscionX::Physics::Shape FiscionX::Physics::CreateCapsuleShape(FiscionX::Vector3 position, FiscionX::Vector3 rotation, float radius, float height, float mass) {
	btCollisionShape* newshape = new btCapsuleShape(radius, height);
	btTransform start;
	start.setIdentity();
	start.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	start.setOrigin(btVector3(position.x, position.y, position.z));
	btVector3 inertia(0, 0, 0);
	newshape->calculateLocalInertia(mass, inertia);
	btDefaultMotionState* motion = new btDefaultMotionState(start);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newshape, inertia);

	FiscionX::Physics::Shape* pshape = new FiscionX::Physics::Shape(newshape, info, *motion, nullptr);
	return *pshape;
}

FiscionX::Physics::Shape FiscionX::Physics::CreateBoxShape(Vector3 position, Vector3 rotation, Vector3 scale, float mass) {
	btCollisionShape* newshape = new btBoxShape(btVector3(scale.x, scale.y, scale.z));
	btTransform start;
	start.setIdentity();
	start.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	start.setOrigin(btVector3(position.x, position.y, position.z));
	btVector3 inertia(0, 0, 0);
	newshape->calculateLocalInertia(mass, inertia);
	btDefaultMotionState* motion = new btDefaultMotionState(start);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newshape, inertia);

	FiscionX::Physics::Shape pshape(newshape, info, *motion, nullptr);
	return pshape;
}

FiscionX::Physics::Shape FiscionX::Physics::CreateConeShape(Vector3 position, Vector3 rotation, float radius, float height, float mass) {
	btCollisionShape* newshape = new btConeShape(radius, height);
	btTransform start;
	start.setIdentity();
	start.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	start.setOrigin(btVector3(position.x, position.y, position.z));
	btVector3 inertia(0, 0, 0);
	newshape->calculateLocalInertia(mass, inertia);
	btDefaultMotionState* motion = new btDefaultMotionState(start);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newshape, inertia);

	FiscionX::Physics::Shape pshape(newshape, info, *motion, nullptr);
	return pshape;
}

FiscionX::Physics::Shape FiscionX::Physics::CreateCyllinderShape(Vector3 position, Vector3 rotation, float radius, float height, float mass) {
	btCollisionShape* newshape = new btCylinderShape(btVector3(radius, height, radius));
	btTransform start;
	start.setIdentity();
	start.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	start.setOrigin(btVector3(position.x, position.y, position.z));
	btVector3 inertia(0, 0, 0);
	newshape->calculateLocalInertia(mass, inertia);
	btDefaultMotionState* motion = new btDefaultMotionState(start);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newshape, inertia);

	FiscionX::Physics::Shape pshape(newshape, info, *motion, nullptr);
	return pshape;
}

FiscionX::Physics::Shape FiscionX::Physics::CreateSphereShape(Vector3 position, Vector3 rotation, float radius, float mass) {
	btCollisionShape* newshape = new btSphereShape(radius);
	btTransform start;
	start.setIdentity();
	start.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	start.setOrigin(btVector3(position.x, position.y, position.z));
	btVector3 inertia(0, 0, 0);
	newshape->calculateLocalInertia(mass, inertia);
	btDefaultMotionState* motion = new btDefaultMotionState(start);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newshape, inertia);

	FiscionX::Physics::Shape pshape(newshape, info, *motion, nullptr);
	return pshape;
}

btTypedConstraint* FiscionX::Physics::CreateJoint(Joint& desc)
{
	if (!desc.bodyA || !desc.bodyB) return nullptr;

	btTypedConstraint* c = nullptr;

	switch (desc.type)
	{
	case JointType::POINT2POINT:
		c = new btPoint2PointConstraint(
			*desc.bodyA, *desc.bodyB,
			desc.frameA.getOrigin(),
			desc.frameB.getOrigin()
		);
		break;

	case JointType::HINGE:
		c = new btHingeConstraint(
			*desc.bodyA, *desc.bodyB,
			desc.frameA,
			desc.frameB
		);
		static_cast<btHingeConstraint*>(c)->setLimit(
			desc.hingeLower, desc.hingeUpper
		);
		break;

	case JointType::CONETWIST:
	{
		auto* ct = new btConeTwistConstraint(
			*desc.bodyA, *desc.bodyB,
			desc.frameA, desc.frameB
		);
		ct->setLimit(desc.swing1, desc.swing2, desc.twist);
		c = ct;
	}
	break;

	case JointType::DOF6:
	{
		auto* dof = new btGeneric6DofConstraint(
			*desc.bodyA, *desc.bodyB,
			desc.frameA, desc.frameB,
			true
		);
		dof->setLinearLowerLimit(desc.linearLower);
		dof->setLinearUpperLimit(desc.linearUpper);
		dof->setAngularLowerLimit(desc.angularLower);
		dof->setAngularUpperLimit(desc.angularUpper);
		c = dof;
	}
	break;

	case JointType::DOF6_SPRING:
	{
		auto* dof = new btGeneric6DofSpring2Constraint(
			*desc.bodyA, *desc.bodyB,
			desc.frameA, desc.frameB
		);

		dof->setLinearLowerLimit(desc.linearLower);
		dof->setLinearUpperLimit(desc.linearUpper);
		dof->setAngularLowerLimit(desc.angularLower);
		dof->setAngularUpperLimit(desc.angularUpper);

		for (int i = 0; i < 6; i++) {
			if (desc.useSpring[i]) {
				dof->enableSpring(i, true);
				dof->setStiffness(i, desc.stiffness[i]);
				dof->setDamping(i, desc.damping[i]);
			}
		}

		c = dof;
	}
	break;

	case JointType::FIXED:
	{
		// fixed = 6DOF com limites 0
		auto* dof = new btGeneric6DofConstraint(
			*desc.bodyA, *desc.bodyB,
			desc.frameA, desc.frameB,
			true
		);
		btVector3 zero(0, 0, 0);
		dof->setLinearLowerLimit(zero);
		dof->setLinearUpperLimit(zero);
		dof->setAngularLowerLimit(zero);
		dof->setAngularUpperLimit(zero);
		c = dof;
	}
	break;
	}

	if (!c) return nullptr;

	FiscionX::Physics::DynamicWorld->addConstraint(c, !desc.collideConnected);
	desc.constraint = c;
	return c;
}


btTriangleMesh* FiscionX::Physics::LoadMeshFromFile(const char* path, FiscionX::Vector3 scale) {
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err, warn;

	if (!loader.LoadBinaryFromFile(&model, &err, &warn, path)) {
		std::cerr << "Failed to load GLB: " << err << std::endl;
		return nullptr;
	}

	btTriangleMesh* triMesh = new btTriangleMesh();
	glm::mat4 globalScale = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

	std::function<void(int, glm::mat4)> processNode;
	processNode = [&](int nodeIndex, glm::mat4 parentTransform) {
		const auto& node = model.nodes[nodeIndex];

		glm::mat4 local(1.0f);
		if (!node.matrix.empty()) {
			local = glm::make_mat4(node.matrix.data());
		}
		else {
			glm::vec3 T(0.0f), S(1.0f);
			glm::quat R(1, 0, 0, 0);
			if (!node.translation.empty()) T = glm::make_vec3(node.translation.data());
			if (!node.rotation.empty()) R = glm::make_quat(node.rotation.data());
			if (!node.scale.empty()) S = glm::make_vec3(node.scale.data());
			local = glm::translate(glm::mat4(1.0f), T)
				* glm::mat4_cast(R)
				* glm::scale(glm::mat4(1.0f), S);
		}

		glm::mat4 globalTransform = parentTransform * local;

		if (node.mesh >= 0) {
			const auto& mesh = model.meshes[node.mesh];
			for (const auto& prim : mesh.primitives) {
				if (prim.mode != TINYGLTF_MODE_TRIANGLES) continue;

				const auto& posAcc = model.accessors[prim.attributes.at("POSITION")];
				const auto& posView = model.bufferViews[posAcc.bufferView];
				const auto& posBuf = model.buffers[posView.buffer];
				const float* posData = reinterpret_cast<const float*>(
					&posBuf.data[posView.byteOffset + posAcc.byteOffset]);

				std::vector<glm::vec3> vertices(posAcc.count);
				for (size_t i = 0; i < posAcc.count; ++i) {
					glm::vec4 v(posData[i * 3 + 0], posData[i * 3 + 1], posData[i * 3 + 2], 1.0f);
					v = globalScale * globalTransform * v;
					vertices[i] = glm::vec3(v);
				}

				const auto& idxAcc = model.accessors[prim.indices];
				const auto& idxView = model.bufferViews[idxAcc.bufferView];
				const auto& idxBuf = model.buffers[idxView.buffer];
				const void* idxData = &idxBuf.data[idxView.byteOffset + idxAcc.byteOffset];

				for (size_t i = 0; i < idxAcc.count; i += 3) {
					uint32_t i0, i1, i2;
					switch (idxAcc.componentType) {
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						i0 = ((uint8_t*)idxData)[i + 0];
						i1 = ((uint8_t*)idxData)[i + 1];
						i2 = ((uint8_t*)idxData)[i + 2];
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						i0 = ((uint16_t*)idxData)[i + 0];
						i1 = ((uint16_t*)idxData)[i + 1];
						i2 = ((uint16_t*)idxData)[i + 2];
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						i0 = ((uint32_t*)idxData)[i + 0];
						i1 = ((uint32_t*)idxData)[i + 1];
						i2 = ((uint32_t*)idxData)[i + 2];
						break;
					default:
						std::cerr << "Unsupported index type\n";
						return nullptr;
					}

					triMesh->addTriangle(
						btVector3(vertices[i0].x, vertices[i0].y, vertices[i0].z),
						btVector3(vertices[i1].x, vertices[i1].y, vertices[i1].z),
						btVector3(vertices[i2].x, vertices[i2].y, vertices[i2].z)
					);
				}
			}
		}

		for (int child : node.children) {
			processNode(child, globalTransform);
		}
		};

	for (int nodeIndex : model.scenes[model.defaultScene].nodes) {
		processNode(nodeIndex, glm::mat4(1.0f));
	}

	return triMesh;
}

FiscionX::Physics::Shape FiscionX::Physics::CreateMeshShape(const char* path, FiscionX::Vector3 position, FiscionX::Vector3 rotation, FiscionX::Vector3 scale, float mass) {
	btGImpactMeshShape* newshape = new btGImpactMeshShape(FiscionX::Physics::LoadMeshFromFile(path, scale));
	btTransform start;
	start.setIdentity();
	start.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z));
	start.setOrigin(btVector3(position.x, position.y, position.z));
	btVector3 inertia(0, 0, 0);
	newshape->calculateLocalInertia(mass, inertia);
	btDefaultMotionState* motion = new btDefaultMotionState(start);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, newshape, inertia);
	FiscionX::Physics::Shape pshape(nullptr, info, *motion, newshape);
	return pshape;
}

bool FiscionX::Physics::CheckCollisionBetween(FiscionX::Physics::Rigidbody* bodyA, FiscionX::Physics::Rigidbody* bodyB) {
	int numManifolds = DynamicWorld->getDispatcher()->getNumManifolds();

	btVector3 posA = bodyA->body->getWorldTransform().getOrigin();
	btVector3 posB = bodyB->body->getWorldTransform().getOrigin();

	if ((posA - posB).length2() > FiscionX::Physics::maxCollisionDistance * FiscionX::Physics::maxCollisionDistance) {
		return false;
	}

	for (int i = 0; i < numManifolds; ++i) {
		btPersistentManifold* manifold = DynamicWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* objA = manifold->getBody0();
		const btCollisionObject* objB = manifold->getBody1();

		if ((objA == bodyA->body && objB == bodyB->body) || (objA == bodyB->body && objB == bodyA->body)) {
			int numContacts = manifold->getNumContacts();
			for (int j = 0; j < numContacts; ++j) {
				const btManifoldPoint& pt = manifold->getContactPoint(j);
				if (pt.getDistance() < 0.0f) {
					return true;
				}
			}
		}
	}
	return false;
}

// Vehicle
FiscionX::Physics::Vehicle::Vehicle(FiscionX::Physics::Rigidbody* chassiBody) {
	tuning = new btRaycastVehicle::btVehicleTuning;
	raycaster = new btDefaultVehicleRaycaster(FiscionX::Physics::DynamicWorld);
	vehicle = new btRaycastVehicle(*tuning, chassiBody->body, raycaster);
	chassiBody->body->setActivationState(DISABLE_DEACTIVATION);
}

void FiscionX::Physics::Vehicle::addWheel(FiscionX::Vector3 relativePosition, FiscionX::Vector3 wheelDirectionCS0, FiscionX::Vector3 wheelAxleCS,
	float suspensionRestLength, float wheelRadius, bool isFrontWheel) {

	vehicle->addWheel(btVector3(relativePosition.x, relativePosition.y, relativePosition.z),
		btVector3(wheelDirectionCS0.x, wheelDirectionCS0.y, wheelDirectionCS0.z),
		btVector3(wheelAxleCS.x, wheelAxleCS.y, wheelAxleCS.z),
		suspensionRestLength, wheelRadius, *tuning, isFrontWheel);
}

int FiscionX::Physics::Vehicle::getNumWheels() {
	return vehicle->getNumWheels();
}

FiscionX::Physics::Vehicle::WheelInfo& FiscionX::Physics::Vehicle::getWheelInfo(int index) {
	FiscionX::Physics::Vehicle::WheelInfo* info = new FiscionX::Physics::Vehicle::WheelInfo;
	info->info = &vehicle->getWheelInfo(index);
	return *info;
}

void FiscionX::Physics::Vehicle::update(float deltaTime) {
	vehicle->updateVehicle(FiscionX::Core::deltaTime);
	vehicle->updateSuspension(FiscionX::Core::deltaTime);
	vehicle->updateFriction(FiscionX::Core::deltaTime);
}

void FiscionX::Physics::Vehicle::applyEngineForce(float force, int wheelIndex) {
	vehicle->applyEngineForce(force, wheelIndex);
}
void FiscionX::Physics::Vehicle::setSteeringValue(float value, int wheelIndex) {
	vehicle->setSteeringValue(value, wheelIndex);
}
void FiscionX::Physics::Vehicle::setBrake(float brake, int wheelIndex) {
	vehicle->setBrake(brake, wheelIndex);
}
float FiscionX::Physics::Vehicle::getCurrentSpeedKmh() {
	return vehicle->getCurrentSpeedKmHour();
}
FiscionX::Vector3 FiscionX::Physics::Vehicle::getWheelWorldPosition(int index) {
	FiscionX::Physics::Vehicle::WheelInfo* info = new FiscionX::Physics::Vehicle::WheelInfo;
	info->info = &vehicle->getWheelInfo(index);

	return FiscionX::Vector3(info->info->m_worldTransform.getOrigin().getX(),
		info->info->m_worldTransform.getOrigin().getY(),
		info->info->m_worldTransform.getOrigin().getZ());
}
FiscionX::Vector3 FiscionX::Physics::Vehicle::getWheelRotation(int index) {
	FiscionX::Physics::Vehicle::WheelInfo* info = new FiscionX::Physics::Vehicle::WheelInfo;
	info->info = &vehicle->getWheelInfo(index);

	return FiscionX::Vector3(info->info->m_worldTransform.getRotation().getX(),
		info->info->m_worldTransform.getRotation().getY(),
		info->info->m_worldTransform.getRotation().getZ());
}

// Raycast
bool FiscionX::Physics::Raycast::CheckCollisionWithBody(FiscionX::Physics::Rigidbody* body, FiscionX::Vector3 origin, FiscionX::Vector3 end)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(origin.x, origin.y, origin.z), btVector3(end.x, end.y, end.z));
	FiscionX::Physics::DynamicWorld->rayTest(btVector3(origin.x, origin.y, origin.z), btVector3(end.x, end.y, end.z), rayCallback);

	if (rayCallback.hasHit())
	{
		const btRigidBody* hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);

		if (body->body == hitBody) {
			return true;
		}
	}
	else
	{
		return false;
	}
}

const btRigidBody* FiscionX::Physics::Raycast::GetFirstBodyCollided(FiscionX::Vector3 origin, FiscionX::Vector3 end)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(btVector3(origin.x, origin.y, origin.z), btVector3(end.x, end.y, end.z));
	FiscionX::Physics::DynamicWorld->rayTest(btVector3(origin.x, origin.y, origin.z), btVector3(end.x, end.y, end.z), rayCallback);

	if (rayCallback.hasHit())
	{
		const btRigidBody* hitBody = btRigidBody::upcast(rayCallback.m_collisionObject);

		if (hitBody)
			return hitBody;
	}
	else
	{
		return nullptr;
	}
}
// =================== CORE ===================
void FiscionX::Core::CreateShadowMap(ShadowMap& sm, int LIGHT_TYPE) {
	glGenFramebuffers(1, &sm.fbo);
	glGenTextures(1, &sm.depthMap);
	glBindTexture(GL_TEXTURE_2D, sm.depthMap);
	if (LIGHT_TYPE == LIGHT_DIRECTIONAL) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			DIR_SHADOW_SIZE, DIR_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	else if (LIGHT_TYPE == LIGHT_SPOT) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			SPOT_SHADOW_SIZE, SPOT_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	else if (LIGHT_TYPE == LIGHT_POINT) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			POINT_SHADOW_SIZE, POINT_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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
		FiscionX::Core::CreateShadowMap(sm, L->type);
		sm.lightSpaceMatrix = FiscionX::Core::ComputeLightSpaceMatrix(*L);
		FiscionX::Core::AllShadowMaps.push_back(sm);
	}
}

glm::mat4 FiscionX::Core::ComputeLightSpaceMatrix(const Light& L) {
	if (L.type == LIGHT_DIRECTIONAL) {
		float orthoSize = FiscionX::Core::SHADOW_VIEW_RADIUS;
		glm::vec3 dir = glm::normalize(glm::vec3(L.direction.x, L.direction.y, L.direction.z));

		glm::vec3 center = glm::vec3(Camera.position.x, Camera.position.y, Camera.position.z) + glm::vec3(Camera.front.x, Camera.front.y, Camera.front.z) * glm::vec3(orthoSize);
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
		glm::mat4 view = glm::lookAt(glm::vec3(L.position.x, L.position.y, L.position.z), glm::vec3(L.position.x, L.position.y, L.position.z) + glm::normalize(glm::vec3(L.direction.x, L.direction.y, L.direction.z)), up);
		return proj * view;
	}

	if (L.type == LIGHT_POINT) {
		glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, L.maxDistance);
		glm::mat4 view = glm::lookAt(glm::vec3(L.position.x, L.position.y, L.position.z), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		return proj * view;
	}

	return glm::mat4(1.0f);
}

void FiscionX::Core::RenderAllShadowPasses(FiscionX::Mat4 view, FiscionX::Mat4 projection, FiscionX::Mat4 viewProj) {
	// update skin UBOs once
	for (auto& model : AllModels) {
		if (!model->castsShadows) continue;
		if (model->isSkinned) {
			const std::vector<glm::mat4>& bones = model->getBoneTransforms();
			glBindBuffer(GL_UNIFORM_BUFFER, model->uboSkin);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * bones.size(), bones.data(), GL_DYNAMIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, model->uboSkin);
		}
	}

	float now = static_cast<float>(glfwGetTime());

	// Cache model world positions
	std::vector<glm::vec3> modelWorldPositions;
	modelWorldPositions.reserve(AllModels.size());
	for (auto& m : AllModels) {
		if (!m->castsShadows) continue;
		glm::vec3 worldPos = glm::vec3(m->position.x, m->position.y, m->position.z);
		modelWorldPositions.push_back(worldPos);
	}

	// Track camera motion so directional shadows update when camera moves
	static glm::vec3 _lastCameraPos = glm::vec3(Camera.position.x, Camera.position.y, Camera.position.z);
	bool cameraMoved = glm::length(_lastCameraPos - glm::vec3(Camera.position.x, Camera.position.y, Camera.position.z)) > 0.05f; // tweak threshold
	_lastCameraPos = glm::vec3(Camera.position.x, Camera.position.y, Camera.position.z);

	for (size_t i = 0; i < AllLights.size(); ++i) {
		Light& L = *AllLights[i];             // Non-const to update timing fields
		ShadowMap& sm = AllShadowMaps[i];

		if (!L.enableShadows) continue;

		// decide if we should rebuild this light's shadow map
		bool moved = (L.lastPosition != glm::vec3(FLT_MAX)) && (glm::length(L.lastPosition - glm::vec3(L.position.x, L.position.y, L.position.z)) > 0.01f);
		bool timeExpired = (now - L.lastShadowUpdateTime) >= L.shadowUpdatePeriod;
		bool firstTime = (L.lastPosition == glm::vec3(FLT_MAX));

		bool shouldUpdate = moved || timeExpired || firstTime;
		// Directional must also update if camera moved (because light space uses camera center)
		if (L.type == LIGHT_DIRECTIONAL) shouldUpdate = shouldUpdate || cameraMoved;

		if (!shouldUpdate) continue;

		// Record update
		L.lastShadowUpdateTime = now;
		L.lastPosition = glm::vec3(L.position.x, L.position.y, L.position.z);

		if (L.type == LIGHT_POINT) {
			// --- OPTIMIZED: prefilter models in range once ---
			glm::vec3 pos = glm::vec3(L.position.x, L.position.y, L.position.z);
			std::vector<size_t> staticIndices;
			std::vector<size_t> skinnedIndices;
			staticIndices.reserve(AllModels.size());
			skinnedIndices.reserve(AllModels.size());

			for (size_t mi = 0; mi < AllModels.size(); ++mi) {
				if (!AllModels[mi]->castsShadows) continue;
				float dist = glm::length(modelWorldPositions[mi] - pos);
				if (dist <= (L.maxDistance + 1.0f)) {
					if (AllModels[mi]->isSkinned) skinnedIndices.push_back(mi);
					else staticIndices.push_back(mi);
				}
			}

			// If nothing is in range, skip entire cubemap generation
			if (staticIndices.empty() && skinnedIndices.empty()) {
				continue;
			}

			float zFar = L.maxDistance;
			glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, NEAR_PLANE, zFar);

			std::array<glm::mat4, 6> shadowMatrices = {
				proj * glm::lookAt(pos, pos + glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)),
				proj * glm::lookAt(pos, pos + glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)),
				proj * glm::lookAt(pos, pos + glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)),
				proj * glm::lookAt(pos, pos + glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)),
				proj * glm::lookAt(pos, pos + glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)),
				proj * glm::lookAt(pos, pos + glm::vec3(0, 0,-1), glm::vec3(0,-1, 0)),
			};

			glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);

			for (int face = 0; face < 6; ++face) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, sm.depthMap, 0);
				glClear(GL_DEPTH_BUFFER_BIT);

				// STATIC - depth-only (only loop the filtered staticIndices)
				if (!staticIndices.empty()) {
					glUseProgram(depthShaderCubeStatic);
					glUniform1f(glGetUniformLocation(depthShaderCubeStatic, "farPlane"), zFar);
					glUniform3fv(glGetUniformLocation(depthShaderCubeStatic, "lightPos"), 1, glm::value_ptr(pos));
					glUniformMatrix4fv(glGetUniformLocation(depthShaderCubeStatic, ("shadowMatrices[" + std::to_string(face) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowMatrices[face]));

					for (size_t idx : staticIndices) {
						if (!AllModels[idx]->castsShadows) continue;
						AllModels[idx]->draw(depthShaderCubeStatic, glm::mat4(1.0f), 0, true, view, projection);
					}
				}

				// SKINNED - depth-only (only loop the filtered skinnedIndices)
				if (!skinnedIndices.empty()) {
					glUseProgram(depthShaderCubeSkinned);
					glUniform1f(glGetUniformLocation(depthShaderCubeSkinned, "farPlane"), zFar);
					glUniform3fv(glGetUniformLocation(depthShaderCubeSkinned, "lightPos"), 1, glm::value_ptr(pos));
					glUniformMatrix4fv(glGetUniformLocation(depthShaderCubeSkinned, ("shadowMatrices[" + std::to_string(face) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowMatrices[face]));

					for (size_t idx : skinnedIndices) {
						if (!AllModels[idx]->castsShadows) continue;
						AllModels[idx]->draw(depthShaderCubeSkinned, glm::mat4(1.0f), 0, true, view, projection);
					}
				}
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else if (L.type == LIGHT_SPOT) {
			// Spot: distance + cone culling (kept as-is)
			sm.lightSpaceMatrix = ComputeLightSpaceMatrix(L);
			glm::vec3 lightPos = glm::vec3(L.position.x, L.position.y, L.position.z);

			glViewport(0, 0, SPOT_SHADOW_SIZE, SPOT_SHADOW_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			// STATIC
			glUseProgram(depthShaderStatic);
			glUniformMatrix4fv(glGetUniformLocation(depthShaderStatic, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));

			for (size_t mi = 0; mi < AllModels.size(); ++mi) {
				float dist = glm::length(modelWorldPositions[mi] - lightPos);
				if (dist > (L.maxDistance + 1.0f)) continue;

				glm::vec3 toModel = glm::normalize(modelWorldPositions[mi] - lightPos);
				float dp = glm::dot(glm::normalize(glm::vec3(L.direction.x, L.direction.y, L.direction.z)), toModel);
				if (dp < L.outerCutOff - 0.01f) continue;

				if (!AllModels[mi]->isSkinned) {
					if (!AllModels[mi]->castsShadows) continue;
					AllModels[mi]->draw(depthShaderStatic, glm::mat4(1.0f), 0, true, view, projection);
				}
			}

			// SKINNED
			glUseProgram(depthShaderSkinned);
			glUniformMatrix4fv(glGetUniformLocation(depthShaderSkinned, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));

			for (size_t mi = 0; mi < AllModels.size(); ++mi) {
				float dist = glm::length(modelWorldPositions[mi] - lightPos);
				if (dist > (L.maxDistance + 1.0f)) continue;

				glm::vec3 toModel = glm::normalize(modelWorldPositions[mi] - lightPos);
				float dp = glm::dot(glm::normalize(glm::vec3(L.direction.x, L.direction.y, L.direction.z)), toModel);
				if (dp < L.outerCutOff - 0.01f) continue;

				if (AllModels[mi]->isSkinned) {
					if (!AllModels[mi]->castsShadows) continue;
					AllModels[mi]->draw(depthShaderSkinned, glm::mat4(1.0f), 0, true, view, projection);
				}
			}

			glCullFace(GL_BACK);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else if (L.type == LIGHT_DIRECTIONAL) {
			// Directional
			sm.lightSpaceMatrix = ComputeLightSpaceMatrix(L);

			glViewport(0, 0, DIR_SHADOW_SIZE, DIR_SHADOW_SIZE);
			glBindFramebuffer(GL_FRAMEBUFFER, sm.fbo);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			// STATIC
			glUseProgram(depthShaderStatic);
			glUniformMatrix4fv(glGetUniformLocation(depthShaderStatic, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));

			for (size_t mi = 0; mi < AllModels.size(); ++mi) {
				if (!AllModels[mi]->castsShadows) continue;
				if (!AllModels[mi]->isSkinned) {
					AllModels[mi]->draw(depthShaderStatic, glm::mat4(1.0f), 0, true, view, projection);
				}
			}

			// SKINNED
			glUseProgram(depthShaderSkinned);
			glUniformMatrix4fv(glGetUniformLocation(depthShaderSkinned, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(sm.lightSpaceMatrix));

			for (size_t mi = 0; mi < AllModels.size(); ++mi) {
				if (!AllModels[mi]->castsShadows) continue;
				if (AllModels[mi]->isSkinned) {
					AllModels[mi]->draw(depthShaderSkinned, glm::mat4(1.0f), 0, true, view, projection);
				}
			}

			glCullFace(GL_BACK);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}

void FiscionX::Core::Set3DSettings(const int _DIRECTIONAL_LIGHT_SHADOW_SIZE, const int _SPOT_LIGHT_SHADOW_SIZE,
	const int _POINT_LIGHT_SHADOW_SIZE, const float _SHADOW_VIEW_RADIUS, const float _NEAR_PLANE, const float _FAR_PLANE) {

	DIR_SHADOW_SIZE = _DIRECTIONAL_LIGHT_SHADOW_SIZE;
	SPOT_SHADOW_SIZE = _SPOT_LIGHT_SHADOW_SIZE;
	POINT_SHADOW_SIZE = _POINT_LIGHT_SHADOW_SIZE;
	NEAR_PLANE = _NEAR_PLANE;
	FAR_PLANE = _FAR_PLANE;
	SHADOW_VIEW_RADIUS = _SHADOW_VIEW_RADIUS;
}

void FiscionX::Core::SetCursorMode(int mode) {
	if (FiscionX::Core::Window) {
		glfwSetInputMode(FiscionX::Core::Window, GLFW_CURSOR, mode);
	}
}

void FiscionX::Core::SetCacheSettings(bool _enableShaderCache, bool _enableModelCache) {
	FiscionX::Core::enableShaderCache = _enableShaderCache;
	FiscionX::Core::enableModelCache = _enableModelCache;
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
	glfwSetScrollCallback(Window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "ERR 0x002 - Failed to initialize GLAD\n";
		glfwTerminate();
		system("pause");
		std::exit(-2);
	}

	glEnable(GL_DEPTH_TEST);

	// ====== SHADERS ======
	/*
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
	*/

	depthShaderStatic = LoadShader(depth2DstaticVertex, depth_fragment);
	depthShaderSkinned = LoadShader(depth2DskinnedVertex, depth_fragment);
	depthShaderCubeStatic = LoadShader(depthCubeStaticVertex, depth_fragment);
	depthShaderCubeSkinned = LoadShader(depthCubeSkinnedVertex, depth_fragment);
	shaderStatic = LoadShader(vertexStatic, fragment);
	shaderSkinned = LoadShader(vertexSkinned, fragment);
	UI::Image::shader = LoadShader(imageVertex, imageFragment);
	UI::Video::shaderVideo = LoadShader(videoVertex, videoFragment);
	textShader = LoadShader(textVertexShader, textFragmentShader);
	shaderUI = LoadShader(uiVertex, uiFragment);

	// Configure Shadow Mapping
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		DIR_SHADOW_SIZE, DIR_SHADOW_SIZE, 0,
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

void FiscionX::Core::SetWindowSize(int width, int height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	glfwSetWindowSize(FiscionX::Core::Window, width, height);
	glViewport(0, 0, width, height);
}

void FiscionX::Core::SetWindowIcon(const char* iconPath) {
	GLFWimage icon;
	icon.pixels = stbi_load(iconPath, &icon.width, &icon.height, nullptr, 4);
	if (icon.pixels) {
		glfwSetWindowIcon(FiscionX::Core::Window, 1, &icon);
		stbi_image_free(icon.pixels);
	}
	else {
		std::cerr << "ERR 0x013 - Failed to load window icon from " << iconPath << std::endl;
	}
}

void FiscionX::Core::SetWindowFullscreen(bool fullscreen, int monitorIndex) {
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);
	if (!monitors || monitorIndex < 0 || monitorIndex >= count)
		return;

	GLFWmonitor* monitor = monitors[monitorIndex];

	if (fullscreen) {
		int modeCount;
		const GLFWvidmode* modes = glfwGetVideoModes(monitor, &modeCount);

		const GLFWvidmode* chosenMode = nullptr;
		int bestRefresh = 0;
		int bestColorBits = 0;

		for (int i = 0; i < modeCount; i++) {
			if (modes[i].width == SCREEN_WIDTH && modes[i].height == SCREEN_HEIGHT) {
				int colorBits = modes[i].redBits + modes[i].greenBits + modes[i].blueBits;
				if (modes[i].refreshRate > bestRefresh ||
					(modes[i].refreshRate == bestRefresh && colorBits > bestColorBits)) {
					chosenMode = &modes[i];
					bestRefresh = modes[i].refreshRate;
					bestColorBits = colorBits;
				}
			}
		}

		// Se não achou, usa modo nativo
		if (!chosenMode) {
			chosenMode = glfwGetVideoMode(monitor);
			std::cerr << "Resolução desejada não encontrada, usando resolução nativa.\n";
		}

		glfwSetWindowMonitor(
			FiscionX::Core::Window,
			monitor,
			0, 0,
			chosenMode->width,
			chosenMode->height,
			chosenMode->refreshRate
		);

		glViewport(0, 0, chosenMode->width, chosenMode->height);

	}
	else {
		glfwSetWindowMonitor(
			FiscionX::Core::Window,
			nullptr,
			100, 100,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			0
		);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
}

void FiscionX::Core::SortModels() {
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
			float da = glm::length2(glm::vec3(a->position.x, a->position.y, a->position.z) - glm::vec3(Camera.position.x, Camera.position.y, Camera.position.z));
			float db = glm::length2(glm::vec3(b->position.x, b->position.y, b->position.z) - glm::vec3(Camera.position.x, Camera.position.y, Camera.position.z));
			return da > db;
		}

		return false;
		});
}

void FiscionX::Core::Terminate() {
	glfwTerminate();
}

// ===================== DRAW ========================
void RenderPrimitive(const std::vector<glm::vec2>& vertices,
	GLenum mode,
	FiscionX::Vector4 color,
	glm::mat4 ortho)
{
	glEnable(GL_BLEND);
	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), vertices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glUseProgram(FiscionX::Core::shaderUI);
	glUniformMatrix4fv(glGetUniformLocation(FiscionX::Core::shaderUI, "uMVP"), 1, GL_FALSE, glm::value_ptr(ortho));
	glUniform4f(glGetUniformLocation(FiscionX::Core::shaderUI, "uColor"), color.x, color.y, color.z, color.w);

	glDrawArrays(mode, 0, (GLsizei)vertices.size());

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void FiscionX::Core::Draw::ClearBackground(float r, float g, float b, float alpha) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, FiscionX::Core::SCREEN_WIDTH, FiscionX::Core::SCREEN_HEIGHT);
	glClearColor(r, g, b, alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FiscionX::Core::Draw::SwapBuffers() {
	glfwSwapBuffers(FiscionX::Core::Window);
}

void FiscionX::Core::Draw::DrawLine(
	FiscionX::Vector2 start,
	FiscionX::Vector2 end,
	float thickness,
	FiscionX::Vector4 color)
{
	glm::mat4 ortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

	glm::vec2 dir = glm::normalize(glm::vec2(end.x - start.x, end.y - start.y));
	glm::vec2 normal = { -dir.y, dir.x }; // perpendicular
	glm::vec2 offset = normal * (thickness * 0.5f);

	std::vector<glm::vec2> verts = {
		{ start.x - offset.x, start.y - offset.y },
		{ start.x + offset.x, start.y + offset.y },
		{ end.x - offset.x, end.y - offset.y },
		{ end.x + offset.x, end.y + offset.y }
	};

	RenderPrimitive(verts, GL_TRIANGLE_STRIP, color, ortho);
}

void FiscionX::Core::Draw::DrawRect(
	FiscionX::Vector2 position,
	FiscionX::Vector2 size,
	FiscionX::Vector4 color,
	float borderThickness,
	float borderRadius,
	int segments)
{
	glm::mat4 ortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);

	// Se não há borda e nem raio, só retângulo sólido simples
	if (borderThickness == 0 && borderRadius == 0) {
		std::vector<glm::vec2> verts = {
			{ position.x,             position.y },
			{ position.x + size.x,    position.y },
			{ position.x + size.x,    position.y + size.y },
			{ position.x,             position.y + size.y }
		};
		RenderPrimitive(verts, GL_TRIANGLE_FAN, color, ortho);
		return;
	}

	// Limita o raio ao máximo possível (metade do lado menor)
	float r = std::min(borderRadius, std::min(size.x, size.y) * 0.5f);

	std::vector<glm::vec2> verts;

	if (borderThickness == 0) {
		// Retângulo sólido com cantos arredondados
		verts.push_back({ position.x + size.x * 0.5f, position.y + size.y * 0.5f }); // centro para TRIANGLE_FAN

		// Gera cada canto arredondado
		auto arc = [&](float cx, float cy, float startAngle, float endAngle) {
			for (int i = 0; i <= segments; i++) {
				float t = startAngle + (endAngle - startAngle) * (float)i / segments;
				float x = cx + cos(t) * r;
				float y = cy + sin(t) * r;
				verts.push_back({ x, y });
			}
			};

		// canto inferior esquerdo
		arc(position.x + r, position.y + r, glm::pi<float>(), 1.5f * glm::pi<float>());
		// canto inferior direito
		arc(position.x + size.x - r, position.y + r, 1.5f * glm::pi<float>(), 2.0f * glm::pi<float>());
		// canto superior direito
		arc(position.x + size.x - r, position.y + size.y - r, 0.0f, 0.5f * glm::pi<float>());
		// canto superior esquerdo
		arc(position.x + r, position.y + size.y - r, 0.5f * glm::pi<float>(), glm::pi<float>());

		RenderPrimitive(verts, GL_TRIANGLE_FAN, color, ortho);
	}
	else {
		// Retângulo apenas de borda com cantos arredondados (faixa entre outer e inner)
		float halfT = borderThickness * 0.5f;

		float ox0 = position.x - halfT;
		float oy0 = position.y - halfT;
		float ox1 = position.x + size.x + halfT;
		float oy1 = position.y + size.y + halfT;

		float ix0 = position.x + halfT;
		float iy0 = position.y + halfT;
		float ix1 = position.x + size.x - halfT;
		float iy1 = position.y + size.y - halfT;

		float ro = r + halfT; // raio externo
		float ri = std::max(0.0f, r - halfT); // raio interno

		std::vector<glm::vec2> strip;

		auto arcStrip = [&](float cx, float cy, float start, float end) {
			for (int i = 0; i <= segments; i++) {
				float t = start + (end - start) * (float)i / segments;
				float xo = cx + cos(t) * ro;
				float yo = cy + sin(t) * ro;
				float xi = cx + cos(t) * ri;
				float yi = cy + sin(t) * ri;
				strip.push_back({ xi, yi });
				strip.push_back({ xo, yo });
			}
			};

		// canto inferior esquerdo
		arcStrip(position.x + r, position.y + r, glm::pi<float>(), 1.5f * glm::pi<float>());
		// canto inferior direito
		arcStrip(position.x + size.x - r, position.y + r, 1.5f * glm::pi<float>(), 2.0f * glm::pi<float>());
		// canto superior direito
		arcStrip(position.x + size.x - r, position.y + size.y - r, 0.0f, 0.5f * glm::pi<float>());
		// canto superior esquerdo
		arcStrip(position.x + r, position.y + size.y - r, 0.5f * glm::pi<float>(), glm::pi<float>());
		// fecha o loop
		arcStrip(position.x + r, position.y + r, glm::pi<float>(), glm::pi<float>());

		RenderPrimitive(strip, GL_TRIANGLE_STRIP, color, ortho);
	}
}

void FiscionX::Core::Draw::DrawCircle(
	FiscionX::Vector2 position,
	float radius,
	FiscionX::Vector4 color,
	float borderThickness,
	int segments)
{
	glm::mat4 ortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	std::vector<glm::vec2> verts;

	if (borderThickness == 0) {
		// Círculo sólido
		verts.push_back({ position.x, position.y });
		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;
			verts.push_back({
				position.x + cosf(theta) * radius,
				position.y + sinf(theta) * radius
				});
		}
		RenderPrimitive(verts, GL_TRIANGLE_FAN, color, ortho);
	}
	else {
		// Círculo com espessura real
		float innerR = radius - borderThickness * 0.5f;
		float outerR = radius + borderThickness * 0.5f;

		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;

			glm::vec2 inner = {
				position.x + cosf(theta) * innerR,
				position.y + sinf(theta) * innerR
			};

			glm::vec2 outer = {
				position.x + cosf(theta) * outerR,
				position.y + sinf(theta) * outerR
			};

			verts.push_back(inner);
			verts.push_back(outer);
		}

		RenderPrimitive(verts, GL_TRIANGLE_STRIP, color, ortho);
	}
}

void FiscionX::Core::Draw::DrawArc(
	FiscionX::Vector2 position,
	FiscionX::Vector4 color,
	float radius,
	float start_angle,
	float end_angle,
	float thickness,
	int segments)
{
	glm::mat4 ortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	std::vector<glm::vec2> verts;

	float angleStep = (end_angle - start_angle) / segments;

	if (thickness == 0) {
		// Arco preenchido (como "fatia de pizza")
		verts.push_back({ position.x, position.y });

		for (int i = 0; i <= segments; i++) {
			float angle = start_angle + i * angleStep;
			verts.push_back({
				position.x + cosf(angle) * radius,
				position.y + sinf(angle) * radius
				});
		}

		RenderPrimitive(verts, GL_TRIANGLE_FAN, color, ortho);
	}
	else {
		// Arco com espessura real (faixa entre raio interno e externo)
		float innerRadius = radius - thickness * 0.5f;
		float outerRadius = radius + thickness * 0.5f;

		for (int i = 0; i <= segments; i++) {
			float angle = start_angle + i * angleStep;

			float cosA = cosf(angle);
			float sinA = sinf(angle);

			glm::vec2 inner = {
				position.x + cosA * innerRadius,
				position.y + sinA * innerRadius
			};

			glm::vec2 outer = {
				position.x + cosA * outerRadius,
				position.y + sinA * outerRadius
			};

			// adiciona no TRIANGLE_STRIP
			verts.push_back(inner);
			verts.push_back(outer);
		}

		RenderPrimitive(verts, GL_TRIANGLE_STRIP, color, ortho);
	}
}

void FiscionX::Core::Draw::DrawPolygon(
	std::vector<FiscionX::Vector2> vertices,
	FiscionX::Vector4 color,
	float borderThickness)
{
	glm::mat4 ortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	std::vector<glm::vec2> verts;

	// Conversão para glm::vec2
	for (auto& v : vertices)
		verts.push_back({ v.x, v.y });

	if (borderThickness == 0) {
		// Polígono sólido
		RenderPrimitive(verts, GL_TRIANGLE_FAN, color, ortho);
	}
	else {
		// Polígono com espessura de borda -> construir faixa
		std::vector<glm::vec2> strip;
		int n = (int)verts.size();

		for (int i = 0; i < n; i++) {
			glm::vec2 p0 = verts[i];
			glm::vec2 p1 = verts[(i + 1) % n];

			glm::vec2 edge = glm::normalize(p1 - p0);
			glm::vec2 normal = { -edge.y, edge.x };

			glm::vec2 offset = normal * (borderThickness * 0.5f);

			// borda interna
			strip.push_back(p0 - offset);
			strip.push_back(p1 - offset);

			// borda externa
			strip.push_back(p0 + offset);
			strip.push_back(p1 + offset);
		}

		RenderPrimitive(strip, GL_TRIANGLE_STRIP, color, ortho);
	}
}

void FiscionX::Core::Draw::DrawEllipse(
	FiscionX::Vector2 position,
	FiscionX::Vector2 size,
	FiscionX::Vector4 color,
	float borderThickness,
	int segments)
{
	glm::mat4 ortho = glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f);
	std::vector<glm::vec2> verts;

	if (borderThickness == 0) {
		// Elipse sólida
		verts.push_back({ position.x, position.y });

		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;
			verts.push_back({
				position.x + cosf(theta) * size.x,
				position.y + sinf(theta) * size.y
				});
		}

		RenderPrimitive(verts, GL_TRIANGLE_FAN, color, ortho);
	}
	else {
		// Elipse com espessura de borda
		std::vector<glm::vec2> strip;

		float innerX = size.x - borderThickness * 0.5f;
		float innerY = size.y - borderThickness * 0.5f;
		float outerX = size.x + borderThickness * 0.5f;
		float outerY = size.y + borderThickness * 0.5f;

		for (int i = 0; i <= segments; i++) {
			float theta = 2.0f * PI * i / segments;

			glm::vec2 inner = {
				position.x + cosf(theta) * innerX,
				position.y + sinf(theta) * innerY
			};

			glm::vec2 outer = {
				position.x + cosf(theta) * outerX,
				position.y + sinf(theta) * outerY
			};

			strip.push_back(inner);
			strip.push_back(outer);
		}

		RenderPrimitive(strip, GL_TRIANGLE_STRIP, color, ortho);
	}
}

// =================== Shader Loader ===================
GLuint LoadShader(const char* vertexSrc, const char* fragmentSrc) {
	std::hash<std::string> hasher;
	size_t vertexHash = hasher(std::string(vertexSrc));
	size_t fragHash = hasher(std::string(fragmentSrc));
	std::string binaryPath = "cache/shaders/" + std::to_string(vertexHash + fragHash) + ".bin";

	// ───── Tentativa de carregar shader binário ─────
	if (std::filesystem::exists(binaryPath) && FiscionX::Core::enableShaderCache == true) {
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

	// ───── Compile from scratch ─────
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

	// ───── Try to save binary file ─────
	if (FiscionX::Core::enableShaderCache) {
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
	}

	return program;
}