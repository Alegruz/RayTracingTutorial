export module Shape;

import Ray;

export class IIntersectable
{
public:
	virtual ~IIntersectable() = default;

	virtual constexpr bool HasIntersected(const Ray& ray) noexcept = 0;
	virtual float GetClosestIntersection(const Ray& ray) noexcept = 0;
};