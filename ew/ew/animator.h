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
		inline float GetTime()const { return m_currentTime; }
		void SetTime(float time); //Manually set time. Will update bone transforms.
		inline void SetPaused(bool paused) { m_paused = paused; }
	private:
		void UpdateBoneTransforms();
		std::vector<Mat4> m_finalBoneMatrices;
		Animation* m_currentAnimation;
		float m_currentTime;
		float m_deltaTime;
		bool m_paused;
	};
}