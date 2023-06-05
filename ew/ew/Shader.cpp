#include "Shader.h"
#include <fstream>
#include <sstream>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/type_ptr.hpp>

ew::ShaderStage::ShaderStage(ew::ShaderType type) {
	m_type = type;
	m_handle = glCreateShader(convertToGLType(type));
}
ew::ShaderStage::ShaderStage(ew::ShaderType type, const std::string& filePath)
	:ShaderStage(type){
	loadSourceFromFile(filePath);
}

ew::ShaderStage::~ShaderStage() {
	glDeleteShader(m_handle);
}

bool ew::ShaderStage::loadSourceFromFile(const std::string& filePath) {
	std::ifstream fileStream;
	fileStream.open(filePath);
	if (!fileStream.is_open()) {
		printf("Failed to open file %s ", filePath.c_str());
		return false;
	}
	std::stringstream stringStream;
	stringStream << fileStream.rdbuf();
	fileStream.close();
	return loadSource(stringStream.str());
}

bool ew::ShaderStage::loadSource(const std::string& shaderSource) {
	const char* src = shaderSource.c_str();
	glShaderSource(m_handle, 1, &src, NULL);
	return tryCompile();
}

bool ew::ShaderStage::tryCompile() {
	//Compiles the shader source
	glCompileShader(m_handle);

	//Get result of last compile - either GL_TRUE or GL_FALSE
	GLint success;
	glGetShaderiv(m_handle, GL_COMPILE_STATUS, &success);
	if (!success) {
		//Dump logs into a char array - 512 is an arbitrary length
		GLchar infoLog[512];
		glGetShaderInfoLog(m_handle, 512, NULL, infoLog);
		printf("Failed to compile %s shader: %s", getName(m_type).c_str(), infoLog);
	}
	return success;
}

unsigned int ew::ShaderStage::convertToGLType(ew::ShaderType shaderType) {
	static unsigned int glTypes[6] = {
		GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
		GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER
	};
	return glTypes[(int)shaderType];
}

std::string ew::ShaderStage::getName(ew::ShaderType shaderType) {
	static std::string shaderTypeNames[6] = {
		"Vertex", "Tesselation Control", "Tesselation Evaluation",
		"Geometry", "Fragment", "Compute"
	};
	return shaderTypeNames[(int)shaderType];
}

ew::Shader::Shader()
{
	m_id = glCreateProgram();
}

ew::Shader::~Shader() {
	glDeleteProgram(m_id);
}

void ew::Shader::attach(const ew::ShaderStage& stage) {
	glAttachShader(m_id, stage.m_handle);
}

bool ew::Shader::link() {
	glLinkProgram(m_id);
	//Logging
	int success;
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success) {

		GLchar infoLog[512];
		glGetProgramInfoLog(m_id, 512, NULL, infoLog);
		printf("Failed to link shader program: %s", infoLog);
	}
	return success;
}

void ew::Shader::use()
{
	glUseProgram(m_id);
}

void ew::Shader::setFloat(std::string name, float value)
{
	glProgramUniform1f(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void ew::Shader::setVec2(std::string name, const glm::vec2& value)
{
	glProgramUniform2f(m_id, glGetUniformLocation(m_id, name.c_str()), value.x, value.y);
}
void ew::Shader::setVec3(std::string name, const glm::vec3& value)
{
	glProgramUniform3f(m_id, glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z);
}
void ew::Shader::setVec4(std::string name, const glm::vec4& value)
{
	glProgramUniform4f(m_id, glGetUniformLocation(m_id, name.c_str()), value.x, value.y, value.z, value.w);
}

void ew::Shader::setInt(std::string name, int value)
{
	glProgramUniform1i(m_id, glGetUniformLocation(m_id, name.c_str()), value);
}

void ew::Shader::setMat3(std::string name, const glm::mat3& value) {
	glProgramUniformMatrix3fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}

void ew::Shader::setMat4(std::string name, const glm::mat4& value) {
	glProgramUniformMatrix4fv(m_id, glGetUniformLocation(m_id, name.c_str()), 1, false, glm::value_ptr(value));
}