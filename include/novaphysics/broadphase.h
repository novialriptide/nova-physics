/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_BROADPHASE_H
#define NOVAPHYSICS_BROADPHASE_H

#include "novaphysics/internal.h"
#include "novaphysics/body.h"


/**
 * @file broadphase.h
 * 
 * @details Broad-phase algorithms
 */


/**
 * @brief Algorithm used to do broad-phase collision detection
 * 
 * @param BRUTE_FORCE Brute-force
 * @param SPATIAL_HASH_GRID Spatial hash grid
 */
typedef enum {
    nv_BroadPhase_BRUTE_FORCE,
    nv_BroadPhase_SPATIAL_HASH_GRID
} nv_BroadPhase;

/**
 * @brief Brute-force algorithm
 * 
 * @param space Space
 */
void nv_BroadPhase_brute_force(struct _nv_Space *space);

/**
 * @brief Spatial hash grid algorithm
 * 
 * @param space Space
 */
void nv_BroadPhase_spatial_hash_grid(struct _nv_Space *space);


/**
 * @brief Expensive narrow-phase function that is called after making sure
 *        bodies could be colliding. Updates resolutions.
 * 
 * @param space Space
 * @param a Body A
 * @param b Body B
 * @param res_exists Whether the resolution already exists or not 
 * @param res_index Existing resolution's index
 */
void nv_narrow_phase(
    struct _nv_Space *space,
    nv_Body *a,
    nv_Body *b,
    bool res_exists,
    size_t res_index
);


/**
 * @brief An internal function used by broad-phase algorithms to check if
 *        a duplicate exists in space's resolution array
 * 
 * @param res Resolution array
 * @param a Body A
 * @param b Body B
 * @param index Resolution index
 * @return bool
 */
bool nv_check_resolution_duplicate(
    nv_Array *res,
    nv_Body *a,
    nv_Body *b,
    size_t *index
);


/**
 * Internal body pair struct used by brute-force algorithm
 */
typedef struct {
    nv_Body *a;
    nv_Body *b;
} nv_BodyPair;


#endif