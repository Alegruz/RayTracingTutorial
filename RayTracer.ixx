module;

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Common.h"

#define CPU_SINGLE_THREAD (0)
#define CPU_MULTI_THREAD (1)

#include "IMaterial.h"

export module RayTracer;

import Camera;
import Color;
import Dielectric;
import HittableList;
import IHittable;
import Lambertian;
import Math;
import Metal;
import Ray;
import Sphere;
import Vector3f;

#if CPU_MULTI_THREAD
static std::mutex sMutex;
#endif

export class RayTracer final
{
public:
	static constexpr const size_t MAX_BOUND_DEPTH = 5;

	static Color RayColor(const Ray& ray, const IHittable& world, size_t depth = MAX_BOUND_DEPTH) noexcept;

public:
	explicit RayTracer() noexcept = delete;
	explicit RayTracer(size_t width, size_t height, size_t numChannels) noexcept;
	explicit RayTracer(const RayTracer& other) noexcept;
	explicit RayTracer(RayTracer&& other) noexcept;
	RayTracer& operator=(const RayTracer& other) noexcept;
	constexpr RayTracer& operator=(RayTracer&& other) noexcept;
	~RayTracer() noexcept;

	void Initialize() noexcept;
	void Render() noexcept;

	static constexpr const size_t SAMPLES_PER_PIXEL = 10;

private:
	static void renderSingleRow(uint8_t* backBuffer, size_t width, size_t height, size_t numChannels, size_t y, const Camera& camera, const HittableList& world) noexcept;
	static void renderSinglePixel(uint8_t* backBuffer, size_t width, size_t height, size_t numChannels, size_t x, size_t y, const Camera& camera, const HittableList& world) noexcept;

	static void createRandomScene(HittableList& outWorld, std::vector<IMaterial*>& outMaterials);

private:
	// Image
	size_t mWidth;
	size_t mHeight;
	size_t mNumChannels;
	uint8_t* mHostBackBuffer;

	// Camera
	Camera mCamera;

	uint32_t mPadding;

	// World
	HittableList mWorld;
	std::vector<IMaterial*> mMaterials;

#if CPU_MULTI_THREAD
	// Multi-Threading
	std::vector<std::thread> mThreadsPerRow;
#endif
};

Color RayTracer::RayColor(const Ray& ray, const IHittable& world, size_t depth) noexcept
{
	HitRecord record;

	if (depth <= 0)
	{
		return Color(0.0f, 0.0f, 0.0f);
	}

	if (world.HasHit(ray, 0.000001f, INFINITY_F, record))
	{
		//Point3f target = record.Point + record.Normal + Math::GetRandomUnitVector3f();
		Ray scattered;
		Color attenuation;
		if (record.Material->Scatter(ray, record, attenuation, scattered))
		{
			return attenuation * RayColor(scattered, world, depth - 1);
		}
		return Color(0.0f, 0.0f, 0.0f);
		//return 0.5f * RayColor(Ray(record.Point, target - record.Point), world, depth - 1);
	}

	Point3f unitDirection = GetUnitVector(ray.Direction);
	float scalar = 0.5f * (unitDirection.Y + 1.0f);

	return Math::Lerp(Color(1.0f, 1.0f, 1.0f), Color(0.5f, 0.7f, 1.0f), scalar);
}

RayTracer::RayTracer(size_t width, size_t height, size_t numChannels) noexcept
	: mWidth(width)
	, mHeight(height)
	, mNumChannels(numChannels)
	, mHostBackBuffer(reinterpret_cast<uint8_t*>(malloc(mWidth* mHeight* mNumChannels * sizeof(mHostBackBuffer[0]))))
	, mCamera(Point3f(13.0f, 2.0f, 3.0f), Point3f(0.0f, 0.0f, 0.0f), Point3f(0.0f, 1.0f, 0.0f), 10.0f, 3.0f / 2.0f, 0.1f, 10.0f)
	, mWorld()
{
	assert(mHostBackBuffer);
}

RayTracer::RayTracer(const RayTracer& other) noexcept
{
	if (this != &other)
	{
		mWidth = other.mWidth;
		mHeight = other.mHeight;
		mNumChannels = other.mNumChannels;

		mHostBackBuffer = reinterpret_cast<uint8_t*>(malloc(mWidth * mHeight * mNumChannels * sizeof(mHostBackBuffer[0])));
		assert(mHostBackBuffer);

		memcpy(mHostBackBuffer, other.mHostBackBuffer, mWidth * mHeight * mNumChannels * sizeof(mHostBackBuffer[0]));

		mCamera = other.mCamera;

		mWorld = other.mWorld;
	}
}

