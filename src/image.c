internal Color_ARGB rgba(u8 r, u8 g, u8 b, u8 a) {
    Color_ARGB result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

internal Color_ARGB rgb(u8 r, u8 g, u8 b) {
    Color_ARGB result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = 255;
    return result;
}

internal u32 get_total_pixel_size(Image_u32 image) {
    u32 result = sizeof(u32)*image.width*image.height;
    return result;
}

internal u32* get_pixel_pointer(Image_u32 image, u32 x, u32 y) {
    u32* result = image.pixels + y*image.width + x;
    return result;
}

internal void write_image(char* file_name, Image_u32 image) {
    u32 pixel_size = get_total_pixel_size(image);
    
    Bitmap_Header header = {};
    header.file_type        = 0x4D42;
    header.file_size        = sizeof(header) + pixel_size;
    header.bitmap_offset    = sizeof(header);
    header.size             = sizeof(header) - 14;
    header.width            = image.width;
    header.height           = image.height;
    header.planes           = 1;
    header.bits_per_pixel   = 32;
    header.compression      = 0;
    header.size_of_bitmap   = pixel_size;
    header.horz_resolution  = 4096;
    header.vert_resolution  = 4096;
    header.colors_used      = 0;
    header.colors_important = 0;
    
    FILE* out_file = fopen(file_name, "wb");
    if (out_file) {
        fwrite(&header, sizeof(header), 1, out_file);
        fwrite(image.pixels, pixel_size, 1, out_file);
        fclose(out_file);
    } else {
        fprintf(stderr, "error: Unable to write output file %s.\n", file_name);
    }
}

internal Image_u32 allocate_image(u32 width, u32 height) {
    Image_u32 image = {};
    image.width = width;
    image.height = height;
    
    u32 pixel_size = get_total_pixel_size(image);
    image.pixels = (u32*)calloc(pixel_size, 1);
    
    return image;
}

internal void clear_image(Image_u32 image, Color_ARGB color) {
    u32* at  = image.pixels;
    u32* end = get_pixel_pointer(image, image.width, image.height);
    while (at != end) {
        *at++ = color.argb;
    }
}
    