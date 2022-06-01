export module Ray;

import Vector3f;

export class Ray final
{
public:
	explicit constexpr Ray() noexcept = default;
	explicit constexpr Ray(const Point3f& origin, const Vector3f& direction) noexcept;
	//explicit constexpr Ray(const Ray& other) noexcept = default;
	//explicit constexpr Ray(Ray&& other) noexcept = default;
	//constexpr Ray& operator=(const Ray& other) noexcept = default;
	//constexpr Ray& operator=(Ray&& other) noexcept = default;
	~Ray() noexcept = default;

	constexpr Point3f At(float scalar) const;

public:
	Point3f Origin;
	Vector3f Direction;
};

constexpr Ray::Ray(const Point3f& origin, const Vector3f& direction) noexcept
	: Origin(origin)
	, Direction(direction)
{
}

constexpr Point3f Ray::At(float scalar) const
{
	return Origin + scalar * Direction;
}
