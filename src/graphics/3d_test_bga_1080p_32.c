#include <system.h> 
#include <chess.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

// #define SCREEN_WIDTH 320
// #define SCREEN_HEIGHT 200

struct vec3d
{
    float x, y, z;
};

struct triangle
{
    struct vec3d p[3];
};

struct mesh
{
    struct triangle tris[12];
};

struct mat4x4
{
    float m[4][4];
};

#define PI 3.14159265358979323846

float sinf(float x) {
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    float term = x, sum = x;
    int i;
    for (i = 1; i <= 10; i++) {
        term *= -1 * x * x / ((2 * i) * (2 * i + 1));
        sum += term;
    }
    return sum;
}

float cosf(float x) {
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    float term = 1, sum = 1;
    int i;
    for (i = 1; i <= 10; i++) {
        term *= -1 * x * x / ((2 * i - 1) * (2 * i));
        sum += term;
    }
    return sum;
}

float tanf(float x) {
    float cos_val = cosf(x);
    if (cos_val == 0) {
        return 0;
    }
    return sinf(x) / cos_val;
}
void multiply_matrix_vector(struct vec3d *i, struct vec3d *o, struct mat4x4 *m)
{

    o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
    o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
    o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
    float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3];

    if (w != 0.0)
    {
        o->x /= w;
        o->y /= w;
        o->z /= w;
    }
}

void enter_3d_test() {
    bga_set_video_mode(1920, 1080, 32, 1, 1);
    // vga_enter();

    while (true) {
        // rotation
        struct mat4x4 matRotZ, matRotX = {0};

        float fTheta = 1.0f * (float)(test * 0.1f);

        // rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5);
        matRotX.m[1][2] = sinf(fTheta * 0.5);
        matRotX.m[2][1] = -sinf(fTheta * 0.5);
        matRotX.m[2][2] = cosf(fTheta * 0.5);
        matRotX.m[3][3] = 1;

        struct mesh cube_mesh = {{
            // south
            {{{0, 0, 0}, {0, 1, 0}, {1, 1, 0}}},
            {{{0, 0, 0}, {1, 1, 0}, {1, 0, 0}}},
            // east
            {{{1, 0, 0}, {1, 1, 0}, {1, 1, 1}}},
            {{{1, 0, 0}, {1, 1, 1}, {1, 0, 1}}},
            // north
            {{{1, 0, 1}, {1, 1, 1}, {0, 1, 1}}},
            {{{1, 0, 1}, {0, 1, 1}, {0, 0, 1}}},
            // west
            {{{0, 0, 1}, {0, 1, 1}, {0, 1, 0}}},
            {{{0, 0, 1}, {0, 1, 0}, {0, 0, 0}}},
            // top
            {{{0, 1, 0}, {0, 1, 1}, {1, 1, 1}}},
            {{{0, 1, 0}, {1, 1, 1}, {1, 1, 0}}},
            // bottom
            {{{0, 0, 0}, {0, 0, 1}, {1, 0, 1}}},
            {{{0, 0, 0}, {1, 0, 1}, {1, 0, 0}}}
        }};

        // projection matrix
        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectRatio = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * (float)PI);

        struct mat4x4 mat_proj = {0};

        mat_proj.m[0][0] = fAspectRatio * fFovRad;
        mat_proj.m[1][1] = fFovRad;
        mat_proj.m[2][2] = fFar / (fFar - fNear);
        mat_proj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        mat_proj.m[2][3] = 1.0f;

        // draw triangles
        for (int i = 0; i < 12; i++) {
            struct triangle triProjected = {0};
            struct triangle triRotatedZ = {0};
            struct triangle triRotatedZX = {0};

            // loop through each point in the triangle
            for (int j = 0; j < 3; j++) {
                struct vec3d p = cube_mesh.tris[i].p[j];

                multiply_matrix_vector(&p, &triRotatedZ.p[j], &matRotZ);
                multiply_matrix_vector(&triRotatedZ.p[j], &triRotatedZX.p[j], &matRotX);

                struct vec3d pTranslated = triRotatedZX.p[j];
                pTranslated.z = triRotatedZX.p[j].z + 3.0f;

                multiply_matrix_vector(&pTranslated, &triProjected.p[j], &mat_proj);

                triProjected.p[j].x /= triProjected.p[j].z;
                triProjected.p[j].y /= triProjected.p[j].z;

                triProjected.p[j].x = (triProjected.p[j].x + 1) * (SCREEN_WIDTH / 2);
                triProjected.p[j].y = (triProjected.p[j].y + 1) * (SCREEN_HEIGHT / 2);
            }

            bga_draw_triangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, 0xffffff, true);
            // draw_triangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, VGA_COLOR_WHITE, true);

        }
        bga_print_frame_buffer();
        // vga_print_frame_buffer();
        // sleep(5);
    }
}
