#ifndef SD_COMMON_H
#define SD_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>

typedef int32_t   b32;

typedef float     f32;
typedef double    f64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    s8;
typedef int16_t   s16;
typedef int32_t   s32;
typedef int64_t   s64;

typedef uintptr_t umm;
typedef intptr_t  smm;

#define F32_MAX           3.402823466e+38f
#define F32_MIN           -F32_MAX
#define F32_SMALLEST      1.175494351e-38f
#define F32_TRUE_SMALLEST 1.401298464e-45f
#define F32_EPSILON       1.192092896e-07f

#define F64_MAX           1.7976931348623158e+308
#define F64_MIN           -F64_MAX
#define F64_SMALLEST      2.2250738585072014e-308
#define F64_TRUE_SMALLEST 4.9406564584124654e-324
#define F64_EPSILON       2.2204460492503131e-016

#define S8_MIN            (-127i8 - 1)
#define S16_MIN           (-32767i16 - 1)
#define S32_MIN           (-2147483647i32 - 1)
#define S64_MIN           (-9223372036854775807i64 - 1)
#define S8_MAX            127i8
#define S16_MAX           32767i16
#define S32_MAX           2147483647i32
#define S64_MAX           9223372036854775807i64
#define U8_MAX            0xffui8
#define U16_MAX           0xffffui16
#define U32_MAX           0xffffffffui32
#define U64_MAX           0xffffffffffffffffui64

#define internal      static
#define global        static
#define local_persist static

#if MY_DEBUG
#define Assert(expression) (!(expression) ? *(volatile int*)0 = 0 : 0)
#else
#define Assert(expression)
#endif

#define InvalidCodePath Assert(!"Invalid Code Path");
#define InvalidDefaultCase default: { Assert(!"Invalid Default Case"); } break;
#define NotImpemented Assert(!"Not Implemented");

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))
#define ArrayEnd(array) (array) + ArrayCount(array)
#define ForArray(it, array) for (auto it = array; it < ArrayEnd(array); ++it)

#define Link(type, name) struct { type* next; } name;
#define DoubleLink(type, name) struct { type* next; type* prev } name;

#define SllStackPush(h, n) ((n)->next = (h), (h) = (n))
#define SllStackPop_(h) ((h) = (h)->next)
#define SllStackPop(h) h; SllStackPop_(h)
#define SllQueuePush(f, l, n) ((n)->next = 0, ((f) ? (l)->next = (n) : (f) = (n)), (l) = (n))
#define SllQueuePop(f, l, n) f; (SllStackPop_(f), ((f) ? 0 : (l) = 0))

#define ForSllUnique(it, head, next) for (auto* it = head; it; it = it->next)
#define ForSll(it, head) ForSllUnique(it, head, next)
#define ForSllOuterUnique(it_at, head, next) for (auto** it_at = &(head); *it_at; it_at = &(*it_at)->next)
#define ForSllOuter(it_at, head) ForSllOuterUnique(it_at, head, next)

#define DllInit(s) ((s)->next = s, (s)->prev = s)
#define DllInsertFront(h, n) ((n)->next = (h)->next, (n)->prev = h, (n)->next->prev = n, (n)->prev->next = n)
#define DllInsertBack(h, n) ((n)->next = h, (n)->prev = (h)->prev, (n)->next->prev = n, (n)->prev->next = n)
#define DllRemove(n) ((n)->next->prev = (n)->prev, (n)->prev->next = (n)->next)
#define DllIsEmpty(s) ((s)->next == (s))

#define ForDllUnique(it, sentinel, next, prev) for (auto prev_##it = (sentinel), it = (sentinel)->next; \
it != (sentinel);                                   \
prev_##it = prev_##it->next, it = prev_##it->next)
#define ForDll(it, sentinel) ForDllUnique(it, sentinel, next, prev)

