#include "FPSCounter.h"
#include <assert.h>

ew::FPSCounter::FPSCounter(float interval)
	:m_interval(interval)
{
	assert(interval > 0.0f);
}

/// <summary>
/// Returns true if FPS has been updated on this frame
/// </summary>
/// <param name="deltaSeconds"></param>
/// <returns></returns>
bool ew::FPSCounter::tick(float deltaSeconds)
{
	m_numFrames++;
	m_accumulatedTime += deltaSeconds;
	if (m_accumulatedTime > m_interval) {
		m_currentFPS = (float)(m_numFrames / m_accumulatedTime);
		m_numFrames = 0;
		m_accumulatedTime = 0;
		return true;
	}
	return false;
}