RayTracer::RayTracer(RayTracer&& other) noexcept
{
	if (this != &other)
	{
		mWidth = other.mWidth;
		mHeight = other.mHeight;
		mNumChannels = other.mNumChannels;
		mHostBackBuffer = other.mHostBackBuffer;

		mCamera = std::move(other.mCamera);
		mWorld = std::move(other.mWorld);

		other.mWidth = 0;
		other.mHeight = 0;
		other.mNumChannels = 0;
		other.mHostBackBuffer = nullptr;
	}
}

RayTracer& RayTracer::operator=(const RayTracer& other) noexcept
{
	if (this != &other)
	{
		if (mWidth * mHeight * mNumChannels != other.mWidth * other.mHeight * other.mNumChannels)
		{
			if (mHostBackBuffer)
			{
				free(mHostBackBuffer);
			}

			mWidth = other.mWidth;
			mHeight = other.mHeight;
			mNumChannels = other.mNumChannels;

			mHostBackBuffer = reinterpret_cast<uint8_t*>(malloc(mWidth * mHeight * mNumChannels * sizeof(mHostBackBuffer[0])));
			assert(mHostBackBuffer);
		}

		memcpy(mHostBackBuffer, other.mHostBackBuffer, mWidth * mHeight * mNumChannels * sizeof(mHostBackBuffer[0]));

		mCamera = other.mCamera;

		mWorld = other.mWorld;
	}

	return *this;
}

constexpr RayTracer& RayTracer::operator=(RayTracer&& other) noexcept
{
	if (this != &other)
	{
		mWidth = other.mWidth;
		mHeight = other.mHeight;
		mNumChannels = other.mNumChannels;
		mHostBackBuffer = other.mHostBackBuffer;

		mCamera = std::move(other.mCamera);
		mWorld = std::move(other.mWorld);

		other.mWidth = 0;
		other.mHeight = 0;
		other.mNumChannels = 0;
		other.mHostBackBuffer = nullptr;
	}

	return *this;
}

RayTracer::~RayTracer() noexcept
{
	for (IMaterial* material : mMaterials)
	{
		delete material;
	}
	mMaterials.clear();

	if (mHostBackBuffer)
	{
		free(mHostBackBuffer);
	}
}

void RayTracer::Initialize() noexcept
{
	// World
	createRandomScene(mWorld, mMaterials);
}

void RayTracer::Render() noexcept
{
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
#if CPU_SINGLE_THREAD
	begin = std::chrono::steady_clock::now();
	size_t imageIndex = 0ull;
	for (size_t j = mHeight; j > 0u; --j)
	{
		//std::cerr << "\rScanlines remaining: " << j - 1u << ' ' << std::flush;
		for (size_t i = 0u; i < mWidth; ++i)
		{
			renderSinglePixel(mHostBackBuffer, mWidth, mHeight, mNumChannels, i, j - 1u, mCamera, mWorld);
			//Color pixelColor;
			//for (size_t sampleIdx = 0; sampleIdx < SAMPLES_PER_PIXEL; ++sampleIdx)
			//{
			//	float u = (static_cast<float>(i) + Math::GetRandom()) / static_cast<float>(mWidth - 1u);
			//	float v = (static_cast<float>(j - 1u) + Math::GetRandom()) / static_cast<float>(mHeight - 1u);
			//	Ray ray = mCamera.GetRay(u, v);
			//	pixelColor += RayColor(ray, mWorld);
			//}
			//WriteColor(&mHostBackBuffer[imageIndex], pixelColor, SAMPLES_PER_PIXEL);
			imageIndex += mNumChannels;
		}
	}
	end = std::chrono::steady_clock::now();

	stbi_write_png(
		"example_cpu.png",
		static_cast<int>(mWidth),
		static_cast<int>(mHeight),
		static_cast<int>(mNumChannels),
		mHostBackBuffer,
		static_cast<int>(mWidth * mNumChannels)
	);

	std::cerr << "\nDone. Single-Threaded CPU Ray Tracing took: " << static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1000000000.0  << " seconds" << std::endl;
#endif

	// Threads
#if CPU_MULTI_THREAD
	begin = std::chrono::steady_clock::now();
	for (size_t i = mHeight; i > 0u; --i)
	{
		mThreadsPerRow.push_back(std::thread(renderSingleRow, mHostBackBuffer, mWidth, mHeight, mNumChannels, i - 1, mCamera, mWorld));
	}
	for (std::thread& thread : mThreadsPerRow)
	{
		thread.join();
	}
	end = std::chrono::steady_clock::now();

	stbi_write_png(
		"example_cpu_multi.png",
		static_cast<int>(mWidth),
		static_cast<int>(mHeight),
		static_cast<int>(mNumChannels),
		mHostBackBuffer,
		static_cast<int>(mWidth * mNumChannels)
	);

	std::cerr << "\nDone. Multi-Threaded CPU Ray Tracing took: " << static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1000000000.0 << " seconds" << std::endl;
#endif
}

