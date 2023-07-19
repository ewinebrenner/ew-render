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

void processInput(GLFWwindow* window);
void onCursorMoved(GLFWwindow* window, double xpos, double ypos);
void onMouseButtonPressed(GLFWwindow* window, int button, int action, int mods);

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

struct Camera {
	float fov = 60.0f;
	ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 10.0f);
	ew::Vec3 target = ew::Vec3(0.0f, 0.0f, 1.0f);
	bool orthographic = false;
	float orthographicHeight = 10.0f;
	
};
struct CameraController {
	float moveSpeed = 5.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	double prevMouseX, prevMouseY;
	bool firstMouse = true;
	float mouseSensitivity = 0.1f;
};

const int NUM_CUBES = 8;
ew::Transform cubeTransforms[NUM_CUBES];
Camera camera;
CameraController cameraController;

float prevTime;
float deltaTime;

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
	glfwSetCursorPosCallback(window, onCursorMoved);
	glfwSetMouseButtonCallback(window, onMouseButtonPressed);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	ew::MeshData cubeMeshData;
	ew::createCube(0.5f,&cubeMeshData);
	ew::Mesh cubeMesh;
	cubeMesh.load(cubeMeshData);

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
		processInput(window);

		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeMesh.bind();

		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		deltaTime = time - prevTime;
		prevTime = time;

		//Update camera forward vector
		float yawRad = ew::Radians(cameraController.yaw);
		float pitchRad = ew::Radians(cameraController.pitch);

		ew::Vec3 forward;
		forward.x = cosf(yawRad) * cosf(pitchRad);
		forward.y = sinf(pitchRad);
		forward.z = sinf(yawRad) * cosf(pitchRad);
		forward = ew::Normalize(forward);

		camera.target = camera.position + forward;

		//Construct View and Projection
		ew::Mat4 view = ew::LookAtMatrix(camera.position, camera.target, ew::Vec3(0, 1, 0));
		ew::Mat4 projection = camera.orthographic ? 
			ew::OrthographicMatrix(camera.orthographicHeight, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f,1000.0f) :
			ew::PerspectiveMatrix(ew::Radians(camera.fov), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.01f, 1000.0f);

		glUseProgram(shader);
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
				ImGui::DragFloat("Move speed", &cameraController.moveSpeed, 0.1f);
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

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
		return;
	}

	ew::Vec3 forward = ew::Normalize(camera.target - camera.position);
	ew::Vec3 right = ew::Normalize(ew::Cross(forward, ew::Vec3(0, 1, 0)));
	ew::Vec3 up = ew::Normalize(ew::Cross(forward,right));
	if (glfwGetKey(window,GLFW_KEY_W)) {
		camera.position += forward * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera.position -= forward * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera.position += right * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera.position -= right * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		camera.position += up * cameraController.moveSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		camera.position -= up * cameraController.moveSpeed * deltaTime;
	}


}

void onCursorMoved(GLFWwindow* window, double xpos, double ypos)
{
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		return;

	if (cameraController.firstMouse) {
		cameraController.prevMouseX = xpos;
		cameraController.prevMouseY = ypos;
		cameraController.firstMouse = false;
	}
	float deltaX = xpos - cameraController.prevMouseX;
	float deltaY = ypos - cameraController.prevMouseY;
	cameraController.yaw += deltaX * cameraController.mouseSensitivity;
	cameraController.pitch += -deltaY * cameraController.mouseSensitivity;
	if (cameraController.pitch < -89.9f) {
		cameraController.pitch = -89.9f;
	}
	else if (cameraController.pitch > 89.9f) {
		cameraController.pitch = 89.9f;
	}

	cameraController.prevMouseX = xpos;
	cameraController.prevMouseY = ypos;
}

void onMouseButtonPressed(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cameraController.firstMouse = true;
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	
}
