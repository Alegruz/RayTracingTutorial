module;

#include "IMaterial.h"

export module Lambertian;

import Color;
import IHittable;
import Math;
import Ray;
import Vector3f;

export class Lambertian : public IMaterial
{
public:
	explicit constexpr Lambertian() noexcept = delete;
	explicit constexpr Lambertian(const Color& albedo) noexcept;
	explicit constexpr Lambertian(const Lambertian& other) noexcept = default;
	explicit constexpr Lambertian(Lambertian&& other) noexcept = default;
	constexpr Lambertian& operator=(const Lambertian& other) noexcept = default;
	constexpr Lambertian& operator=(Lambertian&& other) noexcept = default;
	virtual ~Lambertian() noexcept = default;

	virtual bool Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept override;

public:
	Color Albedo;
};

constexpr Lambertian::Lambertian(const Color& albedo) noexcept
	: Albedo(albedo)
{
}

bool Lambertian::Scatter(const Ray& ray, const HitRecord& hitRecord, Color& outAttenuation, Ray& outScattered) const noexcept
{
	Vector3f scatterDirection = hitRecord.Normal + Math::GetRandomUnitVector3f();

	if (scatterDirection.IsNearZero())
	{
		scatterDirection = hitRecord.Normal;
	}

	outScattered = Ray(hitRecord.Point, scatterDirection);
	outAttenuation = Albedo;
	return true;
}