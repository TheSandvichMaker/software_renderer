#ifndef SD_STRING_H
#define SD_STRING_H

#ifndef SDSTR_API
#ifdef SDSTR_STATIC
#define SDSTR_API static
#else
#define SDSTR_API extern
#endif
#endif

// TODO: non-stdint typedefs
#include <stdint.h>
typedef uint8_t  sdstr_u8;
typedef uint16_t sdstr_u16;
typedef uint32_t sdstr_u32;
typedef uint64_t sdstr_u64;
typedef int8_t   sdstr_s8;
typedef int16_t  sdstr_s16;
typedef int32_t  sdstr_s32;
typedef int64_t  sdstr_s64;
typedef float    sdstr_f32;
typedef double   sdstr_f64;

#define SDSTR_U64_MAX UINT64_MAX
#define SDSTR_U32_MAX UINT32_MAX
#define SDSTR_U16_MAX UINT16_MAX
#define SDSTR_U8_MAX  UINT8_MAX

#define SDSTR_S64_MAX INT64_MAX
#define SDSTR_S64_MIN INT64_MIN
#define SDSTR_S32_MAX INT32_MAX
#define SDSTR_S32_MIN INT32_MIN
#define SDSTR_S16_MAX INT16_MAX
#define SDSTR_S16_MIN INT16_MIN
#define SDSTR_S8_MAX  INT8_MAX
#define SDSTR_S8_MIN  INT8_MIN

#include <stddef.h>
typedef size_t sdstr_usize;

#ifdef SDSTR_CONST
#define sdstr_const const
#else
#define sdstr_const
#endif

#ifndef SDSTR_ASSERT
#include <assert.h>
#define SDSTR_ASSERT(x) assert(x)
#endif

#define SDSTR_INVALID_CODE_PATH SDSTR_ASSERT(!"Invalid Code Path")

typedef struct String_u8 {
    sdstr_usize len;
    sdstr_u8* data;
} String_u8;

static inline String_u8 wrap_string(sdstr_usize length, sdstr_const sdstr_u8* data) {
    String_u8 result;
    result.len = length;
    result.data = (sdstr_u8*)data;
    return result;
}

static inline String_u8 wrap_string_char(sdstr_usize length, sdstr_const char* data) {
    String_u8 result;
    result.len = length;
    result.data = (sdstr_u8*)data;
    return result;
}

#define Str(x) wrap_string(sizeof(x) - 1, (sdstr_const sdstr_u8*)x)
#define StrPrintF(x) (int)(x).len, (char*)(x).data

typedef enum String_Parse_Result {
    STRING_PARSE_FAILURE   = 0,
    STRING_PARSE_SUCCESS   = 1,
    STRING_PARSE_OVERFLOW  = 2,
    STRING_PARSE_UNDERFLOW = 3,
} String_Parse_Result;

// end of header portion

#ifdef SDSTR_IMPLEMENTATION
SDSTR_API sdstr_usize cstring_length(sdstr_const char* cstr) {
    sdstr_usize result = 0;
    while (*cstr++ != 0) {
        ++result;
    }
    return result;
}

SDSTR_API String_u8 wrap_cstring(sdstr_const char* cstr) {
    String_u8 result = wrap_string_char(cstring_length(cstr), cstr);
    return result;
}

SDSTR_API int is_hspace(sdstr_u8 c) {
    int result = (c == ' ' || c == '\t');
    return result;
}

SDSTR_API int is_newline(sdstr_u8 c) {
    int result = (c == '\r' || c == '\n');
    return result;
}

SDSTR_API int is_whitespace(sdstr_u8 c) {
    int result = (is_hspace(c) || is_newline(c));
    return result;
}

SDSTR_API int is_numeric(sdstr_u8 c) {
    int result = (c >= '0' && c <= '9');
    return result;
}

SDSTR_API int is_lower(sdstr_u8 c) {
    sdstr_u8 result = (c >= 'a' && c <= 'z');
    return result;
}

SDSTR_API int is_upper(sdstr_u8 c) {
    sdstr_u8 result = (c >= 'A' && c <= 'Z');
    return result;
}

