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

#include <ew/external/stb_image.h>
#include <ew/ewMath/ewMath.h>
#include <ew/texture.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 1080;

struct Vertex {
	ew::Vec3 pos;
	ew::Vec3 normal;
	ew::Vec2 uv;
};

struct Mesh {
	Vertex* vertices;
	unsigned int numVertices;
	unsigned int* indices;
	unsigned int numIndices;
};

void createCubeFace(ew::Vec3 normal, float size, Mesh* mesh) {
	unsigned int startVertex = mesh->numVertices;
	ew::Vec3 a = ew::Vec3(normal.z, normal.x, normal.y); //U axis
	ew::Vec3 b = ew::Cross(normal, a); //V axis
	for (int i = 0; i < 4; i++)
	{
		int col = i % 2;
		int row = i / 2;

		ew::Vec3 pos = normal * size * 0.5f;
		//ew::Vec3 pos = ew::Vec3(0);
		pos -= (a + b) * size * 0.5f;
		pos +=(a * col + b * row) * size;
		Vertex* vertex = &mesh->vertices[mesh->numVertices];
		vertex->pos = pos;
		vertex->uv = ew::Vec2(col,row);
		vertex->normal = normal;
		mesh->numVertices++;
	}

	//Indices
	mesh->indices[mesh->numIndices++] = startVertex;
	mesh->indices[mesh->numIndices++] = startVertex + 1;
	mesh->indices[mesh->numIndices++] = startVertex + 3;
	mesh->indices[mesh->numIndices++] = startVertex + 3;
	mesh->indices[mesh->numIndices++] = startVertex + 2;
	mesh->indices[mesh->numIndices++] = startVertex;
}

Mesh* createCubeMesh(float size) {
	float halfSize = size;
	Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
	if (mesh == NULL)
		return NULL;
	mesh->vertices = (Vertex*)malloc(sizeof(Vertex) * 24);
	if (mesh->vertices == NULL) {
		free(mesh);
		return NULL;
	}
	mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * 36);
	if (mesh->indices == NULL) {
		free(mesh);
		free(mesh->vertices);
		return NULL;
	}

	mesh->numVertices = 0;
	mesh->numIndices = 0;
	createCubeFace(ew::Vec3{ 0.0f,0.0f,1.0f }, size, mesh); //Front
	createCubeFace(ew::Vec3{ 1.0f,0.0f,0.0f }, size, mesh); //Right
	createCubeFace(ew::Vec3{ 0.0f,1.0f,0.0f }, size, mesh); //Top
	createCubeFace(ew::Vec3{ -1.0f,0.0f,0.0f }, size, mesh); //Left
	createCubeFace(ew::Vec3{ 0.0f,-1.0f,0.0f }, size, mesh); //Bottom
	createCubeFace(ew::Vec3{ 0.0f,0.0f,-1.0f }, size, mesh); //Back
	return mesh;
}

std::string loadShaderSourceFromFile(const char* filePath) {
	std::ifstream fstream(filePath);
	if (!fstream.is_open()) {
		printf("Failed to load file %s", filePath);
		return {};
	}
	std::stringstream buffer;
	buffer << fstream.rdbuf();
	return buffer.str();
}

unsigned int createShader(GLenum shaderType, const char* sourceCode) {
	//Create a new vertex shader object
	unsigned int shader = glCreateShader(shaderType);
	//Supply the shader object with source code
	glShaderSource(shader, 1, &sourceCode, NULL);
	//Compile the shader object
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		//512 is an arbitrary length, but should be plenty of characters for our error message.
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("Failed to compile shader: %s", infoLog);
	}
	return shader;
}

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
	unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	unsigned int shaderProgram = glCreateProgram();
	//Attach each stage
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//Link all the stages together
	glLinkProgram(shaderProgram);
	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}
	//The linked program now contains our compiled code, so we can delete these intermediate objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*numVertices, vertexData, GL_STATIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,pos));
	glEnableVertexAttribArray(0);

	//Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	//UV attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, uv)));
	glEnableVertexAttribArray(2);

	return vao;
}

struct Transform {
	ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
	ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f); //Degrees
	ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
};
Transform cubeTransform;
Mesh* cubeMesh;

ew::Mat4 getModelMatrix(const Transform& transform) {
	return ew::TranslationMatrix(transform.position.x, transform.position.y, transform.position.z)
		* ew::RotateYMatrix(ew::Radians(transform.rotation.y))
		* ew::RotateXMatrix(ew::Radians(transform.rotation.x))
		* ew::RotateZMatrix(ew::Radians(transform.rotation.z))
		* ew::ScaleMatrix(transform.scale.x, transform.scale.y, transform.scale.z);
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

	Mesh* cubeMesh = createCubeMesh(0.5f);

	std::string vertexShaderSource = loadShaderSourceFromFile("assets/unlit.vert");
	std::string fragmentShaderSource = loadShaderSourceFromFile("assets/unlit.frag");
	unsigned int shader = createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	unsigned int vaoA = createVAO(cubeMesh->vertices, cubeMesh->numVertices, cubeMesh->indices, cubeMesh->numIndices);
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

		glBindVertexArray(vaoA);
		
		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		//Set the value of the variable at the location
		glUniform1f(timeLocation, time);

		//Construct model matrix
		ew::Mat4 model = getModelMatrix(cubeTransform);
		
		glUniformMatrix4fv(modelTransformLocation, 1, GL_FALSE, &model[0][0]);

		//Draw using elements
		glDrawElements(GL_TRIANGLES, cubeMesh->numIndices, GL_UNSIGNED_INT, NULL);

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
