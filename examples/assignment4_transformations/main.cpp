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

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 1080;

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
	ew::Transform cubeTransform;

	std::string vertexShaderSource = ew::loadShaderSourceFromFile("assets/unlit.vert");
	std::string fragmentShaderSource = ew::loadShaderSourceFromFile("assets/unlit.frag");
	unsigned int shader = ew::createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	unsigned int texture = ew::loadTexture("assets/bricks_color.jpg",GL_REPEAT,GL_LINEAR);

	//Set static uniforms
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	int textureLocation = glGetUniformLocation(shader, "uTexture");
	glUniform1i(textureLocation, 0);
	int timeLocation = glGetUniformLocation(shader, "uTime");
	int scaleLocation = glGetUniformLocation(shader, "uScale");
	int modelTransformLocation = glGetUniformLocation(shader, "uModel");


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader);

		
		
		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		//Set the value of the variable at the location
		glUniform1f(timeLocation, time);

		//Construct model matrix
		ew::Mat4 model = cubeTransform.getModelMatrix();
		glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, &model[0][0]);

		cubeMesh.bind();
		glDrawElements(GL_TRIANGLES, cubeMesh.getNumIndices(), GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			ImGui::DragFloat3("Position", &cubeTransform.position.x, 0.1f);
			ImGui::DragFloat3("Rotation", &cubeTransform.rotation.x, 1.0f);
			ImGui::DragFloat3("Scale", &cubeTransform.scale.x, 0.1f);
			if (ImGui::Button("Reset")) {
				cubeTransform.position = ew::Vec3(0.0f);
				cubeTransform.rotation = ew::Vec3(0.0f);
				cubeTransform.scale = ew::Vec3(1.0f);
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
