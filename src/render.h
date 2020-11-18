/* date = September 27th 2020 4:45 pm */

#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <string.h>
#include <math.h>

//
//
//

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <float.h>

//

#include "sd_common.h"
#define overload      __attribute__((overloadable))
#define force_inline  __attribute__((always_inline))
#define function      static overload

#include "sd_stretchy_buffer.h"

#define SD_MATH_STATIC
#include "sd_math.h"

#define SDSTR_STATIC
#define SDSTR_USE_STDLIB
#include "sd_string.h"

//
//
//

void* global_heap_alloc(umm size);
void global_heap_free(void* ptr);

#include "image.h"
#include "obj.h"

#endif //RENDER_H
