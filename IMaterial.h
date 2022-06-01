#pragma once

import Color;
import Ray;

struct HitRecord;

class IMaterial
{
public:
	explicit constexpr IMaterial() noexcept = default;
	explicit constexpr IMaterial(const IMaterial& other) noexcept = default;
	explicit constexpr IMaterial(IMaterial&& other) noexcept = default;
	constexpr IMaterial& operator=(const IMaterial& other) noexcept = default;
	constexpr IMaterial& operator=(IMaterial&& other) noexcept = default;
	virtual ~IMaterial() noexcept = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept = 0;
};