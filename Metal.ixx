module;

#include <algorithm>

#include "IMaterial.h"

export module Metal;

import Color;
import IHittable;
import Math;
import Ray;
import Vector3f;

export class Metal : public IMaterial
{
public:
	explicit constexpr Metal() noexcept = delete;
	explicit constexpr Metal(const Color& albedo, float fuzz) noexcept;
	explicit constexpr Metal(const Metal& other) noexcept = default;
	explicit constexpr Metal(Metal&& other) noexcept = default;
	constexpr Metal& operator=(const Metal& other) noexcept = default;
	constexpr Metal& operator=(Metal&& other) noexcept = default;
	virtual ~Metal() noexcept = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept override;

public:
	Color Albedo;
	float Fuzz;

private:
	uint32_t mPadding;
};

constexpr Metal::Metal(const Color& albedo, float fuzz) noexcept
	: Albedo(albedo)
	, Fuzz(std::min(fuzz, 1.0f))
{
	MaterialType = eMaterialType::METAL;
}

bool Metal::Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept
{
	Vector3f reflect = Reflect(GetUnitVector(ray.Direction), hitRecord.Normal);

	outScattered = Ray(hitRecord.Point, reflect + Fuzz * Math::GetRandomUnitSphereVector3f());
	outAttenuation = Albedo;
	return Dot(outScattered.Direction, hitRecord.Normal) > 0.0f;
}