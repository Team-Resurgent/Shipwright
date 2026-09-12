// Minimal SDL_stdinc.h stub for the RXDK/Xbox build (basic SDL scalar types only).
#pragma once
#include <stdint.h>
#include <stddef.h>
#ifndef SDL_STDINC_TYPES_DEFINED
#define SDL_STDINC_TYPES_DEFINED
typedef uint8_t  Uint8;  typedef int8_t  Sint8;
typedef uint16_t Uint16; typedef int16_t Sint16;
typedef uint32_t Uint32; typedef int32_t Sint32;
typedef uint64_t Uint64; typedef int64_t Sint64;
typedef enum { SDL_FALSE = 0, SDL_TRUE = 1 } SDL_bool;
#endif
