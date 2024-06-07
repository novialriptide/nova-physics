/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "novaphysics/internal.h"
#include "novaphysics/shape.h"


/**
 * @file shape.c
 * 
 * @brief Collision shape implementations.
 */


/*
    A pretty cheap solution, but it works!
    No one would need over 4 billion shapes... right?
*/
static nv_uint32 id_counter;


nvShape *nvCircleShape_new(nvVector2 center, nv_float radius) {
    nvShape *shape = NV_NEW(nvShape);
    NV_MEM_CHECK(shape);
    shape->id = id_counter++;

    shape->type = nvShapeType_CIRCLE;
    nvCircle *circle = &shape->circle;

    circle->center = center;
    circle->radius = radius;

    return shape;
}

nvShape *nvPolygonShape_new(
    nvVector2 *vertices,
    size_t num_vertices,
    nvVector2 offset
) {
    nvShape *shape = NV_NEW(nvShape);
    NV_MEM_CHECK(shape);
    shape->id = id_counter++;

    if (num_vertices > NV_POLYGON_MAX_VERTICES) {
        nv_set_error("Exceeds maximum number of vertices per convex polygon shape.");
        return NULL;
    }

    if (num_vertices < 3) {
        nv_set_error("Cannot create a polygon shape with fewer than 3 vertices.");
        return NULL;
    }

    shape->type = nvShapeType_POLYGON;
    nvPolygon *polygon = &shape->polygon;
    polygon->num_vertices = num_vertices;

    for (size_t i = 0; i < num_vertices; i++)
        polygon->vertices[i] = nvVector2_add(vertices[i], offset);

    for (size_t i = 0; i < num_vertices; i++)
        polygon->xvertices[i] = nvVector2_zero;

    for (size_t i = 0; i < num_vertices; i++) {
        nvVector2 va = polygon->vertices[i];
        nvVector2 vb = polygon->vertices[(i + 1) % num_vertices];
    
        nvVector2 edge = nvVector2_sub(vb, va);
        nvVector2 normal = nvVector2_normalize(nvVector2_perpr(edge));

        polygon->normals[i] = normal;
    }

    return shape;
}

nvShape *nvRectShape_new(nv_float width, nv_float height, nvVector2 offset) {
    nv_float w = width / 2.0;
    nv_float h = height / 2.0;

    nvVector2 vertices[4] = {
        NV_VECTOR2(-w, -h),
        NV_VECTOR2( w, -h),
        NV_VECTOR2( w, h),
        NV_VECTOR2(-w, h)
    };

    return nvPolygonShape_new(vertices, 4, offset);
}

nvShape *nvNGonShape_new(size_t n, nv_float radius, nvVector2 offset) {
    if (n < 3) {
        nv_set_error("Cannot create a polygon shape with fewer than 3 vertices.");
        return NULL;
    }
    if (n > NV_POLYGON_MAX_VERTICES) {
        nv_set_error("Too many polygon vertices (check NV_POLYGON_MAX_VERTICES).");
        return NULL;
    }

    nvVector2 vertices[NV_POLYGON_MAX_VERTICES];
    nvVector2 arm = NV_VECTOR2(radius / 2.0, 0.0);

    for (size_t i = 0; i < n; i++) {
        vertices[i] = arm;
        arm = nvVector2_rotate(arm, 2.0 * NV_PI / (nv_float)n);
    }

    return nvPolygonShape_new(vertices, n, offset);
}

nvShape *nvConvexHullShape_new(nvArray *points, nvVector2 offset) {
    if (points->size < 3) {
        nv_set_error("Cannot create a polygon shape with fewer than 3 vertices.");
        return NULL;
    }

    // nvArray *vertices = nv_generate_convex_hull(points);

    // // Transform hull vertices so the center of gravity is at centroid
    // nvVector2 hull_centroid = nv_polygon_centroid(vertices);

    // for (size_t i = 0; i < vertices->size; i++) {
    //     nvVector2 new_vert = nvVector2_sub(NV_TO_VEC2(vertices->data[i]), hull_centroid);
    //     nvVector2 *current_vert = NV_TO_VEC2P(vertices->data[i]);
    //     current_vert->x = new_vert.x;
    //     current_vert->y = new_vert.y;
    // }

    // return nvPolygonShape_new(NULL, 0, offset);

    return NULL;
}

void nvShape_free(nvShape *shape) {
    if (!shape) return;

    free(shape);
}

nvAABB nvShape_get_aabb(nvShape *shape, nvTransform xform) {
    NV_TRACY_ZONE_START;

    nv_float min_x;
    nv_float min_y;
    nv_float max_x;
    nv_float max_y;

    nvAABB aabb;

    switch (shape->type) {
        case nvShapeType_CIRCLE:
            aabb = (nvAABB){
                xform.position.x - shape->circle.radius,
                xform.position.y - shape->circle.radius,
                xform.position.x + shape->circle.radius,
                xform.position.y + shape->circle.radius
            };

            NV_TRACY_ZONE_END;
            return aabb;

        case nvShapeType_POLYGON:
            min_x = NV_INF;
            min_y = NV_INF;
            max_x = -NV_INF;
            max_y = -NV_INF;

            nvPolygon_transform(shape, xform);

            for (size_t i = 0; i < shape->polygon.num_vertices; i++) {
                nvVector2 v = shape->polygon.xvertices[i];
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

void nvPolygon_transform(nvShape *shape, nvTransform xform) {
    NV_TRACY_ZONE_START;

    for (size_t i = 0; i < shape->polygon.num_vertices; i++) {
        nvVector2 new = nvVector2_add(xform.position,
            nvVector2_rotate(
                shape->polygon.vertices[i],
                xform.angle
                )
            );

        shape->polygon.xvertices[i] = new;
    }

    NV_TRACY_ZONE_END;
}