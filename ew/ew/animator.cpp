#include "animator.h"

namespace ew {
	Animator::Animator(Animation* animation)
		:m_currentTime(0), m_currentAnimation(animation), m_paused(false)
	{
		m_finalBoneMatrices.reserve(100);
		for (size_t i = 0; i < 100; i++)
		{
			m_finalBoneMatrices.push_back(ew::IdentityMatrix());
		}
		UpdateBoneTransforms();
	}

	void Animator::Update(float deltaTime)
	{
		m_deltaTime = deltaTime;
		if (m_currentAnimation == nullptr || m_paused)
			return;
		m_currentTime += m_currentAnimation->GetTicksPerSecond() * deltaTime;
		//Loop time
		m_currentTime = fmod(m_currentTime, m_currentAnimation->GetDuration());
		UpdateBoneTransforms();
	}

	void Animator::Play(Animation* animation)
	{
		m_currentAnimation = animation;
		m_currentTime = 0.0f;
	}
	void Animator::SetTime(float time) {
		if (time == m_currentTime)
			return;
		m_currentTime = time;
		UpdateBoneTransforms();
	}
	void Animator::UpdateBoneTransforms()
	{
		CalculateBoneTransform(&m_currentAnimation->GetRootNode(), ew::IdentityMatrix());
	}

	void Animator::CalculateBoneTransform(const AssimpNodeData* node, const Mat4& parentTransform)
	{
		std::string nodeName = node->name;
		Mat4 nodeTransform = node->transformation;
		Bone* bone = m_currentAnimation->FindBone(nodeName);
		if (bone) {
			bone->Update(m_currentTime);
			nodeTransform = bone->GetLocalTransform();
		}
		Mat4 globalTransformation = parentTransform * nodeTransform;
		auto boneInfoMap = m_currentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
			int index = boneInfoMap[nodeName].id;
			Mat4 offset = boneInfoMap[nodeName].offset;
			m_finalBoneMatrices[index] = globalTransformation * offset;
		}
		for (size_t i = 0; i < node->childrenCount; i++)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}


}