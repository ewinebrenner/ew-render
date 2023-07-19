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
#include <ew/procGen.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;


struct Camera {
	float fov = 60.0f;
	ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 10.0f);
	ew::Vec3 lookPosition = ew::Vec3(0.0f, 0.0f, 0.0f);
	float orbitDistance = 10.0f;
	float orbitSpeed = 1.0f;
	bool orthographic = false;
	float orthographicHeight = 10.0f;
};

const int NUM_CUBES = 8;
ew::Transform cubeTransforms[NUM_CUBES];
Camera camera;

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
	ew::createCube(0.5f,&cubeMeshData);
	ew::Mesh cubeMesh;
	cubeMesh.load(cubeMeshData);

	std::string vertexShaderSource = ew::loadShaderSourceFromFile("assets/unlit.vert");
	std::string fragmentShaderSource = ew::loadShaderSourceFromFile("assets/unlit.frag");
	unsigned int shader = ew::createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	//Randomize cube transforms
	float cubeFieldRadius = 5.0f;
	for (size_t i = 0; i < NUM_CUBES; i++)
	{
		ew::Vec3 pos = ew::Vec3(
			ew::RandomRange(-1, 1), 
			ew::RandomRange(-1, 1), 
			ew::RandomRange(-1, 1));
		pos = ew::Normalize(pos) * ew::RandomRange(0, cubeFieldRadius);
		cubeTransforms[i].position = pos;
		cubeTransforms[i].rotation = ew::Vec3(
			ew::RandomRange(0, 360),
			ew::RandomRange(0, 360),
			ew::RandomRange(0, 360));
		cubeTransforms[i].scale = ew::Vec3(
			ew::RandomRange(0.5, 2),
			ew::RandomRange(0.5, 2),
			ew::RandomRange(0.5, 2)
		);
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader);

		cubeMesh.bind();
		
		//The current time in seconds this frame
		float time = (float)glfwGetTime();

		camera.position = ew::Vec3(cos(time * camera.orbitSpeed) * camera.orbitDistance, 0.0f, sin(time * camera.orbitSpeed) * camera.orbitDistance);

		//Construct model matrix
		ew::Mat4 view = ew::LookAtMatrix(camera.position, camera.lookPosition, ew::Vec3(0, 1, 0));
		ew::Mat4 projection = camera.orthographic ? ew::OrthographicMatrix(camera.orthographicHeight, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f,100.0f) :
			ew::PerspectiveMatrix(ew::Radians(camera.fov), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
	
		glUniformMatrix4fv(glGetUniformLocation(shader, "_View"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "_Projection"), 1, GL_FALSE, &projection[0][0]);

		//Draw using elements
		for (size_t i = 0; i < NUM_CUBES; i++)
		{
			ew::Mat4 model = cubeTransforms[i].getModelMatrix();
			glUniformMatrix4fv(glGetUniformLocation(shader, "_Model"), 1, GL_FALSE, &model[0][0]);
			glDrawElements(GL_TRIANGLES, cubeMesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				if (camera.orthographic) {
					ImGui::DragFloat("Height", &camera.orthographicHeight, 0.5f);
				}
				else {
					ImGui::DragFloat("FOV", &camera.fov, 1.0f, 0.0f, 180.0f);
				}
				
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				ImGui::DragFloat("Orbit distance", &camera.orbitDistance, 0.1f);
				ImGui::DragFloat("Orbit speed", &camera.orbitSpeed, 0.1f);
			}
			
			for (size_t i = 0; i < NUM_CUBES; i++)
			{
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &cubeTransforms[i].position.x, 0.1f);
					ImGui::DragFloat3("Rotation", &cubeTransforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &cubeTransforms[i].scale.x, 0.1f);
					if (ImGui::Button("Reset")) {
						cubeTransforms[i].position = ew::Vec3(0.0f);
						cubeTransforms[i].rotation = ew::Vec3(0.0f);
						cubeTransforms[i].scale = ew::Vec3(1.0f);
					}
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
