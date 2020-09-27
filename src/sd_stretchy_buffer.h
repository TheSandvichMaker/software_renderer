/* date = September 26th 2020 9:17 pm */

#ifndef SD_STRETCHY_BUFFER_H
#define SD_STRETCHY_BUFFER_H

#ifndef SD_SB_API
#ifdef SD_SB_STATIC
#define SD_SB_API static
#else
#define SD_SB_API
#endif
#endif

#ifndef SD_SB_ASSERT
#include <assert.h>
#define SD_SB_ASSERT(x) assert(x)
#endif

#ifndef SD_SB_MALLOC
#include <stdlib.h>
#define SD_SB_MALLOC(size) malloc(size)
#endif

#ifndef SD_SB_REALLOC
#include <stdlib.h>
#define SD_SB_REALLOC(old_ptr, size) realloc(old_ptr, size)
#endif

typedef struct Buffer_Header {
    size_t len;
    size_t cap;
    char buf[1];
} Buffer_Header;

#define buf__hdr(b) ((Buffer_Header*)((char*)(b) - offsetof(Buffer_Header, buf)))
#define buf__fits(b, n) (buf_len(b) + n <= buf_cap(b))
#define buf__fit(b, n) (buf__fits(b, n) ? 0 : (*((void**)&(b)) = buf__grow(b, buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, ...) (buf__fit(b, 1), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))
#define buf_push_ptr(b) (buf__fit(b, 1), (b) + buf__hdr(b)->len++)
#define buf_push_array(b, n) (buf__fit(b, n), buf__hdr(b)->len += (n), (b) + buf_len(b) - (n))
#define buf_end(b) ((b) + buf_len(b))
#define buf_free(b) ((b) ? (SD_SB_REALLOC(buf__hdr(b), 0), (b) = 0) : 0)

 SD_SB_API void* buf__grow(void* buf, umm new_len, umm elem_size) {
    SD_SB_ASSERT(buf_cap(buf) <= (SIZE_MAX - 1) / 2);
    umm new_cap = Max(1 + 2*buf_cap(buf), new_len);
    if (new_cap < 8) {
        new_cap = 8;
    }
    SD_SB_ASSERT(new_len <= new_cap);
    umm new_size = offsetof(Buffer_Header, buf) + new_cap*elem_size;
    Buffer_Header* new_hdr;
    if (buf) {
        new_hdr = (Buffer_Header*)SD_SB_REALLOC(buf__hdr(buf), new_size);
    } else {
        new_hdr = (Buffer_Header*)SD_SB_MALLOC(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}

#endif //SD_STRETCHY_BUFFER_H
