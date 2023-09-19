#include "fpsCounter.h"
#include <assert.h>

namespace ew {
	FPSCounter::FPSCounter(float intervalSeconds)
		:m_interval(intervalSeconds)
	{
		assert(intervalSeconds > 0);
	}
	void FPSCounter::update(float deltaTime)
	{
		m_frameCount++;
		m_accumulatedTime += deltaTime;
		if (m_accumulatedTime < m_interval) {
			return;
		}
		m_currentFPS = (float)(m_frameCount / m_accumulatedTime);
		m_frameCount = 0;
		m_accumulatedTime = 0.0f;
	}
}