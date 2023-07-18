#include <stdio.h>
#include <math.h>
#include <fstream>
#include <sstream>

#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <assert.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/external/stb_image.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

struct Color {
	float r, g, b, a;
};
struct Vertex {
	float x, y, z;
	float u, v;
};

Vertex vertices[4] = {
	//x   //y  //z   //u  //v
	{ -0.5, -0.5, 0.0, 0.0, 0.0}, //Bottom left
	{  0.5, -0.5, 0.0, 1.0, 0.0}, //Bottom right
	{  0.5,  0.5, 0.0, 1.0, 1.0},  //Top right
	{ -0.5,  0.5, 0.0, 0.0, 1.0}  //Top left
};

unsigned int indices[6] = {
	0, 1, 2, 2, 3, 0
};

std::string loadShaderSourceFromFile(const std::string& filePath) {
	std::ifstream fstream(filePath);
	if (!fstream.is_open()) {
		printf("Failed to load file %s", filePath);
		return {};
	}
	std::stringstream buffer;
	buffer << fstream.rdbuf();
	return buffer.str();
}

unsigned int createShader(GLenum shaderType, const char* sourceCode) {
	//Create a new vertex shader object
	unsigned int shader = glCreateShader(shaderType);
	//Supply the shader object with source code
	glShaderSource(shader, 1, &sourceCode, NULL);
	//Compile the shader object
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		//512 is an arbitrary length, but should be plenty of characters for our error message.
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}
	return shader;
}

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
	unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	unsigned int shaderProgram = glCreateProgram();
	//Attach each stage
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//Link all the stages together
	glLinkProgram(shaderProgram);
	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}
	//The linked program now contains our compiled code, so we can delete these intermediate objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertexData, GL_STATIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,x));
	glEnableVertexAttribArray(0);

	//UV attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
	glEnableVertexAttribArray(1);

	return vao;
}

int getTextureFormat(int numComponents) {
	switch (numComponents) {
	default:
		return GL_RGBA;
	case 3:
		return GL_RGB;
	case 2:
		return GL_RG;
	case 1:
		return GL_RED;
	}
}

unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, numComponents;
	unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
	if (data == NULL) {
		printf("Failed to load image %s",filePath);
		stbi_image_free(data);
		return 0;
	}
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int format = getTextureFormat(numComponents);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

	float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
	return texture;
}

struct TextureSettings {
	const char* wrapModes[4] = { "Repeat","Clamp to Edge", "Clamp to Border", "Mirrored Repeat"};
	const int wrapModeEnums[4] = { GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT };

	const char* filterModes[3] = { "Nearest","Linear","Mipmap Linear"};
	const int filterModeEnums[3] = {GL_NEAREST, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR};

	int wrapIndex = 0;
	int filterIndex = 0;
	float scale = 1.0f;
	float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};
TextureSettings textureSettings;

struct BackgroundSettings {
	float distortionStrength = 0.1f;
	float distortionSpeed = 0.2f;
	Color glowColor = { 0.1f,0.3f,0.8f, 1.0f };
}bgSettings;

Color characterColor = { 1.0f, 1.0f, 1.0f, 1.0f };

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGLExample", NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return 1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	unsigned int backgroundShader = createShaderProgram(
		loadShaderSourceFromFile("assets/background.vert").c_str(), 
		loadShaderSourceFromFile("assets/background.frag").c_str());

	unsigned int characterShader = createShaderProgram(
		loadShaderSourceFromFile("assets/character.vert").c_str(), 
		loadShaderSourceFromFile("assets/character.frag").c_str());

	unsigned int vaoA = createVAO(vertices, 4, indices, 6);

	unsigned int textureA = loadTexture("assets/bricks_color.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int textureB = loadTexture("assets/smiley.png", GL_REPEAT, GL_LINEAR);
	unsigned int characterTexture = loadTexture("assets/character.png", GL_REPEAT, GL_NEAREST);
	unsigned int noiseTexture = loadTexture("assets/perlinNoise.png", GL_REPEAT, GL_LINEAR);

	//Shaded
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Enable blending for character transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vaoA);

		glUseProgram(backgroundShader);

		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		
		//Draw background
		glUseProgram(backgroundShader);
		glUniform1f(glGetUniformLocation(backgroundShader, "_Time"), time);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureA);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureB);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		glUniform1i(glGetUniformLocation(backgroundShader, "_TextureA"), 0);
		glUniform1i(glGetUniformLocation(backgroundShader, "_TextureB"), 1);
		glUniform1i(glGetUniformLocation(backgroundShader, "_NoiseTexture"), 2);
		glUniform1f(glGetUniformLocation(backgroundShader, "_DistortionStrength"), bgSettings.distortionStrength);
		glUniform1f(glGetUniformLocation(backgroundShader, "_DistortionSpeed"), bgSettings.distortionSpeed);
		glUniform4f(glGetUniformLocation(backgroundShader, "_GlowColor"), bgSettings.glowColor.r, bgSettings.glowColor.g, bgSettings.glowColor.b, bgSettings.glowColor.a);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Draw character
		glUseProgram(characterShader);
		glUniform1f(glGetUniformLocation(characterShader, "_Time"), time);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characterTexture);
		glUniform1i(glGetUniformLocation(characterShader, "_Texture"), 0);
		glUniform4f(glGetUniformLocation(characterShader, "_Color"), characterColor.r, characterColor.g, characterColor.b, characterColor.a);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::Combo("Wrap Mode", &textureSettings.wrapIndex, textureSettings.wrapModes, IM_ARRAYSIZE(textureSettings.wrapModes))) {
				GLint wrapEnum = textureSettings.wrapModeEnums[textureSettings.wrapIndex];
				glTextureParameteri(textureA, GL_TEXTURE_WRAP_S, wrapEnum);
				glTextureParameteri(textureA, GL_TEXTURE_WRAP_T, wrapEnum);
			}
			if (ImGui::Combo("Filter Mode", &textureSettings.filterIndex, textureSettings.filterModes, IM_ARRAYSIZE(textureSettings.filterModes))) {
				GLint filterEnum = textureSettings.filterModeEnums[textureSettings.filterIndex];
				glTextureParameteri(textureA, GL_TEXTURE_MIN_FILTER, filterEnum);
				glTextureParameteri(textureA, GL_TEXTURE_MAG_FILTER, filterEnum);
			}
			if (ImGui::ColorEdit3("Border Color", textureSettings.borderColor)) {
				glTextureParameterfv(textureA, GL_TEXTURE_BORDER_COLOR, textureSettings.borderColor);
			}
			ImGui::SliderFloat("BG Distortion Strength", &bgSettings.distortionStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("BG Distortion Speed", &bgSettings.distortionSpeed,0.0f,1.0f);
			ImGui::ColorEdit3("BG Glow Color", &bgSettings.glowColor.r);
			ImGui::ColorEdit4("Character Color", &characterColor.r);
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}