#ifndef SD_MATH_H
#define SD_MATH_H

#ifndef SD_MATH_API
#ifdef SD_MATH_STATIC
#define SD_MATH_API static
#else
#define SD_MATH_API
#endif
#endif

#include <xmmintrin.h>
#include <math.h>

#define PI_32  3.14159265359f
#define TAU_32 6.28318530717f
#define DEG_TO_RAD (TAU_32/360.0f)
#define RAD_TO_DEG (360.0f/TAU_32)

//
// TODO: Convert all of these to platform-efficient versions
// and remove math.h
//

#if !defined(SD_MATH_COS) || !defined(SD_MATH_SIN)
#include <math.h>
#endif

#ifndef SD_MATH_COS
#define SD_MATH_COS cos
#endif

#ifndef SD_MATH_SIN
#define SD_MATH_SIN sin
#endif

//
// NOTE: Scalar operations
//

SD_MATH_API f32 square_root(f32 f) {
    return sqrtf(f);
}

SD_MATH_API f32 fract(f32 f) {
    f32 ignored;
    return modff(f, &ignored);
}

#undef round
#define round my_round
SD_MATH_API f32 round(f32 f) {
    f32 result = (f32)_mm_cvtss_si32(_mm_set_ss(f));
    return result;
}

#undef floor
#define floor my_floor
SD_MATH_API f32 floor(f32 f) {
    f32 result = (f32)_mm_cvttss_si32(_mm_set_ss(f));
    return result;
}

SD_MATH_API s32 round_f32_to_s32(f32 f) {
    s32 result = _mm_cvtss_si32(_mm_set_ss(f));
    return result;
}

SD_MATH_API u32 round_f32_to_u32(f32 f) {
    u32 result = (u32)_mm_cvtss_si32(_mm_set_ss(f));
    return result;
}

SD_MATH_API s32 floor_f32_to_s32(f32 f) {
    s32 result = _mm_cvttss_si32(_mm_set_ss(f));
    return result;
}

SD_MATH_API s32 ceil_f32_to_s32(f32 f) {
    s32 result = (s32)ceilf(f);
    return result;
}

SD_MATH_API s32 truncate_f32_to_s32(f32 f) {
    s32 result = (s32)f;
    return result;
}

#ifdef sin
#undef sin
#endif
#define sin my_sin
SD_MATH_API f32 my_sin(f32 a) {
    return sinf(a);
}

#ifdef cos
#undef cos
#endif
#define cos my_cos
SD_MATH_API f32 my_cos(f32 a) {
    return cosf(a);
}

#ifdef atan2
#undef atan2
#endif
#define atan2 my_atan2
SD_MATH_API f32 my_atan2(f32 y, f32 x) {
    return atan2f(y, x);
}

SD_MATH_API s32 div_floor_s32(s32 a, s32 b) {
    Assert(b != 0);
    s32 res = a / b;
    s32 rem = a % b;
    s32 corr = (rem != 0 && ((rem < 0) != (b < 0)));
    return res - corr;
}

SD_MATH_API f32 deg_to_rad(f32 deg) {
    f32 rad = deg*DEG_TO_RAD;
    return rad;
}

SD_MATH_API f32 rad_to_deg(f32 rad) {
    f32 deg = rad*RAD_TO_DEG;
    return deg;
}

SD_MATH_API f32 square(f32 a) {
    return a*a;
}

SD_MATH_API s32 square_s32(s32 a) {
    return a*a;
}

SD_MATH_API u32 square_u32(u32 a) {
    return a*a;
}

SD_MATH_API f32 lerp(f32 a, f32 b, f32 t) {
    return a*(1.0f - t) + b*t;
}

#undef max
SD_MATH_API f32 max(f32 a, f32 b) {
    return a > b ? a : b;
}

SD_MATH_API f64 max_f64(f64 a, f64 b) {
    return a > b ? a : b;
}

#undef min
SD_MATH_API f32 min(f32 a, f32 b) {
    return a < b ? a : b;
}

SD_MATH_API f64 min_f64(f64 a, f64 b) {
    return a < b ? a : b;
}

#undef clamp
SD_MATH_API f32 clamp(f32 n, f32 lo, f32 hi) {
    return max(lo, min(hi, n));
}

SD_MATH_API f64 clamp_f64(f64 n, f64 lo, f64 hi) {
    return max(lo, min(hi, n));
}

#define abs sd_abs
SD_MATH_API f32 abs(f32 x) {
    f32 result = (x < 0.0f ? -x : x);
    return result;
}

SD_MATH_API s32 abs_s32(s32 x) {
    s32 result = (x < 0 ? -x : x);
    return result;
}

#define clamp01(n) clamp((n), 0, 1)
#define clamp01_f64(n) clamp_f64((n), 0, 1)

// Source: https://www.iquilezles.org/www/articles/smin/smin.htm 
SD_MATH_API f32 smooth_min(f32 a, f32 b, f32 k) {
    f32 h = max(k - abs(a - b), 0.0f) / k;
    return min(a, b) - 0.25f*h*h*k;
}

SD_MATH_API f32 map_to_range(f32 t, f32 min, f32 max) {
    f32 result = 0.0f;
    f32 range = max - min;
    if (range != 0.0f) {
        result = (t - min) / range;
    }
    return result;
}

SD_MATH_API f32 clamp01_map_to_range(f32 t, f32 min, f32 max) {
    f32 result = clamp01(map_to_range(t, min, max));
    return result;
}

SD_MATH_API f32 safe_ratio_n(f32 numerator, f32 divisor, f32 n) {
    f32 result = n;
    
    if (divisor != 0.0f) {
        result = numerator / divisor;
    }
    
    return result;
}

SD_MATH_API f32 safe_ratio_0(f32 numerator, f32 divisor) {
    f32 result = safe_ratio_n(numerator, divisor, 0.0f);
    return result;
}

SD_MATH_API f32 safe_ratio_1(f32 numerator, f32 divisor) {
    f32 result = safe_ratio_n(numerator, divisor, 1.0f);
    return result;
}

