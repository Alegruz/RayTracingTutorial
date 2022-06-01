#include "RayTracing.cuh"

////////////////////////////////////////////////////////////////////////////////
// GPU-specific defines
////////////////////////////////////////////////////////////////////////////////
//Maps to a single instruction on G8x / G9x / G10x
#define IMAD(a, b, c) ( __mul24((a), (b)) + (c) )

//Round a / b to nearest higher integer value
int iDivUp(int a, int b);

__global__
void cudaInitCurand(curandState* state, unsigned long seed);

__global__
void cudaGetRandom(curandState* state, float* outRandom);

__global__
void cudaRenderRowsDevice(
	uchar* outBackBuffer, 
	size_t width, 
	size_t height, 
	size_t numChannels, 
	size_t samplesPerPixel,
	CudaSphere* spheres,
	size_t numSpheres
);

__device__
void cudaAt(const CudaRay& ray, float scalar, float3& outAt);

__device__
float3 cudaGetRandomFloat3();

__device__
float3 cudaGetRandomFloat3(float min, float max);

__device__
float3 cudaGetRandomUnitSphereFloat3();

__device__
float3 cudaGetRandomUnitFloat3();

__device__
void cudaGetRay(float u, float v, CudaRay& outRay);

__device__
bool cudaHasHit(CudaSphere* spheres, size_t numSpheres, const CudaRay& ray, float minScalar, float maxScalar, CudaHitRecord& outRecord);

__device__
bool cudaHasHitSphere(const CudaSphere& sphere, const CudaRay& ray, float minScalar, float maxScalar, CudaHitRecord& outRecord);

__device__
void cudaSetFaceNormal(CudaHitRecord& hitRecord, const CudaRay& ray, const float3& outwardNormal);

__device__
void cudaRayColor(CudaSphere* spheres, size_t numSpheres, CudaRay& ray, uint depth, float3& outColor);

__device__
void cudaRayColor(CudaSphere* spheres, size_t numSpheres, CudaRay& ray, float3& outColor);

//__device__
//bool cudaScatter(const CudaLambertian& lambertian, const CudaRay& ray, const CudaHitRecord& hitRecord);

__device__
void cudaWriteColor(uint8_t* pixels, const float3& pixelColor, size_t samplesPerPixel);

#define ASPECT_RATIO (16.0f / 9.0f)
#define VIEWPORT_HEIGHT (2.0f)
#define VIEWPORT_WIDTH (ASPECT_RATIO * VIEWPORT_HEIGHT)
#define FOCAL_LENGTH (1.0f)
#define MAX_BOUNCE_DEPTH (50u)

__constant__ CudaCamera camera;
__constant__ curandState devState[3];

/***********************/
/* CUDA ERROR CHECKING */
/***********************/
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, char* file, int line, bool abort = true)
{
	if (code != cudaSuccess)
	{
		fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
		if (abort)
		{
			exit(code);
		}
	}
}