SDSTR_API int is_alphabetic(sdstr_u8 c) {
    int result = (is_lower(c) || is_upper(c));
    return result;
}

SDSTR_API int is_alphanumeric(sdstr_u8 c) {
    int result = (is_alphabetic(c) || is_numeric(c));
    return result;
}

SDSTR_API int is_printable_ascii(sdstr_u8 c) {
    int result = (c >= ' ' && c <= '~');
    return result;
}

SDSTR_API sdstr_u8 to_lower(sdstr_u8 c) {
    sdstr_u8 result = c;
    if (is_upper(result)) {
        result += ('a' - 'A');
    }
    return result;
}

SDSTR_API sdstr_u8 to_upper(sdstr_u8 c) {
    sdstr_u8 result = c;
    if (is_lower(result)) {
        result += ('A' - 'a');
    }
    return result;
}

SDSTR_API void string_ptr_to_lower(sdstr_const String_u8* string) {
    for (sdstr_usize i = 0; i < string->len; ++i) {
        string->data[i] = to_lower(string->data[i]);
    }
}

SDSTR_API void string_to_lower(String_u8 string) {
    string_ptr_to_lower(&string);
}

SDSTR_API void string_ptr_to_upper(sdstr_const String_u8* string) {
    for (sdstr_usize i = 0; i < string->len; ++i) {
        string->data[i] = to_upper(string->data[i]);
    }
}

SDSTR_API void string_to_upper(String_u8 string) {
    string_ptr_to_upper(&string);
}

SDSTR_API sdstr_u8 string_peek(String_u8 string) {
    sdstr_u8 result = 0;
    if (string.len > 0) {
        result = string.data[0];
    }
    return result;
}

SDSTR_API sdstr_u8 string_ptr_peek(sdstr_const String_u8* string) {
    sdstr_u8 result = 0;
    if (string->len > 0) {
        result = string->data[0];
    }
    return result;
}

SDSTR_API sdstr_u8 string_peek_by(String_u8 string, sdstr_usize by) {
    sdstr_u8 result = 0;
    if (string.len > by) {
        result = string.data[by];
    }
    return result;
}

SDSTR_API sdstr_u8 string_ptr_peek_by(sdstr_const String_u8* string, sdstr_usize by) {
    sdstr_u8 result = 0;
    if (string->len > by) {
        result = string->data[by];
    }
    return result;
}

SDSTR_API sdstr_u8 string_peek_end(String_u8 string) {
    sdstr_u8 result = 0;
    if (string.len > 0) {
        result = string.data[string.len - 1];
    }
    return result;
}

SDSTR_API sdstr_u8 string_ptr_peek_end(sdstr_const String_u8* string) {
    sdstr_u8 result = 0;
    if (string->len > 0) {
        result = string->data[string->len - 1];
    }
    return result;
}

SDSTR_API String_u8 string_advance(String_u8 string) {
    String_u8 result = string;
    if (result.len > 0) {
        --result.len;
        ++result.data;
    }
    return result;
}

SDSTR_API sdstr_u8 string_ptr_advance(String_u8* string) {
    sdstr_u8 result = 0;
    if (string->len > 0) {
        result = string->data[0];
        --string->len;
        ++string->data;
    }
    return result;
}

SDSTR_API String_u8 string_shrink(String_u8 string) {
    String_u8 result = string;
    if (result.len > 0) {
        --result.len;
    }
    return result;
}

SDSTR_API sdstr_u8 string_ptr_shrink(String_u8* string) {
    sdstr_u8 result = 0;
    if (string->len > 0) {
        result = string->data[string->len - 1];
        --string->len;
    }
    return result;
}

SDSTR_API String_u8 string_advance_by(String_u8 string, sdstr_usize by) {
    String_u8 result = string;
    if (by > string.len) {
        by = string.len;
    }
    result.len  -= by;
    result.data += by;
    return result;
}

SDSTR_API void string_ptr_advance_by(String_u8* string, sdstr_usize by) {
    if (by > string->len) {
        by = string->len;
    }
    string->len  -= by;
    string->data += by;
}

