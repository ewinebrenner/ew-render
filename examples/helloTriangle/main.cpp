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
#include <ew/Font.h>
#include <ew/TextRenderer.h>
#include <ew/SpriteRenderer.h>

void on_mouse_button_pressed(GLFWwindow* window, int button, int action, int mods);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
bool show_demo_window = false;
float prevTime, currTime;

ew::Camera camera = ew::Camera();
ew::FlyCamController cameraController = ew::FlyCamController(&camera);

double prevMouseX, prevMouseY;
bool firstMouse = true;
bool mouseUnlocked = false;
float mouseSensitivity = 0.1f;
bool debugDrawText = false;

char displayText[128];
glm::vec4 textColor = glm::vec4(238.0f / 256.f, 101.0f / 256.f, 126.0f / 256.f, 1.0f);
glm::vec3 textOutlineColor = glm::vec3(58 / 256.f, 32 / 256.f, 76 / 256.f);
float textThickness = 0.7;
float textOutlineThickness = 0.1;
glm::vec2 textPos = glm::vec2(64);
float textScale = 2.0;

struct SpriteSettings {
	glm::vec4 color = glm::vec4(1.0f);
	glm::vec2 scale = glm::vec2(1.0f);
	glm::vec2 origin = glm::vec2(0.5f);
	float rotation = 0.0f;
}spriteSettings;

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
	glfwSetMouseButtonCallback(window, on_mouse_button_pressed);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	printf("successful!\n");

	printf("Loading fonts...");
	
	const char* robotoFontPath = "assets/Roboto-Regular.ttf";
	const char* dmitriFontPath = "assets/DIMITRI_.TTF";
	const char* chewyFontPath = "assets/Chewy-Regular.ttf";

	ew::Font font_roboto(chewyFontPath);

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

	ew::Shader textShader;
	textShader.attach(ew::ShaderStage(ew::ShaderType::VERTEX, "assets/text.vert"));
	textShader.attach(ew::ShaderStage(ew::ShaderType::FRAGMENT, "assets/text.frag"));
	textShader.link();

	ew::TextRenderer textRenderer = ew::TextRenderer(&font_roboto);

	//Sprite shader, used for drawing atlased sprites
	ew::Shader spriteShader;
	{
		ew::ShaderStage vertexShader(ew::ShaderType::VERTEX);
		vertexShader.loadSourceFromFile("assets/sprite.vert");
		ew::ShaderStage fragmentShader(ew::ShaderType::FRAGMENT);
		fragmentShader.loadSourceFromFile("assets/sprite.frag");
		spriteShader.attach(vertexShader);
		spriteShader.attach(fragmentShader);
		spriteShader.link();
	}

	ew::Texture spriteAtlas = ew::Texture("assets/player_sheet.png");
	spriteAtlas.setFilter(ew::FilterMode::Nearest);

	ew::SpriteRenderer spriteRenderer;

	ew::Shader shader;
	{
		ew::ShaderStage vertexShader(ew::ShaderType::VERTEX);
		vertexShader.loadSourceFromFile("assets/unlit.vert");
		ew::ShaderStage fragmentShader(ew::ShaderType::FRAGMENT);
		fragmentShader.loadSourceFromFile("assets/unlit.frag");
		shader.attach(vertexShader);
		shader.attach(fragmentShader);
		shader.link();
	}


	ew::Material stoneMaterial(&shader);
	stoneMaterial.setTexture("_Texture", &tex_paving_stones_color);
	stoneMaterial.setTexture("_NormalMap", &tex_paving_stones_normal);

	ew::Material brickMaterial(&shader);
	brickMaterial.setTexture("_Texture", &tex_bricks_color);
	brickMaterial.setTexture("_NormalMap", &tex_bricks_normal);

	ew::Material* material = &brickMaterial;
	ew::Transform cubeTransform = ew::Transform();

	//Hide cursor at start
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Rendering config
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//Camera settings
	float camMoveSpeed = 20.0f;
	camera.getTransform()->translate(glm::vec3(0, 0, 10));
	camera.setFov(60.0f);
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);
	cameraController.addYaw(180);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		prevTime = currTime;
		currTime = (float)glfwGetTime();
		float deltaTime = currTime - prevTime;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}

		if (!mouseUnlocked) {
			//Get cam rotation deltas from mouse position
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);
			if (firstMouse) {
				prevMouseX = mouseX;
				prevMouseY = mouseY;
				firstMouse = false;
			}

			float pitchDelta = (mouseY - prevMouseY) * mouseSensitivity;
			float yawDelta = (prevMouseX - mouseX) * mouseSensitivity;

			prevMouseX = mouseX;
			prevMouseY = mouseY;

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

		}
		
		//START DRAWING
		glClearColor(128.0f/256,228.0f/256,254.0f/256, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		cubeTransform.rotate(deltaTime, glm::vec3(0, 1, 0));

		//Draw 3D Geometry
		material->use();
		material->setMat4("_Model", cubeTransform.localToWorld());
		material->setMat4("_View", camera.getViewMatrix());
		material->setMat4("_Projection", camera.getProjectionMatrix());
		material->updateUniforms();
		cubeModel.draw();


		//Draw sprites
		glm::mat4 spriteMVP = camera.getProjectionMatrix() * camera.getViewMatrix() * glm::mat4(1);
		//Clear out vertex buffer
		spriteRenderer.begin(&spriteShader, spriteMVP);

		//Send a bunch of individual "sprites" into vertex buffer
		int numSprites = 100;
		int numCol = 10;

		for (size_t i = 0; i < numSprites; i++)
		{
			float t = (float)i / numSprites;
			/*glm::vec4 color(
				(float)(i % numCol) / numCol,
				(float)(i / numCol) / (numSprites / numCol),
				1.0f,
				1.0f
			);*/
			glm::vec3 position = glm::vec3((i % numCol) * 1.5f, (i / numCol) * 1.5f,t);
			//position.y += glm::sin(position.x + currTime);
			glm::vec4 sourceRect = glm::vec4(32 * (i % 2) + 32 * ((int)currTime % 2), 0.0f, 32.0f, 32.0f);
			glm::vec4 color = spriteSettings.color;
			glm::vec2 scale = spriteSettings.scale;
			float rotation = spriteSettings.rotation;
			glm::vec2 origin = spriteSettings.origin;
			spriteRenderer.draw(&spriteAtlas, sourceRect, position, color, scale, rotation, origin);
		}

		//Send vertex buffer to GPU and draw
		spriteRenderer.end();

		textShader.setInt("_DEBUG", debugDrawText);
		textShader.setFloat("_Time", currTime);
		textShader.setFloat("_Thickness", textThickness);
		textShader.setFloat("_OutlineThickness", textOutlineThickness);
		textShader.setVec3("_OutlineColor", textOutlineColor);

		textRenderer.draw(std::string(displayText), &textShader, textColor, textPos.x,textPos.y,textScale);

		//DRAW IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		ImGui::Checkbox("Debug Draw Text", &debugDrawText);
		ImGui::InputText("Text", displayText, 256);
		ImGui::DragFloat2("Text Pos", &textPos.x);
		ImGui::SliderFloat("Text Scale", &textScale, 0.0f, 5.0f);
		ImGui::ColorEdit4("Text Color", &textColor.r);
		ImGui::ColorEdit3("Text Outline Color", &textOutlineColor.r);
		ImGui::SliderFloat("Text Thickness", &textThickness, 0.0f, 1.0f);
		ImGui::SliderFloat("Text Outline Thickness", &textOutlineThickness, 0.0f, 1.0f);
		ImGui::End();

		ImGui::Begin("Sprite");
		ImGui::ColorEdit4("Color", &spriteSettings.color.r);
		ImGui::DragFloat2("Scale", &spriteSettings.scale.r, 0.1f, -5.0f, 5.0f);
		ImGui::SliderFloat2("Origin",&spriteSettings.origin.r, 0.0f, 1.0f);
		ImGui::SliderFloat("Rotation",&spriteSettings.rotation, 0.0f, 360.0f);
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
