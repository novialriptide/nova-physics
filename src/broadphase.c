/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "novaphysics/broadphase.h"
#include "novaphysics/core/array.h"
#include "novaphysics/aabb.h"
#include "novaphysics/space.h"


/**
 * @file broadphase.c
 * 
 * @brief Broad-phase algorithms.
 */


/**
 * @brief Early-out from checking collisions.
 */
static inline nv_bool nvBroadPhase_early_out(nvSpace *space, nvRigidBody *a, nvRigidBody *b) {
    // Same body or already checked
    if (a->id >= b->id)
        return true;

    // One of the bodies have collision detection disabled
    if (!a->collision_enabled || !b->collision_enabled)
        return true;

    // Two static bodies do not need to interact
    if (a->type == nvRigidBodyType_STATIC && b->type == nvRigidBodyType_STATIC)
        return true;

    // Bodies share the same non-zero group
    if (a->collision_group == b->collision_group && a->collision_group != 0)
        return true;

    // One of the collision mask doesn't fit the category
    if ((a->collision_mask & b->collision_category) == 0 ||
        (b->collision_mask & a->collision_category) == 0)
        return true;

    return false;
}


void nv_broadphase_brute_force(nvSpace *space) {
    NV_TRACY_ZONE_START;

    nvArray_clear(space->broadphase_pairs, free);

    for (size_t i = 0; i < space->bodies->size; i++) {
        nvRigidBody *a = (nvRigidBody *)space->bodies->data[i];
        nvTransform xform_a = (nvTransform){a->origin, a->angle};
        nvAABB abox = nvRigidBody_get_aabb(a);

        for (size_t j = 0; j < space->bodies->size; j++) {
            nvRigidBody *b = (nvRigidBody *)space->bodies->data[j];

            if (nvBroadPhase_early_out(space, a, b)) continue;

            nvTransform xform_b = (nvTransform){b->origin, b->angle};
            nvAABB bbox = nvRigidBody_get_aabb(b);

            // At least one AABB of one shape is overlapping
            nv_bool one_aabb = false;

            if (nv_collide_aabb_x_aabb(abox, bbox)) {

                for (size_t k = 0; k < a->shapes->size; k++) {
                    nvShape *shape_a = a->shapes->data[k];
                    nvAABB sabox = nvShape_get_aabb(shape_a, xform_a);

                    for (size_t l = 0; l < b->shapes->size; l++) {
                        nvShape *shape_b = b->shapes->data[l];
                        nvAABB sbbox = nvShape_get_aabb(shape_b, xform_b);

                        if (nv_collide_aabb_x_aabb(sabox, sbbox)) {
                            one_aabb = true;
                            break;
                        }
                    }

                    if (one_aabb)
                        break;
                }
            }

            if (one_aabb) {
                // TODO: handle allocation error
                //       or pre-allocate an arena for broad-phase pairs
                nvBroadPhasePair *pair = NV_NEW(nvBroadPhasePair);
                pair->a = a;
                pair->b = b;
                nvArray_add(space->broadphase_pairs, pair);
            }

            // AABBs are not touching, destroy any contact
            // TODO: do this ONCE the AABBs stop overlapping
            else {
                for (size_t k = 0; k < a->shapes->size; k++) {
                    nvShape *shape_a = a->shapes->data[k];

                    for (size_t l = 0; l < b->shapes->size; l++) {
                        nvShape *shape_b = b->shapes->data[l];

                        nvPersistentContactPair *key = &(nvPersistentContactPair){.shape_a=shape_a, .shape_b=shape_b};

                        nvPersistentContactPair *pcp = nvHashMap_get(space->contacts, key);
                        if (pcp) {
                            for (size_t c = 0; c < pcp->contact_count; c++) {
                                nvContact *contact = &pcp->contacts[c];

                                nvContactEvent event = {
                                    .body_a = pcp->body_a,
                                    .body_b = pcp->body_b,
                                    .shape_a = pcp->shape_a,
                                    .shape_b = pcp->shape_b,
                                    .normal = pcp->normal,
                                    .penetration = contact->separation,
                                    .position = nvVector2_add(pcp->body_a->position, contact->anchor_a),
                                    .normal_impulse = contact->solver_info.normal_impulse,
                                    .friction_impulse = contact->solver_info.tangent_impulse,
                                    .id = contact->id
                                };

                                if (space->listener && !contact->remove_invoked) {
                                    if (space->listener->on_contact_removed)
                                        space->listener->on_contact_removed(event, space->listener_arg);
                                    contact->remove_invoked = true;
                                };
                            }

                            nvHashMap_remove(space->contacts, key);
                        }
                    }
                }
            }
        }
    }

    NV_TRACY_ZONE_END;
}