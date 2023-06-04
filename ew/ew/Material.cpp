#include "Material.h"
#include "Shader.h"
#include "Texture.h"

ew::Material::Material(Shader* shader)
{
	assert(shader != nullptr);
	m_shader = shader;
}

void ew::Material::setTexture(keyType name, Texture* value)
{
	m_textureValues[name] = value;
}

void ew::Material::setFloat(keyType name, float value)
{
	m_floatValues[name] = value;
}

void ew::Material::setVec2(keyType name, const glm::vec2& value)
{
	m_vec2Values[name] = value;
}

void ew::Material::setVec3(keyType name, const glm::vec3& value)
{
	m_vec3Values[name] = value;
}

void ew::Material::setVec4(keyType name, const glm::vec4& value)
{
	m_vec4Values[name] = value;
}

void ew::Material::setMat4(keyType name, const glm::mat4& value)
{
	m_mat4Values[name] = value;
}

void ew::Material::updateUniforms()
{
	//TODO: Use uniform buffers
	int texSlot = 0;
	for (auto const& p : m_textureValues) {
		glActiveTexture(GL_TEXTURE0 + texSlot);
		glBindTexture(GL_TEXTURE_2D, p.second->getHandle());
		m_shader->setInt(p.first, texSlot);
		texSlot++;
	}
	for (auto const& p : m_floatValues) {
		m_shader->setFloat(p.first, p.second);
	}
	for (auto const& p : m_vec2Values) {
		m_shader->setVec2(p.first, p.second);
	}
	for (auto const& p : m_vec3Values) {
		m_shader->setVec3(p.first, p.second);
	}
	for (auto const& p : m_vec4Values) {
		m_shader->setVec4(p.first, p.second);
	}
	for (auto const& p : m_mat4Values) {
		m_shader->setMat4(p.first, p.second);
	}
}

void ew::Material::use()
{
	m_shader->use();
}
