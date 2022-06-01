#pragma once

#include <chrono>
#include <cmath>
#include <limits>
#include <mutex>
#include <vector>
#include <thread>

// Constants
constexpr const float INFINITY_F = std::numeric_limits<float>::infinity();
constexpr const float PI_F = 3.1415926535897932385f;

// Utility Functions
inline float ConvertDegreesToRadians(float degrees)
{
	return degrees * PI_F / 180.0f;
}