#pragma once
#include <string>
#include "ewMath/ewMath.h"

namespace ew {
	std::string loadShaderSourceFromFile(const std::string& filePath);
	unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
	class Shader {
	public:
		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		void use()const;
		void setInt(const std::string& name, int v) const;
		void setFloat(const std::string& name, float v) const;
		void setVec2(const std::string& name, float x, float y) const;
		void setVec2(const std::string& name, Vec2 v) const;
		void setVec3(const std::string& name, float x, float y, float z) const;
		void setVec3(const std::string& name, Vec3 v) const;
		void setVec4(const std::string& name, float x, float y, float z, float w) const;
		void setVec4(const std::string& name, Vec4 v) const;
		void setMat4(const std::string& name, const Mat4& v) const;
	private:
		unsigned int m_id;
	};
}