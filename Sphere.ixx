module;

#include <cmath>
#include <iostream>

#include "IMaterial.h"

export module Sphere;

import IHittable;
import Ray;
import Shape;
import Vector3f;

export class Sphere final : public IIntersectable, public IHittable
{
public:
	explicit constexpr Sphere() noexcept = delete;
	explicit Sphere(const Point3f& center, float radius, IMaterial* material) noexcept;
	explicit constexpr Sphere(const Sphere& other) noexcept = default;
	explicit constexpr Sphere(Sphere&& other) noexcept = default;
	constexpr Sphere& operator=(const Sphere& other) noexcept = default;
	constexpr Sphere& operator=(Sphere&& other) noexcept = default;
	constexpr ~Sphere() noexcept = default;

	constexpr bool HasIntersected(const Ray& ray) noexcept override;
	float GetClosestIntersection(const Ray& ray) noexcept override;
	bool HasHit(const Ray& ray, float minScalar, float maxScalar, HitRecord& outRecord) const noexcept override;

public:
	Point3f Center;
	float Radius;

private:
	float mPadding = 0.0f;

public:
	IMaterial* Material;
};

Sphere::Sphere(const Point3f& center, float radius, IMaterial* material) noexcept
	: Center(center)
	, Radius(radius)
	, Material(material)
{
}

constexpr bool Sphere::HasIntersected(const Ray& ray) noexcept
{
	Vector3f centerToOrigin = ray.Origin - Center;
	float a = ray.Direction.GetLengthSquared();
	float halfB = Dot(centerToOrigin, ray.Direction);
	float c = centerToOrigin.GetLengthSquared() - Radius * Radius;
	float discriminant = halfB * halfB - a * c;

	return discriminant > 0.0f;
}

float Sphere::GetClosestIntersection(const Ray& ray) noexcept
{
	Vector3f centerToOrigin = ray.Origin - Center;
	float a = ray.Direction.GetLengthSquared();
	float halfB = Dot(centerToOrigin, ray.Direction);
	float c = centerToOrigin.GetLengthSquared() - Radius * Radius;
	float discriminant = halfB * halfB - a * c;

	if (discriminant < 0.0f)
	{
		return -1.0f;
	}

	return (-halfB - sqrt(discriminant)) / a;
}

bool Sphere::HasHit(const Ray& ray, float minScalar, float maxScalar, HitRecord& outRecord) const noexcept
{
	Vector3f centerToOrigin = ray.Origin - Center;
	float a = ray.Direction.GetLengthSquared();
	float halfB = Dot(centerToOrigin, ray.Direction);
	float c = centerToOrigin.GetLengthSquared() - Radius * Radius;
	float discriminant = halfB * halfB - a * c;

	if (discriminant < 0.0f)
	{
		return false;
	}

	float discrimantSqrt = sqrt(discriminant);

	// Find the nearest root that lies in the acceptable range.
	float root = (-halfB - discrimantSqrt) / a;
	if (root < minScalar || maxScalar < root)
	{
		root = (-halfB + discrimantSqrt) / a;
		if (root < minScalar || maxScalar < root)
		{
			return false;
		}
	}

	//std::cerr <<
	//	"CPU ray: (" <<
	//	ray.Direction.X << ", " <<
	//	ray.Direction.Y << ", " <<
	//	ray.Direction.Z << "), " <<
	//	" ~ distance length: " <<
	//	ray.Direction.GetLength() << ", " <<
	//	Dot(ray.Direction, ray.Direction) << ", " <<
	//	"discriminant: " <<
	//	discriminant << '\n';

	outRecord.Point = ray.At(root);
	outRecord.Scalar = root;

	Vector3f outwardNormal = (outRecord.Point - Center) / Radius;
	outRecord.SetFaceNormal(ray, outwardNormal);
	outRecord.Material = Material;

	return true;
}
