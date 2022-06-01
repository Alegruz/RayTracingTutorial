module;

#include "Common.h"

export module Camera;

import Math;
import Ray;
import Vector3f;

export class Camera final
{
public:
	explicit Camera() noexcept;
	explicit Camera(const Point3f& eye, const Point3f& at, const Vector3f& up, float vFov, float aspectRatio, float aperture) noexcept;
	explicit Camera(const Point3f& eye, const Point3f& at, const Vector3f& up, float vFov, float aspectRatio, float aperture, float focusDistance) noexcept;
	//explicit constexpr Camera(const Camera& other) noexcept = default;
	//explicit constexpr Camera(Camera&& other) noexcept = default;
	//constexpr Camera& operator=(const Camera& other) noexcept = default;
	//constexpr Camera& operator=(Camera&& other) noexcept = default;
	~Camera() noexcept = default;

	constexpr Ray GetRay(float u, float v) const noexcept;

	constexpr const Point3f& GetPosition() const noexcept;
	constexpr const Vector3f& GetHorizontal() const noexcept;
	constexpr const Vector3f& GetVertical() const noexcept;
	constexpr const Vector3f& GetLowerLeftCorner() const noexcept;

public:
	static constexpr const float ASPECT_RATIO = 16.0f / 9.0f;
	static constexpr const float VIEWPORT_HEIGHT = 2.0f;
	static constexpr const float VIEWPORT_WIDTH = ASPECT_RATIO * VIEWPORT_HEIGHT;
	static constexpr const float FOCAL_LENGTH = 1.0f;

private:
	Point3f mEye;
	Vector3f mHorizontal;
	Vector3f mVertical;
	Point3f mLowerLeftCorner;

	Vector3f mU;
	Vector3f mV;
	Vector3f mW;
	float mLensRadius;
};

Camera::Camera() noexcept
	: Camera(Point3f(-2.0f, 2.0f, 1.0f), Point3f(0.0f, 0.0f, -1.0f), Point3f(0.0f, 1.0f, 0.0f), 20.0f, Camera::ASPECT_RATIO, 2.0f)
{
}

Camera::Camera(const Point3f& eye, const Point3f& at, const Vector3f& up, float vFov, float aspectRatio, float aperture) noexcept
	: Camera(eye, at, up, vFov, aspectRatio, aperture, (eye - at).GetLength())
{
}

Camera::Camera(const Point3f& eye, const Point3f& at, const Vector3f& up, float vFov, float aspectRatio, float aperture, float focusDistance) noexcept
{
	float theta = ConvertDegreesToRadians(vFov);
	float h = tan(theta / 2.0f);
	float viewportHeight = 2.0f * h;
	float viewportWidth = aspectRatio * viewportHeight;

	mW = GetUnitVector(eye - at);
	mU = GetUnitVector(Cross(up, mW));
	mV = Cross(mW, mU);

	mEye = eye;
	mHorizontal = focusDistance * viewportWidth * mU;
	mVertical = focusDistance * viewportHeight * mV;
	mLowerLeftCorner = mEye - (mHorizontal / 2.0f) - (mVertical / 2.0f) - focusDistance * mW;

	mLensRadius = aperture / 2.0f;
}

constexpr Ray Camera::GetRay(float u, float v) const noexcept
{
	Vector3f rd = mLensRadius * Math::GetRandomUnitDiskVector3f();
	Vector3f offset = mU * rd.X + mV * rd.Y;

	return Ray(mEye + offset, mLowerLeftCorner + u * mHorizontal + v * mVertical - mEye - offset);
}

constexpr const Point3f& Camera::GetPosition() const noexcept
{
	return mEye;
}

constexpr const Vector3f& Camera::GetHorizontal() const noexcept
{
	return mHorizontal;
}

constexpr const Vector3f& Camera::GetVertical() const noexcept
{
	return mHorizontal;
}

constexpr const Vector3f& Camera::GetLowerLeftCorner() const noexcept
{
	return mLowerLeftCorner;
}
