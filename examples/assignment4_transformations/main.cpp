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
#include <ew/ewMath/transformations.h>
#include <ew/procGen.h>

const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

const int NUM_CUBES = 4;
ew::Transform cubeTransforms[NUM_CUBES];

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


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	ew::MeshData cubeMeshData;
	ew::createCube(0.5f, &cubeMeshData);
	ew::Mesh cubeMesh;
	cubeMesh.load(cubeMeshData);

	ew::Shader shader("assets/unlit.vert", "assets/unlit.frag");
	shader.use();

	cubeMesh.bind();

	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		cubeTransforms[i].position.x = i % (NUM_CUBES/2) - 0.5;
		cubeTransforms[i].position.y = i / (NUM_CUBES/2) - 0.5;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			//Construct model matrix
			shader.setMat4("_Model", cubeTransforms[i].getModelMatrix());

			glDrawElements(GL_TRIANGLES, cubeMesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
		}
		
		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Transformations");

			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
