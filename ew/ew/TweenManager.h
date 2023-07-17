#pragma once
namespace ew {
	template<typename T>
	class Tween<T> {
	public:
		void update(float dt) {
			*m_valueRef = startValue + (endValue - startValue) * t;
		}
	private:
		T* m_valueRef;
		T startValue;
		T endValue;
		float t;
	};
	class TweenManager {
	public:
		template<typename T>
		Tween<T>* tween(T* value, float endValue, float time);
		void update();
	};
}