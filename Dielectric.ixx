module;

#include <cmath>

#include "IMaterial.h"

export module Dielectric;

import Color;
import IHittable;
import Math;
import Ray;
import Vector3f;

export class Dielectric : public IMaterial
{
public:
	explicit constexpr Dielectric() noexcept = delete;
	explicit constexpr Dielectric(float indexOfRefraction) noexcept;
	explicit constexpr Dielectric(const Dielectric& other) noexcept = default;
	explicit constexpr Dielectric(Dielectric&& other) noexcept = default;
	constexpr Dielectric& operator=(const Dielectric& other) noexcept = default;
	constexpr Dielectric& operator=(Dielectric&& other) noexcept = default;
	virtual ~Dielectric() noexcept = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept override;

public:
	float IndexOfRefraction;

private:
	static float reflectance(float cosine, float refIdx);

private:
	uint32_t mPadding;
};

constexpr Dielectric::Dielectric(float indexOfRefraction) noexcept
	: IndexOfRefraction(indexOfRefraction)
{
	MaterialType = eMaterialType::DIELECTRIC;
}

bool Dielectric::Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept
{
	outAttenuation = Color(1.0f, 1.0f, 1.0f);
	float refractionRatio = hitRecord.bIsFrontFacing ? (1.0f / IndexOfRefraction) : IndexOfRefraction;

	Vector3f unitDirection = GetUnitVector(ray.Direction);

	float cosTheta = fmin(Dot(-unitDirection, hitRecord.Normal), 1.0f);
	float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

	bool bCannotRefract = refractionRatio * sinTheta > 1.0f;
	Vector3f direction;

	if (bCannotRefract || reflectance(cosTheta, refractionRatio) > Math::GetRandom())
	{
		direction = Reflect(unitDirection, hitRecord.Normal);
	}
	else
	{
		direction = Refract(unitDirection, hitRecord.Normal, refractionRatio);
	}

	outScattered = Ray(hitRecord.Point, direction);
	
	//Vector3f refracted = Refract(unitDirection, hitRecord.Normal, refractionRatio);
	//outScattered = Ray(hitRecord.Point, refracted);
	return true;
}

float Dielectric::reflectance(float cosine, float refIdx)
{
	// Use Schlick's approximation for reflectance.
	float r0 = (1.0f - refIdx) / (1.0f + refIdx);
	r0 = r0 * r0;
	return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}