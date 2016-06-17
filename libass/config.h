#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#define _USE_MATH_DEFINES
#include <math.h>

#define inline __inline

#define CONFIG_DIRECTWRITE	1
#define CONFIG_RASTERIZER	1
#define CONFIG_ASM			1

#ifdef _X86_
#define __i386__
#elif _AMD64_
#define __x86_64__
#endif
