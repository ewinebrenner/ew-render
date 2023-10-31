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
#include <ew/ewMath/transformations.h>

void processInput(GLFWwindow* window);
void onCursorMoved(GLFWwindow* window, double xpos, double ypos);
void onMouseButtonPressed(GLFWwindow* window, int button, int action, int mods);
void onFramebufferResize(GLFWwindow* window, int width, int height);

int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

struct Camera {
	float fov = 60.0f;
	ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 10.0f);
	ew::Vec3 target = ew::Vec3(0.0f, 0.0f, 1.0f);
	bool orthographic = false;
	float orthographicHeight = 10.0f;

};
struct CameraController {
	float moveSpeed = 3.0f;
	float yaw = -90.0f;
	float pitch = 0.0f;
	double prevMouseX, prevMouseY;
	bool firstMouse = true;
	float mouseSensitivity = 0.1f;
};

const int NUM_CUBES = 8;
ew::Transform splineTransform;
ew::Transform sphereTransform;
ew::Transform cylinderTransform;
ew::Transform planeTransform;
ew::Transform torusTransform;

Camera camera;
CameraController cameraController;

float prevTime;
float deltaTime;

ew::Vec3 clipPlaneNormal = ew::Vec3(0, 1, 0);
ew::Vec3 clipPlaneOrigin = ew::Vec3(0, 0, 0);

struct Settings {
	bool drawAsPoints;
	bool wireFrame = true;
	const char* debugModeNames[5] = { "Normals", "UVs", "Texture", "Shaded", "Black"};
	int debugModeIndex = 4;
	int sphereSegments = 16;
	bool sphereMethod = 0;

	int cylinderSegments = 16;
	int planeSegments = 10;
	int torusNumRings = 32;
	int torusRingSegments = 32;
	float torusInnerRadius = 0.25f;
	float torusOuterRadius = 0.5f;

	ew::Vec3 bgColor = ew::Vec3(1.0f);
	ew::Vec3 lightDir = ew::Vec3(0, -1, 0);
	const char* textures[2] = { "Brick", "Earth" };
	int textureIndex = 0;
}settings;

