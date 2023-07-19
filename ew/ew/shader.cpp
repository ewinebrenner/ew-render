#include "shader.h"
#include <fstream>
#include <sstream>
#include "external/glad.h"

namespace ew {
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

	static unsigned int createShader(GLenum shaderType, const char* sourceCode) {
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
	/// <summary>
	/// Creates a shader program with vertex + fragment stages
	/// </summary>
	/// <param name="vertexShader">File path to vertex shader</param>
	/// <param name="fragmentShader">File path to fragment shader</param>
	Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		std::string vertexShaderSource = ew::loadShaderSourceFromFile(vertexShader.c_str());
		std::string fragmentShaderSource = ew::loadShaderSourceFromFile(fragmentShader.c_str());
		m_id = ew::createShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	}
	void Shader::use()
	{
		glUseProgram(m_id);
	}
	void Shader::setInt(const std::string& name, int v) const
	{
		glUniform1i(glGetUniformLocation(m_id, name.c_str()), v);
	}
	void Shader::setFloat(const std::string& name, float v) const
	{
		glUniform1f(glGetUniformLocation(m_id,name.c_str()), v);
	}
	void Shader::setVec2(const std::string& name, Vec2 v) const
	{
		glUniform2f(glGetUniformLocation(m_id, name.c_str()), v.x, v.y);
	}
	void Shader::setVec3(const std::string& name, Vec3 v) const
	{
		glUniform3f(glGetUniformLocation(m_id, name.c_str()), v.x, v.y, v.z);
	}
	void Shader::setVec4(const std::string& name, Vec4 v) const
	{
		glUniform4f(glGetUniformLocation(m_id, name.c_str()), v.x, v.y, v.z, v.w);
	}
	void Shader::setMat4(const std::string& name, const Mat4& v) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &v[0][0]);
	}
}

