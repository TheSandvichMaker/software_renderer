#define SDSTR_STATIC
#define SDSTR_IMPLEMENTATION
#define SDSTR_USE_STDLIB
#include "sd_string.h"

#include "render.h"

#include "image.c"
#include "obj.c"

internal String_u8 read_entire_file(char* file_name, b32 null_terminate) {
    String_u8 result = {};
    
    FILE* in = fopen(file_name, "rb");
    if (in) {
        fseek(in, 0, SEEK_END);
        result.len = ftell(in);
        fseek(in, 0, SEEK_SET);
        
        umm alloc_size = result.len;
        if (null_terminate) {
            alloc_size += 1;
        }
        
        result.data = (u8*)malloc(alloc_size);
        if (result.data) {
            fread(result.data, result.len, 1, in);
        }
        
        fclose(in);
        
        if (null_terminate) {
            result.data[result.len] = 0;
        }
    }
    
    return result;
}

internal void plot_line(Image_u32* image, V2i p0, V2i p1, Color_ARGB color) {
    s32 x0 = p0.x;
    s32 y0 = p0.y;
    s32 x1 = p1.x;
    f32 y1 = p1.y;
    
    s32 dx =  Abs(x1 - x0);
    s32 dy = -Abs(y1 - y0);
    
    s32 xi = (x1 > x0 ? 1 : -1);
    s32 yi = (y1 > y0 ? 1 : -1);
    s32 err = dx + dy;
    
    s32 x = x0;
    s32 y = y0;
    
    for (;;) {
        set_pixel(image, x, y, color);
        
        if (x == x1 && y == y1) {
            break;
        }
        
        s32 err2 = 2*err;
        if (err2 >= dy) {
            err += dy;
            x += xi;
        }
        if (err2 <= dx) {
            err += dx;
            y += yi;
        }
    }
}

internal void plot_triangle(Image_u32* image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
    plot_line(image, p0, p1, color);
    plot_line(image, p1, p2, color);
    plot_line(image, p2, p0, color);
}

internal void bayercentric(V2i a, V2i b, V2i c, V2i p, f32* u, f32* v, f32* w) {
    V2 v0 = vector_convert(V2, (b - a));
    V2 v1 = vector_convert(V2, (c - a));
    V2 v2 = vector_convert(V2, (p - a));
    f32 d00 = v2_dot(v0, v0);
    f32 d01 = v2_dot(v0, v1);
    f32 d11 = v2_dot(v1, v1);
    f32 d20 = v2_dot(v2, v0);
    f32 d21 = v2_dot(v2, v1);
    f32 denom = d00*d11 - d01*d01;
    *v = (d11*d20 - d01*d21) / denom;
    *w = (d00*d21 - d01*d20) / denom;
    *u = (1.0f - *v - *w);
}

typedef struct SlopeData {
    s32 height;
    f32 step;
    f32 x;
} SlopeData;

internal void fill_slope_data(V2i p0, V2i p1, SlopeData* data) {
    data->height = (p1.y - p0.y);
    data->step   = (f32)(p1.x - p0.x) / (f32)data->height;
    data->x      = (f32)p0.x;
}

internal void rasterize_triangle(Image_u32* image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
    if (p1.y < p0.y) { Swap(V2i, p0, p1); }
    if (p2.y < p0.y) { Swap(V2i, p0, p2); }
    if (p2.y < p1.y) { Swap(V2i, p1, p2); }
    
    if (p0.y != p2.y) {
        b32 short_side = (p1.y - p0.y)*(p2.x - p0.x) < (p2.y - p0.y)*(p1.x - p0.x);
        
        SlopeData slope_data[2];
        fill_slope_data(p0, p2, &slope_data[!short_side]);
        
        for (s32 y = p0.y, end_y = p0.y; ; ++y) {
            if (y >= end_y) {
                if (y >= p2.y) {
                    break;
                }
                
                if (y < p1.y) {
                    fill_slope_data(p0, p1, &slope_data[short_side]);
                    end_y = p1.y;
                } else {
                    fill_slope_data(p1, p2, &slope_data[short_side]);
                    end_y = p2.y;
                }
            }
            
            for (s32 x = (s32)slope_data[0].x; x < (s32)slope_data[1].x; ++x) {
                f32 u, v, w;
                bayercentric(p0, p1, p2, v2i(x, y), &u, &v, &w);
                set_pixel(image, x, y, rgb((s32)(255.0f*u), (s32)(255.0f*v), (s32)(255.0f*w)));
            }
            
            slope_data[0].x += slope_data[0].step;
            slope_data[1].x += slope_data[1].step;
        }
    }
}

internal void draw_mesh(Image_u32* image, Mesh* mesh) {
    for (u32 triangle_index = 0; triangle_index < mesh->triangle_count; ++triangle_index) {
        Triangle* t = mesh->triangles + triangle_index;
        V2i screen_coords[3];
        for (u32 vert_index = 0; vert_index < 3; ++vert_index) {
            V3 v0 = mesh->vertices[t->e[vert_index]];
            s32 x0 = (s32)(0.5f*image->width*(v0.x + 1.0f));
            s32 y0 = (s32)(0.5f*image->height*(v0.y + 1.0f));
            screen_coords[vert_index] = v2i(x0, y0);
        }
        rasterize_triangle(image, screen_coords[0], screen_coords[1], screen_coords[2], rgb(rand() % 255,
                                                                                            rand() % 255,
                                                                                            rand() % 255));
    }
}

