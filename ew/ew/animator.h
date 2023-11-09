#pragma once
#include "animation.h"
namespace ew {
	class Animator {
	public:
		Animator(Animation* animation);
		void Update(float deltaTime);
		void Play(Animation* animation);
		void CalculateBoneTransform(const AssimpNodeData* node, const Mat4& parentTransform);
		inline std::vector<Mat4> GetFinalBoneMatrices() { return m_finalBoneMatrices; }
	private:
		std::vector<Mat4> m_finalBoneMatrices;
		Animation* m_currentAnimation;
		float m_currentTime;
		float m_deltaTime;
	};
}