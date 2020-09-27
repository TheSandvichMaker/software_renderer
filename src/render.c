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
        *get_pixel_pointer(image, x, y) = color.argb;
        
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

internal void rasterize_triangle(Image_u32 image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
    if (p1.y < p0.y) { Swap(V2i, p0, p1); }
    if (p2.y < p0.y) { Swap(V2i, p0, p2); }
    if (p2.y < p1.y) { Swap(V2i, p1, p2); }
    
    if (p0.y != p2.y) {
        s32 total_height = p2.y - p0.y;
        for (s32 i = 0; i < total_height; ++i) {
            b32 second_half = (i > (p1.y - p0.y)) || (p1.y == p0.y);
            s32 segment_height = (second_half ? (p2.y - p1.y) : (p1.y - p0.y));
            
            f32 alpha = (f32)i / (f32)total_height;
            f32 beta  = (f32)(i - (second_half ? (p1.y - p0.y) : 0)) / (f32)segment_height;
            
            V2i A = vector_convert(V2i, vector_convert(V2, p0) + alpha*vector_convert(V2, p2 - p0));
            V2i B = (second_half ?
                vector_convert(V2i, vector_convert(V2, p1) +  beta*vector_convert(V2, p2 - p1)) :
                         vector_convert(V2i, vector_convert(V2, p0) +  beta*vector_convert(V2, p1 - p0)));
                
            if (A.x > B.x) { Swap(V2i, A, B); }
            
            for (s32 x = A.x; x < B.x; ++x) {
            *get_pixel_pointer(image, x, p0.y + i) = color.argb;
            }
        }
    }
}

int main(int argc, char** argv) {
    Image_u32 image = allocate_image(512, 512);
    clear_image(image, rgb(0, 0, 0));
    
    #if 1
    String_u8 obj = read_entire_file("african_head.obj", false);
    
    Mesh mesh;
    if (parse_obj(obj, &mesh)) {
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
    