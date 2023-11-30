/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_SPACE_STEP_H
#define NOVAPHYSICS_SPACE_STEP_H

#include "novaphysics/internal.h"


/**
 * @file space_step.h
 * 
 * @brief Internal functions the space uses in a simulation step.
 */


// Hashing functions for space & broadphase hashmaps

nv_uint64 _nv_Space_resolution_hash(void *item);

nv_uint64 _nv_Space_broadphase_pair_hash(void *item);


/**
 * Apply forces, gravity, integrate accelerations (update velocities) and apply damping.
 */
void _nv_Space_integrate_accelerations(
    struct nv_Space *space,
    nv_float dt,
    size_t i
);

/**
 * Integrate velocities (update positions) and check out-of-bound bodies.
 */
void _nv_Space_integrate_velocities(
    struct nv_Space *space,
    nv_float dt,
    size_t i
);

#ifdef NV_AVX

    /**
     * Integrate accelerations using AVX float vectors.
     */
    void _nv_Space_integrate_accelerations_AVX(
            struct nv_Space *space,
            nv_float dt
    );

    /**
     * Integrate velocities using AVX float vectors.
     */
    void _nv_Space_integrate_velocities_AVX(
        struct nv_Space *space,
        nv_float dt
    );

#endif


#endif