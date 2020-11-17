/* date = September 27th 2020 4:44 pm */

#ifndef IMAGE_H
#define IMAGE_H

typedef union Color_ARGB {
    struct {
        u8 b, g, r, a;
    };
    struct {
        u16 bg, ra;
    };
    u32 argb;
} Color_ARGB;

#pragma pack(push, 1)
typedef struct Bitmap_Header {
    u16 file_type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
    u32 size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bits_per_pixel;
    
    u32 compression;
    u32 size_of_bitmap;
    s32 horz_resolution;
    s32 vert_resolution;
    u32 colors_used;
    u32 colors_important;
} Bitmap_Header;
#pragma pack(pop)

typedef struct Image_u32 {
    u32 width;
    u32 height;
    
    u32* pixels;
} Image_u32;

#endif //IMAGE_H
