/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#ifndef NOVAPHYSICS_SPACE_SETTINGS_H
#define NOVAPHYSICS_SPACE_SETTINGS_H

#include "novaphysics/internal.h"
#include "novaphysics/constraints/constraint.h"


/**
 * @file space_settings.h
 * 
 * @brief Physics space simulation settings.
 */


/**
 * @brief Space settings struct.
 */
typedef struct {
    nv_float baumgarte; /**< Baumgarte stabilization factor is used to correct constraint erros in the iterative solver. */

    nv_float penetration_slop; /**< Amount of penetration error allowed in position correction. */

    nvContactPositionCorrection contact_position_correction; /**< Position correction method to use for collisions. */

    nv_uint32 velocity_iterations; /**< Iteration amount used by PGS solver for velocity constraints,
                                        for a game 6-10 should be sufficient. */

    nv_uint32 position_iterations; /**< NGS iteration count for contact position correction only,
                                        for a game 3-6 should be sufficient. */

    nv_uint32 substeps; /**< This defines how many substeps the current simulation step is going to get
                             divided into. This effectively increases the accuracy of the simulation but
                             also impacts the performance greatly because the whole simulation is processed 
                             and collisions are recalculated by given amounts of times internally. In a game,
                             you wouldn't need this much detail. Best to leave it at 1. */

    nv_float linear_damping; /**< Amount of damping applied to linear motion.
                                  It is required to remove potential energy
                                  added trough numerical instability.
                                  The final damping value is calculated as
                                  `0.99 ^ (r * d)` where `d` is this value and
                                  `r` is damping ratio of a rigid body, usually 1.
                                  You can change damping ratios of individual bodies
                                  in order to have them lose energy more. */

    nv_float angular_damping; /**< Same as `linear_damping` but for angular motion. */

    nv_bool warmstarting; /**< Whether to allow warmstarting constraints or not.
                               This is a really neat feature of Gauss-Seidel based solvers that
                               allows to have greatly increased stability with little overhead.
                               Warmstarting is basically using the last frame's solutions
                               for constraints as the starting guess in the solver.
                               For a game, you don't really have any reason to turn it off. */

    nvCoefficientMix restitution_mix; /**< Mixing function used for restitution. */

    nvCoefficientMix friction_mix; /**< Mixing function used for friction. */

} nvSpaceSettings;


#endif