//Round a / b to nearest higher integer value
inline int iDivUp(int a, int b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

void CudaInitialize(CudaCamera* cudaCamera)
{
	checkCudaErrors(cudaMemcpyToSymbol(camera, cudaCamera, sizeof(CudaCamera)));

	//cudaInitCurand<<<1, 1>>>(&devState, 1);
	//gpuErrchk(cudaPeekAtLastError());
	//gpuErrchk(cudaDeviceSynchronize());
}

void CudaRender(uchar* outBackBuffer, size_t width, size_t height, size_t numChannels, size_t samplesPerPixel, CudaSphere* spheres, size_t numSpheres)
{
	dim3 threads(16, 12);
	dim3 blocks(iDivUp(static_cast<int>(width), static_cast<int>(threads.x)), iDivUp(static_cast<int>(height), static_cast<int>(threads.y)));
	printf("Cuda Render\n");
	// execute the kernel
	cudaRenderRowsDevice<<<blocks, threads>>>(
		outBackBuffer,
		width,
		height,
		numChannels,
		samplesPerPixel,
		spheres,
		numSpheres
		);
}

__global__
void cudaInitCurand(curandState* state, unsigned long seed)
{
	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	printf("curand init idx: %d\n", idx);
	curand_init(seed, idx, 0, &state[idx]);
	printf("curand init\n");
}

__global__
void cudaRenderRowsDevice(
	uchar* outBackBuffer, 
	size_t width, 
	size_t height, 
	size_t numChannels, 
	size_t samplesPerPixel, 
	CudaSphere* spheres, 
	size_t numSpheres
)
{
	const int ix = IMAD(blockDim.x, blockIdx.x, threadIdx.x);
	const int iy = (height - 1) - IMAD(blockDim.y, blockIdx.y, threadIdx.y);

	if (ix >= width || iy >= height || ix < 0 || iy < 0)
	{
		return;
	}

	//printf("Processing: %d, %d, samples: %llu, spheres: %llu\n", ix, iy, samplesPerPixel, numSpheres);

	curandState rgnState;
	curand_init(0ull, ix * iy, 0, &rgnState);

	float3 pixelColor = make_float3(0.0f);
	CudaRay ray;
	for (uint sampleIdx = 0; sampleIdx < samplesPerPixel; ++sampleIdx)
	{
		float u = (ix + curand_uniform(&rgnState)) / static_cast<float>(width - 1);
		float v = (iy + curand_uniform(&rgnState)) / static_cast<float>(height - 1);
		cudaGetRay(u, v, ray);
		float3 color = make_float3(0.0f);
		cudaRayColor(spheres, numSpheres, ray, color);
		pixelColor += color;
	}
	cudaWriteColor(&outBackBuffer[(height - 1 - iy) * width * numChannels + ix * numChannels], pixelColor, samplesPerPixel);
}

__device__
void cudaAt(const CudaRay& ray, float scalar, float3& outAt)
{
	outAt = ray.origin + scalar * ray.direction;
}

__device__
float3 cudaGetRandomFloat3()
{
	return cudaGetRandomFloat3(0.0f, 1.0f);
}

__device__
float3 cudaGetRandomFloat3(float min, float max)
{
	static unsigned long long idx = 0;
	curand_init(0, idx, 0, &devState[0]);
	curand_init(0, idx + 1, 0, &devState[1]);
	curand_init(0, idx + 2, 0, &devState[2]);

	float3 result = make_float3(
		curand_uniform(&devState[0]) * (max - min) + min,
		curand_uniform(&devState[1]) * (max - min) + min,
		curand_uniform(&devState[2]) * (max - min) + min
	);
	idx += 3;

	//printf("result: %f, %f, %f\n", result.x, result.y, result.z);

	return result;
}

__device__
float3 cudaGetRandomUnitSphereFloat3()
{
	while (true)
	{
		float3 vec = cudaGetRandomFloat3(-1.0f, 1.0f);

		if (lengthSquared(vec) >= 1.0f)
		{
			continue;
		}

		return vec;
	}
}

__device__
float3 cudaGetRandomUnitFloat3()
{
	float3 vec = cudaGetRandomUnitSphereFloat3();
	return normalize(vec);
}

__device__
void cudaGetRay(float u, float v, CudaRay& outRay)
{
	//outRay.origin = camera.at;

	float3 at = make_float3(0.0f, 0.0f, 0.0f);
	float3 horizontal = make_float3(VIEWPORT_WIDTH, 0.0f, 0.0f);
	float3 vertical = make_float3(0.0f, VIEWPORT_HEIGHT, 0.0f);
	float3 lowerLeftCorner = at - (horizontal / 2.0f) - (vertical / 2.0f) - make_float3(0.0f, 0.0f, FOCAL_LENGTH);

	outRay.origin = at;
	outRay.direction = lowerLeftCorner + u * horizontal + v * vertical - at;
}

__device__
bool cudaHasHit(CudaSphere* spheres, size_t numSpheres, const CudaRay& ray, float minScalar, float maxScalar, CudaHitRecord& outRecord)
{
	CudaHitRecord tempRecord;
	bool bHasHitAnything = false;
	float closestSoFar = maxScalar;

	// check spheres
	for (size_t objectIdx = 0; objectIdx < numSpheres; ++objectIdx)
	{
		if (cudaHasHitSphere(spheres[objectIdx], ray, minScalar, maxScalar, tempRecord))
		{
			bHasHitAnything = true;
			if (closestSoFar > tempRecord.scalar)
			{
				closestSoFar = tempRecord.scalar;
				outRecord = tempRecord;
			}
		}
	}

	return bHasHitAnything;
}

__device__
bool cudaHasHitSphere(const CudaSphere& sphere, const CudaRay& ray, float minScalar, float maxScalar, CudaHitRecord& outRecord)
{
	float3 centerToOrigin = ray.origin - sphere.center;
	float a = dot(ray.direction, ray.direction);
	float halfB = dot(centerToOrigin, ray.direction);
	float c = dot(centerToOrigin, centerToOrigin) - sphere.radius * sphere.radius;
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

	cudaAt(ray, root, outRecord.point);
	outRecord.scalar = root;
	float3 outwardNormal = (outRecord.point - sphere.center) / sphere.radius;
	cudaSetFaceNormal(outRecord, ray, outwardNormal);

	return true;
}

__device__
void cudaSetFaceNormal(CudaHitRecord& hitRecord, const CudaRay& ray, const float3& outwardNormal)
{
	hitRecord.bIsFrontFacing = dot(ray.direction, outwardNormal) < 0.0f;
	hitRecord.normal = (2.0f * static_cast<float>(hitRecord.bIsFrontFacing) - 1.0f) * outwardNormal;
}

__device__
void cudaRayColor(CudaSphere* spheres, size_t numSpheres, CudaRay& ray, uint depth, float3& outColor)
{
	CudaHitRecord record;

	if (depth == 0u)
	{
		//printf("depth: %u\n", depth);
		outColor = make_float3(0.0f);
		return;
	}

	if (cudaHasHit(spheres, numSpheres, ray, 0.000001f, INFINITY, record))
	{
		float3 target = record.point + record.normal + cudaGetRandomUnitFloat3();
		//outColor = (record.normal + make_float3(1.0f, 1.0f, 1.0f)) * 0.5f;

		ray.origin = record.point;
		ray.direction = target - record.point;
		//outColor *= 0.5f;
		//printf("[%u] has hit color: %f, %f, %f\n", depth, outColor.x, outColor.y, outColor.z);
		cudaRayColor(spheres, numSpheres, ray, depth - 1u, outColor);
		outColor *= 0.5f;
		return;
	}

	float3 unitDirection = normalize(ray.direction);
	float scalar = 0.5f * (unitDirection.y + 1.0f);

	outColor = lerp(make_float3(1.0f, 1.0f, 1.0f), make_float3(0.5f, 0.7f, 1.0f), scalar);
}

__device__
void cudaRayColor(CudaSphere* spheres, size_t numSpheres, CudaRay& ray, float3& outColor)
{
	cudaRayColor(spheres, numSpheres, ray, MAX_BOUNCE_DEPTH, outColor);
}

__device__
void cudaWriteColor(uint8_t* pixels, const float3& pixelColor, size_t samplesPerPixel)
{
	float scale = 1.0f / static_cast<float>(samplesPerPixel);
	float r = sqrt(pixelColor.x * scale);
	float g = sqrt(pixelColor.y * scale);
	float b = sqrt(pixelColor.z * scale);

	pixels[0] = static_cast<uint8_t>(255.999999f * clamp(r, 0.0f, 0.999999f));
	pixels[1] = static_cast<uint8_t>(255.999999f * clamp(g, 0.0f, 0.999999f));
	pixels[2] = static_cast<uint8_t>(255.999999f * clamp(b, 0.0f, 0.999999f));
}