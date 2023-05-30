#include <stdio.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float vertices[9] = {
	//x   //y  //z
	-0.5, -0.5, 0.0, //Bottom left
	 0.5, -0.5, 0.0, //Bottom right
	 0.0,  0.5, 0.0  //Top center
};

const char* vertexShaderSource = R"(
	#version 450
	layout(location = 0) in vec3 vPos;
	uniform vec3 Offset;
	void main(){
		gl_Position = vec4(vPos + Offset,1.0);
	}
)";

const char* fragmentShaderSource = R"(
	#version 450
	out vec4 FragColor;
	uniform vec4 Color;
	void main(){
		FragColor = Color;
	}
)";

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

unsigned int createVAO(float* vertexData, int numVertices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Define a new buffer id
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//Allocate space for + send vertex data to GPU.
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numVertices*3, vertexData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);
	glEnableVertexAttribArray(0);

	return vao;
}

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

	unsigned int shader = createShaderProgram(vertexShaderSource, fragmentShaderSource);
	unsigned int vaoA = createVAO(vertices,3);
	int colorUniformLoc = glGetUniformLocation(shader, "Color");
	int offsetUniformLoc = glGetUniformLocation(shader, "Offset");

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);

		glBindVertexArray(vaoA);
		
		float time = (float)glfwGetTime();
		float offsetY = (float)sin(time) * 0.5;
		float offsetX = (float)cos(time) * 0.5;
		float r = (float)fabs(sin(time * 2.0));

		glUniform3f(offsetUniformLoc, offsetX, offsetY, 0.0f);
		glUniform4f(colorUniformLoc, r, 1.0-r, 0.0f, 0.5f);
		glDrawArrays(GL_TRIANGLES, 0, 3);

	/*	glUniform3f(offsetUniformLoc, 0.2f, 0.2f, 0.0f);
		glUniform4f(colorUniformLoc, 1.0f, 1.0f, 0.0f, 0.5f);
		glDrawArrays(GL_TRIANGLES, 0, 3);*/

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
