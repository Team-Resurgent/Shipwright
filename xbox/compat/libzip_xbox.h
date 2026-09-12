// RXDK/Xbox compatibility shim for libzip, force-included ahead of every libzip TU.
//
// libzip auto-detects Windows from _WIN32 (which the RXDK C/C++ build undefines) and
// otherwise takes its POSIX path. Two small things the POSIX path assumes that picolibc
// gates behind feature-test macros:
//   * strdup()          -- POSIX.1-2008
//   * strcasecmp()      -- declared in <strings.h> under _DEFAULT_SOURCE
// A force-include lands before any system header, so defining the feature-test macros
// here actually takes effect (unlike defining them mid-TU). We also pull in <strings.h>
// so strcasecmp is declared even in the TUs that don't include it themselves.
//
// Everything else libzip needs (O_CLOEXEC/O_EXCL/O_CREAT, off_t, snprintf, localtime_r,
// strtoll/strtoull) is provided by picolibc and is selected via the hand-written
// src/config.h (zlib-only, no bzip2/xz/zstd, no encryption).
#pragma once

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <strings.h> // strcasecmp
#include <string.h>  // strdup
