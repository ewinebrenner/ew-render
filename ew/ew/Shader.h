#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <string>

namespace ew {
	enum class ShaderType {
		VERTEX = 0,
		TESS_CONTROL = 1,
		TESS_EVAL = 2,
		GEOMETRY = 3,
		FRAGMENT = 4,
		COMPUTE = 5
	};
	class ShaderStage {
	public:
		ShaderStage(ShaderType type);
		ShaderStage(ShaderType type, const std::string& filePath);
		~ShaderStage();
		bool loadSourceFromFile(const std::string& filePath);
		bool loadSource(const std::string& source);
	private:
		unsigned int m_handle;
		ShaderType m_type;
		bool tryCompile();
		static unsigned int convertToGLType(ShaderType type);
		static std::string getName(ShaderType type);
		friend class Shader;
	};
	class Shader
	{
	public:
		Shader();
		~Shader();
		void use();
		void setFloat(std::string name, float value);
		void setInt(std::string name, int value);
		void setMat3(std::string name, const glm::mat3& value);
		void setMat4(std::string name, const glm::mat4& value);
		void setVec2(std::string name, const glm::vec2& value);
		void setVec3(std::string name, const glm::vec3& value);
		void setVec4(std::string name, const glm::vec4& value);
		void attach(const ew::ShaderStage& stage);
		bool link();
	private:
		Shader(const Shader& r) = delete;
		GLuint m_id;
	};

}