SD_MATH_API f32 sign_of(f32 x) {
    f32 result = 0.0f;
    if (x > 0.0f) {
        result =  1.0f;
    } else if (x < 0.0f) {
        result = -1.0f;
    }
    return result;
}

SD_MATH_API f32 neighborhood_distance(f32 a, f32 b, f32 period) {
    f32 d0 = abs(a - b);
    f32 d1 = abs(a - period*sign_of(a) - b);
    f32 result = min(d0, d1);
    return result;
}

SD_MATH_API f32 smoothstep(f32 x) {
    f32 result = x*x*(3.0f - 2.0f*x);
    return result;
}

SD_MATH_API f32 smootherstep(f32 x) {
    f32 result = x*x*x*(x*(x*6.0f - 15.0f) + 10.0f);
    return result;
}

//
// NOTE: Types
//

#if __has_attribute(ext_vector_type)
typedef f32 V2 __attribute__((ext_vector_type(2)));
typedef f32 V3 __attribute__((ext_vector_type(3)));
typedef f32 V4 __attribute__((ext_vector_type(4)));

#define v2(x, y) (V2){ x, y }
#define v3(x, y, z) (V3){ x, y, z }
#define v4(x, y, z, w) (V4){ x, y, z, w }

typedef s32 V2i __attribute__((ext_vector_type(2)));
typedef s32 V3i __attribute__((ext_vector_type(3)));
typedef s32 V4i __attribute__((ext_vector_type(4)));

#define v2i(x, y) (V2i){ x, y }
#define v3i(x, y, z) (V3i){ x, y, z }
#define v4i(x, y, z, w) (V4i){ x, y, z, w }

#define vector_convert(target, v) __builtin_convertvector(v, target)

#else // __has_attribute(ext_vector_type)
typedef union V2 {
    struct {
        f32 x, y;
    };
    f32 e[2];
} V2;

SD_MATH_API V2 v2(f32 x, f32 y) {
    V2 result;
    result.x = x;
    result.y = y;
    return result;
}

typedef union V3 {
    struct {
        f32 x, y, z;
    };
    struct {
        V2 xy;
        f32 ignored0;
    };
    f32 e[3];
} V3;

SD_MATH_API V3 v3(f32 x, f32 y, f32 z) {
    V3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

typedef union V4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        V3 xyz;
        f32 ignored0;
    };
    struct {
        V2 xy;
        f32 ignored1;
    };
    f32 e[4];
} V4;

SD_MATH_API V4 v4(f32 x, f32 y, f32 z, f32 w) {
    V4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

typedef union V2i {
    struct {
        s32 x, y;
    };
    s32 e[2];
} V2i;

SD_MATH_API V2i v2i(s32 x, s32 y) {
    V2i result;
    result.x = x;
    result.y = y;
    return result;
}

typedef union V3i {
    struct {
        s32 x, y, z;
    };
    s32 e[3];
} V3i;

SD_MATH_API V3i v3i(s32 x, s32 y, s32 z) {
    V3i result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
#endif

typedef union M4x4 {
    f32 e[4][4];
} M4x4;

// @Note: The convention is that the rectangle bounds are [min,max): max is exclusive.

typedef struct Rect2 {
    V2 min;
    V2 max;
} Rect2;

typedef struct Rect3 {
    V3 min;
    V3 max;
} Rect3;

typedef union Rect2i {
    struct {
        V2i min;
        V2i max;
    };
} Rect2i;

typedef struct Rect3i {
    V3i min;
    V3i max;
} Rect3i;

//
// NOTE: V2
//

SD_MATH_API V2 v2_neg(V2 a) {
    V2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

#define IMPLEMENT_VECTOR_FUNCTIONS(op_name, op) \
SD_MATH_API V2 v2_##op_name(V2 a, V2 b) { \
V2 result; \
result.x = a.x op b.x; \
result.y = a.y op b.y; \
return result; \
} \
SD_MATH_API void v2_mut_##op_name(V2* a, V2 b) { \
a->x = a->x op b.x; \
a->y = a->y op b.y; \
}

#define IMPLEMENT_SCALAR_FUNCTIONS(op_name, op) \
SD_MATH_API V2 v2_##op_name##_s(V2 a, f32 b) { \
V2 result; \
result.x = a.x op b; \
result.y = a.y op b; \
return result; \
} \
SD_MATH_API V2 v2_s_##op_name(f32 a, V2 b) { \
V2 result; \
result.x = a op b.x; \
result.y = a op b.y; \
return result; \
} \
SD_MATH_API void v2_mut_##op_name##_s(V2* a, f32 b) { \
a->x = a->x op b; \
a->y = a->y op b; \
}

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_FUNCTIONS(add, +)
IMPLEMENT_VECTOR_FUNCTIONS(sub, -)
IMPLEMENT_SCALAR_FUNCTIONS(mul, *)
IMPLEMENT_SCALAR_FUNCTIONS(div, /)
IMPLEMENT_VECTOR_FUNCTIONS(mul, *)
IMPLEMENT_VECTOR_FUNCTIONS(div, /)

#undef IMPLEMENT_VECTOR_FUNCTIONS
#undef IMPLEMENT_SCALAR_FUNCTIONS

SD_MATH_API V2 v2_perp(V2 v) {
    V2 result = v2(-v.y, v.x);
    return result;
}

SD_MATH_API V2 v2_perp_clockwise(V2 v) {
    V2 result = v2(v.y, -v.x);
    return result;
}

SD_MATH_API V2 v2_square(V2 v) {
    V2 result;
    result.x = v.x*v.x;
    result.y = v.y*v.y;
    return result;
}

SD_MATH_API V2 v2_lerp(V2 a, V2 b, f32 t) {
    return a*(1.0f - t) + b*t;
}

SD_MATH_API f32 v2_dot(V2 a, V2 b) {
    return a.x*b.x + a.y*b.y;
}

SD_MATH_API V2 v2_reflect(V2 a, V2 b) {
    f32 dot2 = 2.0f*v2_dot(a, b);
    V2 result;
    result.x = a.x - dot2*b.x;
    result.y = a.y - dot2*b.y;
    return result;
}

