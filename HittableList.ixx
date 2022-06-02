module;

#include <cassert>
#include <cstdlib>
#include <memory>

export module HittableList;

import Emissive;
import IHittable;
import Sphere;

export class HittableList final : public IHittable
{
public:
	explicit constexpr HittableList() noexcept = default;
	//explicit constexpr HittableList(const std::shared_ptr<IHittable>& object) noexcept;
	//explicit constexpr HittableList(const HittableList& other) noexcept = default;
	//explicit constexpr HittableList(HittableList&& other) noexcept = default;
	//constexpr HittableList& operator=(const HittableList& other) noexcept = default;
	//constexpr HittableList& operator=(HittableList&& other) noexcept = default;
	constexpr ~HittableList() noexcept = default;

	void Clear() noexcept;
	void Add(const std::shared_ptr<IHittable>& object) noexcept;

	virtual bool HasHit(const Ray& ray, float minScalar, float maxScalar, HitRecord& outRecord) const noexcept override;

public:
	static constexpr const size_t MAX_OBJECTS = 512;

	size_t Size;
	std::shared_ptr<IHittable> Objects[MAX_OBJECTS];
	std::shared_ptr<Sphere> Sun;
};

//constexpr HittableList::HittableList(const std::shared_ptr<IHittable>& object) noexcept
//	: Objects()
//{
//	Add(object);
//}

void HittableList::Clear() noexcept
{
	//Objects.clear();
	memset(Objects, 0, MAX_OBJECTS * sizeof(Objects[0]));
}

void HittableList::Add(const std::shared_ptr<IHittable>& object) noexcept
{
	//Objects.push_back(object);
	assert(Size < MAX_OBJECTS);
	Objects[Size++] = object;
}

bool HittableList::HasHit(const Ray& ray, float minScalar, float maxScalar, HitRecord& outRecord) const noexcept
{
	HitRecord tempRecord;
	bool bHasHitAnything = false;
	float closestSoFar = maxScalar;

	for (size_t objectIdx = 0; objectIdx < Size; ++objectIdx)
	//for (IHittable* object : Objects)
	{
		if (Objects[objectIdx]->HasHit(ray, minScalar, maxScalar, tempRecord))
		{
			bHasHitAnything = true;
			if (closestSoFar > tempRecord.Scalar)
			{
				closestSoFar = tempRecord.Scalar;
				outRecord = tempRecord;
			}
		}
	}

	return bHasHitAnything;
}
