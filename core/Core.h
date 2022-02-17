//
// Created by Jonas on 6/02/2022.
//

#ifndef OPTIXMESHCORE_CORE_H
#define OPTIXMESHCORE_CORE_H

#ifdef __CUDA_ARCH__
#include <cuda_runtime.h>
    #define MC_FUNC_QUALIFIER __host__ __device__
#else
    #define MC_FUNC_QUALIFIER [[maybe_unused]]
#endif

#endif //OPTIXMESHCORE_CORE_H
