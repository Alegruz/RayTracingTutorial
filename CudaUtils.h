#pragma once

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

#include "helper_functions.h" // helper utility functions 
#include "helper_cuda.h"      // helper functions for CUDA error checking and initialization

enum GPU_SELECT_MODE
{
	FIRST_PCI_BUS_ID,
	LAST_PCI_BUS_ID,
	MAX_GFLOPS,
	MIN_GFLOPS,
	SPECIFIED_DEVICE_ID,
	GPU_SELECT_MODE_LAST = SPECIFIED_DEVICE_ID
};

struct GPU_INFO
{
	char		szDeviceName[256];
	uint32_t	sm_per_multiproc;
	uint32_t	clock_rate;
	uint32_t	multiProcessorCount;
	float		TFlops;
};


//void ReportGPUMemeoryUsage();
void VerifyCudaError(cudaError err);