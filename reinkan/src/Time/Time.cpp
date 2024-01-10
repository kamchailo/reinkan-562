#include "pch.h"
#include "Time.h"

#include <thread>
#include <chrono>

namespace Reinkan::Time
{
	Time::Time()
	{
		startTime = GetEngineCurrentTime();
	}

	void Time::Update()
	{
		if (deltaTime < 1.0f / fixedFps)
		{
			auto duration = std::chrono::duration<double>((1.0f / fixedFps) - deltaTime);
			//std::this_thread::sleep_for(duration);
		}

		double lastFrame = timeElapse;
		timeElapse = GetEngineCurrentTime() - startTime;

		lastTimeElapseSecond = timeElapseSecond;
		timeElapseSecond = timeElapse;

		deltaTime = timeElapse - lastFrame;

		fps = 1.0 / deltaTime;


		// Change to average FPS
		if (timeElapseSecond > lastTimeElapseSecond)
		{
			stableFps = 1.0 / deltaTime;
		}
	}

	double Time::GetElapseTime() const
	{
		return timeElapse;
	}

	double Time::GetDeltaTime() const
	{
		return deltaTime;
	}

	uint32_t Time::GetFPS() const
	{
		return fps;
	}

	uint32_t Time::GetStableFPS() const
	{
		return stableFps;
	}

	void Time::SetFixedFPS(double fixedFps)
	{
		this->fixedFps = fixedFps;
	}

	uint32_t Time::GetFixedFPS() const
	{
		return fixedFps;
	}

	void Time::SleepUntil(double untilTime)
	{
		//std::this_thread::sleep_until();
	}

	double Time::GetEngineCurrentTime() const
	{
		return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
	}
}
