#include <stdio.h>
#include <math.h>

#define GLAD_GL_IMPLEMENTATION
#include <ew/external/glad.h>

#include <GLFW/glfw3.h>
#include <assert.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/external/stb_image.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float vertices[21] = {
	//x   //y  //z   //r  //g  //b  //a
	-0.5, -0.5, 0.0, 1.0, 0.0, 0.0, 1.0, //Bottom left
	 0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, //Bottom right
	 0.0,  0.5, 0.0, 0.0, 0.0, 1.0, 1.0  //Top center
};

const char* vertexShaderSource = R"(
	#version 450
	layout(location = 0) in vec3 vPos;
	layout(location = 1) in vec4 vColor;
	out vec4 Color;
	uniform float uTime;
	void main(){
		Color = vColor;
		vec3 offset = vec3(0,sin(vPos.x + uTime),0)*0.5;
		gl_Position = vec4(vPos + offset,1.0);
	}
)";

const char* fragmentShaderSource = R"(
	#version 450
	out vec4 FragColor;
	in vec4 Color;
	uniform float uTime;
	void main(){
		FragColor = Color * abs(sin(uTime));
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numVertices*7, vertexData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)0);
	glEnableVertexAttribArray(0);

	//Color attribute
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const void*)(sizeof(float)*3));
	glEnableVertexAttribArray(1);

	return vao;
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

	unsigned int shader = createShaderProgram(vertexShaderSource, fragmentShaderSource);
	unsigned int vaoA = createVAO(vertices,3);
	int width, height, numComponents;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);

		glBindVertexArray(vaoA);
		
		//The current time in seconds this frame
		float time = (float)glfwGetTime();
		//Get the location in the uniform table of the variable
		int timeLocation = glGetUniformLocation(shader, "uTime");
		//Set the value of the variable at the location
		glUniform1f(timeLocation, time);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
