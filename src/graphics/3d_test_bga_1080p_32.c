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
    struct triangle tris[12][3];
};

struct mat4x4
{
    float m[4][4];
};

struct vec3d multiply_matrix_vector(struct vec3d *i, struct mat4x4 *m)
{
    struct vec3d o;

    o.x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
    o.y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
    o.z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
    float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3];

    if (w != 0.0)
    {
        o.x /= w;
        o.y /= w;
        o.z /= w;
    }

    return o;
}

void enter_3d_test() {
    bga_set_video_mode(1920, 1080, 32, 1, 1);

    struct mesh cube_mesh = {{
        // south
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}},
        {{0, 0, 0}, {1, 0, 0}, {1, 0, 0}},
        // east
        {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
        {{1, 0, 0}, {1, 1, 1}, {1, 0, 1}},
        // north
        {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}},
        {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}},
        // west
        {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
        {{0, 0, 1}, {0, 1, 0}, {0, 0, 0}},
        // top
        {{0, 1, 0}, {0, 1, 1}, {1, 1, 1}},
        {{0, 1, 0}, {1, 1, 1}, {1, 1, 0}},
        // bottom
        {{0, 0, 0}, {0, 0, 1}, {1, 0, 1}},
        {{0, 0, 0}, {1, 0, 1}, {1, 0, 0}}
    }};

    // projection matrix
    float fNear = 0.1;
    float fFar = 1000.0;
    float fFov = 90.0;
    float fAspectRatio = 1080.0 / 1920.0;
    float fFovRad = 1.0 / tanf(fFov * 0.5 / 180.0 * 3.14159);

    struct mat4x4 mat_proj = {0};

    mat_proj.m[0][0] = fAspectRatio * fFovRad;
    mat_proj.m[1][1] = fFovRad;
    mat_proj.m[2][2] = fFar / (fFar - fNear);
    mat_proj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    mat_proj.m[2][3] = 1.0;
    mat_proj.m[3][3] = 0.0;

    for (int i = 0; i < 12; i++) {
        struct triangle triProjected = {0};
        struct triangle triTranslated = {0};

        // rotate
        triTranslated.p[0] = cube_mesh.tris[i][0].p[0];
        
        
        triProjected.p[0] = multiply_matrix_vector(&cube_mesh.tris[i][0].p[0], &mat_proj);
        triProjected.p[1] = multiply_matrix_vector(&cube_mesh.tris[i][0].p[1], &mat_proj);
        triProjected.p[2] = multiply_matrix_vector(&cube_mesh.tris[i][0].p[2], &mat_proj);



        // scale into view
        triProjected.p[0].x += 1.0; triProjected.p[0].y += 1.0;
        triProjected.p[1].x += 1.0; triProjected.p[1].y += 1.0;
        triProjected.p[2].x += 1.0; triProjected.p[2].y += 1.0;

        triProjected.p[0].x *= 0.5 * SCREEN_WIDTH;
        triProjected.p[0].y *= 0.5 * SCREEN_HEIGHT;
        triProjected.p[1].x *= 0.5 * SCREEN_WIDTH;
        triProjected.p[1].y *= 0.5 * SCREEN_HEIGHT;
        triProjected.p[2].x *= 0.5 * SCREEN_WIDTH;
        triProjected.p[2].y *= 0.5 * SCREEN_HEIGHT;

        bga_draw_triangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, 0xff0000);
    }
}
