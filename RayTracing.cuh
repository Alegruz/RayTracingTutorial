#pragma once

#include <cuda_runtime.h>

#include <curand.h>
#include <curand_kernel.h>

#include <helper_cuda.h>
#include <helper_math.h>

typedef unsigned char uchar;
typedef unsigned int uint;

struct CudaSphere;

struct CudaCamera
{
	float3 at;
	float3 horizontal;
	float3 vertical;
	float3 lowerLeftCorner;
};

struct CudaLambertian;
struct CudaMetal;

struct CudaHitRecord
{
	float3 point;
	float3 normal;
	CudaLambertian* lambertian;
	CudaMetal* metal;
	float scalar;
	bool bIsFrontFacing;
	char padding[3];
};

struct CudaLambertian
{
	float3 albedo;
};

struct CudaSphere
{
	float3 center;
	float radius;
	CudaLambertian* lambertian;
	CudaMetal* metal;
};

struct CudaRay
{
	float3 origin;
	float3 direction;
};

void CudaInitialize(CudaCamera* cudaCamera);

void CudaRender(uchar* outBackBuffer, size_t width, size_t height, size_t numChannels, size_t samplesPerPixel, CudaSphere* spheres, size_t numSpheres);