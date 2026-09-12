// RXDK/Xbox compatibility shim for StormLib, force-included ahead of StormPort.h.
//
// StormLib auto-detects Windows from _WIN32 (which the RXDK C++ build undefines) and
// otherwise falls to its POSIX ("Linux") path, which uses the Large File Summit *64
// API (stat64/lseek64/fstat64/ftruncate64/off64_t/O_LARGEFILE). picolibc/RXDK libc
// ships only the plain 32-bit file API (open/read/lseek/close/stat/fstat), so map the
// *64 names onto it. OTR archives for the base game (and size-clamped HD packs) stay
// well under the 2 GB signed-off_t limit; revisit if a >2 GB archive is ever needed.
#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

#define stat64      stat
#define fstat64     fstat
#define lseek64     lseek
#define ftruncate64 ftruncate
#define off64_t     off_t

// ftruncate is only reached on the archive-write path (unused for read-only OTR
// loading) and picolibc doesn't declare it; declare it so write TUs still compile.
#ifdef __cplusplus
extern "C" {
#endif
int ftruncate(int fd, off_t length);
#ifdef __cplusplus
}
#endif
