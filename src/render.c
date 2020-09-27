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

internal void plot_line(Image_u32 image, V2i p0, V2i p1, Color_ARGB color) {
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

internal void plot_triangle(Image_u32 image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
    plot_line(image, p0, p1, color);
    plot_line(image, p1, p2, color);
    plot_line(image, p2, p0, color);
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

internal void rasterize_triangle(Image_u32 image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
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
                set_pixel(image, x, y, color);
            }
            
            slope_data[0].x += slope_data[0].step;
            slope_data[1].x += slope_data[1].step;
        }
    }
}

#include <time.h>

int main(int argc, char** argv) {
    Image_u32 image = allocate_image(512, 512);
    clear_image(image, rgb(0, 0, 0));
    
#if 1
    String_u8 obj = read_entire_file("african_head.obj", false);
    
    Mesh mesh;
    if (parse_obj(obj, &mesh)) {
        clock_t timer = clock();
        
        for (u32 i = 0; i < 10000; ++i) {
            for (u32 triangle_index = 0; triangle_index < mesh.triangle_count; ++triangle_index) {
                Triangle* t = mesh.triangles + triangle_index;
                V2i screen_coords[3];
                for (u32 vert_index = 0; vert_index < 3; ++vert_index) {
                    V3 v0 = mesh.vertices[t->e[vert_index]];
                    s32 x0 = (s32)(0.5f*image.width*(v0.x + 1.0f));
                    s32 y0 = (s32)(0.5f*image.height*(v0.y + 1.0f));
                    screen_coords[vert_index] = v2i(x0, y0);
                }
                rasterize_triangle(image, screen_coords[0], screen_coords[1], screen_coords[2], rgb(rand() % 255, rand() % 255, rand() % 255));
            }
        }
        
        clock_t elapsed = clock() - timer;
        printf("Clocks elapsed: %llu", (u64)elapsed);
    }
#else
    V2i t0[3] = { v2i(10, 70),   v2i(50, 160),  v2i(70, 80)   }; 
    V2i t1[3] = { v2i(180, 50),  v2i(150, 1),   v2i(70, 180)  }; 
    V2i t2[3] = { v2i(180, 150), v2i(120, 160), v2i(130, 180) };
    
    rasterize_triangle(image, t0[0], t0[1], t0[2], rgb(255, 0, 0));
    rasterize_triangle(image, t1[0], t1[1], t1[2], rgb(255, 255, 255));
    rasterize_triangle(image, t2[0], t2[1], t2[2], rgb(0, 255, 0));
#endif
    
    write_image("test.bmp", image);
}