SD_MATH_API f32 v2_length_sq(V2 v) {
    f32 result = v2_dot(v, v);
    return result;
}

SD_MATH_API f32 v2_length(V2 v) {
    f32 result = square_root(v2_dot(v, v));
    return result;
}

SD_MATH_API V2 v2_normalize(V2 v) {
    Assert(length(v) != 0.0f);
    return v / v2_length(v);
}

SD_MATH_API V2 v2_noz(V2 v) {
    V2 result = {};
    
    f32 l_sq = v2_length_sq(v);
    if (l_sq > square(0.0001f)) {
        result = v*(1.0f / square_root(l_sq));
    }
    return result;
}

SD_MATH_API V2 v2_min(V2 a, V2 b) {
    V2 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    return result;
}

SD_MATH_API V2 v2_min_s(V2 a, f32 b) {
    V2 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    return result;
}

SD_MATH_API V2 v2_max(V2 a, V2 b) {
    V2 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    return result;
}

SD_MATH_API V2 v2_max_s(V2 a, f32 b) {
    V2 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    return result;
}

SD_MATH_API V2 v2_clamp(V2 n, V2 lo, V2 hi) {
    V2 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    return result;
}

SD_MATH_API V2 v2_clamp_s(V2 n, f32 lo, f32 hi) {
    V2 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    return result;
}

SD_MATH_API V2 v2_arm(f32 angle) {
    V2 result;
    result.x = cos(angle);
    result.y = sin(angle);
    return result;
}

SD_MATH_API V2 v2_rotate_arm(V2 v, V2 cos_sin) {
    V2 result;
    result.x = (v.x*cos_sin.x - v.y*cos_sin.y);
    result.y = (v.x*cos_sin.y + v.y*cos_sin.x);
    return result;
}

SD_MATH_API V2 v2_rotate(V2 v, f32 r) {
    V2 arm = v2_arm(r);
    V2 result = v2_rotate_arm(v, arm);
    return result;
}

SD_MATH_API V2 v2_rotate_arm_clockwise(V2 v, V2 cos_sin) {
    V2 result;
    result.x = (v.x*cos_sin.x + v.y*cos_sin.y);
    result.y = (v.y*cos_sin.x - v.x*cos_sin.y);
    return result;
}

SD_MATH_API V2 v2_rotate_clockwise(V2 v, f32 r) {
    V2 arm = v2_arm(r);
    V2 result = v2_rotate_arm_clockwise(v, arm);
    return result;
}

//
// NOTE: V2i
//

#define IMPLEMENT_VECTOR_FUNCTIONS(op_name, op) \
SD_MATH_API V2i v2i_##op_name(V2i a, V2i b) { \
V2i result; \
result.x = a.x op b.x; \
result.y = a.y op b.y; \
return result; \
} \
SD_MATH_API void v2i_mut_##op_name(V2i* a, V2i b) { \
a->x = a->x op b.x; \
a->y = a->y op b.y; \
}

#define IMPLEMENT_SCALAR_FUNCTIONS(op_name, op) \
SD_MATH_API V2i v2i_##op_name##_s(V2i a, s32 b) { \
V2i result; \
result.x = a.x op b; \
result.y = a.y op b; \
return result; \
} \
SD_MATH_API void v2i_mut_##op_name##_s(V2i* a, s32 b) { \
a->x = a->x op b; \
a->y = a->y op b; \
}

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_FUNCTIONS(add, +)
IMPLEMENT_VECTOR_FUNCTIONS(sub, -)
IMPLEMENT_SCALAR_FUNCTIONS(mul, *)
IMPLEMENT_SCALAR_FUNCTIONS(div, /)
IMPLEMENT_VECTOR_FUNCTIONS(mul, *)
IMPLEMENT_VECTOR_FUNCTIONS(div, /)

#undef IMPLEMENT_VECTOR_FUNCTIONS
#undef IMPLEMENT_SCALAR_FUNCTIONS

SD_MATH_API u32 v2i_length_sq(V2i x) {
    u32 result = square_s32(x.x) + square_s32(x.y);
    return result;
}

SD_MATH_API f32 v2i_length_f32(V2i x) {
    f32 result = square_root((f32)v2i_length_sq(x));
    return result;
}

SD_MATH_API u32 v2i_length(V2i x) {
    u32 result = round_f32_to_u32(v2i_length_f32(x));
    return result;
}

SD_MATH_API s32 v2i_manhattan_distance(V2i a, V2i b) {
    s32 result = Abs(a.x - b.x) + Abs(a.y - b.y);
    return result;
}

SD_MATH_API f32 v2i_diagonal_distance(V2i a, V2i b, f32 diagonal_cost) {
    s32 dx = Abs(a.x - b.x);
    s32 dy = Abs(a.y - b.y);
    f32 result = (f32)(dx + dy) + (diagonal_cost - 2.0f)*(f32)Min(dx, dy);
    return result;
}

//
// NOTE: V3i
//

SD_MATH_API V3i floor_v3_to_v3i(V3 a) {
    V3i result = v3i(floor_f32_to_s32(a.x),
                     floor_f32_to_s32(a.y),
                     floor_f32_to_s32(a.z));
    return result;
}

SD_MATH_API V3i round_v3_to_v3i(V3 a) {
    V3i result = v3i(round_f32_to_s32(a.x),
                     round_f32_to_s32(a.y),
                     round_f32_to_s32(a.z));
    return result;
}

#define IMPLEMENT_VECTOR_FUNCTIONS(op_name, op) \
SD_MATH_API V3i v3i_##op_name(V3i a, V3i b) { \
V3i result; \
result.x = a.x op b.x; \
result.y = a.y op b.y; \
result.z = a.z op b.z; \
return result; \
} \
SD_MATH_API void v3i_mut_##op_name(V3i* a, V3i b) { \
a->x = a->x op b.x; \
a->y = a->y op b.y; \
a->z = a->z op b.z; \
}