SDSTR_API String_u8 string_shrink_by(String_u8 string, sdstr_usize by) {
    String_u8 result = string;
    if (by > string.len) {
        by = string.len;
    }
    result.len -= by;
    return result;
}

SDSTR_API void string_ptr_shrink_by(String_u8* string, sdstr_usize by) {
    if (by > string->len) {
        by = string->len;
    }
    string->len -= by;
}

SDSTR_API String_u8 string_split_at(String_u8* rhs, sdstr_usize at) {
    if (at > rhs->len) {
        at = rhs->len;
    }
    String_u8 lhs;
    lhs.len  = at;
    lhs.data = rhs->data;
    rhs->len  += at;
    rhs->data += at;
    return lhs;
}

SDSTR_API int string_compare(String_u8 a, String_u8 b) {
    int result = (a.len == b.len);
    if (result) {
        for (sdstr_usize i = 0; i < a.len; ++i) {
            if (a.data[i] != b.data[i]) {
                result = 0;
                break;
            }
        }
    }
    return result;
}

SDSTR_API int string_ptr_compare(sdstr_const String_u8* a, sdstr_const String_u8* b) {
    int result = (a->len == b->len);
    if (result) {
        for (sdstr_usize i = 0; i < a->len; ++i) {
            if (a->data[i] != b->data[i]) {
                result = 0;
                break;
            }
        }
    }
    return result;
}

SDSTR_API int string_compare_case_insensitive(String_u8 a, String_u8 b) {
    int result = (a.len == b.len);
    if (result) {
        for (sdstr_usize i = 0; i < a.len; ++i) {
            if (to_lower(a.data[i]) != to_lower(b.data[i])) {
                result = 0;
                break;
            }
        }
    }
    return result;
}

SDSTR_API int string_ptr_compare_case_insensitive(sdstr_const String_u8* a, String_u8* b) {
    int result = (a->len == b->len);
    if (result) {
        for (sdstr_usize i = 0; i < a->len; ++i) {
            if (to_lower(a->data[i]) != to_lower(b->data[i])) {
                result = 0;
                break;
            }
        }
    }
    return result;
}

SDSTR_API String_u8 string_prefix(String_u8 string, sdstr_usize prefix_len) {
    String_u8 result;
    result.len = (prefix_len > string.len ? string.len : prefix_len);
    result.data = string.data;
    return result;
}

SDSTR_API String_u8 string_ptr_prefix(sdstr_const String_u8* string, sdstr_usize prefix_len) {
    String_u8 result;
    result.len = (prefix_len > string->len ? string->len : prefix_len);
    result.data = string->data;
    return result;
}

SDSTR_API String_u8 string_suffix(String_u8 string, sdstr_usize suffix_len) {
    String_u8 result;
    result.len = (suffix_len > string.len ? string.len : suffix_len);
    result.data = string.data + (string.len - suffix_len);
    return result;
}

SDSTR_API String_u8 string_ptr_suffix(sdstr_const String_u8* string, sdstr_usize suffix_len) {
    String_u8 result;
    result.len = (suffix_len > string->len ? string->len : suffix_len);
    result.data = string->data + (string->len - suffix_len);
    return result;
}

SDSTR_API String_u8 string_substring(String_u8 string, sdstr_usize start, sdstr_usize one_past_end) {
    if (start > string.len) {
        start = string.len;
    }
    if (one_past_end > string.len) {
        one_past_end = string.len;
    }
    String_u8 result;
    result.len  = ((one_past_end > start) ? (one_past_end - start) : 0);
    result.data = string.data + start;
    return result;
}

SDSTR_API String_u8 string_ptr_substring(sdstr_const String_u8* string, sdstr_usize start, sdstr_usize one_past_end) {
    if (start > string->len) {
        start = string->len;
    }
    if (one_past_end > string->len) {
        one_past_end = string->len;
    }
    String_u8 result;
    result.len  = ((one_past_end > start) ? (one_past_end - start) : 0);
    result.data = string->data + start;
    return result;
}

SDSTR_API int string_match_prefix(String_u8 string, String_u8 prefix) {
    int result = 0;
    if (string.len >= prefix.len) {
        String_u8 test_string = wrap_string(prefix.len, string.data);
        result = string_compare(test_string, prefix);
    }
    return result;
}

