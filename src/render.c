#define SDSTR_STATIC
#define SDSTR_IMPLEMENTATION
#define SDSTR_USE_STDLIB
#include "sd_string.h"

#include "render.h"

#include "image.c"
#include "obj.c"

function
String_u8 read_entire_file(char* file_name, b32 null_terminate) {
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

function
void plot_line(Image_u32* image, V2i p0, V2i p1, Color_ARGB color) {
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

function
void plot_triangle(Image_u32* image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
    plot_line(image, p0, p1, color);
    plot_line(image, p1, p2, color);
    plot_line(image, p2, p0, color);
}

function
void bayercentric(V2i a, V2i b, V2i c, V2i p, f32* u, f32* v, f32* w) {
    V2 v0 = vector_convert(V2, (b - a));
    V2 v1 = vector_convert(V2, (c - a));
    V2 v2 = vector_convert(V2, (p - a));
    f32 d00 = dot(v0, v0);
    f32 d01 = dot(v0, v1);
    f32 d11 = dot(v1, v1);
    f32 d20 = dot(v2, v0);
    f32 d21 = dot(v2, v1);
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

function
void fill_slope_data(V2i p0, V2i p1, SlopeData* data) {
    data->height = (p1.y - p0.y);
    data->step   = (f32)(p1.x - p0.x) / (f32)data->height;
    data->x      = (f32)p0.x;
}

function
void rasterize_triangle(Image_u32* image, V2i p0, V2i p1, V2i p2, Color_ARGB color) {
    if (p1.y < p0.y) { Swap(p0, p1); }
    if (p2.y < p0.y) { Swap(p0, p2); }
    if (p2.y < p1.y) { Swap(p1, p2); }
    
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

function void draw_mesh(Image_u32* image, Mesh* mesh) {
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

function
u32 u32_log2(u32 n) {
    // https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
#define S(k) if (n >= (1 << k)) { i += k; n >>= k; }
    u32 i = -(n == 0);
    S(16); S(8); S(4); S(2); S(1);
    return i;
#undef S
}

typedef enum DistanceFieldType {
    DistanceField_Outer,
    DistanceField_Inner,
} DistanceFieldType;

function
Image_u32 produce_distance_field(Image_u32* src, u32 bullshit_multiplier, DistanceFieldType type) {
    Image_u32 image_a = allocate_image(src->width, src->height);
    Image_u32 image_b = allocate_image(src->width, src->height);
    
    Image_u32* image_read  = &image_a;
    Image_u32* image_write = &image_b;
    
    for (u32 y = 0; y < image_read->height; ++y) {
        for (u32 x = 0; x < image_read->width; ++x) {
            Color_ARGB pixel = (Color_ARGB) { .argb = get_pixel(src, x, y) };
            if (((type == DistanceField_Outer) && (pixel.a > 127)) ||
                ((type == DistanceField_Inner) && (pixel.a <= 127)))
            {
                set_pixel(image_read, x, y, (Color_ARGB) { .bg = x, .ra = y });
            }
        }
    }
    
    u32 N = Max(src->width, src->height);
    u32 N_log2 = u32_log2(N);
    for (u32 pass_index = 0; pass_index < N_log2; ++pass_index) {
        s32 offset = (s32)(1 << (N_log2 - pass_index - 1));
        
        V2i pairs[] = {
            { -offset, -offset }, { 0, -offset }, { offset, -offset },
            { -offset, 0       }, { 0, 0       }, { offset, 0       },
            { -offset, offset  }, { 0, offset  }, { offset, offset  },
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
                    
                    Color_ARGB pixel = { .argb = get_pixel(image_read, read_x, read_y) };
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
        Swap(image_read, image_write);
    }
    
    for (u32 y = 0; y < image_read->height; ++y) {
        for (u32 x = 0; x < image_read->width; ++x) {
            Color_ARGB pixel = { .argb = get_pixel(image_read, x, y) };
            s32 diff_x = (s32)pixel.bg - (s32)x;
            s32 diff_y = (s32)pixel.ra - (s32)y;
            u32 distance_sq = diff_x*diff_x + diff_y*diff_y;
            f32 distance = sqrt((f32)distance_sq);
            s32 write_distance = bullshit_multiplier*(u32)(255*(distance / (f32)N));
            if (write_distance < 0)   { write_distance = 0; }
            if (write_distance > 255) { write_distance = 255; }
            write_distance = (255 - write_distance);
            set_pixel(image_write, x, y, rgb(write_distance, write_distance, write_distance));
        }
    }
    
    free_image(image_read);

    Image_u32 result = *image_write;
    return result;
}

function
Image_u32 produce_signed_distance_field(Image_u32* src, u32 bullshit_multiplier) {
    Image_u32 positive_distance_field = produce_distance_field(src, 8, DistanceField_Outer);
    Image_u32 negative_distance_field = produce_distance_field(src, 8, DistanceField_Inner);
    
    for (u32 y = 0; y < positive_distance_field.height; ++y) {
        for (u32 x = 0; x < positive_distance_field.width; ++x) {
            u8 positive = (Color_ARGB) { .argb = get_pixel(&positive_distance_field, x, y) }.r;
            u8 negative = (Color_ARGB) { .argb = get_pixel(&negative_distance_field, x, y) }.r;
            u8 combined = (255 - positive) / 2 + negative / 2;
            // NOTE: We're overwriting the contents of positive_distance_field, but this is fine because we're not reading from this position again
            set_pixel(&positive_distance_field, x, y, (Color_ARGB) { .r = combined, .g = combined, .b = combined, .a = 255 });
        }
    }

    free_image(&negative_distance_field);

    return positive_distance_field;
}

function
void voronoi_test(void) {
    enum { N = 512 };
    Image_u32 image_source = allocate_image(N, N);
    
    char a[] = {
        0, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
    };
    
    u32 margin = 128;
    for (u32 y = margin; y < image_source.height - margin; ++y) {
        for (u32 x = margin; x < image_source.width - margin; ++x) {
            u32 read_x = 5*(x - margin) / (image_source.width - 2*margin);
            u32 read_y = 4 - 5*(y - margin) / (image_source.height - 2*margin);
            char read_pixel = a[read_y*5 + read_x];
            if (read_pixel) {
                set_pixel(&image_source, x, y, (Color_ARGB) { .a = 255 });
            }
        }
    }

    Image_u32 sdf = produce_signed_distance_field(&image_source, 8);
    write_image("signed_distance_field.bmp", &sdf);
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