#define IMPLEMENT_SCALAR_FUNCTIONS(op_name, op) \
SD_MATH_API V3i v3i_##op_name##_s(V3i a, s32 b) { \
V3i result; \
result.x = a.x op b; \
result.y = a.y op b; \
result.z = a.z op b; \
return result; \
} \
SD_MATH_API void v3i_mut_##op_name##_s(V3i* a, s32 b) { \
a->x = a->x op b; \
a->y = a->y op b; \
a->z = a->z op b; \
}

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_FUNCTIONS(add, +)
IMPLEMENT_VECTOR_FUNCTIONS(sub, -)
IMPLEMENT_SCALAR_FUNCTIONS(mul, *)
IMPLEMENT_SCALAR_FUNCTIONS(div, /)
IMPLEMENT_VECTOR_FUNCTIONS(mul, *)
IMPLEMENT_VECTOR_FUNCTIONS(div, /)

#undef IMPLEMENT_VECTOR_FUNCTIONS
#undef IMPLEMENT_SCALAR_FUNCTIONS

SD_MATH_API u32 v3i_length_sq(V3i x) {
    u32 result = (x.x*x.x) + (x.y*x.y) + (x.z*x.z);
    return result;
}

SD_MATH_API f32 v3i_length_f32(V3i x) {
    f32 result = square_root((f32)v3i_length_sq(x));
    return result;
}

SD_MATH_API u32 v3i_length(V3i x) {
    u32 result = round_f32_to_u32(v3i_length_f32(x));
    return result;
}

SD_MATH_API f32 v3i_distance_f32(V3i a, V3i b) {
    f32 result = v3i_length_f32(v3i_sub(a, b));
    return result;
}

SD_MATH_API s32 v3i_distance(V3i a, V3i b) {
    s32 result = round_f32_to_s32(v3i_distance_f32(a, b));
    return result;
}

SD_MATH_API s32 v3i_manhattan_distance(V3i a, V3i b) {
    s32 result = Abs(a.x - b.x) + Abs(a.y - b.y) + Abs(a.z - b.z);
    return result;
}

SD_MATH_API f32 v3i_diagonal_distance(V3i a, V3i b, f32 diagonal_cost) {
    s32 dx = Abs(a.x - b.x);
    s32 dy = Abs(a.y - b.y);
    s32 dz = Abs(a.z - b.z);
    f32 result = (f32)(dx + dy + dz) + (diagonal_cost - 2.0f)*(f32)Min(Min(dx, dy), dz);
    return result;
}

SD_MATH_API V3i v3i_min(V3i a, V3i b) {
    V3i result;
    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    result.z = Min(a.z, b.z);
    return result;
}

SD_MATH_API V3i v3i_max(V3i a, V3i b) {
    V3i result;
    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    result.z = Max(a.z, b.z);
    return result;
}

//
// NOTE: Rect2
//

/*
 I'm saying rects start bottom left, wound counter clockwise, regarding the naming of corners.
    D --------- C
    |           |
    |           |
    A --------- B
*/

SD_MATH_API V2 corner_a(Rect2 rect) {
    V2 result = rect.min;
    return result;
}

SD_MATH_API V2 corner_b(Rect2 rect) {
    V2 result = v2(rect.max.x, rect.min.y);
    return result;
}

SD_MATH_API V2 corner_c(Rect2 rect) {
    V2 result = rect.max;
    return result;
}

SD_MATH_API V2 corner_d(Rect2 rect) {
    V2 result = v2(rect.min.x, rect.max.y);
    return result;
}

SD_MATH_API V2 get_dim(Rect2 rect) {
    V2 result = rect.max - rect.min;
    return result;
}

SD_MATH_API V2 get_min_corner(Rect2 rect) {
    V2 result = rect.min;
    return result;
}

SD_MATH_API V2 get_max_corner(Rect2 rect) {
    V2 result = rect.max;
    return result;
}

SD_MATH_API V2 get_center(Rect2 rect) {
    V2 result = 0.5f * (rect.min + rect.max);
    return result;
}

SD_MATH_API f32 get_aspect_ratio(Rect2 rect) {
    V2 dim = get_dim(rect);
    f32 aspect_ratio = dim.x/dim.y;
    return aspect_ratio;
}

SD_MATH_API Rect2 rect_min_max(V2 min, V2 max) {
    Rect2 result;
    result.min = min;
    result.max = max;
    return result;
}

SD_MATH_API Rect2 rect_center_half_dim(V2 center, V2 half_dim) {
    Rect2 result;
    result.min = center - half_dim;
    result.max = center + half_dim;
    return result;
}

SD_MATH_API Rect2 rect_center_dim(V2 center, V2 dim) {
    Rect2 result;
    result.min = center - dim * 0.5f;
    result.max = center + dim * 0.5f;
    return result;
}

SD_MATH_API Rect2 rect_min_dim(V2 min, V2 dim) {
    Rect2 result;
    result.min = min;
    result.max = min + dim;
    return result;
}

SD_MATH_API Rect2 multiply_dimensions(Rect2 rect, V2 mul) {
    Rect2 result;
    result.min = rect.min * mul;
    result.max = rect.max * mul;
    return result;
}

SD_MATH_API Rect2 grow_by_radius(Rect2 rect, V2 r_dim) {
    Rect2 result;
    result.min = rect.min - r_dim;
    result.max = rect.max + r_dim;
    return result;
}

SD_MATH_API Rect2 grow_by_diameter(Rect2 rect, V2 d_dim) {
    Rect2 result;
    result.min = rect.min - d_dim * 0.5f;
    result.max = rect.max + d_dim * 0.5f;
    return result;
}

SD_MATH_API Rect2 offset(Rect2 a, V2 offset) {
    Rect2 result;
    result.min = a.min + offset;
    result.max = a.max + offset;
    return result;
}

SD_MATH_API b32 is_in_rect(Rect2 rect, V2 test) {
    b32 result = (test.x >= rect.min.x && test.x < rect.max.x) &&
        (test.y >= rect.min.y && test.y < rect.max.y);
    return result;
}