SDSTR_API int string_ptr_match_prefix(sdstr_const String_u8* string, String_u8 prefix) {
    int result = 0;
    if (string->len >= prefix.len) {
        String_u8 test_string = wrap_string(prefix.len, string->data);
        result = string_compare(test_string, prefix);
    }
    return result;
}

SDSTR_API int string_match_suffix(String_u8 string, String_u8 suffix) {
    int result = 0;
    if (string.len >= suffix.len) {
        String_u8 test_string = string_advance_by(string, (string.len - suffix.len));
        result = string_compare(test_string, suffix);
    }
    return result;
}

SDSTR_API int string_ptr_match_suffix(sdstr_const String_u8* string, String_u8 suffix) {
    int result = 0;
    if (string->len >= suffix.len) {
        String_u8 test_string = string_advance_by(*string, (string->len - suffix.len));
        result = string_compare(test_string, suffix);
    }
    return result;
}

SDSTR_API int string_eat_char(String_u8* string, sdstr_u8 c) {
    int result = (string_ptr_peek(string) == c);
    string_ptr_advance_by(string, result);
    return result;
}

SDSTR_API int string_eat_prefix(String_u8* string, String_u8 prefix) {
    int result = string_ptr_match_prefix(string, prefix);
    if (result) {
        string_ptr_advance_by(string, prefix.len);
    }
    return result;
}

SDSTR_API int string_eat_suffix(String_u8* string, String_u8 suffix) {
    int result = string_ptr_match_suffix(string, suffix);
    if (result) {
        string_ptr_shrink_by(string, suffix.len);
    }
    return result;
}

SDSTR_API int string_peek_newline(String_u8 string) {
    int result = 0;
    if (string_peek(string) == '\r' && string_peek_by(string, 1) == '\n') {
        result = 2;
    } else if (string_peek(string) == '\n') {
        result = 1;
    }
    return result;
}

SDSTR_API int string_ptr_peek_newline(sdstr_const String_u8* string) {
    int result = 0;
    if (string_ptr_peek(string) == '\r' && string_ptr_peek_by(string, 1) == '\n') {
        result = 2;
    } else if (string_ptr_peek(string) == '\n') {
        result = 1;
    }
    return result;
}

SDSTR_API int string_eat_newline(String_u8* string) {
    int result = 0;
    int newline_len = string_ptr_peek_newline(string);
    if (newline_len) {
        string_ptr_advance_by(string, newline_len);
        result = 1;
    }
    return result;
}

SDSTR_API void string_ptr_trim_hspaces_left(String_u8* string) {
    while (is_hspace(string_ptr_peek(string))) {
        string_ptr_advance(string);
    }
}

SDSTR_API String_u8 string_trim_hspaces_left(String_u8 string) {
    String_u8 result = string;
    string_ptr_trim_hspaces_left(&result);
    return result;
}

SDSTR_API void string_ptr_trim_hspaces_right(String_u8* string) {
    while (is_hspace(string_ptr_peek_end(string))) {
        string_ptr_shrink(string);
    }
}

SDSTR_API String_u8 string_trim_hspaces_right(String_u8 string) {
    String_u8 result = string;
    string_ptr_trim_hspaces_right(&result);
    return result;
}

SDSTR_API void string_ptr_trim_newlines_left(String_u8* string) {
    while (is_newline(string_ptr_peek(string))) {
        string_ptr_advance(string);
    }
}

SDSTR_API String_u8 string_trim_newlines_left(String_u8 string) {
    String_u8 result = string;
    string_ptr_trim_newlines_left(&result);
    return result;
}

SDSTR_API void string_ptr_trim_newlines_right(String_u8* string) {
    while (is_newline(string_ptr_peek_end(string))) {
        string_ptr_shrink(string);
    }
}

SDSTR_API String_u8 string_trim_newlines_right(String_u8 string) {
    String_u8 result = string;
    string_ptr_trim_newlines_right(&result);
    return result;
}

SDSTR_API void string_ptr_trim_whitespaces_left(String_u8* string) {
    while (is_whitespace(string_ptr_peek(string))) {
        string_ptr_advance(string);
    }
}

