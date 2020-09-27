internal b32 obj_parse_vertex(String_u8* line, V3* out_vertex) {
    b32 result = true;
    
    V3 vertex;
    for (u32 e = 0; e < 3; ++e) {
        String_u8 element = string_split_word(line);
        f64 scalar;
        if (string_parse_f64(&element, &scalar)) {
            vertex[e] = (f32)scalar;
        } else {
            result = false;
            break;
        }
    }
    
    if (result) {
        *out_vertex = vertex;
    }
    
    return result;
}

internal s32 obj_get_abs_index(u32 vertex_count, s32 index) {
    s32 abs_index = 0;
    if (index > 0) {
        abs_index = index - 1;
    } else {
        abs_index = vertex_count + index;
    }
    return abs_index;
}

internal b32 parse_obj(String_u8 obj, Mesh* out_mesh) {
    b32 result = false;
    
    V3* vertices = 0;
    Triangle* triangles = 0;
    
    while (obj.len) {
        String_u8 line = string_split_line(&obj);
        String_u8 command = string_split_word(&line);
        if (string_compare(command, Str("#"))) {
            /* It's a comment, so our work is done. */
        } else if (string_compare(command, Str("v"))) {
            V3 vertex = {};
            if (!obj_parse_vertex(&line, &vertex)) {
                fprintf(stderr, "[Obj Parser]: Failed to parse vertex element.\n");
            }
            buf_push(vertices, vertex);
        } else if (string_compare(command, Str("f"))) {
            u32 vert_index_count = 0;
            u32 vert_indices[32] = {};
            
            while (line.len && (vert_index_count < ArrayCount(vert_indices))) {
                String_u8 element = string_split_word(&line);
                s32 index = 0;
                if (string_parse_s32(&element, &index, 10)) {
                    vert_indices[vert_index_count++] = obj_get_abs_index((u32)buf_len(vertices), index);
                } else {
                    fprintf(stderr, "[Obj Parser]: Failed to read face index.\n");
                }
            }
            
            if (vert_index_count >= 3) {
                for (u32 i = 1; i < vert_index_count - 1; ++i) {
                    Triangle t;
                    t.a = vert_indices[0];
                    t.b = vert_indices[i];
                    t.c = vert_indices[i + 1];
                    buf_push(triangles, t);
                }
            } else {
                fprintf(stderr, "[Obj Parser]: A face needs at least 3 indices.\n");
            }
        }
    }
    
    if (vertices && triangles) {
        result = true;
        out_mesh->vertex_count   = (u32)buf_len(vertices);
        out_mesh->vertices       = vertices;
        out_mesh->triangle_count = (u32)buf_len(triangles);
        out_mesh->triangles      = triangles;
    }
    
    return result;
}

