#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <string>

namespace ew {
	class Shader
	{
	public:
		Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
		void use();
		void setFloat(std::string name, float value);
		void setInt(std::string name, int value);
		void setMat3(std::string name, const glm::mat3& value);
		void setMat4(std::string name, const glm::mat4& value);
		void setVec2(std::string name, const glm::vec2& value);
		void setVec3(std::string name, const glm::vec3& value);
		void setVec4(std::string name, const glm::vec4& value);
	private:
		Shader(const Shader& r) = delete;
		std::string readFile(const std::string& filePath);
		GLuint compileShader(const char* shaderSource, GLenum type);
		GLuint m_id;
	};

}

