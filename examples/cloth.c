/*

  This file is a part of the Nova Physics Engine
  project and distributed under the MIT license.

  Copyright © Kadir Aksoy
  https://github.com/kadir014/nova-physics

*/

#include "example_base.h"


void setup(Example *example) {

    // Basically disable broadphase
    nv_Space_set_SHG(example->space, (nv_AABB){0.0, 0.0, 1.0, 1.0}, 1.0, 1.0);
    
    int cols = 50;
    int rows = 50;
    nv_float size = 0.75;
    nv_float gap = 0.3;

    for (nv_float y = 0.0; y < rows; y++) {
        for (nv_float x = 0.0; x < cols; x++) {

            nv_BodyType type;
            if ((y == 0.0 && x == 0.0) || (y == 0.0 && x == cols - 1)) type = nv_BodyType_STATIC;
            else type = nv_BodyType_DYNAMIC;
            type = nv_BodyType_DYNAMIC;

            nv_Body *ball = nv_Circle_new(
                type,
                NV_VEC2(
                    64.0 + x * (size + gap) - ((size + gap) * (nv_float)cols / 2.0),
                    y * (size + gap) + 10.0
                ),
                0.0,
                (nv_Material){0.3, 0.0, 0.0},
                size / 2.0
            );
            ball->collision = false;
            nv_Space_add(example->space, ball);
        }
    }

    nv_Constraint *link;
    nv_float link_stiffness = 600.0;
    nv_float link_damping = 5.0;
    bool use_springs = true;

    for (size_t y = 0; y < rows; y++) {
        for (size_t x = 0; x < cols; x++) {
            if (x > 0) {
                nv_Body *body0 = example->space->bodies->data[y * rows + x + 1];
                nv_Body *body1 = example->space->bodies->data[y * rows + (x - 1) + 1];

                if (use_springs) {
                    link = nv_Spring_new(
                        body0, body1,
                        nv_Vector2_zero, nv_Vector2_zero,
                        size + gap,
                        link_stiffness, link_damping
                    );
                }
                else {
                    link = nv_DistanceJoint_new(
                        body0, body1,
                        nv_Vector2_zero, nv_Vector2_zero,
                        size + gap
                    );
                }

                nv_Space_add_constraint(example->space, link);
            }

            if (y > 0) {
                nv_Body *body0 = example->space->bodies->data[(y - 1) * rows + x + 1];
                nv_Body *body1 = example->space->bodies->data[y * rows + x + 1];

                if (use_springs) {
                    link = nv_Spring_new(
                        body0, body1,
                        nv_Vector2_zero, nv_Vector2_zero,
                        size + gap,
                        link_stiffness, link_damping
                    );
                }
                else {
                    link = nv_DistanceJoint_new(
                        body0, body1,
                        nv_Vector2_zero, nv_Vector2_zero,
                        size + gap
                    );
                }

                nv_Space_add_constraint(example->space, link);
            }

            else {
                nv_Body *body0 = NULL;
                nv_Body *body1 = example->space->bodies->data[y * rows + x + 1];

                if (use_springs) {
                    link = nv_Spring_new(
                        body0, body1,
                        NV_VEC2(body1->position.x, body1->position.y - size - gap), nv_Vector2_zero,
                        size + gap,
                        link_stiffness, link_damping
                    );
                }
                else {
                    link = nv_DistanceJoint_new(
                        body0, body1,
                        NV_VEC2(body1->position.x, body1->position.y - size - gap), nv_Vector2_zero,
                        size + gap
                    );
                }

                nv_Space_add_constraint(example->space, link);
            }
        }
    }

    // Apply horizontal force to some cloth nodes
    for (size_t i = 0; i < example->space->bodies->size; i++) {
        if (i > 1000) {
            nv_Body *body = example->space->bodies->data[i];
            nv_Body_apply_force(body, NV_VEC2(frand(-70.0, 350.0), frand(-100.0, 200.0)));
        }
    }
    
}


int main(int argc, char *argv[]) {
    // Create example
    Example *example = Example_new(
        1280, 720,
        "Nova Physics  -  Cloth Example",
        165.0,
        1.0/60.0,
        ExampleTheme_DARK
    );

    // Set callbacks
    example->setup_callback = setup;

    // Run the example
    Example_run(example);

    // Free the space allocated by example
    Example_free(example);

    return 0;
}