SDSTR_API String_u8 string_trim_whitespaces_left(String_u8 string) {
    String_u8 result = string;
    string_ptr_trim_whitespaces_left(&result);
    return result;
}

SDSTR_API void string_ptr_trim_whitespaces_right(String_u8* string) {
    while (is_whitespace(string_ptr_peek_end(string))) {
        string_ptr_shrink(string);
    }
}

SDSTR_API String_u8 string_trim_whitespaces_right(String_u8 string) {
    String_u8 result = string;
    string_ptr_trim_whitespaces_right(&result);
    return result;
}

static String_Parse_Result sdstr_parse_integer_internal(String_u8* out_string, sdstr_u64* out_unsigned, sdstr_s64* out_signed, int base_init, sdstr_u64 min_value, sdstr_u64 max_value) {
    String_Parse_Result result = STRING_PARSE_SUCCESS;
    String_u8 string = *out_string;

    string_ptr_trim_whitespaces_left(&string);
    int positive = 1;
    if (string_eat_char(&string, '+')) {
        // yay positive
    } else if (string_eat_char(&string, '-')) {
        positive = 0;
    }

    int base = base_init;
    if (base == 0) {
        if (string_eat_prefix(&string, Str("0x")) || string_eat_prefix(&string, Str("0X"))) {
            base = 16;
        } else if (string_eat_char(&string, '0')) {
            base = 8;
        } else {
            base = 10;
        }
    }

    if (base >= 2 && base <= 36) {
        sdstr_u64 result_value = 0;
        for (;;) {
            sdstr_u8 digit = string_peek(string);
            int digit_value = -1;
            if (is_numeric(digit)) {
                digit_value = (digit - '0');
            } else if (is_alphabetic(digit)) {
                digit_value = 10 + (to_lower(digit) - 'a');
            }

            if ((digit_value >= 0) && (digit_value < base)) {
                if (positive) {
                    if (result_value <= (max_value - digit_value) / base) {
                        result_value = base*result_value + digit_value;
                    } else {
                        result_value = max_value;
                        result = STRING_PARSE_OVERFLOW;
                    }
                } else {
                    if (result_value <= (min_value - digit_value) / base) {
                        result_value = base*result_value + digit_value;
                    } else {
                        result_value = min_value;
                        result = STRING_PARSE_UNDERFLOW;
                    }
                }
            } else {
                if (result_value == 0) {
                    result = STRING_PARSE_FAILURE;
                }
                break;
            }
            string_ptr_advance(&string);
        }
        if (out_unsigned) {
            *out_unsigned = result_value;
        }
        if (out_signed) {
            if (positive) {
                *out_signed = (sdstr_s64)result_value;
            } else {
                *out_signed = -(sdstr_s64)result_value;
            }
        }
    } else {
        // TODO: Maybe we wanna communicate why it failed?
        result = STRING_PARSE_FAILURE;
    }

    if (result != STRING_PARSE_FAILURE) {
        *out_string = string;
    }

    return result;
}

SDSTR_API String_Parse_Result string_parse_u64(String_u8* string, sdstr_u64* out_value, int base) {
    String_Parse_Result result = sdstr_parse_integer_internal(string, out_value, 0, base, 0, SDSTR_U64_MAX);
    return result;
}

SDSTR_API String_Parse_Result string_parse_u32(String_u8* string, sdstr_u32* out_value, int base) {
    sdstr_u64 raw_out_value;
    String_Parse_Result result = sdstr_parse_integer_internal(string, &raw_out_value, 0, base, 0, SDSTR_U32_MAX);
    if (out_value && result != STRING_PARSE_FAILURE) {
        *out_value = (sdstr_u32)raw_out_value;
    }
    return result;
}

SDSTR_API String_Parse_Result string_parse_u16(String_u8* string, sdstr_u16* out_value, int base) {
    sdstr_u64 raw_out_value;
    String_Parse_Result result = sdstr_parse_integer_internal(string, &raw_out_value, 0, base, 0, SDSTR_U16_MAX);
    if (out_value && result != STRING_PARSE_FAILURE) {
        *out_value = (sdstr_u16)raw_out_value;
    }
    return result;
}

