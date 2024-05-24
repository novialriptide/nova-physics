/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "novaphysics/shape.h"
#include "novaphysics/vector.h"
#include "novaphysics/math.h"


/**
 * @file shape.c
 * 
 * @brief Collision shape implementations.
 */


nvShape *nvCircleShape_new(nvVector2 center, nv_float radius) {
    nvShape *shape = NV_NEW(nvShape);
    NV_MEM_CHECK(shape);

    shape->type = nvShapeType_CIRCLE;
    nvCircle *circle = &shape->circle;

    circle->center = center;
    circle->radius = radius;

    return shape;
}

nvShape *nvPolygonShape_new(nvArray *vertices, nvVector2 offset) {
    nvShape *shape = NV_NEW(nvShape);
    NV_MEM_CHECK(shape);

    shape->type = nvShapeType_POLYGON;
    nvPolygon *polygon = &shape->polygon;

    polygon->vertices = vertices;

    polygon->xvertices = nvArray_new();
    for (size_t i = 0; i < polygon->xvertices->size; i++)
        nvArray_add(polygon->xvertices, NV_VEC2_NEW(0.0, 0.0));

    polygon->normals = nvArray_new();
    for (size_t i = 0; i < polygon->vertices->size; i++) {
        nvVector2 va = NV_TO_VEC2(polygon->vertices->data[i]);
        nvVector2 vb = NV_TO_VEC2(polygon->vertices->data[(i + 1) % polygon->vertices->size]);
    
        nvVector2 face = nvVector2_sub(vb, va);
        nvVector2 normal = nvVector2_normalize(nvVector2_perpr(face));

        nvArray_add(polygon->normals, NV_VEC2_NEW(normal.x, normal.y));
    }

    return shape;
}

nvShape *nvRectShape_new(nv_float width, nv_float height, nvVector2 offset) {
    nv_float w = width / 2.0;
    nv_float h = height / 2.0;

    nvArray *vertices = nvArray_new();    
    nvArray_add(vertices, NV_VEC2_NEW(-w, -h));
    nvArray_add(vertices, NV_VEC2_NEW( w, -h));
    nvArray_add(vertices, NV_VEC2_NEW( w,  h));
    nvArray_add(vertices, NV_VEC2_NEW(-w,  h));

    return nvPolygonShape_new(vertices, offset);
}

nvShape *nvNGonShape_new(size_t n, nv_float radius, nvVector2 offset) {
    if (n < 3) {
        nv_set_error("Cannot create a polygon with fewer than 3 vertices.");
        return NULL;
    }

    nvArray *vertices = nvArray_new();
    nvVector2 arm = NV_VEC2(radius / 2.0, 0.0);

    for (size_t i = 0; i < n; i++) {
        nvArray_add(vertices, NV_VEC2_NEW(arm.x, arm.y));
        arm = nvVector2_rotate(arm, 2.0 * NV_PI / (nv_float)n);
    }

    return nvPolygonShape_new(vertices, offset);
}

nvShape *nvConvexHullShape_new(nvArray *points, nvVector2 offset) {
    if (points->size < 3) {
        nv_set_error("Cannot create a polygon with fewer than 3 vertices.");
        return NULL;
    }

    nvArray *vertices = nv_generate_convex_hull(points);

    // Transform hull vertices so the center of gravity is at centroid
    nvVector2 hull_centroid = nv_polygon_centroid(vertices);

    for (size_t i = 0; i < vertices->size; i++) {
        nvVector2 new_vert = nvVector2_sub(NV_TO_VEC2(vertices->data[i]), hull_centroid);
        nvVector2 *current_vert = NV_TO_VEC2P(vertices->data[i]);
        current_vert->x = new_vert.x;
        current_vert->y = new_vert.y;
    }

    return nvPolygonShape_new(vertices, offset);
}

void nvShape_free(nvShape *shape) {
    if (!shape) return;

    if (shape->type == nvShapeType_POLYGON) {
        nvArray_free_each(shape->polygon.vertices, free);
        nvArray_free(shape->polygon.vertices);
        nvArray_free_each(shape->polygon.xvertices, free);
        nvArray_free(shape->polygon.xvertices);
        nvArray_free_each(shape->polygon.normals, free);
        nvArray_free(shape->polygon.normals);
    }

    free(shape);
}

nvAABB nvShape_get_aabb(const nvShape *shape, nvVector2 position, nv_float angle) {
    NV_TRACY_ZONE_START;

    nv_float min_x;
    nv_float min_y;
    nv_float max_x;
    nv_float max_y;

    nvAABB aabb;

    switch (shape->type) {
        case nvShapeType_CIRCLE:
            aabb = (nvAABB){
                position.x - shape->circle.radius,
                position.y - shape->circle.radius,
                position.x + shape->circle.radius,
                position.y + shape->circle.radius
            };

            NV_TRACY_ZONE_END;
            return aabb;

        case nvShapeType_POLYGON:
            min_x = NV_INF;
            min_y = NV_INF;
            max_x = -NV_INF;
            max_y = -NV_INF;

            nvPolygon_transform(shape, position, angle);

            for (size_t i = 0; i < shape->polygon.xvertices->size; i++) {
                nvVector2 v = NV_TO_VEC2(shape->polygon.xvertices->data[i]);
                if (v.x < min_x) min_x = v.x;
                if (v.x > max_x) max_x = v.x;
                if (v.y < min_y) min_y = v.y;
                if (v.y > max_y) max_y = v.y;
            }

            aabb = (nvAABB){min_x, min_y, max_x, max_y};

            NV_TRACY_ZONE_END;
            return aabb;

        default:
            NV_TRACY_ZONE_END;
            return (nvAABB){0.0, 0.0, 0.0, 0.0};
    }
}

void nvPolygon_transform(nvShape *shape, nvVector2 position, nv_float angle) {
    NV_TRACY_ZONE_START;

    for (size_t i = 0; i < shape->polygon.vertices->size; i++) {
        nvVector2 new = nvVector2_add(position,
            nvVector2_rotate(
                NV_TO_VEC2(shape->polygon.vertices->data[i]),
                angle
                )
            );

        nvVector2 *xvert = NV_TO_VEC2P(shape->polygon.xvertices->data[i]);
        xvert->x = new.x;
        xvert->y = new.y;
    }

    NV_TRACY_ZONE_END;
}