internal u32 u32_log2(u32 n) {
    // https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
#define S(k) if (n >= (1 << k)) { i += k; n >>= k; }
    u32 i = -(n == 0);
    S(16); S(8); S(4); S(2); S(1);
    return i;
#undef S
}

internal void voronoi_test() {
    enum { N = 512 };
    
    Image_u32 image_a = allocate_image(N, N);
    Image_u32 image_b = allocate_image(N, N);
    
    set_pixel(&image_a, 16*8, 16*16,  (Color_ARGB) { .bg = 16*8,  .ra = 16*16 });
    set_pixel(&image_a, 16*2, 16*18,  (Color_ARGB) { .bg = 16*2,  .ra = 16*18 });
    set_pixel(&image_a, 16*25, 16*28, (Color_ARGB) { .bg = 16*25, .ra = 16*28 });
    set_pixel(&image_a, 16*12, 16*6,  (Color_ARGB) { .bg = 16*12, .ra = 16*6  });
    
    Image_u32* image_read  = &image_a;
    Image_u32* image_write = &image_b;
    
    u32 N_log2 = u32_log2(N);
    for (u32 pass_index = 0; pass_index < N_log2; ++pass_index) {
        s32 offset = (s32)(1 << (N_log2 - pass_index - 1));
        
        struct { s32 x, y; } pairs[] = {
            { .x = -offset, .y = -offset }, { .x = 0, .y = -offset }, { .x = offset, .y = -offset },
            { .x = -offset, .y = 0       }, { .x = 0, .y = 0       }, { .x = offset, .y = 0       },
            { .x = -offset, .y = offset  }, { .x = 0, .y = offset  }, { .x = offset, .y = offset  },
        };
        
        for (u32 y = 0; y < image_read->height; ++y) {
            for (u32 x = 0; x < image_read->width; ++x) {
                u32 closest_distance = UINT32_MAX;
                u32 closest_x = UINT32_MAX;
                u32 closest_y = UINT32_MAX;
                
                for (u32 pair_index = 0; pair_index < 9; ++pair_index) {
                    s32 read_x = (s32)x + pairs[pair_index].x;
                    s32 read_y = (s32)y + pairs[pair_index].y;
                    if (read_x < 0)                        { read_x = 0; }
                    if (read_x >= (s32)image_read->width)  { read_x = image_read->width - 1; }
                    if (read_y < 0)                        { read_y = 0; }
                    if (read_y >= (s32)image_read->height) { read_y = image_read->height - 1; }
                    
                    Color_ARGB pixel = get_pixel(image_read, read_x, read_y);
                    if (pixel.bg || pixel.ra) {
                        s32 diff_x = (s32)pixel.bg - (s32)x;
                        s32 diff_y = (s32)pixel.ra - (s32)y;
                        u32 distance_sq = diff_x*diff_x + diff_y*diff_y;
                        if (closest_distance > distance_sq) {
                            closest_distance = distance_sq;
                            closest_x = pixel.bg;
                            closest_y = pixel.ra;
                        }
                    }
                }
                
                if ((closest_x != UINT32_MAX) &&
                    (closest_y != UINT32_MAX))
                {
                    set_pixel(image_write, x, y, (Color_ARGB) { .bg = closest_x, .ra = closest_y });
                }
            }
        }
        
        copy_image(image_write, image_read);
        Swap(Image_u32*, image_read, image_write);
    }
    
    write_image("voronoi.bmp", image_read);
    
    for (u32 y = 0; y < image_read->height; ++y) {
        for (u32 x = 0; x < image_read->width; ++x) {
            Color_ARGB pixel = get_pixel(image_read, x, y);
            s32 diff_x = (s32)pixel.bg - (s32)x;
            s32 diff_y = (s32)pixel.ra - (s32)y;
            u32 distance_sq = diff_x*diff_x + diff_y*diff_y;
            f32 distance = sqrt((f32)distance_sq);
            u32 write_distance = (u32)(255*(distance / (f32)N));
            if (write_distance < 0)   { write_distance = 0; }
            if (write_distance > 255) { write_distance = 255; }
            set_pixel(image_write, x, y, rgb(write_distance, write_distance, write_distance));
        }
    }
    
    write_image("distance_field.bmp", image_write);
}

#include <time.h>

int main(int argc, char** argv) {
#if 0
    Image_u32 image = allocate_image(512, 512);
    clear_image(&image, rgb(0, 0, 0));
    
    String_u8 obj = read_entire_file("african_head.obj", false);
    
    Mesh mesh;
    if (parse_obj(obj, &mesh)) {
        draw_mesh(&image, &mesh);
    }
    
    write_image("test.bmp", &image);
#else
    voronoi_test();
#endif
}
