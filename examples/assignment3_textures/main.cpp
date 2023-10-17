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

#include <ew/ew.h>
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
	{ -1.0, -1.0, 0.0, 0.0, 0.0}, //Bottom left
	{  1.0, -1.0, 0.0, 1.0, 0.0}, //Bottom right
	{  1.0,  1.0, 0.0, 1.0, 1.0},  //Top right
	{ -1.0,  1.0, 0.0, 0.0, 1.0}  //Top left
};

unsigned int indices[6] = {
	0, 1, 2, 2, 3, 0
};

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

	float scale = 1.0f;
	float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};
TextureSettings textureSettings;

struct BackgroundSettings {
	int wrapIndex = 0;
	int filterIndex = 0;
	float tiling = 1.0f;
	float distortionStrength = 0.1f;
	float distortionSpeed = 0.05f;
	Color glowColor = { 0.1f,0.2f,0.5f, 1.0f };
}bgSettings;

struct CharacterSettings {
	int filterIndex = 0;
	Color characterColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float characterSpeed = 5.0f;
}charSettings;

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

	ew::Shader backgroundShader("assets/background.vert", "assets/background.frag");
	ew::Shader characterShader("assets/character.vert", "assets/character.frag");

	unsigned int vaoA = createVAO(vertices, 4, indices, 6);

	unsigned int textureA = loadTexture("assets/bricks_color.jpg", GL_REPEAT, GL_LINEAR);
	unsigned int textureB = loadTexture("assets/smiley.png", GL_REPEAT, GL_LINEAR);
	unsigned int characterTexture = loadTexture("assets/character.png", GL_REPEAT, GL_NEAREST);
	unsigned int noiseTexture = loadTexture("assets/perlinNoise.png", GL_REPEAT, GL_LINEAR);

	//Shaded
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Enable blending for character transparency
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vaoA);

		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		
		//Draw background
		backgroundShader.use();
		backgroundShader.setFloat("_Time", time);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureA);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureB);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);

		backgroundShader.setInt("_TextureA", 0);
		backgroundShader.setInt("_TextureB", 1);
		backgroundShader.setInt("_NoiseTexture", 2);
		backgroundShader.setFloat("_Tiling", bgSettings.tiling);
		backgroundShader.setFloat("_DistortionStrength", bgSettings.distortionStrength);
		backgroundShader.setFloat("_DistortionSpeed", bgSettings.distortionSpeed);
		backgroundShader.setVec4("_GlowColor", bgSettings.glowColor.r, bgSettings.glowColor.g, bgSettings.glowColor.b, bgSettings.glowColor.a);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Draw character
		characterShader.use();
		characterShader.setFloat("_Time", time);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, characterTexture);
		characterShader.setInt("_Texture", 0);
		characterShader.setVec4("_Color", charSettings.characterColor.r, charSettings.characterColor.g, charSettings.characterColor.b, charSettings.characterColor.a);
		characterShader.setFloat("_Speed", charSettings.characterSpeed);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Background")) {
				if (ImGui::Combo("BG Filter Mode", &bgSettings.filterIndex, textureSettings.filterModes, IM_ARRAYSIZE(textureSettings.filterModes))) {
					GLint filterEnum = textureSettings.filterModeEnums[bgSettings.filterIndex];
					glTextureParameteri(textureA, GL_TEXTURE_MIN_FILTER, filterEnum);
					glTextureParameteri(textureA, GL_TEXTURE_MAG_FILTER, filterEnum);
					glTextureParameteri(textureB, GL_TEXTURE_MIN_FILTER, filterEnum);
					glTextureParameteri(textureB, GL_TEXTURE_MAG_FILTER, filterEnum);
				}
				if (ImGui::Combo("BG Wrap Mode", &bgSettings.wrapIndex, textureSettings.wrapModes, IM_ARRAYSIZE(textureSettings.wrapModes))) {
					GLint wrapEnum = textureSettings.wrapModeEnums[bgSettings.wrapIndex];
					glTextureParameteri(textureA, GL_TEXTURE_WRAP_S, wrapEnum);
					glTextureParameteri(textureA, GL_TEXTURE_WRAP_T, wrapEnum);
					glTextureParameteri(textureB, GL_TEXTURE_WRAP_S, wrapEnum);
					glTextureParameteri(textureB, GL_TEXTURE_WRAP_T, wrapEnum);
				}
				ImGui::DragFloat("Tiling", &bgSettings.tiling, 0.05f);
				if (ImGui::ColorEdit3("Border Color", textureSettings.borderColor)) {
					glTextureParameterfv(textureA, GL_TEXTURE_BORDER_COLOR, textureSettings.borderColor);
				}
				ImGui::SliderFloat("Distortion Strength", &bgSettings.distortionStrength, 0.0f, 1.0f);
				ImGui::SliderFloat("Distortion Speed", &bgSettings.distortionSpeed, 0.0f, 1.0f);
				ImGui::ColorEdit3("Glow Color", &bgSettings.glowColor.r);
			}
			if (ImGui::CollapsingHeader("Character")) {
				if (ImGui::Combo("Character Filter Mode", &charSettings.filterIndex, textureSettings.filterModes, IM_ARRAYSIZE(textureSettings.filterModes))) {
					GLint filterEnum = textureSettings.filterModeEnums[charSettings.filterIndex];
					glTextureParameteri(characterTexture, GL_TEXTURE_MIN_FILTER, filterEnum);
					glTextureParameteri(characterTexture, GL_TEXTURE_MAG_FILTER, filterEnum);
				}
				ImGui::SliderFloat("Speed", &charSettings.characterSpeed, 0.0f, 10.0f);
				ImGui::ColorEdit4("Character Color", &charSettings.characterColor.r);
			}
			
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
