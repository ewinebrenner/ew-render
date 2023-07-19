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
#include <ew/transformations.h>

void processInput(GLFWwindow* window);
void onCursorMoved(GLFWwindow* window, double xpos, double ypos);
void onMouseButtonPressed(GLFWwindow* window, int button, int action, int mods);

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

struct Transform {
	ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
	ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Degrees
	ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
};

ew::Mat4 getModelMatrix(const Transform& transform) {
	return ew::TranslationMatrix(transform.position.x, transform.position.y, transform.position.z)
		* ew::RotateYMatrix(ew::Radians(transform.rotation.y))
		* ew::RotateXMatrix(ew::Radians(transform.rotation.x))
		* ew::RotateZMatrix(ew::Radians(transform.rotation.z))
		* ew::ScaleMatrix(transform.scale.x, transform.scale.y, transform.scale.z);
}

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
Transform cubeTransform;
Transform sphereTransform;
Transform cylinderTransform;
Transform planeTransform;

Camera camera;
CameraController cameraController;

float prevTime;
float deltaTime;

struct Settings {
	const char* drawModeNames[2] = { "Triangles", "Points" };
	int glDrawModes[2] = { GL_TRIANGLES,GL_POINTS };
	int drawModeIndex = 0;
	bool wireFrame = false;

	int sphereSegments = 16;
	int cylinderSegments = 16;
	int planeSegments = 10;

}settings;

void drawMesh(unsigned int shader, const ew::Mesh& mesh, const Transform& transform, int drawMode) {
	mesh.bind();

	glUniformMatrix4fv(glGetUniformLocation(shader, "_Model"), 1, GL_FALSE, &getModelMatrix(transform)[0][0]);
	if (drawMode == GL_TRIANGLES) {
		glDrawElements(drawMode, mesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
	}
	else {
		glDrawArrays(drawMode, 0, mesh.getNumVertices());
	}
}

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
	glPointSize(3);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	ew::MeshData cubeMeshData, sphereMeshData, cylinderMeshData, planeMeshData;
	ew::createCube(0.5f,&cubeMeshData);
	ew::createSphere(0.5f, settings.sphereSegments, &sphereMeshData);
	ew::createCylinder(1.0f, 0.5f, settings.cylinderSegments, &cylinderMeshData);
	ew::createPlane(1.0f, settings.planeSegments, &planeMeshData);

	ew::Mesh cubeMesh, sphereMesh, cylinderMesh, planeMesh;
	cubeMesh.load(cubeMeshData);
	sphereMesh.load(sphereMeshData);
	cylinderMesh.load(cylinderMeshData);
	planeMesh.load(planeMeshData);

	std::string vertexShaderSource = ew::loadShaderSourceFromFile("assets/unlit.vert");
	std::string fragmentShaderSource = ew::loadShaderSourceFromFile("assets/unlit.frag");
	unsigned int shader = ew::createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	sphereTransform.position = ew::Vec3(2.0f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(-2.0f, 0.0f, 0.0f);
	planeTransform.position = ew::Vec3(4.0f, 0.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		processInput(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		int drawMode = settings.glDrawModes[settings.drawModeIndex];
		drawMesh(shader, cubeMesh, cubeTransform, drawMode);
		drawMesh(shader, sphereMesh, sphereTransform, drawMode);
		drawMesh(shader, cylinderMesh, cylinderTransform, drawMode);
		drawMesh(shader, planeMesh, planeTransform, drawMode);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::Checkbox("Wireframe", &settings.wireFrame)) {
				glPolygonMode(GL_FRONT_AND_BACK, settings.wireFrame ? GL_LINE : GL_FILL);
			}

			ImGui::Combo("Draw mode", &settings.drawModeIndex, settings.drawModeNames, IM_ARRAYSIZE(settings.drawModeNames));

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
			
			if (ImGui::CollapsingHeader("Cube")) {
				ImGui::DragFloat3("Position", &cubeTransform.position.x, 0.1f);
				ImGui::DragFloat3("Rotation", &cubeTransform.rotation.x, 1.0f);
				ImGui::DragFloat3("Scale", &cubeTransform.scale.x, 0.1f);
				if (ImGui::Button("Reset")) {
					cubeTransform.position = ew::Vec3(0.0f);
					cubeTransform.rotation = ew::Vec3(0.0f);
					cubeTransform.scale = ew::Vec3(1.0f);
				}
			}
			if (ImGui::DragInt("Sphere Segments", &settings.sphereSegments, 1, 3, 512)) {
				ew::createSphere(0.5f, settings.sphereSegments, &sphereMeshData);
				sphereMesh.load(sphereMeshData);
			}
			if (ImGui::DragInt("Cylinder Segments", &settings.cylinderSegments, 1, 3, 512)) {
				ew::createCylinder(1.0f,0.5f,settings.cylinderSegments, &cylinderMeshData);
				cylinderMesh.load(cylinderMeshData);
			}
			if (ImGui::DragInt("Plane Segments", &settings.planeSegments, 1, 1, 512)) {
				ew::createPlane(1.0f, settings.planeSegments, &planeMeshData);
				planeMesh.load(planeMeshData);
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
