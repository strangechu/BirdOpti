#pragma once

struct Vector3 {
	double x, y, z;
	Vector3(double a, double b, double c) { x = a; y = b; z = c; }

	Vector3 operator*(const double f) const
	{
		return Vector3(this->x * f, this->y * f, this->z * f);
	}

	Vector3 operator+(const Vector3 v) const
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(const Vector3 v) const
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	double length()
	{
		return sqrt((x * x) + (y * y) + (z * z));
	}
};