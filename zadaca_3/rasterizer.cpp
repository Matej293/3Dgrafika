#include <cmath>
#include "tgaimage.h"

using namespace std;

// dimenzije slike
const int width  = 512;
const int height = 512;

// definirajmo boje
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

typedef float Vec3[3];
typedef float Vec2[2];


void set_color(int x, int y, TGAImage &image, TGAColor color, bool invert = false)
{
    image.set(x, y, color);
}

// adapted "line" function from v9 (baricentric coords)
float helper_func(float x0, float y0, float x1, float y1, float x, float y)
{
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

float area(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return abs(( x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0 );
}

bool in_triangle(float x1, float y1, float x2, float y2, float x3, float y3, float x, float y) // (x, y) is the current point
{
    float A  = area (x1, y1, x2, y2, x3, y3);
    float A1 = area (x, y, x2, y2, x3, y3);
    float A2 = area (x1, y1, x, y, x3, y3);
    float A3 = area (x1, y1, x2, y2, x, y);
    return (A == A1 + A2 + A3);
}


void draw_triangle_2d(TGAImage& image, float x0, float y0, float x1, float y1, float x2, float y2, TGAColor color)
{
    int maxX = ceil  (max (max(x0, x1), x2));
    int minX = floor (min (min(x0, x1), x2));
    int maxY = ceil  (max (max(y0, y1), y2));
    int minY = floor (min (min(y0, y1), y2));

    for (int j = minY; j <= maxY ; ++j)
        for (int i = minX; i <= maxX; ++i)
            if (in_triangle(x0, y0, x1, y1, x2, y2, i, j))
                set_color(i, j, image, color); //since the image is 512x512px the difference between setting (i, j) as (x, y) and (j, i) as (x, y) is one 90deg rotation
}


void draw_triangle_2d_gouraurd(TGAImage& image, float x0, float y0, const TGAColor& c0, float x1, float y1, const TGAColor& c1, float x2, float y2, const TGAColor& c2)
{
    TGAColor color;

    int maxX = ceil  (max (max(x0, x1), x2));
    int minX = floor (min (min(x0, x1), x2));
    int maxY = ceil  (max (max(y0, y1), y2));
    int minY = floor (min (min(y0, y1), y2));

    for (int j = minY; j <= maxY; ++j)
    {
        for (int i = minX; i <= maxX; ++i)
        {
            float fst = helper_func(x1, y1, x2, y2, i, j) / helper_func(x1, y1, x2, y2, x0, y0);
            float snd = helper_func(x2, y2, x0, y0, i, j) / helper_func(x2, y2, x0, y0, x1, y1);
            float trd = helper_func(x0, y0, x1, y1, i, j) / helper_func(x0, y0, x1, y1, x2, y2);

            if (in_triangle(x0, y0, x1, y1, x2, y2, i, j))
            {
                color.r = fst*c0.r + snd*c1.r + trd*c2.r;
                color.g = fst*c0.g + snd*c1.g + trd*c2.g;
                color.b = fst*c0.b + snd*c1.b + trd*c2.b;
                set_color(i, j, image, color);
            }
        }
    }
}


void draw_triangle(TGAImage &image, const Vec3 v1, const Vec3 v2, const Vec3 v3, const TGAColor& color)
{
//transformed vertices in Vec2 format
    Vec2 v1_new = { static_cast<float>((1 + (v1[0] / v1[2])) * 0.5 * width), static_cast<float>((1 + (v1[1] / v1[2])) * 0.5 * height) };
    Vec2 v2_new = { static_cast<float>((1 + (v2[0] / v2[2])) * 0.5 * width), static_cast<float>((1 + (v2[1] / v2[2])) * 0.5 * height) };
    Vec2 v3_new = { static_cast<float>((1 + (v3[0] / v3[2])) * 0.5 * width), static_cast<float>((1 + (v3[1] / v3[2])) * 0.5 * height) };

    int maxX = ceil  (max (max(v1_new[0], v2_new[0]), v3_new[0]));
    int minX = floor (min (min(v1_new[0], v2_new[0]), v3_new[0]));
    int maxY = ceil  (max (max(v1_new[1], v2_new[1]), v3_new[1]));
    int minY = floor (min (min(v1_new[1], v2_new[1]), v3_new[1]));

    for (int j = minY; j <= maxY; ++j)
    {
        for (int i = minX; i <= maxX; ++i)
        {
            float fst = helper_func(v2_new[0], v2_new[1], v3_new[0], v3_new[1], i, j) / helper_func(v2_new[0], v2_new[1], v3_new[0], v3_new[1], v1_new[0], v1_new[1]);
            float snd = helper_func(v3_new[0], v3_new[1], v1_new[0], v1_new[1], i, j) / helper_func(v3_new[0], v3_new[1], v1_new[0], v1_new[1], v2_new[0], v2_new[1]);
            float trd = helper_func(v1_new[0], v1_new[1], v2_new[0], v2_new[1], i, j) / helper_func(v1_new[0], v1_new[1], v2_new[0], v2_new[1], v3_new[0], v3_new[1]);

            if (fst > 0 && fst < 1 && snd > 0 && snd < 1 && trd > 0 && trd < 1)
                set_color(i, j, image, color);
        }
    }
}


void draw_triangle_tex(TGAImage& image, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float u0, float v0, float u1, float v1, float u2, float v2, const char* path)
{
    TGAImage texture;
    texture.read_tga_file(path);

    Vec3 A {x0, y0,z0};
    Vec3 B {x1, y1,z1};
    Vec3 C {x2, y2,z2};

    Vec2 uv0 {u0, v0};
    Vec2 uv1 {u1, v1};
    Vec2 uv2 {u2, v2};

    //transformacije koordinata i dijeljenje sa z (1/z)
    Vec3 A_new = { static_cast<float>((1 + (A[0] / A[2])) * 0.5 * width), static_cast<float>((1 + (A[1] / A[2])) * 0.5 * height), 1 / A[2] };
    Vec3 B_new = { static_cast<float>((1 + (B[0] / B[2])) * 0.5 * width), static_cast<float>((1 + (B[1] / B[2])) * 0.5 * height), 1 / B[2] };
    Vec3 C_new = { static_cast<float>((1 + (C[0] / C[2])) * 0.5 * width), static_cast<float>((1 + (C[1] / C[2])) * 0.5 * height), 1 / C[2] };

    //dodatno dijelimo uv vektore sa z-koordinatama inputa
    uv0[0] /= A[2]; uv0[1] /= A[2];
    uv1[0] /= B[2]; uv1[1] /= B[2];
    uv2[0] /= C[2]; uv2[1] /= C[2];

    for (int j = 0; j <= width; ++j)
    {
        for (int i = 0; i <= height; ++i)
        {
            float fst = helper_func(B_new[0], B_new[1], C_new[0], C_new[1], i, j) / helper_func(B_new[0], B_new[1], C_new[0], C_new[1], A_new[0], A_new[1]);
            float snd = helper_func(C_new[0], C_new[1], A_new[0], A_new[1], i, j) / helper_func(C_new[0], C_new[1], A_new[0], A_new[1], B_new[0], B_new[1]);
            float trd = helper_func(A_new[0], A_new[1], B_new[0], B_new[1], i, j) / helper_func(A_new[0], A_new[1], B_new[0], B_new[1], C_new[0], C_new[1]);

            if((fst >= 0 && snd >= 0 && trd >= 0))
            {
                float x = texture.get_width() * (fst * u0 + snd * u1 + trd * u2);
                float y = texture.get_height() * (fst * v0 + snd * v1 + trd * v2);
                set_color(i, j, image, texture.get(round(x), round(y)));
            }
        }
    }
}


void draw_triangle_tex_corrected(TGAImage& image, float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float u0, float v0, float u1, float v1, float u2, float v2, const char* path)
{
    TGAImage texture;
    texture.read_tga_file(path);

    Vec3 A {x0, y0,z0};
    Vec3 B {x1, y1,z1};
    Vec3 C {x2, y2,z2};

    Vec2 uv0 {u0, v0};
    Vec2 uv1 {u1, v1};
    Vec2 uv2 {u2, v2};

    //transformacije koordinata i dijeljenje sa z (1/z)
    Vec3 A_new = { static_cast<float>((1 + (A[0] / A[2])) * 0.5 * width), static_cast<float>((1 + (A[1] / A[2])) * 0.5 * height), 1 / A[2] };
    Vec3 B_new = { static_cast<float>((1 + (B[0] / B[2])) * 0.5 * width), static_cast<float>((1 + (B[1] / B[2])) * 0.5 * height), 1 / B[2] };
    Vec3 C_new = { static_cast<float>((1 + (C[0] / C[2])) * 0.5 * width), static_cast<float>((1 + (C[1] / C[2])) * 0.5 * height), 1 / C[2] };

    //dodatno dijelimo uv vektore sa z-koordinatama inputa
    uv0[0] /= A[2]; uv0[1] /= A[2];
    uv1[0] /= B[2]; uv1[1] /= B[2];
    uv2[0] /= C[2]; uv2[1] /= C[2];

    for (int j = 0; j <= width; ++j)
    {
        for (int i = 0; i <= height; ++i)
        {
            float fst = helper_func(B_new[0], B_new[1], C_new[0], C_new[1], i, j) / helper_func(B_new[0], B_new[1], C_new[0], C_new[1], A_new[0], A_new[1]);
            float snd = helper_func(C_new[0], C_new[1], A_new[0], A_new[1], i, j) / helper_func(C_new[0], C_new[1], A_new[0], A_new[1], B_new[0], B_new[1]);
            float trd = helper_func(A_new[0], A_new[1], B_new[0], B_new[1], i, j) / helper_func(A_new[0], A_new[1], B_new[0], B_new[1], C_new[0], C_new[1]);

            if (fst > 0 && fst < 1 && snd > 0 && snd < 1 && trd > 0 && trd < 1)
            {
                float x = (fst * uv0[0] + snd * uv1[0] + trd * uv2[0]) * texture.get_width();
                float y = (fst * uv0[1] + snd * uv1[1] + trd * uv2[1]) * texture.get_height();
                float z  = 1 / (fst * A_new[2] + snd * B_new[2] + trd * C_new[2]);
                x *= z; y *= z;
                set_color(i, j, image, texture.get(round(x), round(y)));
            }
        }
    }
}

int main()
{
    // definiraj sliku
    TGAImage image(width, height, TGAImage::RGB);
    
    //draw_triangle_2d(image, 20, 30, 180, 80, 100, 200, blue);
    //draw_triangle_2d(image, 380, 30, 220, 80, 300, 200, red);
    //draw_triangle_2d_gouraurd(image, 20, 230, red, 220, 230, green, 20, 430, blue);

    //Vec3 vertex1{-48,-10, 82};
    //Vec3 vertex2{29, -15, 44};
    //Vec3 vertex3{13, 34, 114};
    //draw_triangle(image, vertex1, vertex2, vertex3, white);

    //draw_triangle_tex(image, -48, -10,  82, 29, -15,  44, 13,  34, 114, 0, 0, 0, 1, 1, 0, "./textures/texture.tga");
    draw_triangle_tex_corrected(image, -48, -10,  82, 29, -15,  44, 13,  34, 114, 0, 0, 0, 1, 1, 0, "./textures/texture.tga");

    // spremi sliku
    image.flip_vertically();
    image.write_tga_file("tex_corrected.tga");
}