module;

#include <cstdint>
#include <iostream>

export module Color;

import Math;
import Vector3f;

export typedef Vector3f Color;

export void WriteColor(std::ostream& os, const Color& pixelColor) noexcept;

export void WriteColor(std::ostream& os, const Color& pixelColor, size_t samplesPerPixel) noexcept;

export void WriteColor(uint8_t* pixels, const Color& pixelColor) noexcept;

export void WriteColor(uint8_t* pixels, const Color& pixelColor, size_t samplesPerPixel) noexcept;

void WriteColor(std::ostream& os, const Color& pixelColor) noexcept
{
	os << static_cast<int>(255.999999f * Math::Clamp(pixelColor.X, 0.0f, 0.999999f)) << ' '
		<< static_cast<int>(255.999999f * Math::Clamp(pixelColor.Y, 0.0f, 0.999999f)) << ' '
		<< static_cast<int>(255.999999f * Math::Clamp(pixelColor.Z, 0.0f, 0.999999f)) << '\n';
}

void WriteColor(std::ostream& os, const Color& pixelColor, size_t samplesPerPixel) noexcept
{
	float scale = 1.0f / static_cast<float>(samplesPerPixel);
	float r = sqrt(pixelColor.X * scale);
	float g = sqrt(pixelColor.Y * scale);
	float b = sqrt(pixelColor.Z * scale);

	os << static_cast<int>(255.999999f * Math::Clamp(r, 0.0f, 0.999999f)) << ' '
		<< static_cast<int>(255.999999f * Math::Clamp(g, 0.0f, 0.999999f)) << ' '
		<< static_cast<int>(255.999999f * Math::Clamp(b, 0.0f, 0.999999f)) << '\n';
}

void WriteColor(uint8_t* pixels, const Color& pixelColor) noexcept
{
	pixels[0] = static_cast<uint8_t>(255.999999f * Math::Clamp(pixelColor.X, 0.0f, 0.999999f));
	pixels[1] = static_cast<uint8_t>(255.999999f * Math::Clamp(pixelColor.Y, 0.0f, 0.999999f));
	pixels[2] = static_cast<uint8_t>(255.999999f * Math::Clamp(pixelColor.Z, 0.0f, 0.999999f));
}

void WriteColor(uint8_t* pixels, const Color& pixelColor, size_t samplesPerPixel) noexcept
{
	float scale = 1.0f / static_cast<float>(samplesPerPixel);
	float r = sqrt(pixelColor.X * scale);
	float g = sqrt(pixelColor.Y * scale);
	float b = sqrt(pixelColor.Z * scale);

	pixels[0] = static_cast<uint8_t>(255.999999f * Math::Clamp(r, 0.0f, 0.999999f));
	pixels[1] = static_cast<uint8_t>(255.999999f * Math::Clamp(g, 0.0f, 0.999999f));
	pixels[2] = static_cast<uint8_t>(255.999999f * Math::Clamp(b, 0.0f, 0.999999f));
}