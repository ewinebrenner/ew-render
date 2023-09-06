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
ew::Transform cubeTransform;
ew::Transform sphereTransform;
ew::Transform cylinderTransform;
ew::Transform planeTransform;

Camera camera;
CameraController cameraController;

struct Light {
	ew::Transform transform;
	ew::Vec4 color = ew::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct Material {
	ew::Vec4 color = ew::Vec4(0.8f);
	float ambientK = 0.2f;
	float diffuseK = 0.3f;
	float specularK = 0.5f;
	float shininess = 128.0f;
};

float prevTime;
float deltaTime;

Light light;
Material material;

ew::Vec3 controlPoints[4] = {
	{0.0, 0.0, 0.0 },
	{1.0, 1.0, 0.0 },
	{2.0, 0.0, 0.0 },
	{3.0, 1.0, 0.0 }
};
int splineSegments = 32;
float splineWidth = 0.3f;

void drawMesh(const ew::Shader& shader, const ew::Mesh& mesh, const ew::Transform& transform, int drawMode = GL_TRIANGLES) {
	mesh.bind();
	shader.setMat4("_Model", transform.getModelMatrix());
	if (drawMode == GL_TRIANGLES) {
		glDrawElements(drawMode, mesh.getNumIndices(), GL_UNSIGNED_INT, NULL);
	}
	else {
		glDrawArrays(drawMode, 0, mesh.getNumVertices());
	}
}

ew::Vec3 evaluateQuadraticBezier(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, float t) {
	ew::Vec3 p3 = ew::Lerp(p0, p1, t);
	ew::Vec3 p4 = ew::Lerp(p1, p2, t);
	return ew::Lerp(p3, p4, t);
}

ew::Vec3 evaluateCubicBezier(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, float t) {
	ew::Vec3 p4 = ew::Lerp(p0, p1, t);
	ew::Vec3 p5 = ew::Lerp(p1, p2, t);
	ew::Vec3 p6 = ew::Lerp(p2, p3, t);
	return evaluateQuadraticBezier(p4, p5, p6, t);
}
ew::Vec3 evaluateCubicBezierTangent(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, float t) {
	float oneMinusT = (1.0f - t);
	return 3.0f * oneMinusT * oneMinusT * (p1 - p0)
		+ 6.0f * t * oneMinusT * (p2 - p1)
		+ 3.0f * t * t * (p3 - p2);
}

void createCubicBezierMesh(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, int numSegments, ew::MeshData* mesh) {
	mesh->vertices.clear();
	mesh->indices.clear();
	for (size_t i = 0; i <= numSegments; i++)
	{
		float t = (float)i / numSegments;
		ew::Vertex v;
		v.pos = evaluateCubicBezier(p0, p1, p2, p3, t);
		v.normal = ew::Vec3(0, 0, 1);
		mesh->vertices.push_back(v);
	}
}

void createCubicBezierTrackMesh(ew::Vec3 p0, ew::Vec3 p1, ew::Vec3 p2, ew::Vec3 p3, int numSegments, float splineWidth, ew::MeshData* mesh) {
	mesh->vertices.clear();
	mesh->indices.clear();
	for (size_t i = 0; i <= numSegments; i++)
	{
		float t = (float)i / numSegments;
		ew::Vec3 centerPos = evaluateCubicBezier(p0, p1, p2, p3, t);
		ew::Vec3 tangent = evaluateCubicBezierTangent(p0, p1, p2, p3, t);
		ew::Vec3 right = ew::Normalize(ew::Cross(tangent, ew::Vec3(0, 1, 0)));
		ew::Vec3 normal = ew::Normalize(ew::Cross(right, tangent));

		ew::Vertex v0;
		v0.pos = centerPos + right * splineWidth;
		v0.normal = normal;
		v0.uv = ew::Vec2(0.0f, t);
		ew::Vertex v1;
		v1.pos = centerPos - right * splineWidth;
		v1.normal = normal;
		v1.uv = ew::Vec2(1.0f, t);
		mesh->vertices.push_back(v0);
		mesh->vertices.push_back(v1);
	}
	for (size_t i = 0; i < numSegments; i++)
	{
		int startIndex = i * 2;
		mesh->indices.push_back(startIndex);
		mesh->indices.push_back(startIndex + 2);
		mesh->indices.push_back(startIndex + 1);
		mesh->indices.push_back(startIndex + 1);
		mesh->indices.push_back(startIndex + 2);
		mesh->indices.push_back(startIndex + 3);
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
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	ew::MeshData splineMeshData, sphereMeshData;
	createCubicBezierTrackMesh(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], 32,splineWidth, &splineMeshData);


	ew::createSphere(0.5f, 32, &sphereMeshData);

	ew::Mesh splineMesh, sphereMesh;
	splineMesh.load(splineMeshData);
	sphereMesh.load(sphereMeshData);
	
	ew::Shader unlitShader("assets/unlit.vert", "assets/unlit.frag");
	ew::Shader litShader("assets/lit.vert", "assets/lit.frag");

	unsigned int texture = ew::loadTexture("assets/bricks_color.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR);

	//Set up scene
	sphereTransform.position = ew::Vec3(2.0f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(-2.0f, 0.0f, 0.0f);
	planeTransform.position = ew::Vec3(4.0f, 0.0f, 0.0f);

	light.transform.position = ew::Vec3(2.0f, 2.0f, 0.0f);
	light.transform.scale = ew::Vec3(0.3f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

		//Set uniforms
		litShader.use();
		litShader.setMat4("_ViewProjection", viewProjection);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, texture);
		litShader.setInt("_Texture", 0);

		litShader.setVec3("_EyePos", camera.position);
		litShader.setVec3("_Light.position", light.transform.position);
		litShader.setVec4("_Light.color", light.color);

		litShader.setVec4("_Material.color", material.color);
		litShader.setFloat("_Material.ambientK", material.ambientK);
		litShader.setFloat("_Material.diffuseK", material.diffuseK);
		litShader.setFloat("_Material.specularK", material.specularK);
		litShader.setFloat("_Material.shininess", material.shininess);

		drawMesh(litShader, splineMesh, cubeTransform, GL_TRIANGLES);

		unlitShader.use();
		unlitShader.setMat4("_ViewProjection", viewProjection);
		unlitShader.setVec4("_Color", light.color);
		drawMesh(unlitShader, sphereMesh, light.transform);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Light")) {
				ImGui::ColorEdit4("Color", &light.color.x);
				ImGui::DragFloat3("Position", &light.transform.position.x, 0.2f);
			}
			if (ImGui::CollapsingHeader("Material")) {
				ImGui::SliderFloat("Ambient K", &material.ambientK,0.0f,1.0f);
				ImGui::SliderFloat("Diffuse K", &material.diffuseK,0.0f,1.0f);
				ImGui::SliderFloat("Specular K", &material.specularK,0.0f,1.0f);
				ImGui::DragFloat("Shininess", &material.shininess);
			}

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
			if (ImGui::CollapsingHeader("Spline")) {
				bool dirty = false;
				dirty = dirty || (ImGui::DragInt("Segments", &splineSegments, 1, 3, 512));
				for (size_t i = 0; i < 4; i++)
				{
					ImGui::PushID(i);
					dirty = dirty || ImGui::DragFloat3(("P" + std::to_string(i)).c_str(), &controlPoints[i].x, 0.2f);
					ImGui::PopID();
				}
				dirty = dirty || (ImGui::DragFloat("Width", &splineWidth, 0.1f, 0.0f, 5.0f));
				if (dirty) {
					createCubicBezierTrackMesh(controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3], splineSegments, splineWidth,&splineMeshData);
					splineMesh.load(splineMeshData);
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
