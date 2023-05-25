#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <ew/Model.h>
#include <ew/Shader.h>
#include <ew/Texture.h>
#include <ew/Material.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
bool show_demo_window = false;

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

	ew::Texture tex_paving_stones_color("assets/pavingstones_color.jpg");
	ew::Texture tex_paving_stones_normal("assets/pavingstones_normal.jpg");
	ew::Texture tex_bricks_color("assets/bricks_color.jpg");
	ew::Texture tex_bricks_normal("assets/bricks_normal.jpg");

	ew::Model cubeModel;
	bool success = cubeModel.loadFromFile("assets/cube.obj");
	if (success) {
		printf("successful!\n");
	}

	ew::Shader shader("assets/unlit.vert", "assets/unlit.frag");

	ew::Material stoneMaterial(&shader);
	stoneMaterial.setTexture("_Texture", &tex_paving_stones_color);
	stoneMaterial.setTexture("_NormalMap", &tex_paving_stones_normal);

	ew::Material brickMaterial(&shader);
	brickMaterial.setTexture("_Texture", &tex_bricks_color);
	brickMaterial.setTexture("_NormalMap", &tex_bricks_normal);

	ew::Material* material = &brickMaterial;

	//Rendering config
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		//START DRAWING
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 camPos = glm::vec3(0, 0, -10.0f);
		glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f, 100.0f);
		glm::mat4 lookAt = glm::lookAt(camPos, glm::vec3(0), glm::vec3(0, 1, 0));

		shader.use();

		glm::mat4 model = glm::mat4(1);
		model = glm::rotate(model, (float)glfwGetTime() * 2.0f, glm::vec3(0.2f, 1.0f, 0));

		material->setMat4("_Model", model);
		material->setMat4("_View", lookAt);
		material->setMat4("_Projection", proj);
		material->updateUniforms();
		cubeModel.draw();

		//DRAW IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		ImGui::Text("Hello");
		ImGui::End();

		if (show_demo_window) {
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}