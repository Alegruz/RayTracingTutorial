module;

#include <cstdlib>
#include <random>

export module Math;

import Ray;
import Vector3f;

namespace Math
{
	export float Clamp(float value, float min, float max) noexcept;

	export float GetRandom() noexcept;

	export float GetRandom(float min, float max) noexcept;

	export constexpr Vector3f GetRandomVector3f() noexcept;

	export constexpr Vector3f GetRandomVector3f(float min, float max) noexcept;

	export Vector3f GetRandomUnitSphereVector3f() noexcept;

	export Vector3f GetRandomUnitVector3f() noexcept;

	export Vector3f GetRandomUnitDiskVector3f() noexcept;

	export constexpr float Lerp(float lhs, float rhs, float scalar) noexcept;

	export constexpr Vector3f Lerp(const Vector3f& lhs, const Vector3f& rhs, float scalar) noexcept;

	inline float Clamp(float value, float min, float max) noexcept
	{
		if (value < min)
		{
			return min;
		}

		if (value > max)
		{
			return max;
		}

		return value;
	}

	inline float GetRandom() noexcept
	{
		return GetRandom(0.0f, 1.0f);
	}

	inline float GetRandom(float min, float max) noexcept
	{
		static std::uniform_real_distribution<float> sDistribution(min, max);
		static std::mt19937 sGenerator;

		return sDistribution(sGenerator);
	}

	constexpr Vector3f GetRandomVector3f() noexcept
	{
		return GetRandomVector3f(0.0f, 1.0f);
	}

	constexpr Vector3f GetRandomVector3f(float min, float max) noexcept
	{
		return Vector3f(GetRandom(min, max), GetRandom(min, max), GetRandom(min, max));
	}

	Vector3f GetRandomUnitSphereVector3f() noexcept
	{
		while (true)
		{
			Vector3f vec = GetRandomVector3f(-1.0f, 1.0f);

			if (vec.GetLengthSquared() >= 1.0f)
			{
				continue;
			}

			return vec;
		}
	}

	Vector3f GetRandomUnitVector3f() noexcept
	{
		Vector3f vec = GetRandomUnitSphereVector3f();
		vec.Normalize();
		return vec;
	}

	Vector3f GetRandomUnitDiskVector3f() noexcept
	{
		while (true)
		{
			Vector3f vec = Vector3f(GetRandom(-1.0f, 1.0f), GetRandom(-1.0f, 1.0f), 0.0f);

			if (vec.GetLengthSquared() >= 1.0f)
			{
				continue;
			}

			return vec;
		}
	}

	constexpr float Lerp(float lhs, float rhs, float scalar) noexcept
	{
		return (1.0f - scalar) * lhs + scalar * rhs;
	}

	constexpr Vector3f Lerp(const Vector3f& lhs, const Vector3f& rhs, float scalar) noexcept
	{
		return (1.0f - scalar) * lhs + scalar * rhs;
	}
}