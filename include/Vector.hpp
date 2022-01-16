#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	struct vector3;

	struct Vector2
	{
	public:
		float x;
		float y;

		Vector2(Vector2& v) : x(v.x), y(v.y) {}
		Vector2() : x(0), y(0) {}
		Vector2(float both) : x(both), y(both) {}
		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 operator+(Vector2& next) { return Vector2(x + next.x, y + next.y); }
		Vector2 operator-(Vector2& next) { return Vector2(x - next.x, y - next.y); }
		Vector2 operator*(Vector2& next) { return Vector2(x * next.x, y * next.y); }
		Vector2 operator/(Vector2& next) { return Vector2(x / next.x, y / next.y); }

		Vector2& operator+=(Vector2& next) { x += next.x; y += next.y; return *this; }
		Vector2& operator-=(Vector2& next) { x -= next.x; y -= next.y; return *this; }
		Vector2& operator*=(Vector2& next) { x *= next.x; y *= next.y; return *this; }
		Vector2& operator/=(Vector2& next) { x /= next.x; y /= next.y; return *this; }
	};

	struct Vector3
	{
	public:
		float x;
		float y;
		float z;

		Vector3(Vector2& v) : x(v.x), y(v.y), z(0) {}
		Vector3(Vector3& v) : x(v.x), y(v.y), z(v.z) {}
		Vector3() : x(0), y(0), z(0) {}
		Vector3(float both) : x(both), y(both), z(both) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		Vector3 operator+(Vector3& next) { return Vector3(x + next.x, y + next.y, z + next.z); }
		Vector3 operator-(Vector3& next) { return Vector3(x - next.x, y - next.y, z - next.z); }
		Vector3 operator*(Vector3& next) { return Vector3(x * next.x, y * next.y, z * next.z); }
		Vector3 operator/(Vector3& next) { return Vector3(x / next.x, y / next.y, z / next.z); }

		Vector3& operator+=(Vector3& next) { x += next.x; y += next.y; z += next.z; return *this; }
		Vector3& operator-=(Vector3& next) { x -= next.x; y -= next.y; z -= next.z; return *this; }
		Vector3& operator*=(Vector3& next) { x *= next.x; y *= next.y; z *= next.z; return *this; }
		Vector3& operator/=(Vector3& next) { x /= next.x; y /= next.y; z /= next.z; return *this; }
	};
};
