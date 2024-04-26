#include <system.h> 
#include <chess.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

float sinf(float x) {
    float term = x, sum = x;
    int i;
    for (i = 1; i <= 10; i++) {
        term *= -1 * x * x / ((2 * i + 1) * (2 * i));
        sum += term;
    }
    return sum;
}

float cosf(float x) {
    float term = 1, sum = 1;
    int i;
    for (i = 1; i <= 10; i++) {
        term *= -1 * x * x / ((2 * i) * (2 * i - 1));
        sum += term;
    }
    return sum;
}

float tanf(float x) {
    return sinf(x) / cosf(x);
}

struct vec3d
{
    float x;
    float y;
    float z;
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

    int test = 0;
    while (true) {
        struct mat4x4 matRotZ, matRotX = {0};

        float fTheta = 1.0f * (float)(test * 0.1f);

        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

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
        float fNear = 0.1;
        float fFar = 1000.0;
        float fFov = 90.0;
        float fAspectRatio = (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH;
        float fFovRad = 1.0 / tanf(fFov * 0.5 / 180.0 * 3.14159);

        struct mat4x4 mat_proj = {0};

        mat_proj.m[0][0] = fAspectRatio * fFovRad;
        mat_proj.m[1][1] = fFovRad;
        mat_proj.m[2][2] = fFar / (fFar - fNear);
        mat_proj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        mat_proj.m[2][3] = 1.0;

        for (int i = 0; i < 12; i++) {
            struct triangle triProjected = {0};
            struct triangle triRotatedZ = {0};
            struct triangle triRotatedZX = {0};

            for (int j = 0; j < 3; j++) {
                struct vec3d p = cube_mesh.tris[i].p[j];

                multiply_matrix_vector(&p, &triRotatedZ.p[j], &matRotZ);
                multiply_matrix_vector(&triRotatedZ.p[j], &triRotatedZX.p[j], &matRotX);

                struct vec3d pTranslated = triRotatedZX.p[j];

                pTranslated.z = triRotatedZX.p[j].z + 3.0;

                multiply_matrix_vector(&pTranslated, &triProjected.p[j], &mat_proj);

                triProjected.p[j].x /= triProjected.p[j].z;
                triProjected.p[j].y /= triProjected.p[j].z;

                triProjected.p[j].x = (triProjected.p[j].x + 1) * (SCREEN_WIDTH / 2);
                triProjected.p[j].y = (triProjected.p[j].y + 1) * (SCREEN_HEIGHT / 2);
            }

            bga_draw_triangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, 0xffffff, true);
        }
        bga_print_frame_buffer();
        test++;

        if (test > 50) {
            test = 0;
        }
    }
}
