#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
bool show_demo_window = true;

int main() {
	printf("Initializing...");
	assert(glfwInit());
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGLExample", NULL, NULL);
	assert(window != nullptr);
	glfwMakeContextCurrent(window);
	assert(glewInit() == GLEW_OK);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	printf("successful!\n");

	printf("Loading models...");
	static const char* cubeModelPath = "../assets/models/cube.obj";
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(cubeModelPath, 0);
	if (scene == nullptr) {
		printf("Failed to load file %s", cubeModelPath);
		return 1;
	}
	printf("successful!\n");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//START DRAWING
		glClearColor(0.3f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//DRAW IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		ImGui::Text("Hello");
		ImGui::End();

		ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}