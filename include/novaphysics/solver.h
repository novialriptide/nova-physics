/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_SOLVER_H
#define NOVAPHYSICS_SOLVER_H

#include "novaphysics/internal.h"
#include "novaphysics/body.h"
#include "novaphysics/collision.h"
#include "novaphysics/resolution.h"
#include "novaphysics/constraint.h"


/**
 * @file solver.h
 * 
 * @brief Collision solver functions.
 */


/**
 * @brief Coefficient mixing type is the method to mix various coefficients
 *        values like restitution and friction.
 */
typedef enum {
    nv_CoefficientMix_AVG, /**< (a + b) / 2 */
    nv_CoefficientMix_MUL, /**< a * b */
    nv_CoefficientMix_SQRT, /**< sqrt(a * b) */
    nv_CoefficientMix_MIN, /**< min(a, b) */
    nv_CoefficientMix_MAX /**< max(a, b) */
} nv_CoefficientMix;

/**
 * @brief Mix two coefficient values.
 * 
 * @param a First value
 * @param b Second value
 * @param mix Mixing type
 * @return nv_float 
 */
static inline nv_float nv_mix_coefficients(nv_float a, nv_float b, nv_CoefficientMix mix) {
    switch (mix) {
        case nv_CoefficientMix_AVG:
            return (a + b) / 2.0;

        case nv_CoefficientMix_MUL:
            return a * b;

        case nv_CoefficientMix_SQRT:
            return nv_sqrt(a * b);

        case nv_CoefficientMix_MIN:
            return nv_fmin(a, b);

        case nv_CoefficientMix_MAX:
            return nv_fmax(a, b);

        default:
            NV_ERROR("Unknown coefficient mixing function.");
            return 0.0;
    }
}


/**
 * @brief Prepare for solving collision resolution.
 * 
 * @param space Space
 * @param res Collision resolution
 * @param inv_dt Inverse delta time (1/Δt)
 */
void nv_presolve_collision(
    struct nvSpace *space,
    nvResolution *res,
    nv_float inv_dt
);

/**
 * @brief Apply accumulated impulses.
 * 
 * @param space Space
 * @param res Collision resolution
 */
void nv_warmstart(struct nvSpace *space, nvResolution *res);

/**
 * @brief Solve positions (pseudo-velocities).
 * 
 * @param res Collision resolution
 */
void nv_solve_position(nvResolution *res);

/**
 * @brief Solve velocities.
 * 
 * @param res Collision resolution
 */
void nv_solve_velocity(nvResolution *res);


/**
 * @brief Prepare for solving.
 * 
 * @param space Space
 * @param cons Constraintt
 * @param inv_dt Inverse delta time (1/Δt)
 */
void nv_presolve_constraint(
    struct nvSpace *space,
    nvConstraint *cons,
    nv_float inv_dt
);

/**
 * @brief Solve constraint.
 * 
 * @param cons Constraint
 * @param inv_dt Inverse delta time (1/Δt)
 */
void nv_solve_constraint(nvConstraint *cons, nv_float inv_dt);


#endif