SDSTR_API String_Parse_Result string_parse_u8(String_u8* string, sdstr_u8* out_value, int base) {
    sdstr_u64 raw_out_value;
    String_Parse_Result result = sdstr_parse_integer_internal(string, &raw_out_value, 0, base, 0, SDSTR_U8_MAX);
    if (out_value && result != STRING_PARSE_FAILURE) {
        *out_value = (sdstr_u8)raw_out_value;
    }
    return result;
}

SDSTR_API String_Parse_Result string_parse_s64(String_u8* string, sdstr_s64* out_value, int base) {
    String_Parse_Result result = sdstr_parse_integer_internal(string, 0, out_value, base, (sdstr_u64)(-SDSTR_S64_MIN), SDSTR_S64_MAX);
    return result;
}

SDSTR_API String_Parse_Result string_parse_s32(String_u8* string, sdstr_s32* out_value, int base) {
    sdstr_s64 raw_out_value;
    String_Parse_Result result = sdstr_parse_integer_internal(string, 0, &raw_out_value, base, (sdstr_u64)(-SDSTR_S32_MIN), SDSTR_S32_MAX);
    if (out_value && result != STRING_PARSE_FAILURE) {
        *out_value = (sdstr_s32)raw_out_value;
    }
    return result;
}

SDSTR_API String_Parse_Result string_parse_s16(String_u8* string, sdstr_s16* out_value, int base) {
    sdstr_s64 raw_out_value;
    String_Parse_Result result = sdstr_parse_integer_internal(string, 0, &raw_out_value, base, (sdstr_u64)(-SDSTR_S16_MIN), SDSTR_S16_MAX);
    if (out_value && result != STRING_PARSE_FAILURE) {
        *out_value = (sdstr_s16)raw_out_value;
    }
    return result;
}

SDSTR_API String_Parse_Result string_parse_s8(String_u8* string, sdstr_s8* out_value, int base) {
    sdstr_s64 raw_out_value;
    String_Parse_Result result = sdstr_parse_integer_internal(string, 0, &raw_out_value, base, (sdstr_u64)(-SDSTR_S8_MIN), SDSTR_S8_MAX);
    if (out_value && result != STRING_PARSE_FAILURE) {
        *out_value = (sdstr_s8)raw_out_value;
    }
    return result;
}

#ifdef SDSTR_USE_STDLIB
// strtod, malloc
#include <stdlib.h> 

SDSTR_API String_Parse_Result string_parse_f64(String_u8* string, sdstr_f64* out_value) {
         String_Parse_Result result = STRING_PARSE_FAILURE;
    
    // TODO: What should I use here?
    // IMPORTANT: Whatever it should be, probably not malloc!!!
    char* c_string = (char*)malloc(string->len + 1);
    for (sdstr_usize i = 0; i < string->len; ++i) {
        c_string[i] = string->data[i];
    }
        
        char* end = 0;
        *out_value = strtod(c_string, &end);
        if (end && end != c_string) {
            result = STRING_PARSE_SUCCESS;
        string_ptr_advance_by(string, end - c_string);
        }
    
    free(c_string);
        
        return result;
}
#endif

SDSTR_API String_u8 string_split_line(String_u8* string) {
    String_u8 result;
    result.len  = 0;
    result.data = string->data;
    while (string->len) {
        if (string_eat_newline(string)) {
            break;
        }
        ++result.len;
        string_ptr_advance(string);
    }
    return result;
}

SDSTR_API String_u8 string_split_word(String_u8* string) {
    sdstr_u8* end = string->data + string->len;

    sdstr_u8* a = string->data;
    while ((a < end) && is_whitespace(*a)) {
        ++a;
    }
    sdstr_u8* b = a;
    while ((b < end) && !is_whitespace(*b)) {
        ++b;
    }

    String_u8 result;
    result.len  = (sdstr_usize)(b - a);
    result.data = a;
    
    string_ptr_advance_by(string, (sdstr_usize)(b - string->data));
    
    return result;
}

#endif // SDSTR_IMPLEMENTATION

#endif // SD_STRING_H

// vim: set ft=c