SD_MATH_API b32 rect_contained_in_rect(Rect2 outer, Rect2 inner) {
    b32 result = (outer.min.x <= inner.min.x && outer.max.x >= inner.max.x) &&
        (outer.min.y <= inner.min.y && outer.max.y >= inner.max.y);
    return result;
}

SD_MATH_API b32 rect_intersect(Rect2 a, Rect2 b) {
    b32 result = !(b.max.x <= a.min.x ||
                   b.min.x >= a.max.x ||
                   b.max.y <= a.min.y ||
                   b.min.y >= a.max.y);
    return result;
}

SD_MATH_API Rect2 rect_sum(Rect2 a, Rect2 b) {
    Rect2 result;
    result.min = a.min + b.min;
    result.max = a.max + b.max;
    return result;
}

SD_MATH_API V2 get_barycentric(Rect2 rect, V2 p) {
    V2 result;
    result.x = safe_ratio_0(p.x - rect.min.x, rect.max.x - rect.min.x);
    result.y = safe_ratio_0(p.y - rect.min.y, rect.max.y - rect.min.y);
    return result;
}

SD_MATH_API Rect2 rect_union(Rect2 a, Rect2 b) {
    Rect2 result;
    result.min.x = min(a.min.x, b.min.x);
    result.min.y = min(a.min.y, b.min.y);
    result.max.x = max(a.max.x, b.max.x);
    result.max.y = max(a.max.y, b.max.y);
    return result;
}

SD_MATH_API Rect2 intersect(Rect2 a, Rect2 b) {
    Rect2 result;
    result.min.x = max(a.min.x, b.min.x);
    result.min.y = max(a.min.y, b.min.y);
    result.max.x = min(a.max.x, b.max.x);
    result.max.y = min(a.max.y, b.max.y);
    return result;
}

SD_MATH_API Rect2 grow_to_contain(Rect2 rect, V2 p) {
    Rect2 result = rect;
    result.min = v2_min(result.min, p);
    result.max = v2_max(result.max, p);
    return result;
}

SD_MATH_API Rect2 inverted_infinity_rect2() {
    Rect2 result;
    result.min.x = result.min.y =  F32_MAX;
    result.max.x = result.max.y = -F32_MAX;
    return result;
}

SD_MATH_API Rect2 correct_rect_winding(Rect2 rect) {
    Rect2 result = rect;
    if (rect.min.x > rect.max.x) {
        result.min.x = rect.max.x;
        result.max.x = rect.min.x;
    }
    if (rect.min.y > rect.max.y) {
        result.min.y = rect.max.y;
        result.max.y = rect.min.y;
    }
    return result;
}

SD_MATH_API f32 get_area(Rect2 box) {
    V2 dim = get_dim(box);
    return dim.x*dim.y;
}

//
// NOTE: Rect2i
//

SD_MATH_API Rect2i rect2i_min_max(V2i min, V2i max) {
    Rect2i result;
    result.min = min;
    result.max = max;
    return result;
}

SD_MATH_API Rect2i rect2i_min_dim(V2i min, V2i dim) {
    Rect2i result;
    result.min = min;
    result.max = result.min + dim;
    return result;
}

SD_MATH_API Rect2i rect2i_offset(Rect2i rect, V2i offset) {
    Rect2i result = rect;
    result.min += offset;
    result.max += offset;
    return result;
}

SD_MATH_API Rect2i rect2i_union(Rect2i a, Rect2i b) {
    Rect2i result;
    result.min.x = Min(a.min.x, b.min.x);
    result.min.y = Min(a.min.y, b.min.y);
    result.max.x = Max(a.max.x, b.max.x);
    result.max.y = Max(a.max.y, b.max.y);
    return result;
}

SD_MATH_API Rect2i rect2i_intersect(Rect2i a, Rect2i b) {
    Rect2i result;
    result.min.x = Max(a.min.x, b.min.x);
    result.min.y = Max(a.min.y, b.min.y);
    result.max.x = Min(a.max.x, b.max.x);
    result.max.y = Min(a.max.y, b.max.y);
    return result;
}

SD_MATH_API Rect2i rect2i_grow_to_contain(Rect2i rect, V2i p) {
    Rect2i result = rect;
    result.min.x = Min(result.min.x, p.x);
    result.max.x = Max(result.max.x, p.x);
    result.min.y = Min(result.min.y, p.y);
    result.max.y = Max(result.max.y, p.y);
    return result;
}

SD_MATH_API s32 rect2i_width(Rect2i rect) {
    s32 result = rect.max.x - rect.min.x;
    return result;
}

SD_MATH_API s32 rect2i_height(Rect2i rect) {
    s32 result = rect.max.y - rect.min.y;
    return result;
}

SD_MATH_API V2i rect2i_dim(Rect2i rect) {
    V2i result;
    result.x = rect.max.x - rect.min.x;
    result.y = rect.max.y - rect.min.y;
    return result;
}

SD_MATH_API Rect2i rect2i_grow_by_radius(Rect2i rect, V2i r) {
    Rect2i result;
    result.min = rect.min - r;
    result.max = rect.max + r;
    return result;
}

SD_MATH_API s32 rect2i_get_area(Rect2i rect) {
    s32 w = rect.max.x - rect.min.x;
    s32 h = rect.max.y - rect.min.y;
    s32 area = 0;
    if ((w > 0) && (h > 0)) {
        area = w*h;
    }
    return area;
}

SD_MATH_API Rect2i infinity_rect2i() {
    Rect2i result;
    result.max.x = result.max.y =  INT32_MAX;
    result.min.x = result.min.y = -INT32_MAX;
    return result;
}

SD_MATH_API Rect2i inverted_infinity_rect2i() {
    Rect2i result;
    result.min.x = result.min.y =  INT32_MAX;
    result.max.x = result.max.y = -INT32_MAX;
    return result;
}