void RayTracer::renderSingleRow(uint8_t* backBuffer, size_t width, size_t height, size_t numChannels, size_t y, const Camera& camera, const HittableList& world) noexcept
{
	for (size_t x = 0u; x < width; ++x)
	{
		renderSinglePixel(backBuffer, width, height, numChannels, x, y, camera, world);
	}
}

void RayTracer::renderSinglePixel(uint8_t* backBuffer, size_t width, size_t height, size_t numChannels, size_t x, size_t y, const Camera& camera, const HittableList& world) noexcept
{
	Color pixelColor;
	for (size_t sampleIdx = 0; sampleIdx < SAMPLES_PER_PIXEL; ++sampleIdx)
	{
		float u = (static_cast<float>(x) + Math::GetRandom()) / static_cast<float>(width - 1u);
		float v = (static_cast<float>(y) + Math::GetRandom()) / static_cast<float>(height - 1u);
		Ray ray = camera.GetRay(u, v);
		pixelColor += RayColor(ray, world);
	}
	WriteColor(&backBuffer[((height - 1) - y) * width * numChannels + x * numChannels], pixelColor, SAMPLES_PER_PIXEL);
}

void RayTracer::createRandomScene(HittableList& outWorld, std::vector<IMaterial*>& outMaterials)
{
	Lambertian* groundMaterial = new Lambertian(Color(0.5f, 0.5f, 0.5f));
	outMaterials.push_back(groundMaterial);
	outWorld.Add(std::make_shared<Sphere>(Point3f(0.0f, -1000.0f, 0.0f), 1000.0f, groundMaterial));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float chooseMat = Math::GetRandom();
			Point3f center(static_cast<float>(a) + 0.9f * Math::GetRandom(), 0.2f, static_cast<float>(b) + 0.9f * Math::GetRandom());

			if ((center - Point3f(4.0f, 0.2f, 0.0f)).GetLengthSquared() > 0.81f)
			{
				IMaterial* sphereMaterial;

				if (chooseMat < 0.8f)	// diffuse
				{  
					sphereMaterial = new Lambertian(
						Color(Math::GetRandom() * Math::GetRandom(),
							Math::GetRandom() * Math::GetRandom(),
							Math::GetRandom() * Math::GetRandom())
					);
					outWorld.Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
				else if (chooseMat < 0.95f)	// metal
				{
					sphereMaterial = new Metal(
						Color(0.5f * (1.0f + Math::GetRandom()),
							0.5f * (1.0f + Math::GetRandom()),
							0.5f * (1.0f + Math::GetRandom())),
						0.5f * Math::GetRandom()
					);
					outWorld.Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
				else // glass
				{
					sphereMaterial = new Dielectric(1.5f);
					outWorld.Add(std::make_shared<Sphere>(center, 0.2f, sphereMaterial));
				}
				outMaterials.push_back(sphereMaterial);
			}
		}
	}

	Dielectric* material1 = new Dielectric(1.5);
	outMaterials.push_back(material1);
	outWorld.Add(std::make_shared<Sphere>(Point3f(0.0f, 1.0f, 0.0f), 1.0f, material1));

	Lambertian* material2 = new Lambertian(Color(0.4f, 0.2f, 0.1f));
	outMaterials.push_back(material2);
	outWorld.Add(std::make_shared<Sphere>(Point3f(-4.0f, 1.0f, 0.0f), 1.0f, material2));

	Metal* material3 = new Metal(Color(0.7f, 0.6f, 0.5f), 0.0f);
	outMaterials.push_back(material3);
	outWorld.Add(std::make_shared<Sphere>(Point3f(4.0f, 1.0f, 0.0f), 1.0f, material3));
}