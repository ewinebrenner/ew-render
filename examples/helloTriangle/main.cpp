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

void on_mouse_button_pressed(GLFWwindow* window, int button, int action, int mods);

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
	ew::Font font_roboto(robotoFontPath);

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

	ew::Shader shader;
	ew::ShaderStage vertexShader(ew::ShaderType::VERTEX);
	vertexShader.loadSourceFromFile("assets/unlit.vert");
	ew::ShaderStage fragmentShader(ew::ShaderType::FRAGMENT);
	fragmentShader.loadSourceFromFile("assets/unlit.frag");
	shader.attach(vertexShader);
	shader.attach(fragmentShader);
	shader.link();

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
	glCullFace(GL_BACK);

	//Camera settings
	float camMoveSpeed = 20.0f;
	camera.getTransform()->translate(glm::vec3(0, 0, -10));
	camera.setFov(60.0f);
	camera.setAspectRatio((float)SCREEN_WIDTH / SCREEN_HEIGHT);

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
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();

		//cubeTransform.rotate(deltaTime, glm::vec3(0, 1, 0));

		material->setMat4("_Model", cubeTransform.localToWorld());
		material->setMat4("_View", camera.getViewMatrix());
		material->setMat4("_Projection", camera.getProjectionMatrix());
		material->updateUniforms();
		cubeModel.draw();

		textShader.setInt("_DEBUG", debugDrawText);

		textRenderer.draw(&textShader,glm::vec4(1.0f,1.0f,0.0f,1.0f));

		//DRAW IMGUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Settings");
		ImGui::Checkbox("Debug Draw Text", &debugDrawText);
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