SD_MATH_API b32 is_in_rect2i(Rect2i rect, V2i p) {
    b32 result = ((p.x >= rect.min.x && p.x < rect.max.x) &&
                  (p.y >= rect.min.y && p.y < rect.max.y));
    return result;
}

//
// NOTE: Rect3i
//

SD_MATH_API Rect3i rect3i_rect_union(Rect3i a, Rect3i b) {
    Rect3i result;
    result.min = v3i_min(a.min, b.min);
    result.max = v3i_max(a.max, b.max);
    return result;
}

SD_MATH_API Rect3i v3i_rect_intersect(Rect3i a, Rect3i b) {
    Rect3i result;
    result.min = v3i_max(a.min, b.min);
    result.max = v3i_min(a.max, b.max);
    return result;
}

SD_MATH_API Rect3i v3i_grow_to_contain(Rect3i a, V3i p) {
    Rect3i result = a;
    result.min = v3i_min(a.min, p);
    result.max = v3i_max(a.max, p);
    return result;
}

SD_MATH_API s32 rect3i_width(Rect3i a) {
    s32 result = a.max.x - a.min.x;
    return result;
}

SD_MATH_API s32 rect3i_height(Rect3i a) {
    s32 result = a.max.y - a.min.y;
    return result;
}

SD_MATH_API s32 rect3i_depth(Rect3i a) {
    s32 result = a.max.z - a.min.z;
    return result;
}

SD_MATH_API V3i rect3i_dim(Rect3i a) {
    V3i result;
    result.x = rect3i_width(a);
    result.y = rect3i_height(a);
    result.z = rect3i_depth(a);
    return result;
}

SD_MATH_API s32 rect3i_volume(Rect3i a) {
    s32 w = a.max.x - a.min.x;
    s32 h = a.max.y - a.min.y;
    s32 d = a.max.z - a.min.z;
    s32 area = 0;
    if ((w > 0) && (h > 0) && (d > 0)) {
        area = w*h*d;
    }
    return area;
}

SD_MATH_API Rect3i infinity_rect3i() {
    Rect3i result;
    result.min.x = result.min.y = result.min.z = -INT32_MAX;
    result.max.x = result.max.y = result.max.z =  INT32_MAX;
    return result;
}

SD_MATH_API Rect3i inverted_infinity_rect3i() {
    Rect3i result;
    result.min.x = result.min.y = result.min.z =  INT32_MAX;
    result.max.x = result.max.y = result.max.z = -INT32_MAX;
    return result;
}

SD_MATH_API b32 is_in_rect3i(Rect3i rect, V3i p) {
    b32 result = ((p.x >= rect.min.x && p.x < rect.max.x) &&
                  (p.y >= rect.min.y && p.y < rect.max.y) &&
                  (p.z >= rect.min.z && p.z < rect.max.z));
    return result;
}

//
// NOTE: V3
//

SD_MATH_API V3 v3_neg(V3 a) {
    V3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

#define IMPLEMENT_VECTOR_FUNCTIONS(op_name, op) \
SD_MATH_API V3 v3_##op_name(V3 a, V3 b) { \
V3 result; \
result.x = a.x op b.x; \
result.y = a.y op b.y; \
result.z = a.z op b.z; \
return result; \
} \
SD_MATH_API void v3_mut_##op_name(V3* a, V3 b) { \
a->x = a->x op b.x; \
a->y = a->y op b.y; \
a->z = a->z op b.z; \
}

#define IMPLEMENT_SCALAR_FUNCTIONS(op_name, op) \
SD_MATH_API V3 v3_##op_name##_s(V3 a, f32 b) { \
V3 result; \
result.x = a.x op b; \
result.y = a.y op b; \
result.z = a.z op b; \
return result; \
} \
SD_MATH_API V3 v3_s_##op_name(f32 a, V3 b) { \
V3 result; \
result.x = a op b.x; \
result.y = a op b.y; \
result.z = a op b.z; \
return result; \
} \
SD_MATH_API void v3_mut_##op_name##_s(V3* a, f32 b) { \
a->x = a->x op b; \
a->y = a->y op b; \
a->z = a->z op b; \
}

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_FUNCTIONS(add, +)
IMPLEMENT_VECTOR_FUNCTIONS(sub, -)
IMPLEMENT_SCALAR_FUNCTIONS(mul, *)
IMPLEMENT_SCALAR_FUNCTIONS(div, /)
IMPLEMENT_VECTOR_FUNCTIONS(mul, *)
IMPLEMENT_VECTOR_FUNCTIONS(div, /)

#undef IMPLEMENT_VECTOR_FUNCTIONS
#undef IMPLEMENT_SCALAR_FUNCTIONS

SD_MATH_API V3 v3_square(V3 v) {
    V3 result;
    result.x = v.x*v.x;
    result.y = v.y*v.y;
    result.z = v.z*v.z;
    return result;
}

SD_MATH_API V3 v3_lerp(V3 a, V3 b, f32 t) {
    f32 inv_t = 1.0f - t;
    V3 result;
    result.x = inv_t*a.x + t*b.x;
    result.y = inv_t*a.y + t*b.y;
    result.z = inv_t*a.z + t*b.z;
    return result;
}

