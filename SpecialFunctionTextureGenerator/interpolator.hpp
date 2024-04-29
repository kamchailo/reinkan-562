#pragma once

namespace Jormungandr
{
	template<typename T, typename U = double>
	static T Lerp(T& start, T& end, U t)
	{
		return (1.0 - t) * start + t * end;
	}
}