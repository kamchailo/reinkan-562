#pragma once

#include <glm/glm.hpp>

namespace Reinkan::Math
{
	class Quaternion
	{
		Quaternion(double s, glm::vec3 v) : s(s), v(v) {};

		Quaternion operator+ (Quaternion const& op2);
		Quaternion operator- (Quaternion const& op2);
		Quaternion operator* (Quaternion const& op2);

		Quaternion operator+= (Quaternion& const rhs);
		Quaternion operator-= (Quaternion& const rhs);
		Quaternion operator*= (Quaternion& const rhs);

		double s;
		glm::vec3 v;
	};
}