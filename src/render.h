/* date = September 27th 2020 4:45 pm */

#ifndef RENDER_H
#define RENDER_H

#include <stdio.h>
#include <string.h>
#include <math.h>

//
//
//

#include "sd_common.h"
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
