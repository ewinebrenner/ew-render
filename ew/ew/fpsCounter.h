#pragma once
namespace ew {
	class FPSCounter {
	public:
		FPSCounter(float intervalSeconds);
		void update(float deltaTime);
		inline float getFPS() const { return m_currentFPS; }
	private:
		int m_frameCount = 0;
		float m_accumulatedTime = 0.0f;
		float m_interval; //Interval in seconds to accumulate frames over
		float m_currentFPS = 0.0f;
	};
}