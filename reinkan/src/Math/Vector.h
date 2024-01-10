#pragma once

namespace Reinkan::Math
{
	class Vector2
	{
		double x;
		double y;
	};

	class Vector3
	{
		double x;
		double y;
		double z;
	};

	class Vector4
	{
	public:

		
		Vector4 operator+= (Vector4 const& rhs);
		Vector4 operator-= (Vector4 const& rhs);
		Vector4 operator*= (Vector4 const& rhs);

		double x;
		double y;
		double z;
		double w;
	};

	Vector4 operator+ (int const lhs, Vector4 const& rhs);
	Vector4 operator- (int const lhs, Vector4 const& rhs);
	Vector4 operator* (int const lhs, Vector4 const& rhs);

	Vector4 operator+ (float const lhs, Vector4 const& rhs);
	Vector4 operator- (float const lhs, Vector4 const& rhs);
	Vector4 operator* (float const lhs, Vector4 const& rhs);

	Vector4 operator+ (double const lhs, Vector4 const& rhs);
	Vector4 operator- (double const lhs, Vector4 const& rhs);
	Vector4 operator* (double const lhs, Vector4 const& rhs);
}