#define Paste(a, b) a##b
#define Stringize__(x) #x
#define Stringize_(x) Stringize__(x)
#define Stringize(x) Stringize_(x)
#define Expand_(x) x
#define Expand(x) Expand(x)

#define BitIsSet(mask, bit) ((mask) & ((u64)1 << bit))
#define SetBit(mask, bit)   ((mask) |= ((u64)1 << bit))
#define UnsetBit(mask, bit) ((mask) &= ~((u64)1 << bit))

#define Abs(val) ((val) < 0 ? -(val) : (val))
#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Clamp(x, lo, hi) (Max((lo), Min((hi), (x))))

#define AlignPow2(value, align) (((value) + ((align) - 1)) & ~((align) - 1))
#define Align4(value) ((value + 3) & ~3)
#define Align8(value) ((value + 7) & ~7)
#define Align16(value) ((value + 15) & ~15)

#define Kilobytes(num) ((num) * 1024)
#define Megabytes(num) (Kilobytes(num) * 1024)
#define Gigabytes(num) (Megabytes(num) * 1024)
#define Terabytes(num) (Gigabytes(num) * 1024)

#define Milliseconds(seconds) ((seconds) / 1000.0f)

#define sizeof_member(struct_type, member) sizeof(((struct_type*)0)->member)

#define define_safe_truncate(big, small, min, max)             \
inline small safe_truncate_##big##_##small(big source) {   \
Assert(source >= min && source <= max);                \
small result = (small)source;                          \
return result;                                         \
}

define_safe_truncate(u64, u32, 0, UINT32_MAX);
define_safe_truncate(u64, u16, 0, UINT16_MAX);
define_safe_truncate(u64, u8 , 0, UINT8_MAX);
define_safe_truncate(u32, u16, 0, UINT16_MAX);
define_safe_truncate(u32, u8 , 0, UINT8_MAX);
define_safe_truncate(u16, u8 , 0, UINT8_MAX);

define_safe_truncate(s64, s32, INT32_MIN, INT32_MAX);
define_safe_truncate(s64, s16, INT16_MIN, INT16_MAX);
define_safe_truncate(s64, s8 , INT8_MIN , INT8_MAX);
define_safe_truncate(s32, s16, INT16_MIN, INT16_MAX);
define_safe_truncate(s32, s8 , INT8_MIN , INT8_MAX);
define_safe_truncate(s16, s8 , INT8_MIN , INT8_MAX);

define_safe_truncate(f64, f32, -FLT_MAX , FLT_MAX);

#undef define_safe_truncate

#define define_saturating_cast(big, small, min, max)             \
inline small saturating_cast_##big##_##small(big source) {   \
small result = (small)Clamp(source, min, max);           \
return result;                                           \
}

define_saturating_cast(u64, u32, 0, UINT32_MAX);
define_saturating_cast(u64, u16, 0, UINT16_MAX);
define_saturating_cast(u64, u8 , 0, UINT8_MAX);
define_saturating_cast(u32, u16, 0, UINT16_MAX);
define_saturating_cast(u32, u8 , 0, UINT8_MAX);
define_saturating_cast(u16, u8 , 0, UINT8_MAX);

define_saturating_cast(s64, s32, INT32_MIN, INT32_MAX);
define_saturating_cast(s64, s16, INT16_MIN, INT16_MAX);
define_saturating_cast(s64, s8 , INT8_MIN , INT8_MAX);
define_saturating_cast(s32, s16, INT16_MIN, INT16_MAX);
define_saturating_cast(s32, s8 , INT8_MIN , INT8_MAX);
define_saturating_cast(s16, s8 , INT8_MIN , INT8_MAX);

define_saturating_cast(f64, f32, -FLT_MAX , FLT_MAX);

#undef define_saturating_cast

#define Swap(t, a, b) { t _swap_temp = a; a = b; b = _swap_temp; }
#define Unique(x) Paste(x, __LINE__)

#endif /* SD_COMMON_H */
