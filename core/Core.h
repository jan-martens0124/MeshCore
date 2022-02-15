//
// Created by Jonas on 6/02/2022.
//

#ifndef OPTIXMESHCORE_CORE_H
#define OPTIXMESHCORE_CORE_H

#ifdef __CUDA_ARCH__
#include <crt/host_defines.h>
    #define MC_FUNC_QUALIFIER __host__ __device__ inline
#else
    #define MC_FUNC_QUALIFIER [[maybe_unused]] [[nodiscard]]
#endif

#endif //OPTIXMESHCORE_CORE_H
