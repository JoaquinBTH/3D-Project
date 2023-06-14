#pragma once

struct Vector3
{
	float xyz[3];

	Vector3(float x, float y, float z)
	{
		xyz[0] = x;
		xyz[1] = y;
		xyz[2] = z;
	}

	float GetX() const
	{
		return xyz[0];
	}

	float GetY() const
	{
		return xyz[1];
	}

	float GetZ() const
	{
		return xyz[2];
	}
};

struct Vector2
{
	float UV[2];

	Vector2(float x, float y)
	{
		UV[0] = x;
		UV[1] = y;
	}

	float GetX() const
	{
		return UV[0];
	}

	float GetY() const
	{
		return UV[1];
	}
};