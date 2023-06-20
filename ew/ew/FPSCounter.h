#pragma once
namespace ew {
	class FPSCounter {
	public:
		FPSCounter(float interval = 0.5f);
		bool tick(float deltaSeconds);
		inline float getFPS() const { return m_currentFPS; }
	private:
		float m_currentFPS = 0.0f;
		unsigned int m_numFrames = 0;
		float m_accumulatedTime = 0.0f;
		const float m_interval; //Time period to average FPS over
	};
}