SD_MATH_API f32 v3_dot(V3 a, V3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

SD_MATH_API V3 cross(V3 a, V3 b) {
    return v3(a.y*b.z - a.z*b.y,
              a.z*b.x - a.x*b.z,
              a.x*b.y - a.y*b.x);
}

SD_MATH_API V3 v3_reflect(V3 a, V3 b) {
    f32 dot2 = 2.0f*v3_dot(a, b);
    V3 result;
    result.x = a.x - dot2*b.x;
    result.y = a.y - dot2*b.y;
    result.z = a.z - dot2*b.z;
    return result;
}

SD_MATH_API f32 v3_length_sq(V3 v) {
    f32 result = v3_dot(v, v);
    return result;
}

SD_MATH_API f32 v3_length(V3 v) {
    f32 result = square_root(v3_dot(v, v));
    return result;
}

SD_MATH_API V3 v3_normalize(V3 v) {
    V3 result = v / v3_length(v);
    return result;
}

SD_MATH_API V3 v3_noz(V3 v) {
    V3 result = {};
    
    f32 l_sq = v3_length_sq(v);
    if (l_sq > square(0.0001f)) {
        result = v*(1.0f / square_root(l_sq));
    }
    return result;
}

SD_MATH_API V3 v3_min(V3 a, V3 b) {
    V3 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    return result;
}

SD_MATH_API V3 v3_min_s(V3 a, f32 b) {
    V3 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    result.z = min(a.z, b);
    return result;
}

SD_MATH_API V3 v3_max(V3 a, V3 b) {
    V3 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    return result;
}

SD_MATH_API V3 v3_max_s(V3 a, f32 b) {
    V3 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    result.z = max(a.z, b);
    return result;
}

SD_MATH_API V3 v3_clamp(V3 n, V3 lo, V3 hi) {
    V3 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    result.z = max(lo.z, min(hi.z, n.z));
    return result;
}

SD_MATH_API V3 v3_clamp_s(V3 n, f32 lo, f32 hi) {
    V3 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    result.z = max(lo, min(hi, n.z));
    return result;
}

//
// NOTE: Rect3
//

SD_MATH_API V3 rect3_dim(Rect3 rect) {
    V3 result = v3_sub(rect.max, rect.min);
    return result;
}

SD_MATH_API V3 rect3_center(Rect3 rect) {
    V3 result = v3_s_mul(0.5f, v3_add(rect.min, rect.max));
    return result;
}

SD_MATH_API Rect3 rect3_min_max(V3 min, V3 max) {
    Rect3 result;
    result.min = min;
    result.max = max;
    return result;
}

SD_MATH_API Rect3 rect3_center_half_dim(V3 center, V3 half_dim) {
    Rect3 result;
    result.min = v3_sub(center, half_dim);
    result.max = v3_add(center, half_dim);
    return result;
}

SD_MATH_API Rect3 rect3_center_dim(V3 center, V3 dim) {
    Rect3 result;
    result.min = v3_add(center, v3_s_mul(0.5f, dim));
    result.max = v3_add(center, v3_s_mul(0.5f, dim));
    return result;
}

SD_MATH_API Rect3 rect3_min_dim(V3 min, V3 dim) {
    Rect3 result;
    result.min = min;
    result.max = v3_add(min, dim);
    return result;
}

SD_MATH_API Rect3 rect3_mul_dim(Rect3 rect, V3 mul) {
    Rect3 result;
    result.min = v3_mul(rect.min, mul);
    result.max = v3_mul(rect.max, mul);
    return result;
}

SD_MATH_API Rect3 rect3_grow_by_radius(Rect3 rect, V3 r_dim) {
    Rect3 result;
    result.min = v3_sub(rect.min, r_dim);
    result.max = v3_add(rect.max, r_dim);
    return result;
}

SD_MATH_API Rect3 rect3_grow_by_diameter(Rect3 rect, V3 d_dim) {
    Rect3 result;
    result.min = v3_sub(rect.min, v3_s_mul(0.5f, d_dim));
    result.max = v3_add(rect.max, v3_s_mul(0.5f, d_dim));
    return result;
}

SD_MATH_API Rect3 rect3_offset(Rect3 a, V3 offset) {
    Rect3 result;
    result.min = v3_add(a.min, offset);
    result.max = v3_add(a.max, offset);
    return result;
}

SD_MATH_API b32 is_in_rect3(Rect3 rect, V3 test) {
    b32 result = ((test.x >= rect.min.x && test.x < rect.max.x) &&
                  (test.y >= rect.min.y && test.y < rect.max.y) &&
                  (test.z >= rect.min.z && test.z < rect.max.z));
    return result;
}

SD_MATH_API b32 rect3_contained_in_rect3(Rect3 outer, Rect3 inner) {
    b32 result = ((outer.min.x >= inner.min.x && outer.max.x <= inner.max.x) &&
                  (outer.min.y >= inner.min.y && outer.max.y <= inner.max.y) &&
                  (outer.min.z >= inner.min.z && outer.max.z <= inner.max.z));
    return result;
}

SD_MATH_API b32 rect3_intersect(Rect3 a, Rect3 b) {
    b32 result = !(b.max.x <= a.min.x ||
                   b.min.x >= a.max.x ||
                   b.max.y <= a.min.y ||
                   b.min.y >= a.max.y ||
                   b.max.z <= a.min.z ||
                   b.min.z >= a.max.z);
    return result;
}

SD_MATH_API V3 rect3_get_barycentric(Rect3 rect, V3 p) {
    V3 result;
    result.x = safe_ratio_0(p.x - rect.min.x, rect.max.x - rect.min.x);
    result.y = safe_ratio_0(p.y - rect.min.y, rect.max.y - rect.min.y);
    result.z = safe_ratio_0(p.z - rect.min.z, rect.max.z - rect.min.z);
    return result;
}

SD_MATH_API Rect2 rect3_to_rect2_xy(Rect3 rect) {
    Rect2 result;
    result.min = rect.min.xy;
    result.max = rect.max.xy;
    return result;
}

//
// NOTE: V4
//

SD_MATH_API V4 v4_neg(V4 a) {
    V4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    return result;
}

#define IMPLEMENT_VECTOR_FUNCTIONS(op_name, op) \
SD_MATH_API V4 v4_##op_name(V4 a, V4 b) { \
V4 result; \
result.x = a.x op b.x; \
result.y = a.y op b.y; \
result.z = a.z op b.z; \
result.w = a.w op b.w; \
return result; \
} \
SD_MATH_API void v4_mut_##op_name(V4* a, V4 b) { \
a->x = a->x op b.x; \
a->y = a->y op b.y; \
a->z = a->z op b.z; \
a->w = a->w op b.w; \
}

