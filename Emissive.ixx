module;

#include "IMaterial.h"

export module Emissive;

import Color;
import IHittable;
import Math;
import Ray;
import Vector3f;

export class Emissive : public IMaterial
{
public:
	explicit constexpr Emissive() noexcept = delete;
	explicit constexpr Emissive(const Color& albedo) noexcept;
	explicit constexpr Emissive(const Emissive& other) noexcept = default;
	explicit constexpr Emissive(Emissive&& other) noexcept = default;
	constexpr Emissive& operator=(const Emissive& other) noexcept = default;
	constexpr Emissive& operator=(Emissive&& other) noexcept = default;
	virtual ~Emissive() noexcept = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept override;

public:
	Color Albedo;
};

constexpr Emissive::Emissive(const Color& albedo) noexcept
	: Albedo(albedo)
{
	MaterialType = eMaterialType::EMISSIVE;
}

bool Emissive::Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept
{
	outAttenuation = Albedo;
	return false;
}