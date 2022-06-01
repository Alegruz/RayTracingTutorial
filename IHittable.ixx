module;

#include <cstdint>
#include <iostream>

#include "IMaterial.h"

export module IHittable;

import Ray;
import Vector3f;

export struct HitRecord
{
	Point3f Point;
	Vector3f Normal;
	IMaterial* Material;
	float Scalar;
	bool bIsFrontFacing;

	inline void SetFaceNormal(const Ray& ray, const Vector3f& outwardNormal)
	{
		bIsFrontFacing = Dot(ray.Direction, outwardNormal) < 0.0f;
		Normal = (2.0f * static_cast<float>(bIsFrontFacing) - 1.0f) * outwardNormal;
	}

private:
	uint8_t mPadding[3];
};

export class IHittable
{
public:
	virtual ~IHittable() = default;

	virtual bool HasHit(const Ray& ray, float minScalar, float maxScalar, HitRecord& outRecord) const noexcept = 0;
};