#define IMPLEMENT_SCALAR_FUNCTIONS(op_name, op) \
SD_MATH_API V4 v4_##op_name##_s(V4 a, f32 b) { \
V4 result; \
result.x = a.x op b; \
result.y = a.y op b; \
result.z = a.z op b; \
result.w = a.w op b; \
return result; \
} \
SD_MATH_API V4 v4_s_##op_name(f32 a, V4 b) { \
V4 result; \
result.x = a op b.x; \
result.y = a op b.y; \
result.z = a op b.z; \
result.w = a op b.w; \
return result; \
} \
SD_MATH_API void v4_mut_##op_name##_s(V4* a, f32 b) { \
a->x = a->x op b; \
a->y = a->y op b; \
a->z = a->z op b; \
a->w = a->w op b; \
}

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_FUNCTIONS(add, +)
IMPLEMENT_VECTOR_FUNCTIONS(sub, -)
IMPLEMENT_SCALAR_FUNCTIONS(mul, *)
IMPLEMENT_SCALAR_FUNCTIONS(div, /)
IMPLEMENT_VECTOR_FUNCTIONS(mul, *)
IMPLEMENT_VECTOR_FUNCTIONS(div, /)

#undef IMPLEMENT_VECTOR_FUNCTIONS
#undef IMPLEMENT_SCALAR_FUNCTIONS

SD_MATH_API V4 v4_square(V4 v) {
    V4 result;
    result.x = v.x*v.x;
    result.y = v.y*v.y;
    result.z = v.z*v.z;
    result.w = v.w*v.w;
    return result;
}

SD_MATH_API V4 v4_lerp(V4 a, V4 b, f32 t) {
    f32 inv_t = 1.0f - t;
    V4 result;
    result.x = inv_t*a.x + t*b.x;
    result.y = inv_t*a.y + t*b.y;
    result.z = inv_t*a.z + t*b.z;
    result.w = inv_t*a.w + t*b.w;
    return result;
}

SD_MATH_API f32 v4_dot(V4 a, V4 b) {
    f32 result = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    return result;
}

SD_MATH_API V4 v4_reflect(V4 a, V4 b) {
    f32 dot2 = 2.0f*v4_dot(a, b);
    V4 result;
    result.x = a.x - dot2*b.x;
    result.y = a.y - dot2*b.y;
    result.z = a.z - dot2*b.z;
    result.w = a.w - dot2*b.w;
    return result;
}

SD_MATH_API f32 v4_length_sq(V4 v) {
    f32 result = v4_dot(v, v);
    return result;
}

SD_MATH_API f32 v4_length(V4 v) {
    f32 result = square_root(v4_dot(v, v));
    return result;
}

SD_MATH_API V4 v4_min(V4 a, V4 b) {
    V4 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    result.w = min(a.w, b.w);
    return result;
}

SD_MATH_API V4 v4_min_s(V4 a, f32 b) {
    V4 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    result.z = min(a.z, b);
    result.w = min(a.w, b);
    return result;
}

SD_MATH_API V4 v4_max(V4 a, V4 b) {
    V4 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    result.w = max(a.w, b.w);
    return result;
}

SD_MATH_API V4 v4_max_s(V4 a, f32 b) {
    V4 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    result.z = max(a.z, b);
    result.w = max(a.w, b);
    return result;
}

SD_MATH_API V4 v4_clamp(V4 n, V4 lo, V4 hi) {
    V4 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    result.z = max(lo.z, min(hi.z, n.z));
    result.w = max(lo.w, min(hi.w, n.w));
    return result;
}

SD_MATH_API V4 v4_clamp_s(V4 n, f32 lo, f32 hi) {
    V4 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    result.z = max(lo, min(hi, n.z));
    result.w = max(lo, min(hi, n.w));
    return result;
}

//
// NOTE: M4x4
//

SD_MATH_API M4x4 m4x4_mul(M4x4 a, M4x4 b) {
    M4x4 result;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            for (int i = 0; i < 4; ++i) {
                result.e[r][c] += a.e[r][i]*b.e[i][c];
            }
        }
    }
    return result;
}

SD_MATH_API V4 m4x4_transform_v4(M4x4 m, V4 p) {
    V4 result;
    result.x = p.x*m.e[0][0] + p.y*m.e[0][1] + p.z*m.e[0][2] + p.w*m.e[0][3];
    result.y = p.x*m.e[1][0] + p.y*m.e[1][1] + p.z*m.e[1][2] + p.w*m.e[1][3];
    result.z = p.x*m.e[2][0] + p.y*m.e[2][1] + p.z*m.e[2][2] + p.w*m.e[2][3];
    result.w = p.x*m.e[3][0] + p.y*m.e[3][1] + p.z*m.e[3][2] + p.w*m.e[3][3];
    return result;
}

SD_MATH_API M4x4 m4x4_identity() {
    M4x4 result = {
        {
            { 1, 0, 0, 0, },
            { 0, 1, 0, 0, },
            { 0, 0, 1, 0, },
            { 0, 0, 0, 1, },
        }
    };
    return result;
}

SD_MATH_API M4x4 m4x4_x_rotation(f32 angle) {
    f32 c = SD_MATH_COS(angle);
    f32 s = SD_MATH_SIN(angle);
    M4x4 result = {
        {
            { 1, 0, 0, 0, },
            { 0, c,-s, 0, },
            { 0, s, c, 0, },
            { 0, 0, 0, 1, },
        }
    };
    return result;
}

SD_MATH_API M4x4 m4x4_y_rotation(f32 angle) {
    f32 c = SD_MATH_COS(angle);
    f32 s = SD_MATH_SIN(angle);
    M4x4 result = {
        {
            { c, 0, s, 0, },
            { 0, 1, 0, 0, },
            {-s, 0, c, 0, },
            { 0, 0, 0, 1, },
        }
    };
    return result;
}

SD_MATH_API M4x4 m4x4_z_rotation(f32 angle) {
    f32 c = SD_MATH_COS(angle);
    f32 s = SD_MATH_SIN(angle);
    M4x4 result = {
        {
            { c,-s, 0, 0, },
            { s, c, 0, 0, },
            { 0, 0, 1, 0, },
            { 0, 0, 0, 1, },
        }
    };
    return result;
}

#endif /* SD_MATH_H */
