#pragma once

#include <chrono>

namespace Reinkan::Time
{
	class Time
	{
	public:
		Time();

		void Update();

		double GetElapseTime() const;

		double GetDeltaTime() const;

		uint32_t GetFPS() const;

		uint32_t GetStableFPS() const;

		void SetFixedFPS(double fixedFps);

		uint32_t GetFixedFPS() const;

		void SleepUntil(double untilTime);

	private:

		double GetEngineCurrentTime() const;

		double startTime{ 0.0 };

		std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;

		double timeElapse{ 0.0 };

		double deltaTime{ 0.0 };

		uint32_t fps;

		uint32_t fixedFps{ 60 };

		uint32_t timeElapseSecond;

		uint32_t stableFps;

		uint32_t lastTimeElapseSecond;
	};
}