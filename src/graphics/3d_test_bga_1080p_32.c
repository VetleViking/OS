#include <system.h> 
#include <chess.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// #define SCREEN_WIDTH 1920
// #define SCREEN_HEIGHT 1080

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

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

float fabsf(float x) {
    return (x < 0) ? -x : x;
}

int isnan(float number) {
    uint32_t bits = *((uint32_t*)&number);
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;
    
    return (exponent == 0xFF && fraction != 0);
}

int isinf(float number) {
    uint32_t bits = *((uint32_t*)&number);
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;
    
    return (exponent == 0xFF && fraction == 0);
}

float sinf(float x) {
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    float term = x, sum = x;
    for (int i = 1; i <= 15; i++) { 
        term *= -1 * x * x / ((2 * i) * (2 * i + 1));
        sum += term;
    }
    return sum;
}

float cosf(float x) {
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    float term = 1, sum = 1;
    for (int i = 1; i <= 15; i++) {  
        term *= -1 * x * x / ((2 * i - 1) * (2 * i));
        sum += term;
    }
    return sum;
}

float tanf(float x) {
    float cos_val = cosf(x);
    if (cos_val == 0) {
        return (float)1e6; 
    }
    return sinf(x) / cos_val;
}

typedef union {
    float f;
    unsigned int i;
} float_cast;

unsigned int __HI(float x) {
    float_cast fc;
    fc.f = x;
    return fc.i;
}

void __setHI(float *x, unsigned int hi) {
    float_cast fc;
    fc.f = *x;
    fc.i = hi;
    *x = fc.f;
}

float sqrtf(float x) {
    float z;
    int sign = 0x80000000;
    unsigned int r, t1, s1, ix1, q1;
    int ix0, s0, q, m, t, i;
    float one = 1.0f, tiny = 1.0e-30f;

    ix0 = __HI(x); 

    if((ix0 & 0x7f800000) == 0x7f800000) {            
        return x*x + x;
    } 

    if(ix0 <= 0) {
        if((ix0 & (~sign)) == 0) return x;
        else if(ix0 < 0)
            return (x - x) / (x - x);
    }

    m = (ix0 >> 23);
    if(m == 0) { 
        while((ix0 & 0x00800000) == 0) {
            ix0 <<= 1;
            m -= 1;
        }
        m += 1;
    }
    m -= 127; 
    ix0 = (ix0 & 0x007fffff) | 0x00800000;
    if(m & 1) {
        ix0 += ix0;
    }
    m >>= 1; 

    ix0 += ix0;
    q = s0 = 0;  
    r = 0x01000000;  

    while(r != 0) {
        t = s0 + r; 
        if(t <= ix0) { 
            s0 = t + r; 
            ix0 -= t; 
            q += r; 
        } 
        ix0 += ix0;
        r >>= 1;
    }

    if(ix0 != 0) {
        z = one - tiny; 
        if (z >= one) {
            z = one + tiny;
            if (q == 0x00ffffff) { q = 0; q += 1; }
            else if (z > one) {
                if (q == 0x00fffffe) q += 1;
                q += 2; 
            } else
                q += (q & 1);
        }
    }
    ix0 = (q >> 1) + 0x3f000000;
    if((q & 1) == 1) ix0 += sign;
    ix0 += (m << 23);
    __setHI(&z, ix0);
    return z;
}

void reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int intToStr(int x, char str[], int d) {
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void floatToString(float n, char *res, int afterpoint) {
    int sign = 1;
    if (n < 0) {
        sign = -1;
        n = -n;
    }

    int ipart = (int)n;
    float fpart = n - (float)ipart;

    int i = intToStr(ipart, res, 0); // Handle integer part

    if (sign == -1) {
        // If negative, insert minus sign at the beginning
        for (int j = i; j > 0; j--) {
            res[j] = res[j - 1];
        }
        res[0] = '-';
        i++;
    }

    if (afterpoint != 0) {
        res[i] = '.';
        i++;

        for (int j = 0; j < afterpoint; j++) {
            fpart *= 10;
            int digit = (int)fpart;
            res[i++] = '0' + digit;
            fpart -= digit;
        }
    }

    res[i] = '\0';
}

void multiply_matrix_vector(struct vec3d *i, struct vec3d *o, struct mat4x4 *m) {

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
    //bga_set_video_mode(1920, 1080, 32, 1, 1);
    vga_enter();

    int test = 0;
    int total_drawn = 0;
    while (true) {
        // rotation
        struct mat4x4 matRotZ = {0};
        struct mat4x4 matRotX = {0};

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

        struct mat4x4 mat_proj = {0.0f};

        mat_proj.m[0][0] = fAspectRatio * fFovRad;
        mat_proj.m[1][1] = fFovRad;
        mat_proj.m[2][2] = fFar / (fFar - fNear);
        mat_proj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        mat_proj.m[2][3] = 1.0f;

        // draw triangles
        for (int i = 0; i < 12; i++) {
            struct triangle triProjected = {0};
            struct triangle triTranslated = {0};
            struct triangle triRotatedZ = {0};
            struct triangle triRotatedZX = {0};

            multiply_matrix_vector(&cube_mesh.tris[i].p[0], &triRotatedZ.p[0], &matRotZ);
            multiply_matrix_vector(&cube_mesh.tris[i].p[1], &triRotatedZ.p[1], &matRotZ);
            multiply_matrix_vector(&cube_mesh.tris[i].p[2], &triRotatedZ.p[2], &matRotZ);

            multiply_matrix_vector(&triRotatedZ.p[0], &triRotatedZX.p[0], &matRotX);
            multiply_matrix_vector(&triRotatedZ.p[1], &triRotatedZX.p[1], &matRotX);
            multiply_matrix_vector(&triRotatedZ.p[2], &triRotatedZX.p[2], &matRotX);

            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

            // struct vec3d normal = {0.0f};
            // struct vec3d line1 = {0.0f};
            // struct vec3d line2 = {0.0f};

            // line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
            // line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
            // line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

            // line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
            // line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            // line2.z = triTranslated.p[2].z - triTranslated.p[0].z;
          
            // normal.x = (line1.y * line2.z) - (line1.z * line2.y);
            // normal.y = (line1.z * line2.x) - (line1.x * line2.z);
            // normal.z = (line1.x * line2.y) - (line1.y * line2.x);

            // // Normalize the normal vector
            // float l = sqrtf((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));
            // normal.x /= l;
            // normal.y /= l;
            // normal.z /= l;

            // struct vec3d vCamera = {0.0f, 0.0f, 0.0f};

            // if ((normal.x * (triTranslated.p[0].x - vCamera.x)) + (normal.y * (triTranslated.p[0].y - vCamera.y)) + (normal.z * (triTranslated.p[0].z - vCamera.z)) < 0.0f) {
           
            multiply_matrix_vector(&triTranslated.p[0], &triProjected.p[0], &mat_proj);
            multiply_matrix_vector(&triTranslated.p[1], &triProjected.p[1], &mat_proj);
            multiply_matrix_vector(&triTranslated.p[2], &triProjected.p[2], &mat_proj);

            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
            triProjected.p[0].x *= 0.5f * SCREEN_WIDTH;
            triProjected.p[0].y *= 0.5f * SCREEN_HEIGHT;
            triProjected.p[1].x *= 0.5f * SCREEN_WIDTH;
            triProjected.p[1].y *= 0.5f * SCREEN_HEIGHT;
            triProjected.p[2].x *= 0.5f * SCREEN_WIDTH;
            triProjected.p[2].y *= 0.5f * SCREEN_HEIGHT;

            total_drawn++;

            // Draw the triangle
            // draw_triangle_fill(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, VGA_COLOR_WHITE, true);
            draw_triangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, VGA_COLOR_WHITE, true);
            // vga_plot_pixel(triProjected.p[0].x, triProjected.p[0].y, VGA_COLOR_WHITE, true);
            // vga_plot_pixel(triProjected.p[1].x, triProjected.p[1].y, VGA_COLOR_WHITE, true);
            // vga_plot_pixel(triProjected.p[2].x, triProjected.p[2].y, VGA_COLOR_WHITE, true);

            // }
        }
        //bga_print_frame_buffer();

        // print_int(0, 0, test);
        total_drawn = 0;

        vga_print_frame_buffer();
        sleep(20);

        test++;
    }
}

void print_int(int x, int y, int number) {
    char p[10];
    itoa(number, p, 10);

    for (int i = 0; i < 10; i++) {
        if (p[i] == '\0') {
            break; 
        } else if (p[i] == '-') {
            draw_rectangle(x + (i * 10), y, 10, 20, VGA_COLOR_BLACK, false);
            draw_rectangle(x + (i * 10), y + 8, 9, 2, VGA_COLOR_WHITE, false);
        } else {
            print_num(x + (i * 10), y, p[i] - '0');
        }
    }
}

void print_float(int x, int y, float number) {
    char p[30]; 
    floatToString(number, p, 10);

    
    bool decimalFound = false;
    for (int i = 0; i < 20; i++) {
        if (p[i] == '.') {
            decimalFound = true;
            i++;
            continue;
        }

        if (p[i] == '-') {
            draw_rectangle(x + (i * 10), y, 10, 20, VGA_COLOR_BLACK, true);
            draw_rectangle(x + (i * 10), y + 8, 9, 2, VGA_COLOR_WHITE, true);
        } else {
            print_num_float(x + (i * 10), y, p[i] - '0', true);
        }

        if (decimalFound) {
            draw_rectangle(x + (i * 10), y + 16, 2, 2, VGA_COLOR_WHITE, true);
        }

    }
}

void print_num_float(int x, int y, int number) {
    bool use_buffer = true;

    draw_rectangle(x, y, 10, 20, VGA_COLOR_BLACK, use_buffer);

    switch (number) {
        case 0:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 2, y + 2, 5, 14, VGA_COLOR_BLACK, use_buffer);            
            break;

        case 1:
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);            
            break;

        case 2:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 10, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 9, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 3:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            break;

        case 4: 
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 5:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y + 9, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 6:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y + 9, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 7:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            break;

        case 8:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 2, y + 2, 5, 14, VGA_COLOR_BLACK, use_buffer); 
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 9:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;
        default:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_RED, use_buffer);
            break;
    };
}