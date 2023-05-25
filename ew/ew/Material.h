#pragma once
#include <glm/glm.hpp>
#include <map>
#include "Shader.h"
#include "Texture.h"

//class Shader;
//class Texture;

namespace ew {
	class Material {
	typedef const char* keyType;
	public:
		Material(Shader* shader);
		void setTexture(keyType name, Texture* value);
		void setFloat(keyType name, float value);
		void setVec2(keyType name, const glm::vec2& value);
		void setVec3(keyType name, const glm::vec3& value);
		void setVec4(keyType name, const glm::vec4& value);
		void setMat4(keyType name, const glm::mat4& value);
		void updateUniforms();

	private:
		Shader* m_shader;
		std::map<keyType, Texture*> m_textureValues;
		std::map<keyType, float> m_floatValues;
		std::map<keyType, glm::vec2> m_vec2Values;
		std::map<keyType, glm::vec3> m_vec3Values;
		std::map<keyType, glm::vec4> m_vec4Values;
		std::map<keyType, glm::mat4> m_mat4Values;
	};
}