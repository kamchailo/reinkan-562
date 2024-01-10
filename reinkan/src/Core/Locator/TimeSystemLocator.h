#pragma once

#include "Time/Time.h"

namespace Reinkan::Core
{
	class TimeSystemLocator
	{
	public:
		static Time::Time* GetTime()
		{
			return timeInstance;
		}

		static void Provide(Time::Time* time)
		{
			timeInstance = time;
		}

	private:
		inline static Time::Time* timeInstance{ nullptr };
	};
}