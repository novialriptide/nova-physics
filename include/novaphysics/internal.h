/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_INTERNAL_H
#define NOVAPHYSICS_INTERNAL_H

#include <stdlib.h>
#include <string.h>


/**
 * @file internal.h
 * 
 * @brief Nova Physics internal API header.
 */


#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)

    #define NV_WINDOWS

#endif

#if defined(__EMSCRIPTEN__) || defined(__wasi__)

    #define NV_WEB

#endif

#if defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)

    #define NV_COMPILER_GCC

#elif defined(_MSC_VER) || defined(_MSC_FULL_VER) || defined(_MSVC_LANG)

    #define NV_COMPILER_MSVC

#endif


#include "novaphysics/types.h"
#include "novaphysics/constants.h"
#include "novaphysics/core/error.h"


// Align memory as given byte range. Needed for some SIMD functions.

#if defined(NV_COMPILER_GCC)

    #define NV_ALIGNED_AS(x) __attribute__((aligned(x)))

#elif defined(NV_COMPILER_MSVC)

    #define NV_ALIGNED_AS(x) __declspec(align(x))

#else

    #define NV_ALIGNED_AS(x)

#endif


/*
    Profiling macros.
*/
#ifdef NV_ENABLE_PROFILER

    #define NV_PROFILER_START(timer) (nvPrecisionTimer_start(&timer))
    #define NV_PROFILER_STOP(timer, field) (field = nvPrecisionTimer_stop(&timer))

#else

    #define NV_PROFILER_START(timer)
    #define NV_PROFILER_STOP(timer, field)

#endif


// This is forward declared to prevent circular includes
struct nvSpace;


// Utility macro to allocate on HEAP
#define NV_NEW(type) ((type *)malloc(sizeof(type)))


#define NV_MEM_CHECK(object) {                      \
    if (!(object)) {                                \
        nv_set_error("Failed to allocate memory."); \
        return NULL;                                \
    }                                               \
}                                                   \

#define NV_MEM_CHECKI(object) {                     \
    if (!(object)) {                                \
        nv_set_error("Failed to allocate memory."); \
        return 1;                                   \
    }                                               \
}                                                   \


/*
    Internal Tracy Profiler macros.
*/
#ifdef TRACY_ENABLE

    #include "../../src/tracy/TracyC.h"

    #define NV_TRACY_ZONE_START TracyCZone(_tracy_zone, true)
    #define NV_TRACY_ZONE_END TracyCZoneEnd(_tracy_zone)
    #define NV_TRACY_FRAMEMARK TracyCFrameMark

#else

    #define NV_TRACY_ZONE_START
    #define NV_TRACY_ZONE_END
    #define NV_TRACY_FRAMEMARK

#endif


#endif