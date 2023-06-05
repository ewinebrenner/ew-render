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

const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 960;
bool show_demo_window = false;
float prevTime, currTime;
void on_mouse_button_pressed(GLFWwindow* window, int button, int action, int mods);
void updateCamera(GLFWwindow* window, ew::FlyCamController* cameraController, float camMoveSpeed, float deltaTime);

ew::Camera camera = ew::Camera();
ew::FlyCamController cameraController = ew::FlyCamController(&camera);

double prevMouseX, prevMouseY;
bool firstMouse = true;
float mouseSensitivity = 0.1f;

struct MeshData {
	std::vector<ew::Vertex> vertices;
	std::vector<unsigned int> indices;
};

void createPlane(float width, float height, unsigned int subdivisions, MeshData* mesh) {
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
	for (unsigned int i = 0; i < subdivisions; i++)
	{
		for (unsigned int j = 0; j < subdivisions; j++)
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
	float frequency = 0.5;
	float amplitude = 8;
	float textureTiling = 10;
};
glm::vec3 fogColor = glm::vec3(0.5, 0.6, 0.7);
glm::vec3 bgColor = fogColor * 0.9f;
float fogDensity = 0.01f;

TerrainSettings terrainSettings;
bool mouseUnlocked = false;
float camMoveSpeed = 20.0f;

struct SkySettings {
	glm::vec3 topColor = glm::vec3(0.3, 0.5, 0.9);
	glm::vec3 bottomColor = glm::vec3(0.6, 0.8, 1.0);
	glm::vec4 cloudColor = glm::vec4(1.0, 1.0, 1.0, 0.5);
	glm::vec3 lightDir = glm::vec3(0.0, -1.0, 0.0);
	glm::vec3 lightDiffuse = glm::vec3(1.0, 0.9, 0.8);
	glm::vec3 lightAmbient = glm::vec3(0.9, 0.8, 1.0);
	float sunSize = 512; //exp
}skySettings;

int main() {
	printf("Initializing...");
	assert(glfwInit());
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}
	glfwWindowHint(GLFW_SAMPLES, 4);
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

	ew::Model sphereModel;
	sphereModel.loadFromFile("assets/sphere.obj");
	ew::Model planeModel;
	planeModel.loadFromFile("assets/plane.obj");

	MeshData terrainMeshData;
	createPlane(100.0f, 100.0f, 100, &terrainMeshData);

	ew::Mesh terrainMesh;
	terrainMesh.load(terrainMeshData.vertices, terrainMeshData.indices);

	ew::Shader terrainShader;
	{
		terrainShader.attach(ew::ShaderStage(ew::ShaderType::VERTEX, "assets/terrain.vert"));
		terrainShader.attach(ew::ShaderStage(ew::ShaderType::GEOMETRY, "assets/terrain.geom"));
		terrainShader.attach(ew::ShaderStage(ew::ShaderType::FRAGMENT, "assets/terrain.frag"));
		terrainShader.link();
	}

	ew::Material terrainMaterial(&terrainShader);
	terrainMaterial.setTexture("_HeightMap", &tex_perlinNoise);
	terrainMaterial.setTexture("_Albedo", &tex_rock);
	
	ew::Shader skyboxShader; 
	{
		skyboxShader.attach(ew::ShaderStage(ew::ShaderType::VERTEX, "assets/skybox.vert"));
		skyboxShader.attach(ew::ShaderStage(ew::ShaderType::FRAGMENT, "assets/skybox.frag"));
		skyboxShader.link();
	}

	ew::Material skyboxMaterial(&skyboxShader);
	skyboxMaterial.setTexture("_NoiseTex", &tex_perlinNoise);

	ew::Shader cloudShader; 
	{
		cloudShader.attach(ew::ShaderStage(ew::ShaderType::VERTEX, "assets/clouds.vert"));
		cloudShader.attach(ew::ShaderStage(ew::ShaderType::FRAGMENT, "assets/clouds.frag"));
		cloudShader.link();
	}

	ew::Material cloudMaterial(&cloudShader);
	cloudMaterial.setTexture("_NoiseTex", &tex_perlinNoise);

	ew::Transform cubeTransform = ew::Transform();
	ew::Transform cloudTransform = ew::Transform();
	cloudTransform.scale = glm::vec3(1000);
	cloudTransform.position = glm::vec3(500, 50, 500);

	//Hide cursor at start
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Rendering config
	glEnable(GL_MULTISAMPLE); //Anti-aliasing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	

	//Camera settings
	
	camera.getTransform()->translate(glm::vec3(0, 5, -10));
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

		//UPDATE
		updateCamera(window, &cameraController,camMoveSpeed,deltaTime);

		//RENDER
		glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 viewMatrix = camera.getViewMatrix();
		glm::mat4 projMatrix = camera.getProjectionMatrix();


		//Draw terrain
		glCullFace(GL_BACK);
		glDisable(GL_BLEND);

		terrainMaterial.use();
		terrainMaterial.setFloat("_Time", currTime);
		terrainMaterial.setMat4("_Model", cubeTransform.localToWorld());
		terrainMaterial.setMat4("_View", viewMatrix);
		terrainMaterial.setMat4("_Projection", projMatrix);
		terrainMaterial.setFloat("_Frequency", terrainSettings.frequency);
		terrainMaterial.setFloat("_Amplitude", terrainSettings.amplitude);
		terrainMaterial.setFloat("_TextureTiling", terrainSettings.textureTiling);
		terrainMaterial.setFloat("_TextureHeights[0]", terrainSettings.amplitude * 0.1f);
		terrainMaterial.setFloat("_TextureHeights[1]", terrainSettings.amplitude * 0.3f);
		terrainMaterial.setFloat("_TextureHeights[2]", terrainSettings.amplitude * 0.5f);
		terrainMaterial.setTexture("_Textures[0]", &tex_rock);
		terrainMaterial.setTexture("_Textures[1]", &tex_grass);
		terrainMaterial.setTexture("_Textures[2]", &tex_snow);
		terrainMaterial.setFloat("_FogDensity", fogDensity);
		terrainMaterial.setVec3("_CameraPos", camera.getTransform()->position);
		terrainMaterial.setVec3("_FogColor", fogColor);
		terrainMaterial.setVec3("_Light.direction", skySettings.lightDir);
		terrainMaterial.setVec3("_Light.diffuseColor", skySettings.lightDiffuse * 0.9f);
		terrainMaterial.setVec3("_Light.ambientColor", skySettings.lightAmbient * 0.1f);
		
		terrainMaterial.updateUniforms();
		terrainMesh.draw();

		//Draw sky sphere
		glCullFace(GL_FRONT);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		skyboxMaterial.use();
		skyboxMaterial.setMat4("_Model", glm::mat4(1));
		skyboxMaterial.setMat4("_View", glm::mat4(glm::mat3(viewMatrix))); //Remove translation from view matrix
		skyboxMaterial.setMat4("_Projection", projMatrix);
		skyboxMaterial.setVec3("_BottomColor", skySettings.bottomColor);
		skyboxMaterial.setVec3("_TopColor", skySettings.topColor);
		skyboxMaterial.setVec3("_LightDir", skySettings.lightDir);
		skyboxMaterial.setFloat("_SunSize", skySettings.sunSize);
		skyboxMaterial.updateUniforms();
		sphereModel.draw();

		glDepthMask(GL_TRUE);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);

		//Draw cloud layer
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glCullFace(GL_FRONT);
		cloudMaterial.use();
		cloudMaterial.setFloat("_Time", currTime);
		cloudMaterial.setMat4("_Model", cloudTransform.localToWorld());
		cloudMaterial.setMat4("_View", viewMatrix);
		cloudMaterial.setMat4("_Projection", projMatrix);
		cloudMaterial.setVec4("_Color", skySettings.cloudColor);
		cloudMaterial.updateUniforms();
		planeModel.draw();


		//DRAW IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		if (ImGui::SliderFloat3("Light Dir", &skySettings.lightDir.r, -1.0, 1.0)) {
			skySettings.lightDir = glm::normalize(skySettings.lightDir);
		}
		
		ImGui::ColorEdit3("Sky Bottom Color", &skySettings.bottomColor.r);
		ImGui::ColorEdit3("Sky Top Color", &skySettings.topColor.r);
		ImGui::ColorEdit4("Cloud Color", &skySettings.cloudColor.r);
		ImGui::ColorEdit3("Fog Color", &fogColor.r);
		ImGui::DragFloat("Fog Density", &fogDensity, 0.001f, 0.0f, 0.1f);
		ImGui::DragFloat("Sun size", &skySettings.sunSize, 1.0f, 2.0f, 1024.0f);

		ImGui::DragFloat("Frequency", &terrainSettings.frequency, 0.05f, 0.0f, 10.0f);
		ImGui::DragFloat("Amplitude", &terrainSettings.amplitude, 0.05f, 0.0f, 100.0f);
		ImGui::DragFloat("TextureTiling", &terrainSettings.textureTiling, 0.01f, 0.0f, 10.0f);
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

void updateCamera(GLFWwindow* window, ew::FlyCamController* cameraController, float camMoveSpeed, float deltaTime)
{
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

		pitchDelta = (float)(mouseY - prevMouseY) * mouseSensitivity;
		yawDelta = (float)(prevMouseX - mouseX) * mouseSensitivity;

		prevMouseX = mouseX;
		prevMouseY = mouseY;
	}

	cameraController->addPitch(pitchDelta);
	cameraController->addYaw(yawDelta);
	cameraController->updateCamRotation();

	//Camera movement
	float moveDelta = camMoveSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D)) {
		cameraController->moveRight(-moveDelta);
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		cameraController->moveRight(moveDelta);
	}
	if (glfwGetKey(window, GLFW_KEY_W)) {
		cameraController->moveForward(moveDelta);
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		cameraController->moveForward(-moveDelta);
	}
	if (glfwGetKey(window, GLFW_KEY_E)) {
		cameraController->moveUp(moveDelta);
	}
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		cameraController->moveUp(-moveDelta);
	}
}
