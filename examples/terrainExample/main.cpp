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
#include <ew/Transform.h>
#include <ew/FlyCamController.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
bool show_demo_window = false;
float prevTime, currTime;
void on_mouse_button_pressed(GLFWwindow* window, int button, int action, int mods);

ew::Camera camera = ew::Camera();
ew::FlyCamController cameraController = ew::FlyCamController(&camera);

double prevMouseX, prevMouseY;
bool firstMouse = true;
float mouseSensitivity = 0.1f;

struct MeshData {
	std::vector<ew::Vertex> vertices;
	std::vector<unsigned int> indices;
};

void createPlane(float width, float height, int subdivisions, MeshData* mesh) {
	mesh->vertices.clear();
	mesh->indices.clear();
	//mesh->vertices.resize((subdivisions + 1) * (subdivisions + 1));

	//Vertices
	for (size_t i = 0; i <= subdivisions; i++)
	{
		for (size_t j = 0; j <= subdivisions; j++)
		{
			float u = ((float)i / subdivisions);
			float v = ((float)j / subdivisions);
			float x = width * u;
			float z = height * v;
			ew::Vertex vert = ew::Vertex();
			vert.position = glm::vec3(x, 0, z);
			vert.uv = glm::vec2(u, v);
			vert.normal = glm::vec3(0, 1, 0);
			vert.tangent = glm::vec3(1, 0, 0);
			mesh->vertices.push_back(vert);
		}
	}
	//Indices
	for (size_t i = 0; i < subdivisions; i++)
	{
		for (size_t j = 0; j < subdivisions; j++)
		{
			unsigned int startIndex = i + j * (subdivisions + 1);
			mesh->indices.push_back(startIndex);
			mesh->indices.push_back(startIndex + 1);
			mesh->indices.push_back(startIndex + subdivisions + 2);
			mesh->indices.push_back(startIndex + subdivisions + 2);
			mesh->indices.push_back(startIndex + subdivisions + 1);
			mesh->indices.push_back(startIndex);
		}
	}
}

struct TerrainSettings {
	float frequency = 1;
	float amplitude = 10;
	float textureTiling = 0.5;
};

TerrainSettings terrainSettings;
bool mouseUnlocked = false;

int main() {
	printf("Initializing...");
	assert(glfwInit());
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGLExample", NULL, NULL);
	glfwSetMouseButtonCallback(window, on_mouse_button_pressed);
	assert(window != nullptr);
	glfwMakeContextCurrent(window);
	assert(glewInit() == GLEW_OK);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	printf("successful!\n");

	printf("Loading models...");

	ew::Texture tex_perlinNoise("assets/perlinNoise.png");
	ew::Texture tex_rock("assets/rock_albedo.jpg");
	ew::Texture tex_grass("assets/grass_albedo.jpg");
	ew::Texture tex_snow("assets/snow_albedo.jpg");

	ew::Shader shader("assets/terrain.vert", "assets/terrain.frag");

	MeshData terrainMeshData;
	createPlane(100.0f, 100.0f, 100, &terrainMeshData);

	ew::Mesh terrainMesh;
	terrainMesh.load(terrainMeshData.vertices, terrainMeshData.indices);

	ew::Material terrainMaterial(&shader);
	terrainMaterial.setTexture("_HeightMap", &tex_perlinNoise);
	terrainMaterial.setTexture("_Albedo", &tex_rock);

	ew::Material* material = &terrainMaterial;
	ew::Transform cubeTransform = ew::Transform();

	//Hide cursor at start
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Rendering config
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Camera settings
	float camMoveSpeed = 20.0f;
	camera.getTransform()->translate(glm::vec3(0, 0, -10));
	camera.setFov(60.0f);
	camera.setFarPlane(1000.0f);
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		prevTime = currTime;
		currTime = (float)glfwGetTime();
		float deltaTime = currTime - prevTime;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}

		float pitchDelta = 0;
		float yawDelta = 0;

		if (!mouseUnlocked) {
			//Get cam rotation deltas from mouse position
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			if (firstMouse) {
				prevMouseX = mouseX;
				prevMouseY = mouseY;
				firstMouse = false;
			}

			pitchDelta = (mouseY - prevMouseY) * mouseSensitivity;
			yawDelta = (prevMouseX - mouseX) * mouseSensitivity;

			prevMouseX = mouseX;
			prevMouseY = mouseY;
		}

		cameraController.addPitch(pitchDelta);
		cameraController.addYaw(yawDelta);
		cameraController.updateCamRotation();

		//Camera movement
		float moveDelta = camMoveSpeed * deltaTime;
		if (glfwGetKey(window, GLFW_KEY_D)) {
			cameraController.moveRight(-moveDelta);
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			cameraController.moveRight(moveDelta);
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			cameraController.moveForward(moveDelta);
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			cameraController.moveForward(-moveDelta);
		}
		if (glfwGetKey(window, GLFW_KEY_E)) {
			cameraController.moveUp(moveDelta);
		}
		if (glfwGetKey(window, GLFW_KEY_Q)) {
			cameraController.moveUp(-moveDelta);
		}

		//START DRAWING
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		material->setMat4("_Model", cubeTransform.localToWorld());
		material->setMat4("_View", camera.getViewMatrix());
		material->setMat4("_Projection", camera.getProjectionMatrix());
		material->setFloat("_Frequency", terrainSettings.frequency);
		material->setFloat("_Amplitude", terrainSettings.amplitude);
		material->setFloat("_TextureTiling", terrainSettings.textureTiling);
		material->setFloat("_TextureHeights[0]", terrainSettings.amplitude * 0.1);
		material->setFloat("_TextureHeights[1]", terrainSettings.amplitude * 0.3);
		material->setFloat("_TextureHeights[2]", terrainSettings.amplitude * 0.5);
		material->setTexture("_Textures[0]", &tex_rock);
		material->setTexture("_Textures[1]", &tex_grass);
		material->setTexture("_Textures[2]", &tex_snow);

		material->updateUniforms();
		terrainMesh.draw();

		//DRAW IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		ImGui::Text("Hello");
		ImGui::DragFloat("Frequency", &terrainSettings.frequency, 0.05f, 0.0f, 10.0f);
		ImGui::DragFloat("Amplitude", &terrainSettings.amplitude, 0.05f, 0.0f, 100.0f);
		ImGui::DragFloat("TextureTiling", &terrainSettings.textureTiling, 0.01f, 0.0f, 1.0f);
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

void on_mouse_button_pressed(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (!mouseUnlocked) {
			mouseUnlocked = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			mouseUnlocked = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			firstMouse = true;
		}
	}
}
