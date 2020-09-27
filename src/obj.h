/* date = September 27th 2020 4:45 pm */

#ifndef OBJ_H

typedef union Triangle {
    struct {
        u32 a, b, c;
    };
    u32 e[3];
} Triangle;

typedef struct Mesh {
    u32 vertex_count;
    V3* vertices;
    
    u32 triangle_count;
    Triangle* triangles;
} Mesh;

#define OBJ_H

#endif //OBJ_H