void drawMesh(const ew::Shader& shader, const ew::Mesh& mesh, const ew::Transform& transform, bool drawPoints) {
	mesh.bind();
	shader.setMat4("_Model", transform.getModelMatrix());
	if (!drawPoints) {
		glDrawElements(GL_TRIANGLES, mesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
	}
	else {
		glDrawArrays(GL_POINTS, 0, mesh.getNumVertices());
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
	glfwSetFramebufferSizeCallback(window, onFramebufferResize);
	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

	glfwSwapInterval(1);
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glPointSize(5);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, settings.wireFrame ? GL_LINE : GL_FILL);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	ew::MeshData cubeMeshData, sphereMeshData, cylinderMeshData, planeMeshData, torusMeshData;
	ew::createCube(0.5f,&cubeMeshData);
	ew::createSphere(0.5f, settings.sphereSegments, &sphereMeshData);
	ew::createCylinder(1.0f, 0.5f, settings.cylinderSegments, &cylinderMeshData);
	ew::createPlane(1.0f, settings.planeSegments, &planeMeshData);
	ew::createTorus(settings.torusInnerRadius,settings.torusOuterRadius,settings.torusNumRings, settings.torusRingSegments, &torusMeshData);

	ew::Mesh cubeMesh, sphereMesh, cylinderMesh, planeMesh, torusMesh;
	cubeMesh.load(cubeMeshData);
	sphereMesh.load(sphereMeshData);
	cylinderMesh.load(cylinderMeshData);
	planeMesh.load(planeMeshData);
	torusMesh.load(torusMeshData);

	ew::Shader shader("assets/unlit.vert", "assets/unlit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/bricks_color.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);
	unsigned int earthTexture = ew::loadTexture("assets/earth_8k.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);

	sphereTransform.position = ew::Vec3(2.0f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(-2.0f, 0.0f, 0.0f);
	planeTransform.position = ew::Vec3(4.0f, -0.5f, 0.0f);
	torusTransform.position = ew::Vec3(6.0f, 0.0f, 0.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		processInput(window);

		glClearColor(settings.bgColor.x,settings.bgColor.y,settings.bgColor.z, 1.0f);
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

		ew::Mat4 viewProjection = projection * view;
		shader.use();
		shader.setMat4("_ViewProjection", viewProjection);
		shader.setVec3("_LightDir", settings.lightDir);
		shader.setInt("_DebugMode", settings.debugModeIndex);
		shader.setVec3("_ClipPlaneOrigin", clipPlaneOrigin);
		shader.setVec3("_ClipPlaneNormal", clipPlaneNormal);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, settings.textureIndex+1);

		shader.setInt("_Texture", 0);

		drawMesh(shader, cubeMesh, splineTransform, settings.drawAsPoints);
		drawMesh(shader, sphereMesh, sphereTransform, settings.drawAsPoints);
		drawMesh(shader, cylinderMesh, cylinderTransform, settings.drawAsPoints);
		drawMesh(shader, planeMesh, planeTransform, settings.drawAsPoints);
		drawMesh(shader, torusMesh, torusTransform, settings.drawAsPoints);

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
			ImGui::ColorEdit3("BG Color", &settings.bgColor.x);
			ImGui::Combo("Texture", &settings.textureIndex, settings.textures, IM_ARRAYSIZE(settings.textures));
			ImGui::Combo("Shading Mode", &settings.debugModeIndex, settings.debugModeNames, IM_ARRAYSIZE(settings.debugModeNames));
			ImGui::Checkbox("Point Drawing", &settings.drawAsPoints);
			if (ImGui::Checkbox("Wireframe", &settings.wireFrame)) {
				glPolygonMode(GL_FRONT_AND_BACK, settings.wireFrame ? GL_LINE : GL_FILL);
			}
			ImGui::SliderFloat3("Light Dir", &settings.lightDir.x, -1.0f, 1.0f);
			ImGui::SliderFloat3("Clip Plane Normal", &clipPlaneNormal.x, -1.0f, 1.0f);
			ImGui::DragFloat3("Clip Plane Origin", &clipPlaneOrigin.x, 0.1f);

			if (ImGui::CollapsingHeader("Bonus - Dynamic")) {
				bool sphereChanged = false;
				sphereChanged |= (ImGui::DragInt("Sphere Segments", &settings.sphereSegments, 1, 3, 512) && settings.sphereSegments >= 3);
				sphereChanged |= ImGui::Checkbox("Sphere Method", &settings.sphereMethod);
				if (sphereChanged) {
					if (settings.sphereMethod == 0)
						ew::createSphere(0.5f, settings.sphereSegments, &sphereMeshData);
					else
						ew::createSphereOld(0.5f, settings.sphereSegments, &sphereMeshData);
					sphereMesh.load(sphereMeshData);
				}
				if (ImGui::DragInt("Cylinder Segments", &settings.cylinderSegments, 1, 3, 512) && settings.cylinderSegments >= 3) {
					ew::createCylinder(1.0f, 0.5f, settings.cylinderSegments, &cylinderMeshData);
					cylinderMesh.load(cylinderMeshData);
				}
				if (ImGui::DragInt("Plane Segments", &settings.planeSegments, 1, 1, 512) && settings.planeSegments >= 1) {
					ew::createPlane(1.0f, settings.planeSegments, &planeMeshData);
					planeMesh.load(planeMeshData);
				}
				bool torusChanged = false;
				torusChanged |= (ImGui::DragFloat("Torus Inner Radius", &settings.torusInnerRadius, 0.05f));
				torusChanged |= (ImGui::DragFloat("Torus Outer Radius", &settings.torusOuterRadius, 0.05f));
				torusChanged |= (ImGui::DragInt("Torus Num Rings", &settings.torusNumRings, 1, 3, 512));
				torusChanged |= (ImGui::DragInt("Torus Ring Segments", &settings.torusRingSegments, 1, 3, 512));
				if (torusChanged) {
					ew::createTorus(settings.torusInnerRadius, settings.torusOuterRadius, settings.torusNumRings, settings.torusRingSegments, &torusMeshData);
					torusMesh.load(torusMeshData);
				}
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
void onFramebufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}