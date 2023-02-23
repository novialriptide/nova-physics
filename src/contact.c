/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "novaphysics/contact.h"
#include "novaphysics/collision.h"
#include "novaphysics/array.h"
#include "novaphysics/constants.h"
#include "novaphysics/math.h"


/**
 * contact.c
 * 
 * Contact point calculation functions
 */


void nv_contact_circle_x_circle(nv_Resolution *res) {
    nv_Vector2 dir = nv_Vector2_sub(res->b->position, res->a->position);
    // If the bodies are in the exact same position, direct the normal upwards
    if (nv_Vector2_len2(dir) == 0.0) dir = NV_VEC2(0.0, 1.0);
    dir = nv_Vector2_normalize(dir);

    nv_Vector2 cp = nv_Vector2_add(res->a->position, nv_Vector2_muls(dir, res->a->radius));

    res->contact_count = 1;
    res->contacts[0] = cp;
}

void nv_contact_polygon_x_circle(nv_Resolution *res) {
    nv_Body *polygon;
    nv_Body *circle;

    if (res->a->shape == nv_BodyShape_POLYGON) {
        polygon = res->a;
        circle = res->b;
    } else {
        polygon = res->b;
        circle = res->a;
    }

    nv_Vector2 cp;
    double min_dist = NV_INF;

    nv_Polygon_model_to_world(polygon);
    nv_Array *vertices = polygon->trans_vertices;
    size_t n = vertices->size;

    double dist;
    nv_Vector2 contact;

    for (size_t i = 0; i < n; i++) {
        nv_Vector2 va = NV_TO_VEC2(vertices->data[i]);
        nv_Vector2 vb = NV_TO_VEC2(vertices->data[(i + 1) % n]);

        nv_point_segment_dist(circle->position, va, vb, &dist, &contact);

        if (dist < min_dist) {
            min_dist = dist;
            cp = contact;
        }
    }

    res->contact_count = 1;
    res->contacts[0] = cp;
}


// TODO: Implement polygon face clipping to collect contact points


bool segment_intersect(nv_Vector2 a1, nv_Vector2 a2, nv_Vector2 b1, nv_Vector2 b2, nv_Vector2 *c) {
    double x1 = a1.x;
    double y1 = a1.y;
    double x2 = a2.x;
    double y2 = a2.y;
    double x3 = b1.x;
    double y3 = b1.y;
    double x4 = b2.x;
    double y4 = b2.y;

    double denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    if (denom == 0.0) return false; // Parallel

    double ua = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / denom;
    if (ua < 0.0 || ua > 1.0) return false; // Out of range

    double ub = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / denom;
    if (ub < 0.0 || ub > 1.0) return false; // Out of range

    *c = NV_VEC2(x1 + ua * (x2 - x1), y1 + ua * (y2 - y1));
    return true;
}


void nv_contact_polygon_x_polygon(nv_Resolution *res) {

    res->contact_count = 0;

    nv_Polygon_model_to_world(res->a);
    nv_Polygon_model_to_world(res->b);
    nv_Array *vertices_a = res->a->trans_vertices;
    nv_Array *vertices_b = res->b->trans_vertices;
    size_t na = vertices_a->size;
    size_t nb = vertices_b->size;

    for (size_t i = 0; i < na; i++) {
        nv_Vector2 va1 = NV_TO_VEC2(vertices_a->data[i]);
        nv_Vector2 va2 = NV_TO_VEC2(vertices_a->data[(i + 1) % na]);

        for (size_t j = 0; j < nb; j++) {
            nv_Vector2 vb1 = NV_TO_VEC2(vertices_b->data[j]);
            nv_Vector2 vb2 = NV_TO_VEC2(vertices_b->data[(j + 1) % nb]);

            nv_Vector2 c;
            bool intersect = segment_intersect(va1, va2, vb1, vb2, &c);
            if (intersect) {
                res->contacts[res->contact_count] = c;
                res->contact_count++;

                if (res->contact_count == 2) return;
            }
        }
    }
}